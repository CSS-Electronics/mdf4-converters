#ifndef MDFSIMPLECONVERTERS_HDBLOCK_H
#define MDFSIMPLECONVERTERS_HDBLOCK_H

#include "MdfBlock.h"

#include "DGBlock.h"
#include "MDBlock.h"

namespace mdf {

  struct HDBlock : MdfBlock {
    [[nodiscard]] std::shared_ptr<DGBlock> getFirstDGBlock() const;

    /*!Get the start time of the measurement in nanoseconds.
     * @return
     */
    [[nodiscard]] uint64_t getStartTimeNs() const;

    [[nodiscard]] int16_t getTzOffsetMin() const;
    [[nodiscard]] int16_t getDstOffsetMin() const;

    std::shared_ptr<MDBlock> getComment() const;

  protected:
    bool load(std::shared_ptr<std::streambuf> stream) override;
    bool saveBlockData(uint8_t *dataPtr) override;

  private:
    uint64_t startTimeNs;
    int16_t tzOffsetMin;
    int16_t dstOffsetMin;
    uint8_t timeFlags;
    uint8_t timeClass;
    uint8_t flags;
    uint8_t reserved;
    double start_angle_rad;
    double start_distance_m;
  };

}

#endif //MDFSIMPLECONVERTERS_HDBLOCK_H
