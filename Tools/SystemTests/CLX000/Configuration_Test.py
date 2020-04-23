import filecmp
import subprocess

from pathlib import Path

from Common.BaseTestCase import BaseTestCase
from Common.DataAccess import get_tool_data


class CommonBase(BaseTestCase):
    def test_clx000_configuration(self, copy_test_data_to_data_folder):
        # Execute the target tool on the input file with the arguments.
        command = [
            str(self._executable)
        ]

        # Insert options.
        for key, value in self._data.options.items():
            command.append(key)
            
            # Handle file arguments by discarding the empty value.
            if value is not "":
                command.append(value)

        # Handle files to auto include.
        for f in self._data.input:
            if f.auto_include:
                target_path = self._data_folder / f.relative_path / f.name
                command.append(str(target_path))

        print(f"Executing command: {command}")
        
        # Perform test.
        call_result = subprocess.run(command, cwd=self._binary_folder)
        
        # Ensure the return code matches the expected code.
        assert call_result.returncode == self._data.return_code, f"Unexpected returncode. Expected {self._data.return_code} but got {call_result.returncode}."
        
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
            
            comparison_result = filecmp.cmp(comparison_file, data_file)
                
            if not comparison_result:
                comparison_errors.append(output_file.name)
        
        assert len(comparison_errors) == 0, f"Mismatch in output data for the following file(s): {comparison_errors}"
        
        return
    
    pass

