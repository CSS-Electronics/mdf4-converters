#ifndef MDFSIMPLECONVERTERS_MAPPINGINFORMATION_H
#define MDFSIMPLECONVERTERS_MAPPINGINFORMATION_H

#include <cstdint>
#include <set>
#include <tuple>
#include <variant>

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <any>

#include "../Blocks/CNBlock.h"

namespace mdf {

  struct MappingInformationEntry {
    // Offset in bytes into the data.
    uint8_t byteOffset;

    // Offset in bits after the last byte into the data.
    uint8_t bitOffset;

    // Number of bits to read as data.
    uint8_t bitLength;

    // Type of the data.
    CNDataType dataType;

    // Optional function to extract additional data (SD blocks).
    boost::function<uint64_t(uint64_t)> dataFunc;

    // Name of the field.
    std::string fieldName;

    // Function used to convert the data.
    boost::function<void (void*)> conversionFunc;
  };

  inline bool operator<(const MappingInformationEntry &lhs, const MappingInformationEntry &rhs) {
    if (lhs.byteOffset != rhs.byteOffset) {
      return lhs.byteOffset < rhs.byteOffset;
    }

    if (lhs.bitOffset != rhs.bitOffset) {
      return lhs.bitOffset < rhs.bitOffset;
    }

    if (lhs.bitLength != rhs.bitLength) {
      return lhs.bitLength < rhs.bitLength;
    }

    if (lhs.dataType != rhs.dataType) {
      return lhs.dataType < rhs.dataType;
    }

    // TODO: Consider pointer comparison.

    if (lhs.fieldName != rhs.fieldName) {
      return lhs.fieldName < rhs.fieldName;
    }

    return false;
  }

  typedef std::set<MappingInformationEntry> MappingInformation;

}

#endif //MDFSIMPLECONVERTERS_MAPPINGINFORMATION_H
