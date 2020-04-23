#include "ExecutableEntryPoint.h"
#include "MDFTool.h"

using namespace mdf::tools::finalizer;

int main(int argc, char* argv[]) {
    std::unique_ptr<MDFTool> exporter(new MDFTool());
    return programEntry(argc, argv, std::move(exporter));
}
