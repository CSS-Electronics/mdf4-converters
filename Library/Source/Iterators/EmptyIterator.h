#ifndef MDFSIMPLECONVERTERS_EMPTYITERATOR_H
#define MDFSIMPLECONVERTERS_EMPTYITERATOR_H

#include "IIterator.h"

namespace mdf {

    template<typename RecordType>
    struct EmptyIterator : public IIterator<RecordType const> {
        EmptyIterator();

        void increment() override;

        RecordType const &dereference() override;

        std::unique_ptr<IIterator<RecordType const>> clone() const override;

        std::unique_ptr<IIterator<RecordType const>> begin() const override;

        std::unique_ptr<IIterator<RecordType const>> end() const override;

        bool equals(IIterator<RecordType const> const &other) const override;

    };

}

#endif //MDFSIMPLECONVERTERS_EMPTYITERATOR_H
