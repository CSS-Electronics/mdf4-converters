#include "ExecutableEntryPoint.h"
#include "PEAK_Exporter.h"

using namespace mdf::tools::peak;

int main(int argc, char* argv[]) {
    std::unique_ptr<PEAK_Exporter> exporter(new PEAK_Exporter());
    return programEntry(argc, argv, std::move(exporter));
}
