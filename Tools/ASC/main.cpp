#include "ExecutableEntryPoint.h"
#include "ASC_Exporter.h"

using namespace tools::asc;

int main(int argc, char* argv[]) {
    std::unique_ptr<ASC_Exporter> exporter(new ASC_Exporter());
    return programEntry(argc, argv, std::move(exporter));
}
