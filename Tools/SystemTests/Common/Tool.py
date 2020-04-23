from enum import Enum


class Tool(Enum):
    Unknown = None
    ASC = "mdf2asc"
    CLX000 = "mdf2clx000"
    CSV = "mdf2csv"
    PCAP = "mdf2pcap"
    PEAK = "mdf2peak"
    SocketCAN = "mdf2socketcan"
    
    def __str__(self):
        return self.name

    pass
