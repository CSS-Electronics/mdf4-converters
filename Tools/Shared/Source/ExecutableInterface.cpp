#include "ExecutableInterface.h"

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/predef.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/range/iterator_range.hpp>
#include <fmt/format.h>
#include <neargye/semver.hpp>

#include "GenericFile.h"
#include "HeatshrinkFile.h"
#include "AESGCMFile.h"
#include "PasswordStorage.h"

#include "MDFBusLoggingLibrary.h"
#include "ProjectInformation.h"

using namespace std::placeholders;
namespace bfs = boost::filesystem;
namespace blo = boost::log;
namespace bpo = boost::program_options;

static std::vector<std::string> acceptableExtensions = {".mf4", ".mfe", ".mfc", ".mfm"};

namespace mdf::tools::shared {

    ExecutableInterface::ExecutableInterface(std::unique_ptr<ConverterInterface> interface) : interface(
        std::move(interface)) {
        deleteInputFiles = false;
        commonOptions = std::make_shared<CommonOptions>();
    }

    StatusCode ExecutableInterface::main(int argc, char **argv) {
        // Register progress callback.
        StatusCode mainStatus = StatusCode::NoErrors;

        // Configure which arguments to parse. Start with basic and then custom.
        configureParser();
        interface->configureParser(commandlineOptions);
        interface->configureFileParser(configFileOptions);

        // Parse options. Again, start with basic and continue to derived. The commandline parser is allowed to take
        // ALL options, to ensure a default value is populated.
        boost::program_options::options_description allOptions("All allowed options");
        allOptions.add(commandlineOptions);
        allOptions.add(configFileOptions);

        auto parser = bpo::command_line_parser(argc, argv);
        parser.options(allOptions);
        parser.positional(commandlinePositionalOptions);
        parser.allow_unregistered();

        bpo::basic_parsed_options<char> parsedOptions(&commandlineOptions);

        ParseOptionStatus status = ParseOptionStatus::NoError;

        // Handle initial parsing of options from the command line.
        try {
            parsedOptions = parser.run();
            bpo::store(parsedOptions, optionResult);
            bpo::notify(optionResult);
        } catch (bpo::invalid_command_line_syntax &e) {
            switch (e.kind()) {
                case bpo::invalid_command_line_syntax::missing_parameter:
                    BOOST_LOG_TRIVIAL(error) << "Missing argument for option '" << e.get_option_name() << "'";
                    std::cout << "Missing argument for option '" << e.get_option_name() << "'";
                    return StatusCode::MissingArgument;
                default:
                    BOOST_LOG_TRIVIAL(fatal) << "Could not complete parsing due to exception: " << e.what();
                    return StatusCode::CriticalError;
            }
        } catch (std::exception &e) {
            BOOST_LOG_TRIVIAL(fatal) << "Error occurred during initial input argument parsing: " << e.what();
            return StatusCode::CriticalError;
        }

        // Continue on with the configuration file.
        bool noConfigFileFound = false;
        if (interface->usesConfigFile()) {
            std::stringstream ss;

            earlyLogMessages.emplace_back(
                "Converter has requested config file data, attempting to load relative to executable");
            bfs::path fullPath = boost::dll::program_location();
            auto configFilePath = fullPath.parent_path() / (std::string(interface->programName) + "_config.ini");

            ss << "Attempting to load additional settings from config file at: " << configFilePath;
            earlyLogMessages.emplace_back(ss.str());
            ss.str("");

            if (bfs::exists(configFilePath)) {
                try {
                    auto configFile = std::ifstream(configFilePath.c_str());
                    auto configParserOptions = bpo::parse_config_file(configFile, configFileOptions, true);
                    bpo::store(configParserOptions, optionResult);
                    bpo::notify(optionResult);

                    ss << "Loaded configuration file at: " << configFilePath;
                    earlyLogMessages.emplace_back(ss.str());
                    ss.str("");
                } catch (std::exception &e) {
                    ss << "Error during parsing of configuration file: " << e.what();
                    earlyLogMessages.emplace_back(ss.str());
                    ss.str("");
                    return StatusCode::ConfigurationFileParseError;
                }
            } else {
                ss << "Error during parsing of configuration file. Could not locate file at: " << configFilePath;
                earlyLogMessages.emplace_back(ss.str());
                ss.str("");
                noConfigFileFound = true;
            }
        }

        // If no arguments are supplied, or an error occurred during parsing, display the help message.
        if (argc <= 1) {
            status |= ParseOptionStatus::DisplayHelp;
        }

        // Perform core parsing.
        earlyLogMessages.emplace_back("Parsing general input options");
        try {
            status |= parseOptions(optionResult);
        } catch (bpo::validation_error &e) {
            BOOST_LOG_TRIVIAL(fatal) << "Error occurred during general input argument parsing: " << e.what();
            return StatusCode::InputArgumentParsingError;
        } catch (std::exception &e) {
            BOOST_LOG_TRIVIAL(fatal) << "Error occurred during general input argument parsing: " << e.what();
            return StatusCode::CriticalError;
        }

        // Parsing for anything but errors can now be done.
        for (auto const &traceMessage: earlyLogMessages) {
            BOOST_LOG_TRIVIAL(trace) << traceMessage;
        }

        // Perform specialization parsing.
        interface->setCommonOptions(commonOptions);
        BOOST_LOG_TRIVIAL(trace) << "Parsing specialized input options";
        try {
            status |= interface->parseOptions(optionResult);
        } catch (bpo::validation_error &e) {
            BOOST_LOG_TRIVIAL(fatal) << "Error occurred during specialized input argument parsing: " << e.what();
            return StatusCode::InputArgumentParsingError;
        } catch (std::exception &e) {
            BOOST_LOG_TRIVIAL(fatal) << "Error occurred during specialized input argument parsing: " << e.what();
            return StatusCode::CriticalError;
        }

        // Collect any unrecognized options.
        std::vector<std::string> unrecognizedOptions = bpo::collect_unrecognized(parsedOptions.options,
                                                                                 bpo::exclude_positional);

        if (!unrecognizedOptions.empty()) {
            status |= ParseOptionStatus::UnrecognizedOption;
        }

        if (noConfigFileFound) {
            bool failOnMissingConfigFile = (optionResult.count("error-on-missing-config-file") != 0);

            if (failOnMissingConfigFile) {
                BOOST_LOG_TRIVIAL(fatal) << "No configuration file found, but required by the user, exiting.";
                return StatusCode::ConfigurationFileParseError;
            } else {
                BOOST_LOG_TRIVIAL(info) << "No configuration file found, skipping.";
            }
        }

        // Handle parsing result.
        if ((status & ParseOptionStatus::UnrecognizedOption) == ParseOptionStatus::UnrecognizedOption) {
            displayUnrecognizedOptions(unrecognizedOptions);
            return StatusCode::UnrecognizedOption;
        } else if ((status & ParseOptionStatus::DisplayHelp) == ParseOptionStatus::DisplayHelp) {
            displayHelp();
            return StatusCode::NoErrors;
        } else if ((status & ParseOptionStatus::DisplayVersion) == ParseOptionStatus::DisplayVersion) {
            displayVersion();
            return StatusCode::NoErrors;
        } else if ((status & ParseOptionStatus::CouldNotFindPasswordFile) ==
                   ParseOptionStatus::CouldNotFindPasswordFile) {
            BOOST_LOG_TRIVIAL(error) << "Could not locate password file";
            return StatusCode::InvalidInputArgument;
        } else if ((status & ParseOptionStatus::CouldNotParsePasswordFile) ==
                   ParseOptionStatus::CouldNotParsePasswordFile) {
            BOOST_LOG_TRIVIAL(error) << "Could not parse password file";
            return StatusCode::InvalidInputArgument;
        } else if ((status & ParseOptionStatus::NoInputFiles) == ParseOptionStatus::NoInputFiles) {
            return StatusCode::NoErrors;
        }

        std::map<boost::filesystem::path, boost::filesystem::path> curatedInputFiles;

        // Create a list of files to work on. For each folder in the input, walk it recursively (without following links).
        for (auto &path: inputFiles) {
            bfs::path inputPath(path);

            // Ensure the full path is used.
            if (!inputPath.is_absolute()) {
                inputPath = boost::filesystem::weakly_canonical(inputPath);
            }

            // Ensure that the current path exists.
            if (!boost::filesystem::exists(inputPath)) {
                BOOST_LOG_TRIVIAL(error) << "Path does not exist: " << inputPath;
                mainStatus |= StatusCode::MissingFile;
                continue;
            }

            // If it is a folder, iterate recursively.
            if (bfs::is_directory(inputPath)) {
                BOOST_LOG_TRIVIAL(info) << "Received folder as input argument, performing recursive walk: "
                                        << inputPath;

                /* Determine name of target output directory */
                bfs::path const baseDirectory = inputPath;
                bfs::path const baseOutputDirectory = inputPath.string() + "_out";

                for (auto &entry: bfs::recursive_directory_iterator(inputPath, bfs::symlink_option::no_recurse)) {
                    std::string extension = entry.path().extension().string();
                    boost::algorithm::to_lower(extension);

                    if (std::find(std::cbegin(acceptableExtensions), std::cend(acceptableExtensions), extension) !=
                        std::end(acceptableExtensions)) {
                        BOOST_LOG_TRIVIAL(info) << "Found file with matching extensions; " << entry;

                        auto rel = bfs::relative(entry.path().parent_path(), baseDirectory);

                        curatedInputFiles.insert(
                            {
                                entry,
                                baseOutputDirectory / rel
                            }
                        );
                    }
                }
            } else {
                curatedInputFiles.insert(
                    {
                        inputPath,
                        inputPath.parent_path()
                    }
                );
            }
        }

        // Create a mapping between all input files and their corresponding output files.
        // If the output directory is set, override the destination path, else place them in the same folder as the
        // input file.

        for (auto &path: curatedInputFiles) {
            bfs::path inputFilePath(path.first);
            bool inputFileIsTemporary = false;

            // Determine where to place the result.
            boost::filesystem::path outputFolder;
            if (optionResult.count("output-directory")) {
                // An output directory is specified, place everything here.
                outputFolder = boost::filesystem::path(optionResult["output-directory"].as<std::string>());
            } else {
                // The output path is already calculated.
                outputFolder = path.second;
            }

            if (!outputFolder.is_absolute()) {
                boost::filesystem::weakly_canonical(outputFolder);
            }

            if (!boost::filesystem::exists(outputFolder)) {
                BOOST_LOG_TRIVIAL(info) << "Output folder does not exists. Creating " << outputFolder << "";
                try {
                    boost::filesystem::create_directories(outputFolder);
                } catch (boost::filesystem::filesystem_error &e) {
                    BOOST_LOG_TRIVIAL(fatal) << "Could not create output folder " << outputFolder
                                             << ". Logged error is:\n"
                                             << e.what();
                    return StatusCode::CriticalError;
                }
            }

            // If the file is encrypted, decrypt it first.
            if (mdf::generic::getGenericFileType(inputFilePath) == mdf::generic::GenericFileType::EncryptedFile) {
                BOOST_LOG_TRIVIAL(info) << "Detected encrypted file.";
                std::shared_ptr<PasswordStorage> currentPasswordStorage;

                // Is a password storage generated?
                if (!passwordStorage) {
                    auto localPasswordFilePath = inputFilePath.parent_path() / "passwords.json";

                    if (boost::filesystem::exists(localPasswordFilePath)) {
                        try {
                            currentPasswordStorage = std::make_shared<PasswordStorage>(localPasswordFilePath.string());
                        } catch (std::exception &e) {
                            BOOST_LOG_TRIVIAL(error)
                                << "Could not load local password file, even though the file exists. Input file: "
                                << inputFilePath
                                << ".";
                            continue;
                        }
                    } else {
                        BOOST_LOG_TRIVIAL(error)
                            << "Could not decrypt file, as no password file is specified, no default password file is found and no local password file is found. Input file: "
                            << inputFilePath << ".";
                        mainStatus |= StatusCode::DecryptionError;
                        continue;
                    }
                } else {
                    currentPasswordStorage = passwordStorage;
                }

                // Create a temporary folder with a file with the correct name.
                bfs::path temporaryDirectoryPath = bfs::temp_directory_path() / bfs::unique_path();
                BOOST_LOG_TRIVIAL(info) << "Creating temporary folder: " << temporaryDirectoryPath << ".";
                bfs::create_directories(temporaryDirectoryPath);
                bfs::path temporaryPath = temporaryDirectoryPath / inputFilePath.filename();

                // Attempt to use the correct naming scheme.
                if (boost::algorithm::iequals(temporaryPath.extension().string(), ".MFM")) {
                    temporaryPath.replace_extension(".MFC");
                } else if (boost::algorithm::iequals(temporaryPath.extension().string(), ".MFE")) {
                    temporaryPath.replace_extension(".MF4");
                }

                // Determine which logger this file originates from.
                uint32_t device = mdf::getDeviceIdFromFile(inputFilePath.string());

                // Attempt to get the corresponding password.
                bool decryptionStatus = false;
                auto passwordAvailable = currentPasswordStorage->hasPassword(device);
                if (passwordAvailable != PasswordType::Missing) {
                    std::string const &password = currentPasswordStorage->getPassword(device);

                    // Decrypt to temporary file, and pass this on for further processing.
                    decryptionStatus = mdf::decryptFile(inputFilePath.string(), temporaryPath.string(), password);
                }

                // Check if the current file is a temporary one. If so, then delete it.
                if (inputFileIsTemporary) {
                    BOOST_LOG_TRIVIAL(info) << "Deleting temporary file: " << inputFilePath << ".";
                    bfs::remove(inputFilePath);
                }

                // Pass the temporary file on as the input file.
                inputFilePath = temporaryPath;
                inputFileIsTemporary = true;

                if (!decryptionStatus) {
                    std::stringstream ss;
                    ss << "Error during conversion of " << inputFilePath << ". Failed to perform decryption - ";

                    switch (passwordAvailable) {
                        case PasswordType::Specific:
                            ss << "device specific password is incorrect.";
                            break;
                        case PasswordType::Default:
                            ss << "default password is incorrect.";
                            break;
                        case PasswordType::Missing:
                            // Fallthrough.
                        default:
                            ss << "no default password and/or no matching serial number found in password file.";
                            break;
                    }

                    ss << "Attempted to use " << currentPasswordStorage->getPasswordFilePath() << " as password list.";

                    BOOST_LOG_TRIVIAL(error) << ss.str();
                    mainStatus |= StatusCode::DecryptionError;

                    bfs::remove(inputFilePath);
                    bfs::remove(inputFilePath.parent_path());

                    continue;
                }
            }

            // If the file is compressed, decompress it first.
            if (mdf::generic::getGenericFileType(inputFilePath) == mdf::generic::GenericFileType::CompressedFile) {
                BOOST_LOG_TRIVIAL(info) << "Detected compressed file.";

                // Create a temporary folder.
                bfs::path temporaryDirectoryPath = bfs::temp_directory_path() / bfs::unique_path();
                BOOST_LOG_TRIVIAL(info) << "Creating temporary folder: " << temporaryDirectoryPath << ".";
                bfs::create_directories(temporaryDirectoryPath);
                bfs::path temporaryPath = temporaryDirectoryPath / inputFilePath.filename();

                if (boost::algorithm::equals(temporaryPath.extension().string(), ".MFC")) {
                    temporaryPath.replace_extension(".MF4");
                }

                // Decompress to temporary file, and pass this on for further processing.
                bool decompressionStatus = mdf::decompressFile(inputFilePath.string(), temporaryPath.string());

                // Check if the current file is a temporary one. If so, then delete it.
                if (inputFileIsTemporary) {
                    BOOST_LOG_TRIVIAL(info) << "Deleting temporary file and parent folder: " << inputFilePath << ".";
                    bfs::remove(inputFilePath);
                    bfs::remove(inputFilePath.parent_path());
                }

                // Pass the temporary file on as the input file.
                inputFilePath = temporaryPath;
                inputFileIsTemporary = true;

                if (!decompressionStatus) {
                    BOOST_LOG_TRIVIAL(error) << "Error during conversion of " << inputFilePath
                                             << ". Failed to perform decompression.";
                    mainStatus |= StatusCode::DecompressionError;
                    continue;
                }
            }

            // Call the exporter for the conversion.
            bool conversionStatus = false;
            try {
                conversionStatus = interface->convert(inputFilePath, outputFolder);
            } catch (std::exception &e) {
                BOOST_LOG_TRIVIAL(error) << e.what();
                conversionStatus = false;
            }

            // Check if the current file is a temporary one. If so, then delete it.
            if (inputFileIsTemporary) {
                BOOST_LOG_TRIVIAL(info) << "Deleting temporary file and parent folder: " << inputFilePath << ".";
                bfs::remove(inputFilePath);
                bfs::remove(inputFilePath.parent_path());
            }

            if (!conversionStatus) {
                BOOST_LOG_TRIVIAL(error) << "Error during conversion of " << inputFilePath << ".";
                mainStatus |= StatusCode::DecodingError;
                continue;
            } else if(deleteInputFiles) {
                bfs::path originalFilePath(path.first);
                BOOST_LOG_TRIVIAL(info) << "Deleting original input file " << originalFilePath << ".";
                bfs::remove(originalFilePath);
            }
        }

        return mainStatus;
    }

    void ExecutableInterface::configureParser() {
        // Add default options.
        commandlineOptions.add_options()
            ("help,h", bpo::bool_switch()->default_value(false), "Print this help message.")
            ("version,v", bpo::bool_switch()->default_value(false), "Print version information.")
            ("verbose", bpo::value<int>()->default_value(1), "Set verbosity of output (0-5).")
            ("output-directory,O", bpo::value<std::string>(), "Output directory to place converted files into.")
            ("delete-converted,d", bpo::bool_switch()->default_value(false), "Delete input files on success.")
            ("non-interactive", bpo::bool_switch()->default_value(false),
             "Run in non-interactive mode, with no progress output.")
            ("timezone,t", bpo::value<std::string>()->default_value("l"),
             "Display times in UTC (u), logger localtime (l, default) or PC local time (p).")
            ("password-file,p", bpo::value<std::string>(),
             "Path to password json file. If left empty, and an encrypted file is encountered, the folder of the input file will be searched.")
            ("input-files,i", bpo::value<std::vector<std::string>>(),
             "List of files to convert. All unknown arguments will be interpreted as input files.");

        if (interface->usesConfigFile()) {
            commandlineOptions.add_options()
                ("error-on-missing-config-file,e",
                 "Emit error on no configuration file instead of using default values");
        }

        // Capture all other options (Positional options) as input files.
        commandlinePositionalOptions.add("input-files", -1);
    }

    void ExecutableInterface::displayHelp() const {
        std::cout << "Usage:" << "\n";
        std::cout << interface->programName << " [-short-option value --long-option value] [-i] file_a [file_b ...]:\n";
        std::cout << "\n";
        std::cout << "Short options start with a single \"-\", while long options start with \"--\".\n";
        std::cout << "A value enclosed in \"[]\" signifies it is optional.\n";
        std::cout << "Some options only exists in the long form, while others exist in both forms.\n";
        std::cout << "Not all options require arguments (arg).\n";
        std::cout << "\n";
        std::cout << commandlineOptions << std::endl;
    }

    void ExecutableInterface::displayUnrecognizedOptions(std::vector<std::string> const &unrecognizedOptions) const {
        if (unrecognizedOptions.size() == 1) {
            std::cout << "Unrecognized option:" << "\n";
        } else {
            std::cout << "Unrecognized options:" << "\n";
        }

        for (auto const &option: unrecognizedOptions) {
            std::cout << option << "\n";
        }

        std::cout << "\n";
        displayHelp();
    }

    void ExecutableInterface::displayVersion() const {
        // Display version information on the base library, the shared base and the specialization.
        std::cout << "Version of " << interface->programName << ": " << interface->getVersion() << std::endl;
        std::cout << "Version of converter base: " << semver::to_string(mdf::tools::shared::MDFSimpleConverters_Shared_VERSION) << std::endl;
        std::cout << "Version of MDF library: " << semver::to_string(mdf::getVersion()) << std::endl;
    }

    ParseOptionStatus ExecutableInterface::parseOptions(boost::program_options::variables_map const &result) {
        // Predicate for usage with trimming of '=' and ' '.
        auto predicate = [](char const &input) -> bool {
            return (input == ' ' || input == '=');
        };

        // Setup verbosity.
        int verboseToken = result["verbose"].as<int>();

        switch (verboseToken) {
            case 0:
                blo::core::get()->set_filter(blo::trivial::severity >= blo::trivial::fatal);
                break;
            case 1:
                blo::core::get()->set_filter(blo::trivial::severity >= blo::trivial::error);
                break;
            case 2:
                blo::core::get()->set_filter(blo::trivial::severity >= blo::trivial::warning);
                break;
            case 3:
                blo::core::get()->set_filter(blo::trivial::severity >= blo::trivial::info);
                break;
            case 4:
                blo::core::get()->set_filter(blo::trivial::severity >= blo::trivial::debug);
                break;
            case 5:
                blo::core::get()->set_filter(blo::trivial::severity >= blo::trivial::trace);
                break;
            default:
                // Unknown value.
                std::string const originalToken = fmt::format(FMT_STRING("{:d}"), verboseToken);

                throw bpo::validation_error(
                    bpo::validation_error::kind_t::invalid_option_value,
                    "verbose",
                    originalToken
                );
        }

        // Handle request for help messages.
        if (result["help"].as<bool>()) {
            return ParseOptionStatus::DisplayHelp;
        }

        // Handle request for version information.
        if (result["version"].as<bool>()) {
            return ParseOptionStatus::DisplayVersion;
        }

        commonOptions->nonInteractiveMode = result["non-interactive"].as<bool>();

        // Handle timezone. Utilize the default value is set.
        std::string timeZoneDisplay = result["timezone"].as<std::string>();
        boost::algorithm::trim_left_if(timeZoneDisplay, predicate);

        if (boost::algorithm::iequals("u", timeZoneDisplay)) {
            commonOptions->displayTimeFormat = DisplayTimeFormat::UTC;
        } else if (boost::algorithm::iequals("p", timeZoneDisplay)) {
            commonOptions->displayTimeFormat = DisplayTimeFormat::PCLocalTime;
        } else if (boost::algorithm::iequals("l", timeZoneDisplay)) {
            commonOptions->displayTimeFormat = DisplayTimeFormat::LoggerLocalTime;
        } else {
            // Unknown value.
            throw bpo::validation_error(
                bpo::validation_error::kind_t::invalid_option_value,
                "timezone",
                result["timezone"].as<std::string>()
            );
        }

        // Attempt to load passwords.
        if (result.count("password-file")) {
            std::string passwordFile = result["password-file"].as<std::string>();
            boost::algorithm::trim_left_if(passwordFile, predicate);

            boost::filesystem::path passwordFilePath = boost::filesystem::weakly_canonical(passwordFile);

            BOOST_LOG_TRIVIAL(trace) << "Attempting to load password file from argument at " << passwordFilePath;

            if (boost::filesystem::exists(passwordFilePath)) {
                try {
                    passwordStorage = std::make_shared<PasswordStorage>(passwordFilePath.string());
                } catch (boost::property_tree::json_parser_error &e) {
                    BOOST_LOG_TRIVIAL(fatal) << "Error during parsing of " << passwordFilePath << ": " << e.message()
                                             << " at line " << e.line();
                    return ParseOptionStatus::CouldNotParsePasswordFile;
                } catch (std::exception &e) {
                    return ParseOptionStatus::CouldNotFindPasswordFile;
                }
            } else {
                return ParseOptionStatus::CouldNotFindPasswordFile;
            }
        } else {
            // Attempted to load default password file here.
            bfs::path current_path = boost::dll::program_location();
            boost::filesystem::path passwordFilePath = current_path.parent_path() / "passwords.json";

            BOOST_LOG_TRIVIAL(trace) << "Attempting to load default password file next to executable at "
                                     << passwordFilePath;

            if (boost::filesystem::exists(passwordFilePath)) {
                try {
                    passwordStorage = std::make_shared<PasswordStorage>(passwordFilePath.string());
                } catch (boost::property_tree::json_parser_error &e) {
                    BOOST_LOG_TRIVIAL(fatal) << "Error during parsing of " << passwordFilePath << ": " << e.message()
                                             << " at line " << e.line();
                    return ParseOptionStatus::CouldNotParsePasswordFile;
                } catch (std::exception &e) {
                    // Ignore password file errors in the default path. User may not need passwords.
                }
            }
        }

        deleteInputFiles = result["delete-converted"].as<bool>();

        if (result.count("input-files")) {
            std::vector<std::string> files = result["input-files"].as<std::vector<std::string>>();
            for (std::string const &entry: files) {
                inputFiles.emplace_back(entry);
            }
        } else {
            // No input files.
            return ParseOptionStatus::NoInputFiles;
        }

        return ParseOptionStatus::NoError;
    }
}
