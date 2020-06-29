import fsspec
import mdf_iter


def setup_fs() -> fsspec:
    """Helper function to setup the file system for the examples.
    """
    import s3fs
    
    fs = s3fs.S3FileSystem(
        key="<key>",
        secret="<secret>",
        client_kwargs={
            "endpoint_url": "http://address.of.remote.s3.server:9000",
        },
    )
	
    return fs


def example_iterator():
    """Simple example which extracts all CAN records in a pandas DataFrame.
    """
    # Get a handle to the remote file.
    fs = setup_fs()
    log_file = r"LOG/EEEE0001/00000001/00000001.MF4"
    
    with fs.open(log_file, "rb") as handle:
        # Load the file.
        mdf_file = mdf_iter.MdfFile(handle)
        
        # Extract a dataframe with all the CAN measurements.
        result = mdf_file.get_data_frame()
        
        print(result)
        
    return


if __name__ == '__main__':
    example_iterator()
    pass
