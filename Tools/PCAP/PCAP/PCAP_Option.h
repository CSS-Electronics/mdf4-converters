#ifndef TOOLS_PCAP_PCAP_OPTION_H
#define TOOLS_PCAP_PCAP_OPTION_H

#include <cstdint>
#include <ostream>

namespace tools::pcap {

    class PCAP_Option {
    public:
        explicit PCAP_Option(uint16_t optionCode);
        virtual ~PCAP_Option();

        [[nodiscard]] virtual PCAP_Option* clone() const = 0;
        [[nodiscard]] virtual uint32_t getSize() const;

        friend std::ostream& operator<<(std::ostream& stream, PCAP_Option const& data);
    protected:
        [[nodiscard]] uint16_t getOptionCode() const;
        virtual void serialize(std::ostream& stream) const = 0;
    private:
        uint16_t const optionCode;
        uint16_t optionLength;
    };

}

#endif //TOOLS_PCAP_PCAP_OPTION_H
