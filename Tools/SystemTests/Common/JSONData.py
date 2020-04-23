import json

from dataclasses import dataclass, field
from dataclasses_json import dataclass_json, Undefined
from pathlib import Path
from typing import Dict, List, Optional, Union


@dataclass_json(undefined=Undefined.EXCLUDE)
@dataclass
class FileInputSpecification(object):
    name: str
    relative_path: str = "."
    auto_include: bool = True
    
    pass


@dataclass_json(undefined=Undefined.EXCLUDE)
@dataclass
class FileOutputSpecification(object):
    name: str
    comparision_data: Optional[str] = None
    relative_path: str = "."
    
    pass


@dataclass_json(undefined=Undefined.EXCLUDE)
@dataclass
class FileAdditionalSpecification(object):
    input_name: str
    name: str
    relative_path: str = "."
    relative_to_data: bool = True
    
    pass


@dataclass_json(undefined=Undefined.EXCLUDE)
@dataclass
class DataForTestCase(object):
    name: Optional[str] = None
    input: List[FileInputSpecification] = field(default_factory=list)
    output: List[FileOutputSpecification] = field(default_factory=list)
    options: Dict[str, str] = field(default_factory=dict)
    options_uses_relative: bool = False
    return_code: int = 0
    additional_files: List[FileAdditionalSpecification] = field(default_factory=list)
    tool: str = ""
    
    pass


def load_params_from_json_files(files: Union[str, Path, List[str], List[Path]]) -> List[DataForTestCase]:
    result: List[DataForTestCase] = []
    
    # Determine type.
    if isinstance(files, List):
        # Determine if any elements are in the list.
        if len(files) == 0:
            raise RuntimeError("No files to load")
        
        # Load the list.
        for element in files:
            result.extend(_load_params_from_single_json_file(element))
        pass
    else:
        result.extend(_load_params_from_single_json_file(files))
    
    return result


def _load_params_from_single_json_file(file: Union[str, Path]) -> List[DataForTestCase]:
    # Is this a supported type.
    if isinstance(file, str):
        pass
    elif not isinstance(file, Path):
        raise RuntimeError(f"Unknown file type: {type(file)}")
    
    # Attempt to load the data.
    json_data: dict
    
    with open(file) as handle:
        json_data = json.load(handle)
        
    # Convert to a list of test cases and a corresponding tool.
    tool = json_data.get("tool")
    cases_raw = json_data.get("cases")
    
    cases: [DataForTestCase] = DataForTestCase.schema().load(cases_raw, many=True)
    
    for case in cases:
        case.tool = tool
    
    return cases
