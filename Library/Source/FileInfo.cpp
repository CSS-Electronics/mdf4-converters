#include "FileInfo.h"

namespace mdf {

  FileInfo::FileInfo() :
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
    //
  }

}
