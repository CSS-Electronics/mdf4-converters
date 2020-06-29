#include <cstring>

#include "ParsedFileInfo.h"

namespace mdf::tools::shared {

    ParsedFileInfo::ParsedFileInfo(FileInfo const& fileInfo, MetadataMap const& metadata) :
        Session(0),
        Split(0),
        LoggerID(0),
        LoggerType(0),
        BitrateCAN1(0),
        BitrateCAN2(0),
        BitrateLIN1(0),
        BitrateLIN2(0),
        CANMessages(0),
        LINMessages(0),
        Time(0),
        TimezoneOffsetMinutes(0)
    {
        // Attempt to load information.
        auto const& findIterComment = metadata.find("HDComment.Device Information.File Information.comment");
        if(findIterComment != metadata.end()) {
            strncpy(Comment, findIterComment->second.valueRaw.c_str(), sizeof(Comment));
        }

        auto const& findIterSession = metadata.find("HDComment.Device Information.File Information.session");
        if(findIterSession != metadata.end()) {
            Session = strtoul(findIterSession->second.valueRaw.c_str(), nullptr, 10);
        }

        auto const& findIterSplit = metadata.find("HDComment.Device Information.File Information.split");
        if(findIterSplit != metadata.end()) {
            Split = strtoul(findIterSplit->second.valueRaw.c_str(), nullptr, 10);
        }

        auto const& findIterDeviceType = metadata.find("HDComment.Device Information.File Information.device type");
        if(findIterDeviceType != metadata.end()) {
            LoggerType = strtoul(findIterDeviceType->second.valueRaw.c_str(), nullptr, 16);
        }

        auto const& findIterFirmwareVersion = metadata.find("HDComment.Device Information.File Information.firmware version");
        if(findIterFirmwareVersion != metadata.end()) {
            Version fwVersion;

            auto scanRes = sscanf(findIterFirmwareVersion->second.valueRaw.c_str(), "%2u.%2u", &fwVersion.major, &fwVersion.minor);

            if(scanRes == 3) {
                FW_Version = fwVersion;
            }
        }

        auto const& findIterHardwareVersion = metadata.find("HDComment.Device Information.File Information.hardware version");
        if(findIterHardwareVersion != metadata.end()) {
            Version hwVersion;

            auto scanRes = sscanf(findIterHardwareVersion->second.valueRaw.c_str(), "%2u.%2u", &hwVersion.major, &hwVersion.minor);

            if(scanRes == 2) {
                HW_Version = hwVersion;
            }
        }

        auto const& findIterSerialNumber = metadata.find("HDComment.Device Information.File Information.serial number");
        if(findIterSerialNumber != metadata.end()) {
            LoggerID = strtoul(findIterSerialNumber->second.valueRaw.c_str(), nullptr, 16);
        }

        auto const& findIterCANBitrate1 = metadata.find("SIcomment.CAN.ASAM Measurement Environment.Bus Information.CAN1 Bit-rate");
        if(findIterCANBitrate1 != metadata.end()) {
            BitrateCAN1 = strtoul(findIterCANBitrate1->second.valueRaw.c_str(), nullptr, 10);
        }

        auto const& findIterCANBitrate2 = metadata.find("SIcomment.CAN.ASAM Measurement Environment.Bus Information.CAN2 Bit-rate");
        if(findIterCANBitrate2 != metadata.end()) {
            BitrateCAN2 = strtoul(findIterCANBitrate2->second.valueRaw.c_str(), nullptr, 10);
        }

        auto const& findIterLINBitrate1 = metadata.find("SIcomment.LIN.ASAM Measurement Environment.Bus Information.LIN1 Bit-rate");
        if(findIterLINBitrate1 != metadata.end()) {
            BitrateLIN1 = strtoul(findIterLINBitrate1->second.valueRaw.c_str(), nullptr, 10);
        }

        auto const& findIterLINBitrate2 = metadata.find("SIcomment.LIN.ASAM Measurement Environment.Bus Information.LIN2 Bit-rate");
        if(findIterLINBitrate2 != metadata.end()) {
            BitrateLIN2 = strtoul(findIterLINBitrate2->second.valueRaw.c_str(), nullptr, 10);
        }
    }

}
