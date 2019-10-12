#include "ExecutableEntryPoint.h"
#include "ExecutableInterface.h"

namespace tools::shared {

    int programEntry(int argc, char** argv, std::unique_ptr<ConverterInterface> interface) {
        // Create an exporter and start the main loop.
        ExecutableInterface e(std::move(interface));

        return e.main(argc, argv);
    }

}
