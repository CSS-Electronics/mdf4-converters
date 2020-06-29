#ifndef MDFSIMPLECONVERTERS_GENERICITERATOR_H
#define MDFSIMPLECONVERTERS_GENERICITERATOR_H

#include "MappingInformation.h"
#include "IIterator.h"
#include "RecordFunctor.h"

#include "../Blocks/DGBlock.h"
#include "../CachingStreamBuffer.h"

#include <boost/dynamic_bitset.hpp>
#include <streambuf>

namespace mdf {

    typedef std::function<uint64_t(std::size_t)> NextRawDataLocationFunc;

    template<typename RecordType>
    struct GenericIterator : public IIterator<RecordType const> {
        explicit GenericIterator(
                std::shared_ptr<DGBlock> dgBlock,
                std::shared_ptr<std::streambuf> stream,
                std::chrono::nanoseconds offset = std::chrono::nanoseconds(0),
                std::size_t start = 0
                        );

        GenericIterator(GenericIterator<RecordType> const &other);

        void increment() override;

        RecordType const &dereference() override;

        std::unique_ptr<IIterator<RecordType const>> clone() const override;

        std::unique_ptr<IIterator<RecordType const>> begin() const override;

        std::unique_ptr<IIterator<RecordType const>> end() const override;

        bool equals(IIterator<RecordType const> const &other) const override;

    protected:
        std::size_t start;
        std::size_t stop;
        std::size_t current;
        std::shared_ptr<std::streambuf> stream;
        std::shared_ptr<CachingStreamBuffer> stream2;
        NextRawDataLocationFunc func;

        /**
         * Mapping between the fields in the RecordType and the DG block.
         */
        std::map<unsigned, unsigned, std::less<unsigned>, std::allocator<std::pair<const unsigned, unsigned>>> mapping;

        MappingInformation mappingInformation;
        RecordType record;

        std::vector<RecordFunctor> functorStorage;
        std::vector<MappingInformationEntry> mappingEntries;

        boost::dynamic_bitset<uint8_t> bitData;

        void mapDataToRecord();
    };

}

#endif //MDFSIMPLECONVERTERS_GENERICITERATOR_H
