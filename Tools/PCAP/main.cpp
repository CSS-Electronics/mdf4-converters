#include "ExecutableEntryPoint.h"
#include "PCAP_Exporter.h"

using namespace tools::pcap;

int main(int argc, char* argv[]) {
    std::unique_ptr<PCAP_Exporter> exporter(new PCAP_Exporter());
    return programEntry(argc, argv, std::move(exporter));
}

/**
 * NOTE: See https://github.com/the-tcpdump-group/libpcap/blob/master/pcap-common.c#L620
 *       #define LINKTYPE_LIN		212
 */
