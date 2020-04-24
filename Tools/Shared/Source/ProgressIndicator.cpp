#include "ProgressIndicator.h"

#include <fmt/ostream.h>
#include <iostream>
#include <vector>

#ifdef __MINGW64__
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace mdf::tools::shared {

  ProgressIndicator::ProgressIndicator(std::size_t start, std::size_t stop, bool suppressed) : start(start), stop(stop), suppressed(suppressed) {
    // Determine the size of the terminal
  }

  void ProgressIndicator::update(std::size_t current) {
    // Only update if enough time has elapsed.
    auto const currentTime = std::chrono::system_clock::now();

    if (currentTime - lastUpdate >= std::chrono::milliseconds(10)) {
      lastUpdate = currentTime;

      update_percent(current);
    }
  }

  void ProgressIndicator::update_percent(std::size_t current) {
    if(suppressed) {
      return;
    }

    double ratio = static_cast<double>(current - start) * 1.0 / static_cast<double>(stop - start);
    leftStream.str("");
    rightStream.str("");

    // Determine total width available.
    std::size_t const width = getTerminalSize();
    std::size_t stringLength = 0;

    // Return to the beginning of the line.
    leftStream << '\r';

    // Write prefix.
    if (prefix.empty()) {
      leftStream << "[";
      stringLength += prefix.size() + 1;
    } else {
      leftStream << prefix << " [";
      stringLength += prefix.size() + 2;
    }

    // Determine space required at the end.
    fmt::print(rightStream,
               FMT_STRING("] ( {:{}} / {:} )"),
               current,
               numDigits(stop),
               stop
    );

    std::string endString = rightStream.str();
    stringLength += endString.size();

    std::size_t const filledSpaces = std::round(ratio * static_cast<double>(width - stringLength));
    std::size_t const emptySpaces = (width - stringLength) - filledSpaces;

    auto iter = std::ostream_iterator<char>(leftStream);

    std::fill_n(iter, filledSpaces, '#');
    std::fill_n(iter, emptySpaces, ' ');

    // TODO: Spin the progress indicator.
    std::cout << "\r" << std::flush;
    std::cout << leftStream.str() << endString << std::flush;
  }

  void ProgressIndicator::begin() {
    lastUpdate = std::chrono::system_clock::now();
  }

  void ProgressIndicator::end() {
    update_percent((stop - start));
    if(!suppressed) {
      std::cout << std::endl;
    }
  }

  std::size_t ProgressIndicator::getTerminalSize() const {
    // See https://stackoverflow.com/a/23370070
    std::size_t width = 80;

    #ifdef __MINGW64__
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {

      if (csbi.srWindow.Right != 0) {
        unsigned w = csbi.srWindow.Right - csbi.srWindow.Left;

        if (w > 0) {
          width = w;
        }
      }
    }
    #else
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    if(w.ws_col > 0) {
      width = w.ws_col;
    }
    #endif

    return width;
  }

  void ProgressIndicator::setPrefix(std::string value) {
    prefix = value;
  }

  void ProgressIndicator::setSuffix(std::string value) {
    suffix = value;
  }

}
