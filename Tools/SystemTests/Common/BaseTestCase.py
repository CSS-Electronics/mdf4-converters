import os

import pytest
import shutil

from pathlib import Path

from Common.DataAccess import get_shared_data, get_shared_file
from Common.JSONData import DataForTestCase


def is_library(f):
    if f.suffix.lower() == ".dll":
        return True
    return False


def is_executable(f):
    if f.suffix.lower() == ".exe":
        return True
    return False


class BaseTestCase(object):
    @pytest.fixture()
    def copy_binary_files(self, data: DataForTestCase, temporary_folder: Path, get_release_binaries):
        # Save parameters.
        self._data = data
        self._temporary_folder = temporary_folder
        self._binary_folder = self._temporary_folder / "bin"
        self._executable = None
        
        # Create list of files to copy.
        release_binaries = get_release_binaries
        target_files = []
        
        # Locate all runtime dependencies.
        for f in release_binaries.iterdir():
            if is_library(f):
                target_files.append(f.name)
                continue
            
            if f.stem == data.tool:
                target_files.append(f.name)
                self._executable = self._binary_folder / f.name
                continue
        
        if self._executable is None:
            raise RuntimeError("Could not locate tool executable")
        
        # Copy files.
        for f in target_files:
            src = release_binaries / f
            dst = self._binary_folder / f

            # Ensure the target folder exists.
            os.makedirs(dst.parent, exist_ok=True)
            shutil.copy(src, dst)
            pass
        
        return
    
    def _parse_options(self):
        _directory_keys = [
            "-i",
            "--input-files",
            "-O",
            "--output-directory",
            "-I",
            "--input-directory",
            "-p",
            "--password-file"
        ]
        
        # Handle some arguments.
        for key, value in self._data.options.items():
            # Handle some keys related to paths.
            if key in _directory_keys:
                if value is None or value == "":
                    continue

                p = Path(value)
                if not p.is_absolute():
                    p = self._data_folder / p
                    
                    # Convert to relative key if requested.
                    if self._data.options_uses_relative:
                        p = Path(os.path.relpath(p, self._binary_folder))
                    
                    self._data.options[key] = str(p)
                    
    def _copy_additional_files(self):
        # Copy any additional files.
        for additional_file in self._data.additional_files:
            # Attempt to load from the tool first, fall back to the shared pool.
            shared_file = get_shared_file(additional_file.input_name, self._data.tool)
        
            if shared_file is None:
                raise RuntimeError(f"Could not get additional file: {additional_file}")
        
            src = shared_file
            if additional_file.relative_to_data:
                dst = self._data_folder / Path(additional_file.relative_path) / additional_file.name
            else:
                dst = self._binary_folder / Path(additional_file.relative_path) / additional_file.name
            os.makedirs(dst.parent, exist_ok=True)
            shutil.copy(src, dst)
        
        return
    
    def _copy_data_to_folder(self):
        # Copy test data into the target folder.
        for input_file in self._data.input:
            src = get_shared_data(input_file.name)
            dst = self._data_folder / Path(input_file.relative_path) / input_file.name
            
            # Ensure the target folder exists.
            os.makedirs(dst.parent, exist_ok=True)
            shutil.copy(src, dst)
        
        self._parse_options()
        self._copy_additional_files()
        
        return
    
    @pytest.fixture()
    def copy_test_data_to_data_folder(self, copy_binary_files):
        self._data_folder = self._temporary_folder / "data"
        self._copy_data_to_folder()
        
        return
    
    @pytest.fixture()
    def copy_test_data_to_binary_folder(self, copy_binary_files):
        self._data_folder = self._binary_folder
        self._copy_data_to_folder()
        
        return
    
    pass
