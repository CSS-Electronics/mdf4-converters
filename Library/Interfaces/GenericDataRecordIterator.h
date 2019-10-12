#ifndef MDFSORTER_GENERICDATARECORDITERATOR_H
#define MDFSORTER_GENERICDATARECORDITERATOR_H

#include <functional>
#include <iterator>
#include <memory>

#include "GenericDataRecord.h"
#include "MDF_DataSource.h"

namespace mdf {

    struct GenericDataRecordIterator {
        friend class MDF_DataSource;
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::shared_ptr<GenericDataRecord>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::shared_ptr<GenericDataRecord>*;
        using reference = std::shared_ptr<GenericDataRecord>&;

        explicit GenericDataRecordIterator(MDF_DataSource& dataSource, bool end = false);

        bool operator==(GenericDataRecordIterator const& other) const;
        bool operator!=(GenericDataRecordIterator const& other) const;
        std::shared_ptr<GenericDataRecord> operator*() const;
        GenericDataRecordIterator& operator++();
        GenericDataRecordIterator operator++(int);
    private:
        std::shared_ptr<GenericDataRecord> value;
        std::size_t index;
        MDF_DataSource& dataSource;
    };

}

#endif //MDFSORTER_GENERICDATARECORDITERATOR_H
