#ifndef MDFSORTER_MDF_NOTIMPLEMENTEDERROR_H
#define MDFSORTER_MDF_NOTIMPLEMENTEDERROR_H

#include "MDF_Exception.h"

namespace mdf {

    struct MDF_NotImplementedError : public MDF_Exception {
        explicit MDF_NotImplementedError(std::string const& what);
    };

}

#endif //MDFSORTER_MDF_NOTIMPLEMENTEDERROR_H
