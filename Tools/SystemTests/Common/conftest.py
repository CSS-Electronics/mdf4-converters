from typing import List

import pytest

from Common.BaseTestCase import BaseTestCase
from Common.JSONData import load_params_from_json_files
from Common.Tool import Tool


def get_tool_mark(tool: Tool) -> pytest.mark:
    result = None
    
    if tool == Tool.ASC:
        result = pytest.mark.ASC
    elif tool == Tool.CLX000:
        result = pytest.mark.CLX000
    elif tool == Tool.CSV:
        result = pytest.mark.CSV
    elif tool == Tool.PCAP:
        result = pytest.mark.PCAP
    elif tool == Tool.PEAK:
        result = pytest.mark.PEAK
    elif tool == Tool.SocketCAN:
        result = pytest.mark.SocketCAN
    
    return result


def pytest_generate_tests(metafunc):
    # Determine if it contains json data.
    if issubclass(metafunc.cls, BaseTestCase):
        parameter_files = metafunc.cls.parameter_files
        
        if not isinstance(parameter_files, List):
            return
        
        if not "data" in metafunc.fixturenames:
            return
        
        argvalues = []
        ids = []
        
        for parameter_file in parameter_files:
            # Load the data.
            cases_in_parameter_file = load_params_from_json_files(parameter_file)
            
            # Parametrize using each record.
            for case in cases_in_parameter_file:
                tool = Tool(case.tool)
                mark = get_tool_mark(tool)
                
                if mark is not None:
                    argvalues.append(pytest.param(case, marks=mark))
                else:
                    argvalues.append(pytest.param(case))
                
                # Use the name if present, else increment.
                if case.name is not None:
                    ids.append(case.name)
                else:
                    ids.append(str(len(ids)))
            
        # Mark with the tool name and parametrize.
        metafunc.parametrize(argnames=("data",), argvalues=argvalues, ids=ids)
        
    return
