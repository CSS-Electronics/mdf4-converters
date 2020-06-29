#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/algorithm/string.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/bind.hpp>
#include <boost/predef.h>
#include <boost/endian.hpp>
#include <boost/predef.h>
#include <boost/mpl/contains.hpp>
#include <iostream>
#include <bitset>

#include <algorithm>
#include <iterator>

#include "CAN_DataFrame.h"
#include "GenericIterator.h"
#include "LIN_Frame.h"
#include "MappingInformation.h"
#include "Assign.h"

#include "../StructAdaptors.h"
#include "../Utility.h"
#include "../Blocks/TXBlock.h"
#include "../Blocks/SDBlock.h"
#include "../Blocks/DTBlockSingleDiscontinuous.h"

#include <bitset>

namespace mdf {

    template<typename RecordType>
    GenericIterator<RecordType>::GenericIterator(
            std::shared_ptr<DGBlock> dgBlock,
            std::shared_ptr<std::streambuf> stream_,
            std::chrono::nanoseconds offset,
            std::size_t start
    ) : start(start), current(0) {
        // Ensure this is valid input.
        if (!dgBlock) {
            throw std::runtime_error("No DG block passed");
        }

        // Ensure it is sorted.
        std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();
        if (!cgBlock) {
            throw std::runtime_error("No CG block in DG block");
        } else if (cgBlock->getNextCGBlock()) {
            throw std::runtime_error("More than one CG block in DG block, not a sorted DG block");
        }

        stream = std::make_shared<CachingStreamBuffer>(stream_, 10 * 1024 * 1024);
        stream2 = std::make_shared<CachingStreamBuffer>(stream_, 10 * 1024 * 1024);

        // Extract end location.
        stop = cgBlock->getCycleCount();

        // Extract mapping information.
        mappingInformation.clear();
        std::size_t highestBitIndex = 0;

        auto cnBlocks = getAllCNBlocks(cgBlock);

        for (auto const &cnBlock: cnBlocks) {
            // Store name for correlation with structure members later.
            auto nameBlock = std::dynamic_pointer_cast<TXBlock>(cnBlock->getNameBlock());
            std::string_view name = nameBlock->getText();

            // If this is a VLSD channel, get access to the data.
            boost::function<uint64_t(uint64_t)> f;
            boost::function<double(double)> c;

            if (cnBlock->getDataBlock()) {
                auto dataBlock = std::dynamic_pointer_cast<SDBlock>(cnBlock->getDataBlock());

                f = RecordFunctor(dataBlock);

                //f = boost::bind(&SDBlock::operator[], dataBlock.get(), _1);
            }

            // Gather data.
            MappingInformationEntry m;
            m.byteOffset = static_cast<uint8_t>(cnBlock->getByteOffset());
            m.bitOffset = cnBlock->getBitOffset();
            m.bitLength = static_cast<uint8_t>(cnBlock->getBitCount());
            m.dataType = cnBlock->getDataType();
            m.dataFunc = f;
            m.fieldName = std::string(name);

            // If this is the master channel, it should be time.
            if ((cnBlock->getChannelType() == CNType::MasterChannel) && (cnBlock->getSyncType() == CNSyncType::Time)) {
                m.conversionFunc = [offset](void *value) {
                    auto castedValue = static_cast<std::chrono::nanoseconds *>(value);
                    *castedValue += offset;
                };
            }

            // Determine the highest possible bit index.
            std::size_t const currentBitIndex = m.byteOffset * 8 + m.bitOffset + m.bitLength;
            if (currentBitIndex > highestBitIndex) {
                highestBitIndex = currentBitIndex;
            }

            mappingInformation.insert(m);
        }

        // Pre-allocate the dynamic bit array to the correct size.
        bitData.reserve(highestBitIndex);

        // Correlate mapping information with the record fields. Using ideas from https://stackoverflow.com/a/39613483.
        mapping.clear();

        // Loop with an additional index.
        for (auto const &entry: mappingInformation | boost::adaptors::indexed(0)) {
            // Extract the channel name and convert it to lower case.
            std::string_view mapName = entry.value().fieldName;

            // If the channel name contains any '.', strip the leading part and the '.'.
            auto dotPos = mapName.find_last_of('.');
            if (dotPos != std::string_view::npos) {
                // Get a handle to the remaining as the name of the channel for matching against the fields in the RecordType.
                mapName = mapName.substr(dotPos + 1);
            }

            // Lambda function specific for this iteration, to compare the field name and the entry name.
            auto f = [&](auto index) {
                // Attempt to match the member name by lowercase comparison. Need to store the string here locally.
                std::string memberName = boost::fusion::extension::struct_member_name<RecordType, index>::call();

                if (boost::algorithm::iequals(memberName, mapName)) {
                    mapping.insert(std::make_pair(index, entry.index()));
                }
            };

            // Loop over all members of RecordType using their index.
            boost::fusion::for_each(
                    boost::mpl::range_c<unsigned, 0, boost::fusion::result_of::size<RecordType>::value>(),
                    f);

            // Store in easy access vector,
            mappingEntries.push_back(entry.value());
        }

        // Get structured data extractor.
        auto dtBlock = std::dynamic_pointer_cast<IDataBlock>(dgBlock->getDataBlock());

        // NOTE: May not need this, and can return to lambda functions, as long as they are copied in the copy constructor.
        functorStorage.emplace_back(RecordFunctor(dtBlock));
        std::function<uint64_t(std::size_t)> ff = functorStorage[0];

        func = ff;

        // Initialize the record if not at the end.
        if (current < stop) {
            mapDataToRecord();
        }
    }

    template<typename RecordType>
    GenericIterator<RecordType>::GenericIterator(const GenericIterator<RecordType> &other) :
            start(other.start),
            stop(other.stop),
            current(other.current),
            record(other.record),
            mappingInformation(other.mappingInformation),
            mapping(other.mapping),
            mappingEntries(other.mappingEntries),
            bitData(other.bitData),
            functorStorage(other.functorStorage),
            stream(other.stream),
            stream2(other.stream2) {
        func = functorStorage[0];
    }

    template<typename RecordType>
    void GenericIterator<RecordType>::increment() {
        if (current < stop) {
            current += 1;
            if (current < stop) {
                mapDataToRecord();
            }
        }
    }

    template<typename RecordType>
    RecordType const &GenericIterator<RecordType>::dereference() {
        return record;
    }

    template<typename RecordType>
    std::unique_ptr<IIterator<RecordType const>> GenericIterator<RecordType>::clone() const {
        return std::make_unique<GenericIterator<RecordType>>(*this);
    }

    template<typename RecordType>
    std::unique_ptr<IIterator<RecordType const>> GenericIterator<RecordType>::begin() const {
        auto result = std::make_unique<GenericIterator<RecordType>>(*this);
        result->current = result->start;

        return result;
    }

    template<typename RecordType>
    std::unique_ptr<IIterator<RecordType const>> GenericIterator<RecordType>::end() const {
        auto result = std::make_unique<GenericIterator<RecordType>>(*this);
        result->current = result->stop;

        return result;
    }

    template<typename RecordType>
    void GenericIterator<RecordType>::mapDataToRecord() {
        // Get access to the current data.
        uint64_t dataLocation = func(current);

        if (dataLocation == 0) {
            // No data available at this record.
            return;
        }

        // Use special assign functions from https://www.meetingcpp.com/blog/items/Accessing-a-fusion-sequence-with-a-run-time-defined-index.html.

        // Clear and copy data to the bit set.
        char buffer[128];

        auto location = stream->pubseekoff(dataLocation, std::ios_base::beg);

        if(location != dataLocation) {
            throw;
        }

        auto const expectedNumberOfBytes = bitData.capacity() / 8;
        auto recovered = stream->sgetn(buffer, expectedNumberOfBytes);

        if(recovered != expectedNumberOfBytes) {
            throw;
        }

        bitData.clear();
        bitData.init_from_block_range(buffer, buffer + bitData.capacity() / 8);

        // Lambda function for mapping each field in the final struct to the data.
        auto f = [&](auto index) {
            // Does this index have a corresponding column in the MDF file?
            auto iter = mapping.find(index);

            if (iter != mapping.end()) {
                auto const &s = iter->second;

                MappingInformationEntry const &entry = mappingEntries[s];
                std::bitset<64> raw;

                unsigned bitOffset = entry.byteOffset * 8 + entry.bitOffset;
                for (unsigned i = 0; i < entry.bitLength; ++i) {
                    raw[i] = bitData[bitOffset + i];
                }

                uint64_t rawValue = raw.to_ullong();

                // If the type requires endian swap, do it on the raw data.
                if constexpr(BOOST_ENDIAN_LITTLE_BYTE) {
                    if (CNDataTypeIsBigEndian(entry.dataType)) {
                        boost::endian::endian_reverse_inplace(rawValue);
                    }
                } else {
                    if (!CNDataTypeIsBigEndian(entry.dataType)) {
                        boost::endian::endian_reverse_inplace(rawValue);
                    }
                }

                // If a callback function is specified, use that to get the data.
                if (!entry.dataFunc) {
                    assign2(boost::fusion::at_c<index>(record), rawValue);
                } else {
                    // Get the correct offset to load data from.
                    uint64_t dataOffset = entry.dataFunc(rawValue);

                    assign3(boost::fusion::at_c<index>(record), *stream2.get(), dataOffset);
                }

                // If a conversion function is specified, run it afterwards.
                if (entry.conversionFunc) {
                    entry.conversionFunc(static_cast<void *>(&boost::fusion::at_c<index>(record)));
                }
            }
        };

        boost::fusion::for_each(boost::mpl::range_c<unsigned, 0, boost::fusion::result_of::size<RecordType>::value>(),
                                f);

        // Truncate data in the case of LIN records.
        if constexpr(std::is_same_v<LIN_Frame, RecordType>) {
            auto &r = reinterpret_cast<LIN_Frame &>(record);

            r.DataBytes.resize(r.DataLength);
        }
    }

    template<typename RecordType>
    bool GenericIterator<RecordType>::equals(IIterator<RecordType const> const &other) const {
        auto otherCasted = static_cast<GenericIterator<RecordType> const *>(&other);

        bool result = false;

        do {
            if (start != otherCasted->start) {
                break;
            }

            if (stop != otherCasted->stop) {
                break;
            }

            if (current != otherCasted->current) {
                break;
            }

            result = true;
        } while (false);

        return result;
    }

    // Initialize for all supported record types.
    template
    struct GenericIterator<CAN_DataFrame>;

    template
    struct GenericIterator<CAN_ErrorFrame>;

    template
    struct GenericIterator<CAN_RemoteFrame>;

    template
    struct GenericIterator<LIN_ChecksumError>;

    template
    struct GenericIterator<LIN_Frame>;

    template
    struct GenericIterator<LIN_ReceiveError>;

    template
    struct GenericIterator<LIN_SyncError>;

    template
    struct GenericIterator<LIN_TransmissionError>;

}
