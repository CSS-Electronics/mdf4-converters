#ifndef MDFSORTER_CAN_DATAFRAME_ITERATOR_H
#define MDFSORTER_CAN_DATAFRAME_ITERATOR_H

#include <functional>
#include <iterator>
#include <memory>

#include "CAN_DataFrame.h"
#include "CAN_DataFrame_Mapping.h"
#include "GenericDataRecord.h"
#include "MDF_DataSource.h"

namespace mdf {

    struct CAN_DataFrame_Iterator {
        friend class MDF_DataSource;
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::shared_ptr<CAN_DataFrame_t>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::shared_ptr<CAN_DataFrame_t>*;
        using reference = std::shared_ptr<CAN_DataFrame_t>&;

        CAN_DataFrame_Iterator();
        CAN_DataFrame_Iterator(std::shared_ptr<MDF_DataSource> dataSource, CANMappingTable mapping, uint64_t startTime = 0);

        bool operator==(CAN_DataFrame_Iterator const& other) const;
        bool operator!=(CAN_DataFrame_Iterator const& other) const;
        std::shared_ptr<CAN_DataFrame_t> operator*() const;
        CAN_DataFrame_Iterator& operator++();
        CAN_DataFrame_Iterator operator++(int);
    private:
        void getNextRecord();

        std::shared_ptr<CAN_DataFrame_t> value;
        std::size_t index;
        std::shared_ptr<MDF_DataSource> dataSource;
        CANMappingTable mapping;
        uint64_t startTime;
    };

}

#endif //MDFSORTER_CAN_DATAFRAME_ITERATOR_H
