#ifndef MDFSUPER_PYTHONLOGGER_H
#define MDFSUPER_PYTHONLOGGER_H

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include "CXX/Objects.hxx"

enum severity_level
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

BOOST_LOG_GLOBAL_LOGGER(pythonLogger, boost::log::sources::severity_logger<severity_level>)

namespace mdf::python {

    struct PythonLogger : public boost::log::sinks::basic_sink_backend<boost::log::sinks::synchronized_feeding> {
        PythonLogger();

        // The function consumes the log records that come from the frontend
        void consume(boost::log::record_view const& rec);
    private:
        Py::Object pythonLogger;
    };

    bool setupLogging();
}


#endif //MDFSUPER_PYTHONLOGGER_H
