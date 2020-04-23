#ifndef MDFSIMPLECONVERTERS_PROGRESSINDICATOR_H
#define MDFSIMPLECONVERTERS_PROGRESSINDICATOR_H

#include <chrono>
#include <cstdint>
#include <sstream>

namespace mdf::tools::shared {

  struct ProgressIndicator {
    ProgressIndicator(std::size_t start, std::size_t stop);

    void begin();
    void update(std::size_t current);
    void end();

    void setPrefix(std::string value);

    void setSuffix(std::string value);

  private:
    std::size_t getTerminalSize() const;

    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> lastUpdate;
    std::stringstream leftStream;
    std::stringstream rightStream;
    std::size_t start;
    std::size_t stop;

    std::string prefix;
    std::string suffix;

    void update_percent(std::size_t percent);
  };

  // generic solution
  template <class T>
  int numDigits(T number)
  {
    int digits = 0;
    if (number < 0) digits = 1; // remove this line if '-' counts as a digit
    while (number) {
      number /= 10;
      digits++;
    }
    return digits;
  }

}

#endif //MDFSIMPLECONVERTERS_PROGRESSINDICATOR_H
