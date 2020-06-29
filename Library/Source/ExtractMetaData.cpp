#include "ExtractMetaData.h"

#include <boost/algorithm/string.hpp>
#include <boost/iostreams/stream.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace bio = boost::iostreams;
namespace bpt = boost::property_tree;

namespace mdf {

    static bool parseCommonPropertiesTree(bpt::ptree const& tree, MetadataMap& map, std::string prefix = "");

    static bpt::ptree const& getAttributesTree(bpt::ptree const& tree);


    // Helper to have an empty property tree.
    bpt::ptree const& getEmptyTree() {
        static bpt::ptree t;
        return t;
    }

    bool extractMetadata(std::string const& xml, MetadataMap& metadata) {
        bool result = false;

        bio::stream<bio::array_source> stream(xml.c_str(), xml.length());

        // Parse using boost property tree.
        bpt::ptree propertyTree;
        bpt::read_xml(stream, propertyTree);

        // Look for the tool independent data.
        bpt::ptree const& HDTree = propertyTree.get_child("HDcomment.common_properties", getEmptyTree());

        parseCommonPropertiesTree(HDTree, metadata, "HDComment");

        return result;
    }

    bool extractMetadataHD(std::string const& xml, MetadataMap& metadata) {
        bool result = false;

        bio::stream<bio::array_source> stream(xml.c_str(), xml.length());

        // Parse using boost property tree.
        bpt::ptree propertyTree;
        bpt::read_xml(stream, propertyTree);

        // Look for the tool independent data.
        bpt::ptree const& HDTree = propertyTree.get_child("HDcomment.common_properties", getEmptyTree());

        result = parseCommonPropertiesTree(HDTree, metadata, "HDComment.");

        return result;
    }

    bool extractMetadataSI(std::string const& xml, MetadataMap& metadata) {
        bool result = false;
        std::string prefix = "SIcomment.";

        bio::stream<bio::array_source> stream(xml.c_str(), xml.length());

        // Parse using boost property tree.
        bpt::ptree propertyTree;
        bpt::read_xml(stream, propertyTree);

        // Extract root tag.
        bpt::ptree const& root = propertyTree.get_child("SIcomment", getEmptyTree());

        // Read out TX field.
        bpt::ptree const& txField = root.get_child("TX", getEmptyTree());

        // Read out bus.
        bpt::ptree const& busField = root.get_child("bus", getEmptyTree());
        bpt::ptree const& busFieldAttributes = getAttributesTree(busField);
        std::string bus = busFieldAttributes.get<std::string>("name", "");
        if(!bus.empty()) {
            prefix += bus;
            prefix += ".";
        }

        // Look for the tool independent data.
        bpt::ptree const& common = root.get_child("common_properties", getEmptyTree());
        result = parseCommonPropertiesTree(common, metadata, prefix);

        return result;
    }

    static bool parseCommonPropertiesTree(bpt::ptree const& tree, MetadataMap& map, std::string prefix) {
        for(auto const& child: tree) {
            // Extract attributes.
            bpt::ptree const& attributesTree = getAttributesTree(child.second);

            // Extract key type.
            std::string const& keyType = child.first;

            // Check the tag type. Accepted types are "e" for value and "tree" for nested values.
            if (boost::equals(keyType, "e")) {
                // Extract all attributes for the e field.
                MdfMetadataEntry attributes;

                attributes.name = attributesTree.get<std::string>("name");
                attributes.description = attributesTree.get<std::string>("desc", "");
                attributes.unit = attributesTree.get<std::string>("unit", "");
                attributes.valueType = attributesTree.get<std::string>("type", "");
                attributes.readOnly = attributesTree.get<bool>("ro", false);

                // Value.
                attributes.valueRaw = child.second.get_value<std::string>();
                std::string key = prefix + attributes.name;

                // Check if this already exists in the map.
                auto findIter = map.find(key);
                if(findIter != map.end()) {
                    // Error., key already exists in the tree.
                    return false;
                }

                map.emplace(std::make_pair(key, attributes));
            } else if (boost::equals(keyType, "tree")) {
                // Update prefix.
                std::string treeName = attributesTree.get<std::string>("name");
                prefix += treeName;
                prefix += ".";

                // Tree, recursive parsing. Extract the key to prepend on the values.
                parseCommonPropertiesTree(child.second, map, prefix);
            } else {
                // Unknown, skip.
            }
        }

        return true;
    }

    static bpt::ptree const& getAttributesTree(bpt::ptree const& tree) {
        bpt::ptree const& attributesTree = tree.get_child("<xmlattr>", getEmptyTree());

        return attributesTree;
    }

    /*
    bool getLINInfo(FileInfo &fileInfo, std::string xml) {
        // Parse using tinyxml2.
        XMLDocument doc;
        XMLError errorStatus = doc.Parse(xml.c_str(), xml.length());

        if (errorStatus != XML_SUCCESS) {
            return false;
        }

        // Find the correct node.
        // The path we are looking for is: "SIcomment.common_properties.tree".
        XMLElement const *rootElement = doc.RootElement();
        const char *rootName = rootElement->Name();

        if (strncmp(rootName, "SIcomment", strlen(rootName)) != 0) {
            return false;
        }

        // Lookup common properties.
        XMLElement const *cpElement = rootElement->FirstChildElement("common_properties");
        if (cpElement == nullptr) {
            return false;
        }
        if (cpElement->NoChildren()) {
            return false;
        }

        // Find a tree with the correct name.
        XMLElement const *treeElement = nullptr;

        for (XMLElement const *element = cpElement->FirstChildElement();
             element != nullptr; element = element->NextSiblingElement()) {
            // Ensure the node type is "tree" and the name matches.
            const char *elementName = element->Name();
            if (strncmp(elementName, "tree", strlen(elementName)) != 0) {
                continue;
            }

            XMLAttribute const *nameAttribute = element->FindAttribute("name");
            if (nameAttribute == nullptr) {
                continue;
            }

            const char *attributeName = nameAttribute->Value();
            if (strncmp(attributeName, "Bus Information", strlen(attributeName)) != 0) {
                continue;
            }

            treeElement = element;
        }

        if (treeElement == nullptr) {
            return false;
        }

        // Read out data.
        for (XMLElement const *element = treeElement->FirstChildElement();
             element != nullptr; element = element->NextSiblingElement()) {
            // Ensure the node type is "e".
            const char *elementName = element->Name();
            if (strncmp(elementName, "e", strlen(elementName)) != 0) {
                continue;
            }

            // Get the name and read out data based on this.
            XMLAttribute const *nameAttribute = element->FindAttribute("name");
            if (nameAttribute == nullptr) {
                continue;
            }

            const char *attributeName = nameAttribute->Value();
            const char *elementValue = element->GetText();

            if (strncmp(attributeName, "LIN1 Bit-rate", strlen(attributeName)) == 0) {
                uint32_t bitRate;

                if (elementValue == nullptr) {
                    bitRate = 0;
                } else {
                    bitRate = strtoul(elementValue, nullptr, 10);
                }

                fileInfo.BitrateLIN1 = bitRate;
            } else if (strncmp(attributeName, "LIN2 Bit-rate", strlen(attributeName)) == 0) {
                uint32_t bitRate;

                if (elementValue == nullptr) {
                    bitRate = 0;
                } else {
                    bitRate = strtoul(elementValue, nullptr, 10);
                }

                fileInfo.BitrateLIN2 = bitRate;
            }
        }

        return true;
    }

    bool getCANInfo(FileInfo &fileInfo, std::string xml) {
        // Parse using tinyxml2.
        XMLDocument doc;
        XMLError errorStatus = doc.Parse(xml.c_str(), xml.length());

        if (errorStatus != XML_SUCCESS) {
            return false;
        }

        // Find the correct node.
        // The path we are looking for is: "SIcomment.common_properties.tree".
        XMLElement const *rootElement = doc.RootElement();
        const char *rootName = rootElement->Name();

        if (strncmp(rootName, "SIcomment", strlen(rootName)) != 0) {
            return false;
        }

        // Lookup common properties.
        XMLElement const *cpElement = rootElement->FirstChildElement("common_properties");
        if (cpElement == nullptr) {
            return false;
        }
        if (cpElement->NoChildren()) {
            return false;
        }

        // Find a tree with the correct name.
        XMLElement const *treeElement = nullptr;

        for (XMLElement const *element = cpElement->FirstChildElement();
             element != nullptr; element = element->NextSiblingElement()) {
            // Ensure the node type is "tree" and the name matches.
            const char *elementName = element->Name();
            if (strncmp(elementName, "tree", strlen(elementName)) != 0) {
                continue;
            }

            XMLAttribute const *nameAttribute = element->FindAttribute("name");
            if (nameAttribute == nullptr) {
                continue;
            }

            const char *attributeName = nameAttribute->Value();
            if (strncmp(attributeName, "Bus Information", strlen(attributeName)) != 0) {
                continue;
            }

            treeElement = element;
        }

        if (treeElement == nullptr) {
            return false;
        }

        // Read out data.
        for (XMLElement const *element = treeElement->FirstChildElement();
             element != nullptr; element = element->NextSiblingElement()) {
            // Ensure the node type is "e".
            const char *elementName = element->Name();
            if (strncmp(elementName, "e", strlen(elementName)) != 0) {
                continue;
            }

            // Get the name and read out data based on this.
            XMLAttribute const *nameAttribute = element->FindAttribute("name");
            if (nameAttribute == nullptr) {
                continue;
            }

            const char *attributeName = nameAttribute->Value();
            const char *elementValue = element->GetText();

            if (strncmp(attributeName, "CAN1 Bit-rate", strlen(attributeName)) == 0) {
                uint32_t bitRate;

                if (elementValue == nullptr) {
                    bitRate = 0;
                } else {
                    bitRate = strtoul(elementValue, nullptr, 10);
                }

                fileInfo.BitrateCAN1 = bitRate;
            } else if (strncmp(attributeName, "CAN2 Bit-rate", strlen(attributeName)) == 0) {
                uint32_t bitRate;

                if (elementValue == nullptr) {
                    bitRate = 0;
                } else {
                    bitRate = strtoul(elementValue, nullptr, 10);
                }

                fileInfo.BitrateCAN2 = bitRate;
            }
        }

        return true;
    }

    bool parseDeviceInformation(FileInfo &fileInfo, XMLElement const *treeRoot) {
        // Loop over all the attributes, and map their properties to the file information.
        for (XMLElement const *element = treeRoot->FirstChildElement();
             element != nullptr; element = element->NextSiblingElement()) {
            // Ensure the node type is "e".
            const char *elementName = element->Name();
            if (strncmp(elementName, "e", strlen(elementName)) != 0) {
                continue;
            }

            // Get the name and read out data based on this.
            XMLAttribute const *nameAttribute = element->FindAttribute("name");
            if (nameAttribute == nullptr) {
                continue;
            }

            const char *attributeName = nameAttribute->Value();
            const char *elementValue = element->GetText();

            if (strncmp(attributeName, "firmware version", strlen(attributeName)) == 0) {
                if (elementValue == nullptr) {
                    return false;
                }

                Version fwVersion;
                unsigned int patch;
                auto scanRes = sscanf(elementValue, "%2u.%2u.%2u", &fwVersion.major, &fwVersion.minor, &patch);

                // Ensure the correct number of fields were read.
                if (scanRes != 3) {
                    return false;
                }

                // Insert the optional value.
                fwVersion.patch = patch;

                fileInfo.FW_Version = fwVersion;
            } else if (strncmp(attributeName, "hardware version", strlen(attributeName)) == 0) {
                if (elementValue == nullptr) {
                    return false;
                }

                Version hwVersion;
                auto scanRes = sscanf(elementValue, "%2u.%2u", &hwVersion.major, &hwVersion.minor);

                // Ensure the correct number of fields were read.
                if (scanRes != 2) {
                    return false;
                }

                fileInfo.HW_Version = hwVersion;
            } else if (strncmp(attributeName, "serial number", strlen(attributeName)) == 0) {
                fileInfo.LoggerID = strtoul(elementValue, nullptr, 16);
            } else if (strncmp(attributeName, "device type", strlen(attributeName)) == 0) {
                fileInfo.LoggerType = strtoul(elementValue, nullptr, 16);
            }
        }

        return true;
    }

    bool parseFileInformation(FileInfo &fileInfo, XMLElement const *treeRoot) {
        // Loop over all the attributes, and map their properties to the file information.
        for (XMLElement const *element = treeRoot->FirstChildElement();
             element != nullptr; element = element->NextSiblingElement()) {
            // Ensure the node type is "e".
            const char *elementName = element->Name();
            if (strncmp(elementName, "e", strlen(elementName)) != 0) {
                continue;
            }

            // Get the name and read out data based on this.
            XMLAttribute const *nameAttribute = element->FindAttribute("name");
            if (nameAttribute == nullptr) {
                continue;
            }

            const char *attributeName = nameAttribute->Value();
            const char *elementValue = element->GetText();

            if (strncmp(attributeName, "session", strlen(attributeName)) == 0) {
                if (elementValue == nullptr) {
                    return false;
                }

                fileInfo.Session = strtoul(elementValue, nullptr, 10);
            } else if (strncmp(attributeName, "split", strlen(attributeName)) == 0) {
                if (elementValue == nullptr) {
                    return false;
                }

                fileInfo.Split = strtoul(elementValue, nullptr, 10);
            } else if (strncmp(attributeName, "comment", strlen(attributeName)) == 0) {
                if (elementValue == nullptr) {
                    fileInfo.Comment[0] = '\0';
                } else {
                    strncpy(fileInfo.Comment, elementValue, sizeof(fileInfo.Comment));
                }
            }
        }

        return true;
    }*/

}
