#include "ExecutableEntryPoint.h"
#include "ExecutableInterface.h"

bool shouldStall();

#ifdef __MINGW32__

#include <windows.h>

bool shouldStall() {
  // Utilize trick from https://stackoverflow.com/questions/9009333/how-to-check-if-the-program-is-run-from-a-console
  HWND consoleWnd = GetConsoleWindow();
  DWORD dwProcessId;
  GetWindowThreadProcessId(consoleWnd, &dwProcessId);

  return (GetCurrentProcessId() == dwProcessId);
}

#else

bool shouldStall() {
  return false;
}

#endif

namespace mdf::tools::shared {

  int programEntry(int argc, char **argv, std::unique_ptr<ConverterInterface> converterInterface) {
    // Create an exporter and start the main loop.
    ExecutableInterface e(std::move(converterInterface));

    StatusCode status = e.main(argc, argv);

    // If any errors occurred, stall the dialog.
    if ((status != StatusCode::NoErrors) && (shouldStall())) {
      std::cout << "Press \"Enter\" key to exit..." << std::endl;
      std::cin.get();
    }

    return static_cast<uint32_t>(status);
  }

}
