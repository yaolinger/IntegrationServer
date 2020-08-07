#include "sql.hpp"

#include <sstream>

#include "db_base.hpp"

NS_UTILS_BEGIN

SQLTable::SQLTable(const std::string& table, const std::string& where, const std::vector<SQLField>& fieldVec)
    : m_table(table), m_where(where), m_fieldVec(fieldVec) {
    std::ostringstream oss;

    // selectAll;
    oss << "select ";
    for (uint32 i = 0; i < fieldVec.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss <<fieldVec[i].name;
    }
    oss << " from " << m_table << ";";
    m_selectAll = oss.str();
    oss.str("");

    // selectLimit
    oss << "select ";
    for (uint32 i = 0; i < fieldVec.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss <<fieldVec[i].name;
    }
    oss << " from " << m_table << " limit %u, %u;";
    m_selectLimit = oss.str();
    oss.str("");

    // selectCount;
    oss << "select count(*) as num from " << m_table << ";";
    m_selectCount = oss.str();
    oss.str("");

    // selectWhere
    oss << "select ";
    for (uint32 i = 0; i < fieldVec.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss <<fieldVec[i].name;
    }
    oss << " from " << m_table << " where " << m_where << ";";
    m_selectWhere = oss.str();
    oss.str("");

    // update
    oss << "update " << m_table << " set ";
    for (uint32_t i = 0; i < fieldVec.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss << "set " << fieldVec[i].name << "=" << getTypeFormatParam(fieldVec[i].type);
    }
    oss << " where " << m_where << ";";
    m_update = oss.str();
    oss.str("");

    // insert
    oss << "insert into " << m_table << " (";
    for (uint32 i = 0; i < fieldVec.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss <<fieldVec[i].name;
    }
    oss << ") values(";
    for (uint32 i = 0; i < fieldVec.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss << getTypeFormatParam(fieldVec[i].type);
    }
    oss << ")";
    m_insert = oss.str();
    oss.str("");
}

const std::string& SQLTable::selectAll() {
    return m_selectAll;
}

const std::string& SQLTable::selectLimit() {
    return m_selectLimit;
}

const std::string& SQLTable::selectCount() {
    return m_selectCount;
}

const std::string& SQLTable::selectWhere() {
    return m_selectWhere;
}

const std::string& SQLTable::update() {
    return m_update;
}

const std::string& SQLTable::insert() {
    return m_insert;
}

NS_UTILS_END
