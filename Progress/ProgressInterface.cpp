#include "ProgressInterface.h"

namespace progress {

    void ProgressController::registerProgressCallback(boost::signals2::signal<void(unsigned int, unsigned int)>::slot_type progress) {
        signal.connect(progress);
    }

    void ProgressController::updateProgress(unsigned int current, unsigned  int total) {
        signal(current, total);
    }

}
