#ifndef TOOLS_SHARED_COMMONOPTIONS_H
#define TOOLS_SHARED_COMMONOPTIONS_H

namespace tools::shared {

    enum DisplayTimeFormat : char {
        UTC = 'u',
        LoggerLocalTime = 'l',
        PCLocalTime = 'p',
    };

    struct CommonOptions {
        DisplayTimeFormat displayTimeFormat;
        bool non_interactiveMode;
    };

}

#endif //TOOLS_SHARED_COMMONOPTIONS_H
