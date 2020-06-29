#include "MDFBusLoggingLibrary.h"
#include "Version.h"

namespace mdf {

    constexpr semver::version Version {
        MDF_BusLogging_Library_VERSION_MAJOR,
        MDF_BusLogging_Library_VERSION_MINOR,
        MDF_BusLogging_Library_VERSION_PATCH
    };

    semver::version const& getVersion() {
        return Version;
    }

}