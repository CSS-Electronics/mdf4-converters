#ifndef MDFSORTER_MDF_EXCEPTION_H
#define MDFSORTER_MDF_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace mdf {

    struct MDF_Exception : public std::runtime_error {
        explicit MDF_Exception(std::string const& what);
    };

}

#endif //MDFSORTER_MDF_EXCEPTION_H
