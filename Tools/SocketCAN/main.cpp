#include "ExecutableEntryPoint.h"
#include "SocketCAN_Exporter.h"

using namespace tools::socketcan;

int main(int argc, char* argv[]) {
    std::unique_ptr<SocketCAN_Exporter> exporter(new SocketCAN_Exporter());
    return programEntry(argc, argv, std::move(exporter));
}
