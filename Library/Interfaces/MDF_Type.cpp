#include "MDF_Type.h"

namespace mdf {

    std::string typeToString(MDF_Type type) {
        std::string result;

        switch(type) {
            case MDF_Type_CC:
                result = "CC";
                break;
            case MDF_Type_CG:
                result = "CG";
                break;
            case MDF_Type_CN:
                result = "CN";
                break;
            case MDF_Type_DG:
                result = "DG";
                break;
            case MDF_Type_DT:
                result = "DT";
                break;
            case MDF_Type_DZ:
                result = "DT";
                break;
            case MDF_Type_FH:
                result = "FH";
                break;
            case MDF_Type_HD:
                result = "HD";
                break;
            case MDF_Type_ID:
                result = "ID";
                break;
            case MDF_Type_MD:
                result = "MD";
                break;
            case MDF_Type_SD:
                result = "SD";
                break;
            case MDF_Type_SI:
                result = "SI";
                break;
            case MDF_Type_TX:
                result = "TX";
                break;
            default:
                result = "Invalid";
                break;
        }

        return result;
    }

}
