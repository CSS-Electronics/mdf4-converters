#include "Include/SemanticVersion.h"

#include <iomanip>

// Place in the global namespace, as the exported struct is also global due to C linkage.

// NOTE: Since glibc in at least version 2.25, major() and minor() has been defined as internal macros.
//       Should be fixed in future versions, as of now, simply undefined the macros.
//       See https://bugzilla.redhat.com/show_bug.cgi?id=130601
#ifdef major
#undef major
#endif

#ifdef minor
#undef minor
#endif

std::ostream& operator<< (std::ostream& stream, const SemanticVersion_t& version) {
    stream << std::setfill('0')
           << std::setw(2) << static_cast<unsigned int>(version.major) << "."
           << std::setw(2) << static_cast<unsigned int>(version.minor) << "."
           << std::setw(2) << static_cast<unsigned int>(version.patch);

    return stream;
}

std::istream& operator>> (std::istream& stream, SemanticVersionLimited_t& version) {
    std::string temp;
    std::getline(stream, temp, '.');
    version.major = static_cast<uint8_t>(strtoul(temp.c_str(), nullptr, 10));

    temp = "";
    std::getline(stream, temp, '.');
    version.minor = static_cast<uint8_t>(strtoul(temp.c_str(), nullptr, 10));

    return stream;
}

std::ostream& operator<< (std::ostream& stream, const SemanticVersionLimited_t& version) {
    stream << std::setfill('0')
           << std::setw(2) << static_cast<unsigned int>(version.major) << "."
           << std::setw(2) << static_cast<unsigned int>(version.minor);

    return stream;
}

bool operator== (SemanticVersionLimited_t const& lhs, SemanticVersionLimited_t const& rhs) {
    bool result = (lhs.major == rhs.major);

    result &= (lhs.minor == rhs.minor);

    return result;
}
