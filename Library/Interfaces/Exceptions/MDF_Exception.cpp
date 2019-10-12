#include "MDF_Exception.h"

namespace mdf {

    MDF_Exception::MDF_Exception(std::string const& what) : std::runtime_error(what) {
        //
    }

}
