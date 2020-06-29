#ifndef MDFSIMPLECONVERTERS_CNBLOCK_H
#define MDFSIMPLECONVERTERS_CNBLOCK_H

#include "MdfBlock.h"

namespace mdf {

  enum CNType : uint8_t {
    FixedLengthData = 0x00u,
    VariableLengthData = 0x01u,
    MasterChannel = 0x02u,
    VirtualMasterChannel = 0x03u,
    SynchronizationChannel = 0x04u,
    MaximumLengthData = 0x05u,
    VirtualData = 0x06u,
  };

  enum struct CNDataType : uint8_t {
    UnsignedIntegerLittleEndian = 0x00u,
    UnsignedIntegerBigEndian = 0x01u,
    SignedIntegerLittleEndian = 0x02u,
    SignedIntegerBigEndian = 0x03u,
    RealLittleEndian = 0x04u,
    RealBigEndian = 0x05u,
    ByteArray = 0x0Au
  };

  enum CNSyncType : uint8_t {
    None = 0x00u,
    Time = 0x01u,
  };

  inline bool CNDataTypeIsBigEndian(CNDataType dataType) {
    bool result = false;

    switch(dataType) {
      case CNDataType::UnsignedIntegerBigEndian:
        // fallthrough.
      case CNDataType::SignedIntegerBigEndian:
        // fallthrough.
      case CNDataType::RealBigEndian:
        result = true;
        break;
      default:
        break;
    }

    return result;
  }

  enum CNFlags : uint32_t {
    AllValuesInvalid = 0x01u << 0u,
    InvalidationBitValid = 0x01u << 1u,
    PrecisionValid = 0x01u << 2u,
    ValueRangeValid = 0x01u << 3u,
    LimitRangeValid = 0x01u << 4u,
    ExtendedLimitRangeValid = 0x01u << 5u,
    DiscreteValue = 0x01u << 6u,
    Calibration = 0x01u << 7u,
    Calculated = 0x01u << 8u,
    Virtual = 0x01u << 9u,
    BusEvent = 0x01u << 10u,
    Monotonous = 0x01u << 11u,
    DefaultXAxis = 0x01u << 12u,
  };

  struct CNBlock : MdfBlock {
    [[nodiscard]] std::shared_ptr<CNBlock> getNextCNBlock() const;

    [[nodiscard]] std::shared_ptr<CNBlock> getCompositionBlock() const;

    [[nodiscard]] std::shared_ptr<MdfBlock> getNameBlock() const;

    [[nodiscard]] std::shared_ptr<MdfBlock> getSourceBlock() const;

    [[nodiscard]] std::shared_ptr<MdfBlock> getDataBlock() const;

    void setDataBlock(std::shared_ptr<MdfBlock>);

    [[nodiscard]] uint8_t getBitOffset() const;

    [[nodiscard]] uint32_t getByteOffset() const;

    [[nodiscard]] uint32_t getBitCount() const;

    [[nodiscard]] CNType getChannelType() const;

    [[nodiscard]] CNDataType getDataType() const;

    [[nodiscard]] CNSyncType getSyncType() const;

  protected:
    bool load(std::shared_ptr<std::streambuf> stream) override;

    bool saveBlockData(uint8_t *dataPtr) override;

    uint8_t bitOffset;
    uint32_t byteOffset;
    uint32_t bitCount;
    uint32_t flags;
    uint8_t dataType;
    uint8_t type;
    uint8_t syncType;

    double valRangeMin;
    double valRangeMax;
  };

}

#endif //MDFSIMPLECONVERTERS_CNBLOCK_H
