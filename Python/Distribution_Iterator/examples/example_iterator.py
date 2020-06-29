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
    """Simple example which iterates over the CAN records of a file stored on a remote filesystem.
    """
    # Get a handle to the remote file.
    fs = setup_fs()
    log_file = r"LOG/EEEE0001/00000001/00000001.MF4"
    
    with fs.open(log_file, "rb") as handle:
        # Load the file.
        mdf_file = mdf_iter.MdfFile(handle)
        
        # Extract an iterator over the CAN records.
        record_iterator = mdf_file.get_can_iterator()
        
        # Iterate over the records.
        ctr = 0
        for record in record_iterator:
            print(record)
            ctr += 1
        
        print("There are {} records in total".format(ctr))
        
    return


if __name__ == '__main__':
    example_iterator()
    pass
