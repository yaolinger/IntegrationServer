#include "db_base.hpp"

NS_UTILS_BEGIN

const char * getTypeFormatParam(uint32_t type) {
    std::string param;
    switch (type) {
        case DATA_TYPE_STRING:
        case DATA_TYPE_BLOB:
            param = "%s";
            break;
        case DATA_TYPE_INT:
            param = "%d";
            break;
        case DATA_TYPE_LLONG:
            param = "%ld";
            break;
        case DATA_TYPE_UINT:
            param = "%u";
            break;
        case DATA_TYPE_ULLONG:
            param = "%lu";
            break;
        default:
            // TODO:其他类型暂时不处理
            param = "%$";
            break;
    }
    return param.c_str();
}

NS_UTILS_END
