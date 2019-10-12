#include "ExecutableEntryPoint.h"
#include "CLX000_Exporter.h"

using namespace tools::clx;

int main(int argc, char* argv[]) {
    std::unique_ptr<CLX000_Exporter> exporter(new CLX000_Exporter());
    return programEntry(argc, argv, std::move(exporter));
}
