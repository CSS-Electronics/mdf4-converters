#ifndef MDFSIMPLECONVERTERS_RECORDITERATOR_H
#define MDFSIMPLECONVERTERS_RECORDITERATOR_H

#include <map>

#include <boost/function.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include "IIterator.h"

namespace mdf {

    template<typename RecordType>
    struct RecordIterator
        : public boost::iterator_facade<RecordIterator<RecordType>, RecordType const, boost::forward_traversal_tag, RecordType const &, std::size_t> {
        RecordIterator();

        explicit RecordIterator(std::unique_ptr<IIterator<RecordType>> iterator);

        RecordIterator(RecordIterator<RecordType> const &other);

        RecordIterator &operator=(RecordIterator<RecordType>&& other) noexcept;

        RecordIterator begin() const;

        RecordIterator cbegin() const;

        RecordIterator end() const;

        RecordIterator cend() const;

    protected:
        friend class boost::iterator_core_access;

        std::unique_ptr<IIterator<RecordType>> iterator;

        RecordType const &dereference() const;

        void increment();

        bool equal(RecordIterator<RecordType> const &other) const;
    };

}

#endif //MDFSIMPLECONVERTERS_RECORDITERATOR_H
