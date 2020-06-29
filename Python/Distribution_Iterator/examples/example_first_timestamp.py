import fsspec
import mdf_iter

from datetime import datetime, timezone


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


def example_first_timestamp():
    """Simple example which get the timestamp of the first measurement in the log file.
    """
    # Get a handle to the remote file.
    fs = setup_fs()
    log_file = r"LOG/EEEE0001/00000001/00000001.MF4"
    
    with fs.open(log_file, "rb") as handle:
        # Load the file.
        mdf_file = mdf_iter.MdfFile(handle)
        
        # Extract the first timestamp in nanoseconds.
        timestamp_raw = mdf_file.get_first_measurement()
        
        # Convert to datetime and set the timezone to UTC.
        timestamp = datetime.utcfromtimestamp(timestamp_raw * 1E-9)
        timestamp = timestamp.replace(tzinfo=timezone.utc)
        
        print("First measurement at {}".format(timestamp))
        
    return


if __name__ == '__main__':
    example_first_timestamp()
    pass
