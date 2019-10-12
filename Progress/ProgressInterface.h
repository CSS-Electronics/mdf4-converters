#ifndef MDFSORTER_PROGRESSINTERFACE_H
#define MDFSORTER_PROGRESSINTERFACE_H

#include <boost/signals2.hpp>

namespace progress {

    class ProgressController {
    private:
        boost::signals2::signal<void(int, int)> signal;
    public:
        void registerProgressCallback(boost::signals2::signal<void(unsigned int, unsigned int)>::slot_type progress);
    protected:
        void updateProgress(unsigned int current, unsigned int total);
    };

}

#endif //MDFSORTER_PROGRESSINTERFACE_H
