#include "reactor_unit.hpp"

NS_UTILS_BEGIN

void ReactorUnit::complete() {
    m_func(m_error, m_transBytes);
}

NS_UTILS_END
