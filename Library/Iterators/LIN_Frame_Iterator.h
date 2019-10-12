#ifndef MDFSORTER_LIN_FRAME_ITERATOR_H
#define MDFSORTER_LIN_FRAME_ITERATOR_H

#include <functional>
#include <iterator>
#include <memory>

#include "LIN_Frame.h"
#include "LIN_Frame_Mapping.h"
#include "GenericDataRecord.h"
#include "MDF_DataSource.h"

namespace mdf {

    struct LIN_Frame_Iterator {
        friend class MDF_DataSource;
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::shared_ptr<LIN_Frame_t>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::shared_ptr<LIN_Frame_t>*;
        using reference = std::shared_ptr<LIN_Frame_t>&;

        LIN_Frame_Iterator();
        LIN_Frame_Iterator(std::shared_ptr<MDF_DataSource> dataSource, LINMappingTable mapping, uint64_t startTime = 0);

        bool operator==(LIN_Frame_Iterator const& other) const;
        bool operator!=(LIN_Frame_Iterator const& other) const;
        std::shared_ptr<LIN_Frame_t> operator*() const;
        LIN_Frame_Iterator& operator++();
        LIN_Frame_Iterator const operator++(int);
    private:
        void getNextRecord();

        std::shared_ptr<LIN_Frame_t> value;
        std::size_t index;
        std::shared_ptr<MDF_DataSource> dataSource;
        LINMappingTable mapping;
        uint64_t startTime;
    };

}

#endif //MDFSORTER_LIN_FRAME_ITERATOR_H
