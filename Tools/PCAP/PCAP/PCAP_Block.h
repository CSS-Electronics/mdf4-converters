#ifndef TOOLS_PCAP_PCAP_BLOCK_H
#define TOOLS_PCAP_PCAP_BLOCK_H

#include <cstdint>
#include <ostream>

namespace tools::pcap {

    class PCAP_Block {
    public:
        friend std::ostream& operator<<(std::ostream& stream, PCAP_Block const& data);
    protected:
        explicit PCAP_Block(uint32_t blockType);
        uint32_t blockLength;

        virtual void serialize(std::ostream& stream) const = 0;
    private:
        uint32_t const blockType;

        void writeFooter(std::ostream& stream) const;
        void writeHeader(std::ostream& stream) const;
    };



}

#endif //TOOLS_PCAP_PCAP_BLOCK_H
