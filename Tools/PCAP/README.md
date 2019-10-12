# PCAP-NF Exporter
Exports data to the [PCAP-NG format](https://github.com/pcapng/pcapng), which can be read by
[Wireshark](https://www.wireshark.org/). CAN data is represented via the [Linux SLL](https://www.tcpdump.org/linktypes.html)
type (Link Type 113, Protocol IDs 0x000C and 0x000D). While a Link Type exists for LIN (212), the format the data has to
be packed in is unclear. Thus, LIN is not properly supported yet.

See [libpcap](https://github.com/the-tcpdump-group/libpcap/blob/ac945a40156ecbbd759528bb147b800cf9dd37dd/pcap-common.c#L620)
for the link types.
