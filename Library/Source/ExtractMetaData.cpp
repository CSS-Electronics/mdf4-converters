#include "ExtractMetaData.h"

#include <tinyxml2.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace mdf {

  using namespace tinyxml2;

  bool parseDeviceInformation(FileInfo& fileInfo, XMLElement const* treeRoot);
  bool parseFileInformation(FileInfo& fileInfo, XMLElement const* treeRoot);

  bool getSharedInfo(FileInfo& fileInfo, std::string xml) {
    bool result = false;

    // Parse using tinyxml2.
    XMLDocument doc;
    XMLError errorStatus = doc.Parse(xml.c_str(), xml.length());

    if(errorStatus != XML_SUCCESS) {
      return false;
    }

    // Find the correct node.
    // The path we are looking for is: "HDcomment.common_properties".
    XMLElement const* rootElement = doc.RootElement();
    const char* rootName = rootElement->Name();

    if(strncmp(rootName, "HDcomment", strlen(rootName)) != 0) {
      return false;
    }

    // Lookup common properties.
    XMLElement const* cpElement = rootElement->FirstChildElement("common_properties");
    if(cpElement == nullptr) {
      return false;
    }
    if(cpElement->NoChildren()) {
      return false;
    }

    result = true;

    // There are two trees here with information: "Device Information" and "File Information".
    for(XMLElement const* element = cpElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
      // Ensure the node type is "tree" and the name matches.
      const char* elementName = element->Name();
      if(strncmp(elementName, "tree", strlen(elementName)) != 0) {
        continue;
      }

      XMLAttribute const* nameAttribute = element->FindAttribute("name");
      if(nameAttribute == nullptr) {
        continue;
      }

      const char* attributeName = nameAttribute->Value();
      if(strncmp(attributeName, "Device Information", strlen(attributeName)) == 0) {
        result &= parseDeviceInformation(fileInfo, element);
      } else if(strncmp(attributeName, "File Information", strlen(attributeName)) == 0) {
        result &= parseFileInformation(fileInfo, element);
      } else {
        continue;
      }
    }

    return result;
  }

  bool getLINInfo(FileInfo& fileInfo, std::string xml) {
    return true;
  }

  bool getCANInfo(FileInfo& fileInfo, std::string xml) {
    // Parse using tinyxml2.
    XMLDocument doc;
    XMLError errorStatus = doc.Parse(xml.c_str(), xml.length());

    if(errorStatus != XML_SUCCESS) {
      return false;
    }

    // Find the correct node.
    // The path we are looking for is: "SIcomment.common_properties.tree".
    XMLElement const* rootElement = doc.RootElement();
    const char* rootName = rootElement->Name();

    if(strncmp(rootName, "SIcomment", strlen(rootName)) != 0) {
      return false;
    }

    // Lookup common properties.
    XMLElement const* cpElement = rootElement->FirstChildElement("common_properties");
    if(cpElement == nullptr) {
      return false;
    }
    if(cpElement->NoChildren()) {
      return false;
    }

    // Find a tree with the correct name.
    XMLElement const* treeElement = nullptr;

    for(XMLElement const* element = cpElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
      // Ensure the node type is "tree" and the name matches.
      const char* elementName = element->Name();
      if(strncmp(elementName, "tree", strlen(elementName)) != 0) {
        continue;
      }

      XMLAttribute const* nameAttribute = element->FindAttribute("name");
      if(nameAttribute == nullptr) {
        continue;
      }

      const char* attributeName = nameAttribute->Value();
      if(strncmp(attributeName, "Bus Information", strlen(attributeName)) != 0) {
        continue;
      }

      treeElement = element;
    }

    if(treeElement == nullptr) {
      return false;
    }

    // Read out data.
    for(XMLElement const* element = treeElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
      // Ensure the node type is "e".
      const char* elementName = element->Name();
      if(strncmp(elementName, "e", strlen(elementName)) != 0) {
        continue;
      }

      // Get the name and read out data based on this.
      XMLAttribute const* nameAttribute = element->FindAttribute("name");
      if(nameAttribute == nullptr) {
        continue;
      }

      const char* attributeName = nameAttribute->Value();
      const char* elementValue = element->GetText();

      if(strncmp(attributeName, "CAN1 Bit-rate", strlen(attributeName)) == 0) {
        uint32_t bitRate;

        if(elementValue == nullptr) {
          bitRate = 0;
        } else {
          bitRate = strtoul(elementValue, nullptr, 10);
        }

        fileInfo.BitrateCAN1 = bitRate;
      } else if(strncmp(attributeName, "CAN2 Bit-rate", strlen(attributeName)) == 0) {
        uint32_t bitRate;

        if(elementValue == nullptr) {
          bitRate = 0;
        } else {
          bitRate = strtoul(elementValue, nullptr, 10);
        }

        fileInfo.BitrateCAN2 = bitRate;
      }
    }

    return true;
  }

  bool parseDeviceInformation(FileInfo& fileInfo, XMLElement const* treeRoot) {
    // Loop over all the attributes, and map their properties to the file information.
    for(XMLElement const* element = treeRoot->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
      // Ensure the node type is "e".
      const char* elementName = element->Name();
      if(strncmp(elementName, "e", strlen(elementName)) != 0) {
        continue;
      }

      // Get the name and read out data based on this.
      XMLAttribute const* nameAttribute = element->FindAttribute("name");
      if(nameAttribute == nullptr) {
        continue;
      }

      const char* attributeName = nameAttribute->Value();
      const char* elementValue = element->GetText();

      if(strncmp(attributeName, "firmware version", strlen(attributeName)) == 0) {
        if(elementValue == nullptr) {
          return false;
        }

        Version fwVersion;
        unsigned int patch;
        auto scanRes = sscanf(elementValue, "%2u.%2u.%2u", &fwVersion.major, &fwVersion.minor, &patch);

        // Ensure the correct number of fields were read.
        if(scanRes != 3) {
          return false;
        }

        // Insert the optional value.
        fwVersion.patch = patch;

        fileInfo.FW_Version = fwVersion;
      } else if(strncmp(attributeName, "hardware version", strlen(attributeName)) == 0) {
        if(elementValue == nullptr) {
          return false;
        }

        Version hwVersion;
        auto scanRes = sscanf(elementValue, "%2u.%2u", &hwVersion.major, &hwVersion.minor);

        // Ensure the correct number of fields were read.
        if(scanRes != 2) {
          return false;
        }

        fileInfo.HW_Version = hwVersion;
      } else if(strncmp(attributeName, "serial number", strlen(attributeName)) == 0) {
        fileInfo.LoggerID = strtoul(elementValue, nullptr, 16);
      } else if(strncmp(attributeName, "device type", strlen(attributeName)) == 0) {
        fileInfo.LoggerType = strtoul(elementValue, nullptr, 16);
      }
    }

    return true;
  }

  bool parseFileInformation(FileInfo& fileInfo, XMLElement const* treeRoot) {
    // Loop over all the attributes, and map their properties to the file information.
    for(XMLElement const* element = treeRoot->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
      // Ensure the node type is "e".
      const char* elementName = element->Name();
      if(strncmp(elementName, "e", strlen(elementName)) != 0) {
        continue;
      }

      // Get the name and read out data based on this.
      XMLAttribute const* nameAttribute = element->FindAttribute("name");
      if(nameAttribute == nullptr) {
        continue;
      }

      const char* attributeName = nameAttribute->Value();
      const char* elementValue = element->GetText();

      if(strncmp(attributeName, "session", strlen(attributeName)) == 0) {
        if(elementValue == nullptr) {
          return false;
        }

        fileInfo.Session = strtoul(elementValue, nullptr, 10);
      } else if(strncmp(attributeName, "split", strlen(attributeName)) == 0) {
        if(elementValue == nullptr) {
          return false;
        }

        fileInfo.Split = strtoul(elementValue, nullptr, 10);
      } else if(strncmp(attributeName, "comment", strlen(attributeName)) == 0) {
        if(elementValue == nullptr) {
          fileInfo.Comment[0] = '\0';
        } else {
          strncpy(fileInfo.Comment, elementValue, sizeof(fileInfo.Comment));
        }
      }
    }

    return true;
  }

}
