# Shared base for MDF tools
This is a shared base, allowing for a common set of options, in order to quickly generate converters to different
formats. All tools follow a common structure:
* An implementation of the `ConverterInterface` interface/abstract class.
* A main file, redirecting to the entrypoint in the `Shared` folder.

## Create a new converter/exporter.
To create a new exporter, do the following:

1) Create a new folder for the exporter.
2) Copy CMakeLists, exporter class and main.
3) Edit CMakeLists to match the new names.
4) Change class in the copied main.
5) Compile.

## Commandline arguments.
A set of common options are included, which all sub-tools inherit.
* `-h` or `--help`. Display help message.
* `-v` or `--version`. Display version information.
* `--non-interactive`. Flag controlling the display of progress during the lifetime of the application. Default value
is `false`. Set to `true` when using the program in scripts, to avoid a log line for each progress update.
* `-i` or `--input-files`. Specify a list of files to convert. All unknown arguments will be interpreted as an input
file, allowing for the use of the program without specifying any options. 
* `-I` or `--input-directory`. Specify a directory to read input files from. All input files specified via the `-i`
flag, directly or indirectly, will be ignored.
* `-O` or `--output-directory`. Specify a directory to place the output files. If not specified, the same directory as
the corresponding input file will be used.
* `-t` or `--timezone`. Determine which format to output the log messages in when converting. May not be applicable
for all output formats. The possible values are: Local time (`p`), logger local time (`l`) or UTC (`u`).
* `--verbosity`. Alters how much information is shown. For normal operation, the default value of `1` is sufficient.
Values range from `0` (Fatal errors only) to `5` (Debug information).

## Return codes.
If no errors occurred during the run, the program will return `0`.
* `0`: No errors.
* `1`: Unrecognized input parameter.
* `2`: Failure to read one or more input files, see error output for more information.
* `-1`: Fatal error (Terminating the program immediately), see output for more info.

Error codes above `2^16 = 65536` originate from the sub-tool.

## Verbosity.
The 6 verbosity levels are defined as the following, where each level contains all messages from the level above.
* `0`: Fatal errors only.
* `1`: Error messages.
* `2`: Warnings.
* `3`: Info messages.
* `4`: Debug messages.
* `5`: Trace messages.

For normal usage, a level of `1` corresponding to error messages is sufficient. This is also specified as the default
value.
