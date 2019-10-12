#ifndef TOOLS_SHARED_GENERICRECORDEXPORTER_H
#define TOOLS_SHARED_GENERICRECORDEXPORTER_H

#include <ostream>

namespace tools::shared {

    template <typename T>
    class GenericRecordExporter {
    public:
        virtual void writeHeader() = 0;
        virtual void writeRecord(T record) = 0;
    protected:
        explicit GenericRecordExporter(std::ostream& output);
        std::ostream& output;
    };

    template <typename T>
    GenericRecordExporter<T>::GenericRecordExporter(std::ostream &output) : output(output) {

    }

}

#endif //TOOLS_SHARED_GENERICRECORDEXPORTER_H
