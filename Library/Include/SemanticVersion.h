#ifndef MDFSORTER_VERSION_H
#define MDFSORTER_VERSION_H

#ifdef __cplusplus
#include <iostream>

extern "C" {
#endif

#include <stdint.h>

typedef struct SemanticVersion_s {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} SemanticVersion_t;

typedef struct SemanticVersionLimited_s {
    uint8_t major;
    uint8_t minor;
} SemanticVersionLimited_t;

#ifdef __cplusplus
}

std::ostream& operator<< (std::ostream& stream, const SemanticVersion_t& version);

std::istream& operator>> (std::istream& stream, SemanticVersionLimited_t& version);
std::ostream& operator<< (std::ostream& stream, const SemanticVersionLimited_t& version);
bool operator== (SemanticVersionLimited_t const& lhs, SemanticVersionLimited_t const& rhs);
#endif

#endif //MDFSORTER_VERSION_H
