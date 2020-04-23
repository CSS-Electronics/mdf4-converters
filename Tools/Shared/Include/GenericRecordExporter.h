#ifndef TOOLS_SHARED_GENERICRECORDEXPORTER_H
#define TOOLS_SHARED_GENERICRECORDEXPORTER_H

#include <ostream>

namespace mdf::tools::shared {

  template<typename T>
  class GenericRecordExporter {
  public:
    virtual ~GenericRecordExporter() = default;

    /**
     * Called after all data has been processed.
     */
    virtual void writeFooter();

    /**
     * Called before any data is passed.
     */
    virtual void writeHeader();

    /**
     * Called for each data record.
     * @param record Record to write.
     */
    virtual void writeRecord(T const& record) = 0;

  protected:
    /**
     * Constructor storing data in a stream.
     * @param output Stream to write converted data to.
     */
    explicit GenericRecordExporter(std::ostream &output);

    /**
     * Handle to the output stream.
     */
    std::ostream &output;
  };

  template<typename T>
  GenericRecordExporter<T>::GenericRecordExporter(std::ostream &output) : output(output) {

  }

  template<typename T>
  void GenericRecordExporter<T>::writeFooter() {
    // Do nothing in default implementation.
  }

  template<typename T>
  void GenericRecordExporter<T>::writeHeader() {
    // Do nothing in default implementation.
  }

}

#endif //TOOLS_SHARED_GENERICRECORDEXPORTER_H
