#ifndef TEST_SYNTAX_TEST_REFERENCE
#define TEST_SYNTAX_TEST_REFERENCE

#include <iostream>

#include <boost/type_index.hpp>

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

/*关键字:值类别(cppreference 查询)*/
/*泛左值, 纯右值,演化为 亡值,左值,右值*/

/*左值引用 右值引用 万能引用 引用折叠 完美转发测试*/
/*引用折叠(引用坍塌): T& & = T&
 *                    T& && = &
 *                    T&& & = &
 *                    T&& && = &&
 *  规则:引用的引用, 含有一次左值引用 推导结果则为左值引用
 * */

/*c++ 提供的typeid().name() 输出数据类型为简写且无法打印引用 不适于下面分析
 *所以采用 boost::typeindex::type_id_with_cvr<decltype(对象)>().pretty_name()
 * cvr 分别代表 const volatile reference
 * */

// 引用折叠:编译器允许出现引用的引用, 但是可以通过模板推导或者typedef构成引用的引用
typedef uint32& lRef;    // 左值引用类型
typedef uint32&& rRef;   // 右值引用类型

// 获取对象类型
#define GetValueName(val) \
    boost::typeindex::type_id_with_cvr<decltype(val)>().pretty_name().c_str()
// 获取模板类型
#define GetTemplateName(T) \
    boost::typeindex::type_id_with_cvr<T>().pretty_name().c_str()

// 左值引用函数模板测试(只能接受左值)
template<typename T>
void LeftReference(T& val) {
    log_info("左值引用 模板类型[%s] 对象类型[%s]", GetTemplateName(T), GetValueName(val));
}

// 右值引用测试(只能接受右值)
void RightReference(uint32&& val) {
    log_info("右值引用 对象类型[%s]", GetValueName(val));
}

// 万能引用测试(左右值都可)
template<typename T>
void AllReference(T&& val) {
    log_info("万能引用 模板类型[%s] 对象类型[%s]", GetTemplateName(T), GetValueName(val));
}

// 完美转发测试
void foo(uint32& index) {
    log_info("函数调用:左值引用");
}
void foo(uint32&& index) {
    log_info("函数调用:右值引用");
}
template<typename T>
void PerfertReference(T&& index) {
    foo(std::forward<T>(index));
}

void TestReference() {
    FUNC_TRACE;
    USING_NS_UTILS;

    // 基础测试
    {
        // 左值引用,右值引用都具有内存,所以都为左值
        uint32 num = 1;      // 左值
        uint32& num2 = num;  // 左值引用
        uint32&& num3 = 1;   // 右值引用
        log_info("Type num[%s] num2[%s] num3[%s]", GetValueName(num), GetValueName(num2), GetValueName(num3));
    }
    // 左值引用模板测试
    {
        uint32 index = 1;
        uint32& index2 = index;
        uint32&& index3 = 3;
        LeftReference(index);
        LeftReference(index2);
        LeftReference(index3);
        log_warn("左值引用模板测试: 左值引用,右值引用皆为左值");
        // LeftReference(2); 不可传入右值
    }
    // 右值引用测试
    {
        RightReference(2);
        uint32 index = 2;
        RightReference(std::move(index));
        log_warn("右值引用测试: 只可只使用纯右值");
    }
    // 万能引用测试
    {
        uint32 num = 1;
        AllReference(num);                    // T:uint32&, T&&:uint32& &&, valeType: uint32&
        AllReference<uint32&>(num);           // T:uint32&, T&&:uint32& &&, valeType: uint32&
        AllReference(uint32(2));              // T:uint32, T&&:uint32&&, valueType: uint32&&
        AllReference<uint32&&>(uint32(2));    // T:uint32&&, T&&:uint32&& &&, valueType: uint32&&
        log_warn("万能引用测试: 左右值皆可");
        // 以上引用推导的过程,为引用折叠
    }
    // 引用折叠(引用坍塌)测试
    {
        uint32 index = 3;
        log_info("引用折叠推导 uint32 => type[%s]", GetValueName(index));
        lRef& lIndex = index;
        log_info("引用折叠推导 uint32& & => type[%s]", GetValueName(lIndex));
        lRef&& lIndex2 = index;
        log_info("引用折叠推导 uint32& && => type[%s]", GetValueName(lIndex2));
        rRef& rIndex = index;
        log_info("引用折叠推导 uint32&& & => type[%s]", GetValueName(rIndex));
        rRef&& rIndex2 = 1;
        log_info("引用折叠推导 uint32&& && => type[%s]", GetValueName(rIndex2));
        log_warn("引用折叠规则测试");
    }
    // 完美转发测试
    {
        uint32 num = 1;
        PerfertReference(num);
        PerfertReference(2);
        log_warn("保证传递引用类型参数后，仍然维持原参数类型");
    }
}

#endif
