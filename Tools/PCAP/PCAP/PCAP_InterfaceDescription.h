#ifndef TOOLS_PCAP_PCAP_INTERFACEDESCRIPTION_H
#define TOOLS_PCAP_PCAP_INTERFACEDESCRIPTION_H

#include <memory>

#include "PCAP_Block.h"
#include "PCAP_OptionList.h"
#include "PCAP_OptionString.h"
#include "PCAP_OptionU64.h"

namespace tools::pcap {

    class PCAP_InterfaceDescription : public PCAP_Block {
    public:
        PCAP_InterfaceDescription();

        void setDescription(std::string description);
        void setName(std::string name);
        void setSpeed(uint64_t bitRate);

        uint16_t linkType;
        uint32_t snapLen;
    private:
        PCAP_OptionList options;

        std::shared_ptr<PCAP_OptionU64> optionBitRate;
        std::shared_ptr<PCAP_OptionString> optionDescription;
        std::shared_ptr<PCAP_OptionString> optionName;

        void serialize(std::ostream& stream) const override;
        void updateSize();
    };

}

#endif //TOOLS_PCAP_PCAP_INTERFACEDESCRIPTION_H
