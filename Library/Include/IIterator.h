#ifndef MDFSIMPLECONVERTERS_IITERATOR_H
#define MDFSIMPLECONVERTERS_IITERATOR_H

#include <memory>

namespace mdf {

  template <typename T>
  struct IIterator {
    virtual ~IIterator() = default;

    virtual void increment() = 0;
    virtual T& dereference() = 0;

    virtual std::unique_ptr<IIterator<T>> begin() const = 0;
    virtual std::unique_ptr<IIterator<T>> end() const = 0;

    virtual std::unique_ptr<IIterator<T>> clone() const = 0;
    virtual bool equals(IIterator<T> const& other) const = 0;

    bool operator==(IIterator<T> const& other) const {
      bool result = false;

      if(typeid(*this) == typeid(other)) {
        result = equals(other);
      }

      return result;
    }
  };

}

#endif //MDFSIMPLECONVERTERS_IITERATOR_H
