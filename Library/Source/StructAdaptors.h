#ifndef MDFSIMPLECONVERTERS_STRUCTADAPTORS_H
#define MDFSIMPLECONVERTERS_STRUCTADAPTORS_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>

#include "CANRecord.h"
#include "LINRecord.h"

BOOST_FUSION_ADAPT_STRUCT(
  mdf::CANRecord,
  TimeStamp,
  BusChannel,
  ID,
  IDE,
  DLC,
  DataLength,
  Dir,
  EDL,
  BRS,
  DataBytes
);

BOOST_FUSION_ADAPT_STRUCT(
  mdf::LINRecord,
  TimeStamp,
  BusChannel,
  ID,
  Dir,
  ReceivedDataByteCount,
  DataLength,
  DataBytes
);

#endif //MDFSIMPLECONVERTERS_STRUCTADAPTORS_H
