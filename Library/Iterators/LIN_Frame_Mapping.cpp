#include "LIN_Frame_Mapping.h"

#include "boost/dynamic_bitset.hpp"
#include <bitset>
#include <cstring>
#include "SD_Block.h"

namespace mdf {

    template<typename T>
    static void getData2(MappingInformation information, boost::dynamic_bitset<uint8_t> const& data, T& storage);

    /**
     *
     *
     * This will overwrite the contents of storage.
     *
     * @tparam T
     * @param information
     * @param data
     * @param storage
     */
    template<typename T>
    static void getData2(MappingInformation information, boost::dynamic_bitset<uint8_t> const& data, T& storage) {
        // Find the correct offsets.
        uint8_t byteOffset = std::get<0>(information);
        uint8_t bitOffset = std::get<1>(information);
        uint8_t bitLength = std::get<2>(information);
        uint8_t dataType = std::get<3>(information);
        std::shared_ptr<SD_Block> dataBlock = std::get<4>(information);

        // Sanity check, ensure that the number of bits requested to read can be stored in the designated storage.
        if(sizeof(T) * 8 < bitLength) {
            // TODO: Throw exception.
        }

        // Create a temporary bitset to contain the value.
        std::bitset<sizeof(T) * 8> raw;

        bitOffset += byteOffset * 8;

        for(uint8_t i = 0; i < bitLength; ++i) {
            raw[i] = data[bitOffset + i];
        }

        // Convert to a number and read out.
        uint64_t temp = raw.to_ullong();
        switch (dataType) {
            case 0:
                storage = reinterpret_cast<T&>(temp);
                break;
            case 4:
                storage = reinterpret_cast<double&>(temp);
                break;
            default:
                storage = static_cast<T>(temp);
                break;
        }
    }

    static void getData2(MappingInformation information, boost::dynamic_bitset<uint8_t> const& data, std::array<uint8_t, 8>& storage) {
        // Find the correct offsets.
        uint8_t byteOffset = std::get<0>(information);
        uint8_t bitOffset = std::get<1>(information);
        uint8_t bitLength = std::get<2>(information);
        uint8_t dataType = std::get<3>(information);
        std::shared_ptr<SD_Block> dataBlock = std::get<4>(information);

        // Sanity check, ensure that the number of bits requested to read can be stored in the designated storage.
        if(storage.size() * 8 <= data.size()) {
            // TODO: Throw exception.
        }

        // Get the data address if set.
        std::bitset<64> raw;

        bitOffset += byteOffset * 8;

        for(uint8_t i = 0; i < bitLength; ++i) {
            raw[i] = data[bitOffset + i];
        }

        // Convert to a number and read out.
        uint64_t result = raw.to_ullong();
        std::memcpy(storage.data(), &result, 8);
    }

    static uint32_t getData(MappingInformation information, boost::dynamic_bitset<uint8_t> data) {
        // Find the correct offsets.
        uint8_t byteOffset = std::get<0>(information);
        uint8_t bitOffset = std::get<1>(information);
        uint8_t bitLength = std::get<2>(information);
        uint8_t dataType = std::get<3>(information);
        std::shared_ptr<SD_Block> dataBlock = std::get<4>(information);

        uint32_t result = 0;
        bitOffset += byteOffset * 8;

        for(uint8_t i = 0; i < bitLength; ++i) {
            result |= data[bitOffset + i] << i;
        }

        // If a data block is defined, lookup the data there.
        if(dataBlock) {

            std::pair<std::shared_ptr<GenericDataRecord>, std::size_t> newResult = dataBlock->getRecord(result);

            result = newResult.first->data[0];
        }

        return result;
    }

    LIN_Frame_t Convert_Record_LIN_Frame(GenericDataRecord const& data, LINMappingTable const& mapping) {
        LIN_Frame_t result = { 0 };

        // Extract the data.
        boost::dynamic_bitset<uint8_t> bits(data.data.cbegin(), data.data.cend());

        for(auto entry: mapping) {
            switch (entry.first) {
                case LIN_Timestamp:
                    getData2(entry.second, bits, result.TimeStamp);
                    break;
                case LIN_BusChannel:
                    result.BusChannel = getData(entry.second, bits);
                    break;
                case LIN_ID:
                    getData2(entry.second, bits, result.ID);
                    break;
                case LIN_ReceivedDataByteCount:
                    result.ReceivedDataByteCount = getData(entry.second, bits);
                    break;
                case LIN_DataLength:
                    result.DataLength = getData(entry.second, bits);
                    break;
                case LIN_Dir:
                    result.Dir = getData(entry.second, bits);
                    break;
                case LIN_DataBytes:
                    getData2(entry.second, bits, reinterpret_cast<std::array<uint8_t, 8>&>(result.DataBytes));
                    break;
                default:
                    break;
            }
        }

        return result;
    }

}
