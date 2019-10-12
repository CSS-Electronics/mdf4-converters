#include "PCAP_OptionList.h"

#include "PCAP_OptionEnd.h"

namespace tools::pcap {

    void PCAP_OptionList::addOption(PCAP_Option const& option) {
        // Allocate a copy on the heap.
        options.emplace_back(std::shared_ptr<PCAP_Option>(option.clone()));
    }

    void PCAP_OptionList::addOption(std::shared_ptr<PCAP_Option> const& option) {
        options.emplace_back(option);
    }

    uint32_t PCAP_OptionList::getSize() const {
        uint32_t result = 0;

        // Loop over each element, determining the size.
        for(auto const& option: options) {
            result += option->getSize();

            // Ensure the options is 32 bit aligned.
            if(result % 4 != 0) {
                auto mismatch = 4 - result % 4;
                result += mismatch;
            }
        }

        // Add size of the tail option.
        result += 4;

        return result;
    }

    std::ostream& operator<<(std::ostream& stream, PCAP_OptionList const& data) {
        // Write all the options.
        for(auto const& option: data.options) {
            stream << *option;
        }

        // Write the terminating option.
        stream << PCAP_OptionEnd();

        return stream;
    }

}
