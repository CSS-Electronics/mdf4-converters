from abc import ABCMeta, abstractmethod


class IFileInterface(object, metaclass=ABCMeta):
    """Generic interface for binding an arbitrary data source to the library.
    
    """
    def __init__(self, fl):
        self._handle = fl
    
    @abstractmethod
    def read(self, buffer: bytearray, number_of_bytes: int) -> int:
        """Method called by the library when additional data is required from the current position.
        
        :param buffer:              Object supporting the buffer protocol to write the read data into.
        :param number_of_bytes:     Number of bytes requested by the library.
        :return:                    Number of bytes written to the buffer.
        """
        raise NotImplementedError("Interface method not implemented")

    def seek(self, offset: int, direction: int = 0) -> int:
        """Method called by the library when the current position in the data source needs to be moved.
        
        :param offset:              Offset to seek.
        :param direction:           Base to calculate the offset from. (0 = beginning, 1 = current, 2 = end)
        :return:                    Absolute position from the beginning of the data source.
        """
        raise NotImplementedError("Interface method not implemented")

    def readable(self) -> bool:
        return True

    def seekable(self) -> bool:
        return True

    pass
