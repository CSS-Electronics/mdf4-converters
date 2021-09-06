from mdf_iter.IFileInterface import IFileInterface


class FileInterface(IFileInterface):
    """Implementation of the IFileInterface for use with file-like objects.
    
    """
    def __init__(self, fl):
        """Setup the interface with a file-like object.
        
        :param fl: File-like object. Assumed to be open and valid.
        """
        super(IFileInterface, self).__init__()
        self._handle = fl

    def read(self, buffer: bytearray, number_of_bytes: int) -> int:
        res = self._handle.read(number_of_bytes)
        buffer[:len(res)] = res

        return len(res)

    def seek(self, offset: int, direction: int = 0) -> int:
        abs_pos = self._handle.seek(offset, direction)

        return abs_pos
    
    pass
