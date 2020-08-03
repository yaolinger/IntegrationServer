#include "sql.hpp"

#include <sstream>

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
    oss << "from " << m_table << ";";
    m_selectAll = oss.str();
    oss.clear();

    // selectLimit
    oss << "select ";
    for (uint32 i = 0; i < fieldVec.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss <<fieldVec[i].name;
    }
    oss << "from " << m_table << " limit ?, ?;";
    m_selectLimit = oss.str();
    oss.clear();

    // selectCount;
    oss << "select count(*) from " << m_table << ";";
    m_selectCount = oss.str();
    oss.clear();

    // selectWhere
    oss << "select ";
    for (uint32 i = 0; i < fieldVec.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss <<fieldVec[i].name;
    }
    oss << "from " << m_table << " where " << m_where << ";";
    m_selectWhere = oss.str();
    oss.clear();

    // update
    oss << "update " << m_table << " set ";
    for (uint32_t i = 0; i < fieldVec.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss << "set " <<fieldVec[i].name <<"=?";
    }
    oss << " where " << m_where << ";";
    m_update = oss.str();
    oss.clear();

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
        oss << "?";
    }
    oss << ")";
    m_insert = oss.str();
    oss.clear();
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
