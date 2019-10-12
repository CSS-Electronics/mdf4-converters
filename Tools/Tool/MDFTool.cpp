#include "MDFTool.h"

#include "Library.h"
#include "ProjectInformation.h"

using namespace mdf;

namespace tools::tool {

    MDFTool::MDFTool() : ConverterInterface("mdftool") {
        sortOutput = false;
        overwrite = false;
    }

    void MDFTool::configureParser(boost::program_options::options_description &opts) {
        opts.add_options()
            // Add option for sorting.
            ("sort,s", boost::program_options::bool_switch()->default_value(true), "sort the data")

            // Add option for overwriting.
            ("overwrite", boost::program_options::bool_switch()->default_value(false), "replace the input data with the output")

            // Add option for pattern to append.
            ("name,n", boost::program_options::value<std::string>()->default_value("_fin"), "name to append on processed data")
            ;
    }

    bool MDFTool::convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) {
        bool status = false;

        // Generate the new filename.
        auto fileNameWithoutExtension = outputFolder / inputFilePath.stem();
        fileNameWithoutExtension += appendName;
        auto fileNameWithExtension = fileNameWithoutExtension.replace_extension(inputFilePath.extension());

        {
            // Attempt to open the input file.
            MdfFile mdfFile(inputFilePath.string());

            std::cout << "Opening file" << std::endl;

            mdfFile.finalize();

            // If sorting is enabled, sort the file.
            if (sortOutput) {
                std::cout << "Sorting" << std::endl;
                mdfFile.sort();
            }

            // Save as a new file.
            mdfFile.save(fileNameWithExtension.string());
        }

        // If overwrite is enabled, delete the old file and rename the saved file into place.
        if(overwrite) {
            std::cout << "Overwriting" << std::endl;
            boost::filesystem::rename(fileNameWithExtension, inputFilePath);
        }

        status = true;

        return status;
    }

    interfaces::Version MDFTool::getVersion() const {
        return tools::tool::version;
    }

    tools::shared::ParseOptionStatus MDFTool::parseOptions(boost::program_options::variables_map const &result) {
        sortOutput = result["sort"].as<bool>();

        appendName = result["name"].as<std::string>();

        return tools::shared::ParseOptionStatus::NoError;
    }

}
