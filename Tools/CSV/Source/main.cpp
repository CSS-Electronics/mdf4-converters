#include "ExecutableEntryPoint.h"
#include "CSV_Exporter.h"

using namespace mdf::tools::csv;

int main(int argc, char* argv[]) {
    std::unique_ptr<CSV_Exporter> exporter(new CSV_Exporter());
    return programEntry(argc, argv, std::move(exporter));
}
