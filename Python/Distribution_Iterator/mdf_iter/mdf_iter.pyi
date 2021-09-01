import io

from pathlib import Path
from typing import Dict, Iterator, List, Optional, Type, TypedDict, Union

from mdf_iter.IFileInterface import IFileInterface


class CAN_DataFrame(object):
    """Simple object with all CAN data as public attributes.
    
    """
    TimeStamp: float
    BusChannel: int
    ID: int
    IDE: bool
    DLC: int
    DataLength: int
    Dir: bool
    EDL: bool
    BRS: bool
    DataBytes: List[int]
    ...


class CAN_ErrorFrame(object):
    """Simple object with all CAN error frame data as public attributes.

    """
    TimeStamp: float
    BusChannel: int
    ErrorType: int
    ...


class CAN_RemoteFrame(object):
    """Simple object with all CAN remote frame data as public attributes.

    """
    TimeStamp: float
    BusChannel: int
    ID: int
    IDE: bool
    DLC: int
    DataLength: int
    Dir: bool
    DataBytes: List[int]
    ...


class CAN_DataFrameIterator(object):
    """Iterator over CAN records.
    
    """
    def __iter__(self) -> Iterator[CAN_DataFrame]: ...
    ...


class CAN_ErrorFrameIterator(object):
    """Iterator over CAN error records.

    """
    def __iter__(self) -> Iterator[CAN_ErrorFrame]: ...
    ...


class CAN_RemoteFrameIterator(object):
    """Iterator over CAN remote request records.

    """
    def __iter__(self) -> Iterator[CAN_RemoteFrame]: ...
    ...


class LIN_ChecksumError(object):
    """Simple object with all LIN checksum data as public attributes.

    """
    TimeStamp: float
    BusChannel: int
    ID: int
    DataLength: int
    Dir: bool
    Checksum: int
    ReceivedDataByteCount: int
    DataBytes: List[int]
    ...


class LIN_Frame(object):
    """Simple object with all LIN data as public attributes.

    """
    TimeStamp: float
    BusChannel: int
    ID: int
    DataLength: int
    Dir: bool
    ReceivedDataByteCount: int
    DataBytes: List[int]
    ...

class LIN_ReceiveError(object):
    """Simple object with all LIN receive error data as public attributes.
    
    """
    TimeStamp: float
    BusChannel: int
    ID: int


class LIN_SyncError(object):
    """Simple object with all LIN sync error data as public attributes.
    
    """
    TimeStamp: float
    BusChannel: int
    Baudrate: int


class LIN_TransmissionError(object):
    """Simple object with all LIN transmission error data as public attributes.
    
    """
    TimeStamp: float
    BusChannel: int
    ID: int



class LIN_ChecksumErrorIterator(object):
    """Iterator over LIN checksum error records.
    
    """
    def __iter__(self) -> Iterator[LIN_ChecksumError]: ...
    ...


class LIN_FrameIterator(object):
    """Iterator over LIN frames records.

    """
    def __iter__(self) -> Iterator[LIN_Frame]: ...
    ...


class LIN_ReceiveErrorIterator(object):
    """Iterator over LIN receive error records.

    """
    def __iter__(self) -> Iterator[LIN_ReceiveError]: ...
    ...


class LIN_SyncErrorIterator(object):
    """Iterator over LIN sync error records.

    """
    def __iter__(self) -> Iterator[LIN_SyncError]: ...
    ...


class LIN_TransmissionErrorIterator(object):
    """Iterator over LIN transmission error records.

    """
    def __iter__(self) -> Iterator[LIN_TransmissionError]: ...
    ...


class MdfMetadataEntry(TypedDict):
    description: str
    name: str
    read_only: bool
    unit: str
    value_raw: str
    value_type: str


class MdfFile(object):
    """MDF file loaded by the C++ library.
    
    """
    def __init__(self, data_source: Union[str, Path, io.BytesIO, IFileInterface], passwords: Optional[Dict[str, str]] = None, cache_size: int = 4096):
        """Constructor. Requires a callback to read data through.
        
        :param data_source:     Object to support data access through read and seek calls.
        :param passwords:       Dictionary with each device specific password, and/or a "DEFAULT" entry.
        :param cache_size:      Specify how large the internal buffer is. Larger values reduces the number of read
                                calls.
        :raises ValueError:     Invalid input arguments, more detail in the error message.
        :raises RuntimeError:   Error from the C++ library, more detail in the error message.
        """
        ...
    
    def get_can_iterator(self) -> CAN_DataFrameIterator:
        """Get a corresponding CAN record iterator from the C++ library.
        
        :return: CAN iterator.
        """
        ...

    def get_can_error_iterator(self) -> CAN_ErrorFrameIterator:
        """Get a corresponding CAN record iterator from the C++ library.

        :return: CAN iterator.
        """
        ...

    def get_can_remote_iterator(self) -> CAN_RemoteFrameIterator:
        """Get a corresponding CAN record iterator from the C++ library.

        :return: CAN iterator.
        """
        ...
    
    def get_lin_checksum_iterator(self) -> LIN_ChecksumErrorIterator:
        """Get a corresponding CAN record iterator from the C++ library.

        :return: CAN iterator.
        """
        ...
    
    def get_lin_iterator(self) -> LIN_FrameIterator:
        """Get a corresponding CAN record iterator from the C++ library.

        :return: CAN iterator.
        """
        ...
    
    def get_lin_receive_iterator(self) -> LIN_ReceiveErrorIterator:
        """Get a corresponding CAN record iterator from the C++ library.

        :return: CAN iterator.
        """
        ...
    
    def get_lin_sync_iterator(self) -> LIN_SyncErrorIterator:
        """Get a corresponding CAN record iterator from the C++ library.

        :return: CAN iterator.
        """
        ...
    
    def get_lin_transmission_iterator(self) -> LIN_TransmissionErrorIterator:
        """Get a corresponding CAN record iterator from the C++ library.

        :return: CAN iterator.
        """
        ...
    
    def get_data_frame(self) -> Type["pandas.DataFrame"]:
        """If all required dependencies are detected, allows the export of the entire file as a pandas DataFrame.
        
        :return: pandas DataFrame with all CAN data.
        """
        ...
    
    def get_first_measurement(self) -> int:
        """Get the epoch of the first CAN measurement in the file.
        
        :return: Timestamp in nanoseconds since epoch.
        """
        ...

    def get_metadata(self) -> Dict[MdfMetadataEntry]:
        ...

    ...
