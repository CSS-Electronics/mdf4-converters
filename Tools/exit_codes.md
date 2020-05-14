# Exit codes
The tools return a single byte with information about the last invocation. The interpretation is split into two cases depending on the presence of critical errors. If a critical error occurs, the exit code will be 128 or greater. If a non-critical error occurs, the exit code will be between 0 and 128. If no errors occur, the exit code will be 0.

### No critical error
Non-critical errors include errors occurring during the processing of an input file, where the program can recover and continue with the next file. Several of these can be returned as once as they are encoded as bit flags in the lower 7 bits.

| Flag (Hex) | Flag (Decimal) | Name               | Description                                           |
|:----------:|:--------------:| ------------------ | ----------------------------------------------------- |
|       0x00 |              0 | NoErrors           | No detected errors during execution                   |
|       0x01 |              1 | MissingFile        | One or more input files could not be found            |
|       0x02 |              2 | DecryptionError    | Error during decryption of one or more input files    |
|       0x04 |              4 | DecompressionError | Error during decompression of one or more input files |
|       0x08 |              8 | DecodingError      | Error during MDF decoding of one or more input files  |

All error states can be active at the same time, resulting in a return code of `0x0F` or `15`. A return code of `5` would indicate that flags `1`(MissingFile) and `4`(DecompressionError) are active, such that one or more files could not be found, as well as an error during decompression of one or more files.

Note that an error during decompression may sometimes manifest as an error during decoding instead. This is caused by a corrupt compressed file, which is then decompressed into a corrupt regular file. Only during the MDF parsing of the regular file is the corruption detected.

### Critical error
In case of a critical error, the most significant bit of the byte is set, and the remaining 7 bits are used to transfer the error code. The values are tabulated below.

| Value (Hex) | Value (Decimal) | Name                        | Description                                         |
|:-----------:|:---------------:|:---------------------------:| --------------------------------------------------- |
|        0x81 |             129 | CriticalError               | Critical error during program execution             |
|        0x82 |             130 | MissingArgument             | Missing argument to a specified input parameter     |
|        0x83 |             131 | InvalidInputArgument        | Invalid argument to an input parameter              |
|        0x84 |             132 | InputArgumentParsingError   | Error during parsing of an input parameter          |
|        0x85 |             133 | ConfigurationFileParseError | Error during parsing of supplied configuration file |
|        0x86 |             134 | UnrecognizedOption          | Unrecognized option passed to the program           |

### Full non-critical error table
The table below represents all possible combinations of non-critical error flags and a short description.

| Value (Decimal) |  Description                                                                                                                                                                                                                       |
|:---------------:| ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|               0 | No detected errors during execution                                                                                                                                                                                                |
|               1 | One or more input files could not be found                                                                                                                                                                                         |
|               2 | Error during decryption of one or more input files                                                                                                                                                                                 |
|               3 | One or more input files could not be found, and an error occurred during decryption of one or more files                                                                                                                           |
|               4 | Error during decompression of one or more input files                                                                                                                                                                              |
|               5 | One or more input files could not be found, and an error occurred during decompression of one or more files                                                                                                                        |
|               6 | An error occurred during decryption of one or more files, and another error occurred during decompression of one or more files                                                                                                     |
|               7 | One or more input files could not be found, an error occurred during decryption of one or more files, and another error occurred during decompression of one or more files                                                         |
|               8 | Error during MDF decoding of one or more input files                                                                                                                                                                               |
|               9 | One or more input files could not be found, and an occurred during MDF decoding of one or more files                                                                                                                               |
|              10 | An error occurred during decryption of one or more files, and another error occurred during MDF decoding of one or more files                                                                                                      |
|              11 | One or more input files could not be found, an error occurred during decryption of one or more files, and another error occurred during MDF decoding of one or more files                                                          |
|              12 | An error occurred during decompression of one or more files, and another error during MDF decoding of one or more files                                                                                                            |
|              13 | One or more input files could not be found, an error occurred during decompression of one or more files, and an another error during MDF decoding of one or more files                                                             |
|              14 | An error occurred during decryption of one or more files, another error occurred during decompression of one or more files, and another error during MDF decoding of one or more files                                             |
|              15 | One or more input files could not be found, an error occurred during decryption of one or more files, another error occurred during decompression of one or more files, and another error during MDF decoding of one or more files |
