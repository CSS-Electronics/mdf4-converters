#ifndef TOOLS_SHARED_EXECUTABLEENTRYPOINT_H
#define TOOLS_SHARED_EXECUTABLEENTRYPOINT_H

#include "ConverterInterface.h"

namespace tools::shared {

    int programEntry(int argc, char** argv, std::unique_ptr<ConverterInterface> exporter);

}

#endif //TOOLS_SHARED_EXECUTABLEENTRYPOINT_H
