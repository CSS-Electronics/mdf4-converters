#include "../Include/TimeConverter.h"

#include <chrono>
#include <fmt/chrono.h>

namespace mdf::tools::shared {

    time_t
    convertTimeStamp(DisplayTimeFormat const &displayTimeFormat, time_t timeStamp, ParsedFileInfo const &loggerInfo) {
        time_t result = timeStamp;

        // Determine which action to take.
        switch (displayTimeFormat) {
            case DisplayTimeFormat::LoggerLocalTime:
                // Correct for the logger timezone.
                result += loggerInfo.TimezoneOffsetMinutes * 60;
                break;
            case DisplayTimeFormat::UTC:
                // Timestamps are logged in UTC by default, no need to do any conversion.
                break;
            case DisplayTimeFormat::PCLocalTime:
                // Also the default action.
                [[fallthrough]];
            default:
                // Correct for the local timezone.
                time_t const dummyTime = 60 * 60 * 24;
                auto const local = localtime(&dummyTime);
                int const localHour = local->tm_hour;
                auto const utc = gmtime(&dummyTime);
                int const utcHour = utc->tm_hour;

                auto difference = localHour - utcHour;

                result += difference * 60 * 60;
                break;
        }

        return result;
    }

    std::chrono::nanoseconds convertTimeStamp(DisplayTimeFormat const &displayTimeFormat, std::chrono::nanoseconds timeStamp, ParsedFileInfo const &loggerInfo) {
        std::chrono::nanoseconds result = timeStamp;

        // Determine which action to take.
        switch (displayTimeFormat) {
            case DisplayTimeFormat::LoggerLocalTime:
                // Correct for the logger timezone.
                result += std::chrono::minutes(loggerInfo.TimezoneOffsetMinutes);
                break;
            case DisplayTimeFormat::UTC:
                // Timestamps are logged in UTC by default, no need to do any conversion.
                break;
            case DisplayTimeFormat::PCLocalTime:
                // Also the default action.
                [[fallthrough]];
            default:
                // Correct for the local timezone.
                time_t const dummyTime = 60 * 60 * 24;

                auto const local = fmt::localtime(dummyTime);
                std::chrono::minutes offsetLocal(local.tm_min);
                offsetLocal += std::chrono::hours(local.tm_hour);

                auto const utc = fmt::gmtime(dummyTime);
                std::chrono::minutes offsetUTC(utc.tm_min);
                offsetUTC += std::chrono::hours(utc.tm_hour);

                auto difference = offsetLocal - offsetUTC;

                result += difference;
                break;
        }

        return result;
    }

}
