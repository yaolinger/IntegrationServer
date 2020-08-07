#ifndef UTILS_SQL_HPP
#define UTILS_SQL_HPP

#include <memory>
#include <string>
#include <vector>

#include "macros.hpp"

NS_UTILS_BEGIN

// sql字段
struct SQLField {
    uint32 type;          // 字段类型
    std::string name;     // 字段名称
};

// sql表语句
class SQLTable {
public:
    SQLTable(const std::string& table, const std::string& where, const std::vector<SQLField>& fieldVec);
    // 查询所有数据
    const std::string& selectAll();
    // 查询范围
    const std::string& selectLimit();
    // 查询数量
    const std::string& selectCount();
    // 条件查询
    const std::string& selectWhere();
    // 获取更新语句
    const std::string& update();
    // 获取插入语句
    const std::string& insert();

private:
    std::string m_table;
    std::string m_where;
    std::vector<SQLField> m_fieldVec;

    std::string m_selectAll;
    std::string m_selectLimit;
    std::string m_selectCount;
    std::string m_selectWhere;
    std::string m_update;
    std::string m_insert;
};
typedef std::shared_ptr<SQLTable> SQLTablePtr;

NS_UTILS_END

#endif
