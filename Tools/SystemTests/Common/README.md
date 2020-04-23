# How to configure a test
Tests can be grouped in json files for a common subject. Each test file starts with a tool and a series of test cases related to that tool. By default, each test will be performed in a new temporary directory `tmp`. Here, all required binaries are copied into `tmp/bin`, while data files are copied into `tmp/data`.

Each test case has several sections:
* `input` specifies input files and has the following properties:
  * `name` - name of the input file to collect from the `Common/TestData` folder. Mandatory.
  * `relative_path` - where to place the file in relation to the data folder (NOTE: While the temporary folder is auto cleaned, it is possible to escape the confines with the relative path. These files will *NOT* be cleaned up upon test completion). Defaults to `.`.
  * `auto_include` - Should the file be appended as an argument without any parameters at the end of the command line? Defaults to true. Set to false if manual control via the `options` field later is desired.
* `output` specifies expected generated output files after execution of the tool, and has the following properties:
  * `name` - name of the generated output file from the tool. Mandatory.
  * `comparision_data` - name of the file to compare with. Searching starts in the the `TestData` folder for the tool, and ends in the `Common/TestData` folder. If left blank, it will be the same as the `name` value.
  * `relative_path` - where to expect the output file in relation to the data directory. Defaults to `.`.
* `options` allows for manual control of the commandline options passed to the tool during execution as key-value pairs. If manual control of input files is desired, simply write them as keys with empty values.
* `options_uses_relative` controls the formatting of paths entered to some arguments in options. By default the paths will be converted to absolute paths, but by settings this value to true, they will be set to relative values compared to the tool binary directory. Defaults to false.
* `return_code` signifies which return code to expect from the tool execution. Defaults to 0.
* `additional_files` is a list of additional files required for the test, with the following properties:
  * `input_name` - the name of the file to search for in the tools `Files` folder, and later on in the `Common/Files` folder.
  * `name` - name of the file in the temporary test folder.
  * `relative_path` - where to expect the output file in relation to the data directory. Defaults to `.`.
  * `relative_to_data` - Whether to use the data or the binary folder as the relative base target. Defaults to data.
* `name` allows for manual naming of the tests when run by py.test. If left blank, tests will be sequentially numbered.
* `tool` allows for manual control of the tool field if multiple tools are to be merged into a single json file, not recommended to utilize in configuration. Will be set to the file tool upon runtime.

Any unknown term should be ignored. This can be utilized to write comments on test cases, e.g.
```
"_comment" : "This is a comment on a test"
```

These configuration files are loaded via the `conftest.py` file in the `Common` folder, and requires the test class to inherit from `CommonBase` and fill in the `parameter_files` class attribute.

```
class TestClass(CommonBase):
  parameter_files = [
    path_to_json_file_1,
    path_to_json_file_2,
    ...
  ]
```

Recognized tools are mapped via the `Tool` enum in `Common/Tool.py`. Each tool gets a unique marker, such that only tests relating to a specific tool can be selected using the marker. E.g. for CSV tests only:
```
pytest -m CSV
```
Markers are defined and documented in the `pytest.ini` file.

Additional options include `--keep`, which leaves the temporary directory intact after test execution, if the test failed.
