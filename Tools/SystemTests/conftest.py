import json
import pytest
import shutil
import tempfile

from pathlib import Path


def pytest_addoption(parser):
    parser.addoption("--config", action="store", default="config.json")
    parser.addoption("--keep", action="store", default=False)


def pytest_generate_tests(metafunc):
    # If the test is tool agnostic, parametrize with each tool config.
    if "tool" in metafunc.fixturenames:
        metafunc.parametrize("tool", metafunc.config.getoption("stringinput"))
        
    return


# See https://docs.pytest.org/en/latest/example/simple.html#making-test-result-information-available-in-fixtures for
# origin and license.
@pytest.hookimpl(tryfirst=True, hookwrapper=True)
def pytest_runtest_makereport(item, call):
    # execute all other hooks to obtain the report object
    outcome = yield
    rep = outcome.get_result()

    # set a report attribute for each phase of a call, which can
    # be "setup", "call", "teardown"

    setattr(item, "rep_" + rep.when, rep)


@pytest.fixture()
def get_global_config_file(pytestconfig) -> dict:
    config_path = Path(pytestconfig.getoption("config"))
    
    if not config_path.is_absolute():
        # Set path relative to current working directory.
        config_path = Path(__file__).parent / config_path

    if not config_path.exists():
        raise RuntimeError("Could not load configuration file, file does not exist")
    
    with open(config_path) as fh:
        json_data = json.load(fh)
    
    return json_data


@pytest.fixture()
def get_release_binaries(get_global_config_file) -> Path:
    result = Path(get_global_config_file["release_binaries"])
    
    if not result.is_absolute():
        result = Path(__file__).parent / result
    
    return result


@pytest.fixture()
def temporary_folder(request, pytestconfig) -> Path:
    # Create a temporary directory.
    temporary_directory = tempfile.mkdtemp()

    # Let the test run its cause.
    yield Path(temporary_directory)

    # Cleanup.
    if (pytestconfig.getoption("keep") == 1) and request.node.rep_call.failed:
        print(f"Error during test. The temporary directory has NOT been deleted, and can be found at {temporary_directory}")
    else:
        shutil.rmtree(temporary_directory, ignore_errors=True)

    return

