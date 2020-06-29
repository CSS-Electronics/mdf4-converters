#include <memory>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/sinks.hpp>

#include "PythonLogger.h"

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level);

BOOST_LOG_GLOBAL_LOGGER_INIT(pythonLogger, boost::log::sources::severity_logger<severity_level>)
{
    boost::log::sources::severity_logger<severity_level> lg;
    return lg;
}

namespace mdf::python {

    typedef boost::log::sinks::synchronous_sink<PythonLogger> python_sink_t;

    bool setupLogging() {
        bool status = true;

        try {
            // Add synchronous sink for logged messages.
            boost::shared_ptr<python_sink_t> pythonSink = boost::make_shared<python_sink_t>();

            // Register the new sink in the logging core.
            boost::log::core::get()->add_sink(pythonSink);

            BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Logger initialized";
        } catch (std::exception &e) {
            status = false;
        }

        return status;
    }

    PythonLogger::PythonLogger() {
        // Attempt to find the required python module.
        PyObject* pythonLoggerModuleRaw = nullptr;
        try {
            pythonLoggerModuleRaw = PyImport_ImportModule("logging");
            Py::ifPyErrorThrowCxxException();
        } catch(Py::BaseException &e) {
            e.clear();
            pythonLoggerModuleRaw = nullptr;
        }

        if(pythonLoggerModuleRaw != nullptr) {
            auto pythonLoggerModule = Py::asObject(pythonLoggerModuleRaw);

            // Create a new logger (or get the existing one) for the extension and library.
            pythonLogger = pythonLoggerModule.callMemberFunction("getLogger", Py::TupleN(Py::String("mdf_iter")));

            // Check the current log level. If it is the default level, change it.
            if(pythonLogger.hasAttr("level")) {
                Py::Object currentLevelObj = pythonLogger.getAttr("level");
                Py::Long currentLevel(0);

                if(currentLevelObj.isNumeric()) {
                    currentLevel = Py::Long(currentLevelObj);
                }

                if(currentLevel == 0) {
                    // Set the default level to fatal or higher.
                    auto fatalLevel = pythonLoggerModule.getAttr("FATAL");
                    pythonLogger.callMemberFunction("setLevel", Py::TupleN(fatalLevel));
                }
            }
        }
    }

    void PythonLogger::consume(const boost::log::record_view &rec) {
        // Extract the severity.
        auto a = rec[severity];
        severity_level severity = a.get();
        Py::Long loggerLevel;

        switch(severity) {
            case trace:
                loggerLevel = 5;
                break;
            case debug:
                loggerLevel = 10;
                break;
            case info:
                loggerLevel = 20;
                break;
            case warning:
                loggerLevel = 30;
                break;
            case error:
                loggerLevel = 40;
                break;
            case fatal:
                loggerLevel = 50;
                break;
            default:
                loggerLevel = 0;
                break;
        }

        // Extract the message.
        std::string loggingMessage = *rec[boost::log::expressions::smessage];

        // Delegate to python logging module.
        Py::String msg(loggingMessage);
        Py::String log_name("log");

        pythonLogger.callMemberFunction("log", Py::TupleN(loggerLevel, msg));
    }

}
