from pathlib import Path
from typing import Optional

from Common.Tool import Tool


def get_shared_file(name: str, tool: str = None) -> Optional[Path]:
    # Attempt to load from the tool specific path.
    # Root directory.
    root = Path(__file__).parent.parent
    
    # Attempt to get tool from enum, else from direct string.
    t = Tool.Unknown
    try:
        t = Tool(tool)
    except ValueError:
        pass
    
    if t == Tool.Unknown:
        t = tool
    
    tool_path = root / str(t) / "Files"
    tool_file_path = tool_path / name
    
    if tool_file_path.exists():
        return tool_file_path
    
    # Attempt the generic path.
    generic_path = root / "Common" / "Files"
    generic_file_path = generic_path / name
    
    if generic_file_path.exists():
        return generic_file_path
    
    return None


def get_shared_data(name: str) -> Path:
    # Root directory.
    result = Path(__file__).parent.parent
    
    # Enter tool directory and select file.
    result = result / "Common" / "TestData" / name
    
    return result


def get_tool_data(tool: str, name: str) -> Path:
    # Root directory.
    result = Path(__file__).parent.parent
    
    # Attempt to get tool from enum, else from direct string.
    t = Tool.Unknown
    try:
        t = Tool(tool)
    except ValueError:
        pass
    
    if t == Tool.Unknown:
        t = tool
    
    tool_path = result / str(t)
    
    if not tool_path.exists():
        raise RuntimeError(f"Tool path {tool_path} does not exist.")
    
    # Enter tool directory and select file.
    result = tool_path / "TestData" / name
    
    if not result.exists():
        raise RuntimeError(f"Tool data file {result} does not exist.")
    
    return result
