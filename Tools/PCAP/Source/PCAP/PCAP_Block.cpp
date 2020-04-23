#include "PCAP_Block.h"

namespace mdf::tools::pcap {

    PCAP_Block::PCAP_Block(uint32_t blockType) : blockType(blockType), blockLength(0) {

    }

    void PCAP_Block::writeFooter(std::ostream &stream) const {
        // Write block length in native endian format.
        stream.write(reinterpret_cast<char const*>(&blockLength), sizeof(blockLength));
    }

    void PCAP_Block::writeHeader(std::ostream &stream) const {
        // Write block type and length in native endian format.
        stream.write(reinterpret_cast<char const*>(&blockType), sizeof(blockType));
        stream.write(reinterpret_cast<char const*>(&blockLength), sizeof(blockLength));
    }

    std::ostream& operator<<(std::ostream& stream, PCAP_Block const& data) {
        // Write the header.
        data.writeHeader(stream);
        stream.flush();

        // Let the block handle the serialization.
        data.serialize(stream);
        stream.flush();

        // Write the footer.
        data.writeFooter(stream);
        stream.flush();

        return stream;
    }

}
