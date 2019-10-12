#ifndef TOOLS_CLX_CONFIGURATION_H
#define TOOLS_CLX_CONFIGURATION_H

namespace tools::clx {

    struct CLX000_Configuration {
        char ValueSeparator[2];
        int TimestampFormat;
        char TimestampTimeSeparator[2];
        char TimestampTimeMsSeparator[2];
        char TimestampDateSeparator[2];
        char TimeTimeDateSeparator[2];

        bool DataFields_timestamp;
        bool DataFields_type;
        bool DataFields_id;
        bool DataFields_dataLength;
        bool DataFields_data;
    };

}

#endif //TOOLS_CLX_CONFIGURATION_H
