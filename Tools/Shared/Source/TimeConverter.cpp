#include "../Include/TimeConverter.h"

#include <chrono>

namespace tools::shared {

    time_t convertTimeStamp(DisplayTimeFormat const& displayTimeFormat, time_t timeStamp, FileInfo_t const& loggerInfo) {
        time_t result = timeStamp;

        // Determine which action to take.
        switch (displayTimeFormat) {
            case LoggerLocalTime:
                // Correct for the logger timezone.
                result += loggerInfo.TimezoneOffset;
                break;
            case UTC:
                // Timestamps are logged in UTC by default, no need to do any conversion.
                break;
            case PCLocalTime:
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

}
