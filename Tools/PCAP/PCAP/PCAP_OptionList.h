#ifndef TOOLS_PCAP_PCAP_OPTIONLIST_H
#define TOOLS_PCAP_PCAP_OPTIONLIST_H

#include <memory>
#include <vector>

#include "PCAP_Option.h"

namespace tools::pcap {

    class PCAP_OptionList {
    public:
        void addOption(PCAP_Option const& option);
        void addOption(std::shared_ptr<PCAP_Option> const& option);

        [[nodiscard]] uint32_t getSize() const;

        friend std::ostream& operator<<(std::ostream& stream, PCAP_OptionList const& data);
    private:
        std::vector<std::shared_ptr<PCAP_Option>> options;
    };

}

#endif //TOOLS_PCAP_PCAP_OPTIONLIST_H
