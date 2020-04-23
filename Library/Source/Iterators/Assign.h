#ifndef MDFSIMPLECONVERTERS_ASSIGN_H
#define MDFSIMPLECONVERTERS_ASSIGN_H

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

#include <boost/container/static_vector.hpp>

namespace mdf {

  template<typename>
  struct is_static_vector2 : public std::false_type {
  };

  template<typename T, std::size_t A>
  struct is_static_vector2<boost::container::static_vector<T, A>> : public std::true_type {
  };

  template<typename>
  struct static_vector_length {
    static constexpr std::size_t capacity = 0;
  };

  template<typename T, std::size_t A>
  struct static_vector_length<boost::container::static_vector<T, A>> {
    static constexpr std::size_t capacity = A;
  };

  template<class ToType, class FromType>
  void assign2(ToType &to, FromType const &from) {
    // There are two cases: Reading variable length data and constant length data.

    // Handle the first case of variable length.
    if constexpr(is_static_vector2<ToType>::value) {
      // Ensure all access is done on byte basis.
      uint8_t const *basePtr = nullptr;
      std::size_t dataLength = 0;

      if (std::is_pointer_v<FromType>) {
        basePtr = reinterpret_cast<uint8_t const *>(from);

        // Special case of VLSD data. Read the first 4 bytes to determine data length.
        dataLength = *reinterpret_cast<uint32_t const *>(basePtr);
        basePtr += 4;

        // Sanity check that we don't write over the length of the array.
        if (dataLength > ToType::static_capacity) {
          throw std::runtime_error("Attempting to copy more bytes than the container can handle");
        }
      } else {
        basePtr = reinterpret_cast<uint8_t const *>(&from);

        // Static data length, must be the same as the vector.
        dataLength = ToType::static_capacity;

        assert(ToType::static_capacity <= sizeof(FromType));
      }

      // Clear the vector and copy over the data.
      to.clear();
      std::copy_n(basePtr, dataLength, std::back_inserter(to));

    } else if constexpr(std::is_same_v<ToType, double> && (sizeof(FromType) == sizeof(ToType))) {
      // Double specialization
      if constexpr(!std::is_pointer_v<FromType>) {
        to = reinterpret_cast<double const &>(from);
      }
    } else if constexpr(std::is_same_v<ToType, std::chrono::nanoseconds> && (sizeof(FromType) == sizeof(ToType))) {
      // std::chrono::nanoseconds specialization
      if constexpr(!std::is_pointer_v<FromType>) {
        double const translated = reinterpret_cast<double const &>(from);
        to = std::chrono::nanoseconds(static_cast<uint64_t>(translated));
      }
    } else if constexpr(std::is_convertible_v<FromType, ToType>) {
      to = from;
    } else {
      // All other cases. May be needed at compile time, but are not allowed at run time.
      throw std::runtime_error("Illegal conversion attempted");
    }

  }

}


#endif //MDFSIMPLECONVERTERS_ASSIGN_H
