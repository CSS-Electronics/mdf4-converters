import filecmp
import subprocess

from pathlib import Path

from Common.BaseTestCase import BaseTestCase
from Common.DataAccess import get_tool_data


class CommonBase(BaseTestCase):
    def test_common_convert_input_folder(self, copy_test_data_to_data_folder):
        # Execute the target tool on the input file with the arguments.
        command = [
            str(self._executable)
        ]

        # Insert options.
        for key, value in self._data.options.items():
            command.append(key)
            
            # Handle file arguments by discarding the empty value.
            if value != "":
                command.append(value)

        # Handle files to auto include.
        for f in self._data.input:
            if f.auto_include:
                target_path = self._data_folder / f.relative_path / f.name
                command.append(str(target_path))

        # Ensure the data path exists.
        if not self._data_folder.exists():
            self._data_folder.mkdir(parents=True)

        if self._data.name is not None:
            print(f"Starting test with comment: {self._data.name}")

        print(f"Executing command: {command}")

        # Perform test.
        call_result = subprocess.run(command, cwd=self._data_folder)
        
        # Ensure the return code matches the expected code.
        return_code = call_result.returncode

        assert return_code == self._data.return_code, f"Unexpected returncode. Expected {self._data.return_code} but got {return_code}."
        
        # Ensure the expected files are present.
        expected_output_files = []
        for output_file in self._data.output:
            target_path = self._data_folder / output_file.relative_path / output_file.name
            expected_output_files.append(target_path)
        
        missing_files = []
        for expected_output_file in expected_output_files:
            if not expected_output_file.exists():
                missing_files.append(expected_output_file)

        assert len(missing_files) == 0, f"Missing expected files: {missing_files}"
        
        # Ensure the file contents match.
        comparison_errors = []
        for output_file in self._data.output:
            # Find the data file to compare against.
            if output_file.comparision_data is None:
                output_file.comparision_data = output_file.name
            comparison_file = get_tool_data(tool=self._data.tool, name=output_file.comparision_data)
            data_file = self._data_folder / output_file.relative_path / output_file.name

            # Determine comparison to perform based on file extension.
            extension = data_file.suffix.lower()
            if extension in [".log", ".trc", ".asc", ".txt", ".csv"]:
                comparison_result = compare_ignore_line_ending(comparison_file, data_file)
            else:
                comparison_result = filecmp.cmp(comparison_file, data_file)
                
            if not comparison_result:
                comparison_errors.append(output_file.name)

        assert len(comparison_errors) == 0, f"Mismatch in output data for the following file(s): {comparison_errors}"
        
        return

    pass


def compare_ignore_line_ending(file_a, file_b):
    line_a = True
    line_b = True

    with open(file_a, "r") as fa, open(file_b, "r") as fb:
        while line_a and line_b:
            line_a = fa.readline()
            line_b = fb.readline()

            if line_a != line_b:
                return False

    return True


class TestASCFiles(CommonBase):
    parameter_files = [
        Path(__file__).parent / "TestConfiguration" / "ASC" / "ConvertIndividualFiles_ASC.json",
        Path(__file__).parent / "TestConfiguration" / "ASC" / "ConvertIndividualFilesErrorCodes_ASC.json",
        Path(__file__).parent / "TestConfiguration" / "ASC" / "ConvertIndividualFilesTime_ASC.json",
        Path(__file__).parent / "TestConfiguration" / "ASC" / "ConvertIndividualCompressedFiles_ASC.json",
        Path(__file__).parent / "TestConfiguration" / "ASC" / "ConvertIndividualEncryptedFiles_ASC.json",
        Path(__file__).parent / "TestConfiguration" / "ASC" / "ConvertInputFolder_ASC.json",
        Path(__file__).parent / "TestConfiguration" / "ASC" / "ConvertMultipleIndividualFiles_ASC.json",
    ]
    pass


class TestCLX000Files_Standard(CommonBase):
    parameter_files = [
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "ConvertIndividualFiles_CLX000.json",
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "ConvertIndividualFilesErrorCodes_CLX000.json",
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "ConvertIndividualFilesTime_CLX000.json",
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "ConvertIndividualCompressedFiles_CLX000.json",
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "ConvertIndividualEncryptedFiles_CLX000.json",
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "ConvertInputFolder_CLX000.json",
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "ConvertMultipleIndividualFiles_CLX000.json",
    ]
    pass


class TestCLX000Config(CommonBase):
    parameter_files = [
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "Configuration_DataFields.json",
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "Configuration_Log.json",
        Path(__file__).parent / "TestConfiguration" / "CLX000" / "Configuration_Timeformat.json",
    ]
    pass


class TestCSVFiles(CommonBase):
    parameter_files = [
        Path(__file__).parent / "TestConfiguration" / "CSV" / "ConvertIndividualFiles_CSV.json",
        Path(__file__).parent / "TestConfiguration" / "CSV" / "ConvertIndividualFilesErrorCodes_CSV.json",
        Path(__file__).parent / "TestConfiguration" / "CSV" / "ConvertIndividualFilesTime_CSV.json",
        Path(__file__).parent / "TestConfiguration" / "CSV" / "ConvertIndividualCompressedFiles_CSV.json",
        Path(__file__).parent / "TestConfiguration" / "CSV" / "ConvertIndividualEncryptedFiles_CSV.json",
        Path(__file__).parent / "TestConfiguration" / "CSV" / "ConvertInputFolder_CSV.json",
        Path(__file__).parent / "TestConfiguration" / "CSV" / "ConvertMultipleIndividualFiles_CSV.json",
    ]
    pass


class TestPCAPFiles(CommonBase):
    parameter_files = [
        Path(__file__).parent / "TestConfiguration" / "PCAP" / "ConvertIndividualFiles_PCAP.json",
        Path(__file__).parent / "TestConfiguration" / "PCAP" / "ConvertIndividualFilesErrorCodes_PCAP.json",
        Path(__file__).parent / "TestConfiguration" / "PCAP" / "ConvertIndividualFilesTime_PCAP.json",
        Path(__file__).parent / "TestConfiguration" / "PCAP" / "ConvertIndividualCompressedFiles_PCAP.json",
        Path(__file__).parent / "TestConfiguration" / "PCAP" / "ConvertIndividualEncryptedFiles_PCAP.json",
        Path(__file__).parent / "TestConfiguration" / "PCAP" / "ConvertInputFolder_PCAP.json",
        Path(__file__).parent / "TestConfiguration" / "PCAP" / "ConvertMultipleIndividualFiles_PCAP.json",
    ]
    pass


class TestPEAKFiles_2_1(CommonBase):
    parameter_files = [
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_2_1" / "ConvertIndividualFiles_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_2_1" / "ConvertIndividualFilesErrorCodes_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_2_1" / "ConvertIndividualFilesTime_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_2_1" / "ConvertIndividualCompressedFiles_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_2_1" / "ConvertIndividualEncryptedFiles_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_2_1" / "ConvertInputFolder_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_2_1" / "ConvertMultipleIndividualFiles_PEAK.json",
        ]
    pass


class TestPEAKFiles_1_1(CommonBase):
    parameter_files = [
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_1_1" / "ConvertIndividualFiles_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_1_1" / "ConvertIndividualFilesErrorCodes_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_1_1" / "ConvertIndividualFilesTime_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_1_1" / "ConvertIndividualCompressedFiles_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_1_1" / "ConvertIndividualEncryptedFiles_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_1_1" / "ConvertInputFolder_PEAK.json",
        Path(__file__).parent / "TestConfiguration" / "PEAK" / "Format_1_1" / "ConvertMultipleIndividualFiles_PEAK.json",
        ]
    pass


class TestSocketCANFiles(CommonBase):
    parameter_files = [
        Path(__file__).parent / "TestConfiguration" / "SocketCAN" / "ConvertIndividualFiles_SocketCAN.json",
        Path(__file__).parent / "TestConfiguration" / "SocketCAN" / "ConvertIndividualFilesErrorCodes_SocketCAN.json",
        Path(__file__).parent / "TestConfiguration" / "SocketCAN" / "ConvertIndividualFilesTime_SocketCAN.json",
        Path(__file__).parent / "TestConfiguration" / "SocketCAN" / "ConvertIndividualCompressedFiles_SocketCAN.json",
        Path(__file__).parent / "TestConfiguration" / "SocketCAN" / "ConvertIndividualEncryptedFiles_SocketCAN.json",
        Path(__file__).parent / "TestConfiguration" / "SocketCAN" / "ConvertInputFolder_SocketCAN.json",
        Path(__file__).parent / "TestConfiguration" / "SocketCAN" / "ConvertMultipleIndividualFiles_SocketCAN.json",
    ]
    pass
