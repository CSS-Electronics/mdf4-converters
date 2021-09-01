## Commandline arguments.
A set of common options are included, which all sub-tools inherit.
* `-h` or `--help`. Display help message
* `-v` or `--version`. Display version information
* `--non-interactive`. Flag controlling the display of progress during the lifetime of the application. Default value is `false`. Set to `true` when using the program in scripts, to avoid a log line for each progress update
* `-i` or `--input-files`. Specify a list of files to convert. All unknown arguments will be interpreted as an input file, allowing for the use of the program without specifying any options. If the patn to a folder is passed, the folder will be traversed recursively, looking for supported file extensions (".MF4", ".MFE", ".MFC" and ".MFM")
* `-O` or `--output-directory`. Specify a directory to place the output files. If not specified, the same directory as the corresponding input file will be used
* `-d` or `--delete-converted`. Delete files after conversion, defaults to false
* `-p` or `--password-file`. Path to password json file to use for decryption. If not specified, the same directory as the executable will be used, looking for `passwords.json`. See further down for the format of the password file
* `-t` or `--timezone`. Determine which format to output the log messages in when converting. The possible values are:
Local time (`p`), logger local time (`l`) or UTC (`u`). May not be applicable for all output formats, e.g. CSV, PCAP and the finalizer.
* `--verbosity`. Alters how much information is shown. For normal operation, the default value of `1` is sufficient. Values range from `0` (Fatal errors only) to `5` (Debug information)

If the converter allows for options from a config file, an additional flag is available:
* `-e` or `--error-on-missing-config-file`. If no config file is found, the program will error out instead of utilizing default values.

## Exit codes.
See `Exit codes.md`

If the converter is started using drag-and-drop and an error occurs, the output window will stay visible until the user closes it by pressing the `Enter` key.

## Password file format.
The password file is formatted in JSON. The file may contain a default key, labelled `default`, and device specific keys, with the ID of the corresponding logger as the key. An example with both a default and a device specific key could be:
```
{
  "default": "thedefaultpasswordforall",
  "25674460": "devicespecificpasswordex"
}
```
The device specific key is used if present. In case of a MAC error on decryption, the default key is NOT used as a fallback key.

## Verbosity.
The 6 verbosity levels are defined as the following, where each level contains all messages from the level above.
* `0`: Fatal errors only.
* `1`: Error messages.
* `2`: Warnings.
* `3`: Info messages.
* `4`: Debug messages.
* `5`: Trace messages.

For normal usage, a level of `1` corresponding to error messages is sufficient. This is also specified as the default value.
