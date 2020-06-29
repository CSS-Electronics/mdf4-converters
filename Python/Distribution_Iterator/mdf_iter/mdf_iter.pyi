import io

from pathlib import Path
from typing import Dict, Iterator, List, Type, TypedDict, Union

from mdf_iter.IFileInterface import IFileInterface


class CANRecord(object):
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


class CANIterator(object):
    """Iterator over CAN records.
    
    """
    def __iter__(self) -> Iterator[CANRecord]: ...
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
    def __init__(self, data_source: Union[str, Path, io.BytesIO, IFileInterface], cache_size: int = 4096):
        """Constructor. Requires a callback to read data through.
        
        :param data_source:     Object to support data access through read and seek calls.
        :param cache_size:      Specify how large the internal buffer is. Larger values reduces the number of read
                                calls.
        :raises ValueError:     Invalid input arguments, more detail in the error message.
        :raises RuntimeError:   Error from the C++ library, more detail in the error message.
        """
        ...
    
    def get_can_iterator(self) -> CANIterator:
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
