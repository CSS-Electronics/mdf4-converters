#include <cstring>
#include <boost/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

#include "ParsedFileInfo.h"

namespace mdf::tools::shared {

    template<typename IteratorType>
    std::optional<uint32_t> parseUnsigned(IteratorType begin, IteratorType end) {
        std::optional<uint32_t> value;

        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::ulong_;
        using boost::spirit::qi::ascii::space;

        phrase_parse(
            begin,
            end,
            ulong_,
            space,
            value
        );

        return value;
    }

    ParsedFileInfo::ParsedFileInfo(FileInfo const &fileInfo, MetadataMap const &metadata) :
        CANMessages(fileInfo.CANMessages),
        LINMessages(fileInfo.LINMessages),
        Time(fileInfo.Time),
        TimezoneOffsetMinutes(fileInfo.TimezoneOffsetMinutes) {
        // Attempt to map information from the metadata into the structure. Start with the comment.
        if (!Comment.has_value()) {
            auto const &metadataIter = metadata.find("HDComment.Device Information.File Information.comment");
            if (metadataIter != metadata.end()) {
                Comment = std::string(metadataIter->second.valueRaw.c_str());
            }
        }

        // Session number.
        if (!Session.has_value()) {
            auto const &metadataIter = metadata.find("HDComment.Device Information.File Information.session");
            if (metadataIter != metadata.end()) {
                Session = strtoul(metadataIter->second.valueRaw.c_str(), nullptr, 10);
            }
        }

        // Split number.
        if (!Split.has_value()) {
            auto const &metadataIter = metadata.find("HDComment.Device Information.File Information.split");
            if (metadataIter != metadata.end()) {
                Split = strtoul(metadataIter->second.valueRaw.c_str(), nullptr, 10);
            }
        }

        // Logger type.
        if (!LoggerType.has_value()) {
            auto const &metadataIter = metadata.find("HDComment.Device Information.device type");
            if (metadataIter != metadata.end()) {
                LoggerType = strtoul(metadataIter->second.valueRaw.c_str(), nullptr, 16);
            }
        }

        // Firmware version.
        if (!FW_Version.has_value()) {
            auto const &metadataIter = metadata.find("HDComment.Device Information.File Information.firmware version");
            if (metadataIter != metadata.end()) {
                FW_Version = semver::from_string_noexcept(metadataIter->second.valueRaw);
            }
        }

        if (!FW_Version.has_value()) {
            auto const &metadataIter = metadata.find("HDComment.Device Information.firmware version");
            if (metadataIter != metadata.end()) {
                FW_Version = semver::from_string_noexcept(metadataIter->second.valueRaw);
            }
        }

        // Hardware version.
        if (!HW_Version.has_value()) {
            auto const &metadataIter = metadata.find("HDComment.Device Information.File Information.hardware version");
            if (metadataIter != metadata.end()) {
                // Cannot use the automatic parser in semver, since this encoding is not 100% semver.
                semver::version value;
                auto parseResult = boost::spirit::qi::parse(
                    metadataIter->second.valueRaw.cbegin(),
                    metadataIter->second.valueRaw.cend(),
                    boost::spirit::ushort_[boost::phoenix::ref(value.major)] >> '.'
                                                                             >> boost::spirit::ushort_[boost::phoenix::ref(
                                                                                 value.minor)]
                );

                if (parseResult) {
                    HW_Version = value;
                }
            }
        }

        if (!HW_Version.has_value()) {
            auto const &metadataIter = metadata.find("HDComment.Device Information.hardware version");
            if (metadataIter != metadata.end()) {
                // Cannot use the automatic parser in semver, since this encoding is not 100% semver.
                semver::version value;
                auto parseResult = boost::spirit::qi::parse(
                    metadataIter->second.valueRaw.cbegin(),
                    metadataIter->second.valueRaw.cend(),
                    boost::spirit::ushort_[boost::phoenix::ref(value.major)] >> '.'
                                                                             >> boost::spirit::ushort_[boost::phoenix::ref(
                                                                                 value.minor)]
                );

                if (parseResult) {
                    HW_Version = value;
                }
            }
        }

        // Serial number.
        if (!LoggerID.has_value()) {
            auto const &metadataIter = metadata.find("HDComment.Device Information.serial number");
            if (metadataIter != metadata.end()) {
                LoggerID = strtoul(metadataIter->second.valueRaw.c_str(), nullptr, 16);
            }
        }

        // Bitrate for CAN bus 1.
        if (!BitrateCAN1.has_value()) {
            auto const &metadataIter = metadata.find(
                "SIcomment.CAN.ASAM Measurement Environment.Bus Information.CAN1 Bit-rate"
            );
            if (metadataIter != metadata.end()) {
                BitrateCAN1 = parseUnsigned(
                    metadataIter->second.valueRaw.cbegin(),
                    metadataIter->second.valueRaw.cend()
                );
            }
        }

        // Bitrate for CAN bus 2.
        if (!BitrateCAN2.has_value()) {
            auto const &metadataIter = metadata.find(
                "SIcomment.CAN.ASAM Measurement Environment.Bus Information.CAN2 Bit-rate"
            );
            if (metadataIter != metadata.end()) {
                BitrateCAN2 = parseUnsigned(
                    metadataIter->second.valueRaw.cbegin(),
                    metadataIter->second.valueRaw.cend()
                );
            }
        }

        // Bitrate for LIN bus 1.
        if (!BitrateLIN1.has_value()) {
            auto const &metadataIter = metadata.find(
                "SIcomment.LIN.ASAM Measurement Environment.Bus Information.LIN1 Bit-rate"
            );
            if (metadataIter != metadata.end()) {
                BitrateLIN1 = parseUnsigned(
                    metadataIter->second.valueRaw.cbegin(),
                    metadataIter->second.valueRaw.cend()
                );
            }
        }

        // Bitrate for LIN bus 2.
        if (!BitrateLIN2.has_value()) {
            auto const &metadataIter = metadata.find(
                "SIcomment.LIN.ASAM Measurement Environment.Bus Information.LIN2 Bit-rate"
            );
            if (metadataIter != metadata.end()) {
                BitrateLIN2 = parseUnsigned(
                    metadataIter->second.valueRaw.cbegin(),
                    metadataIter->second.valueRaw.cend()
                );
            }
        }
    }

}
