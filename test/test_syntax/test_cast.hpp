#ifndef TEST_SYNTAX_TEST_CAST
#define TEST_SYNTAX_TEST_CAST

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

/* 转换语法测试 */

class Base {
public:
    Base(uint32 index) : m_index(index) {
    }

    void nothing() {};

    uint32 m_index;
};


// 禁止隐式转换
class BanImplicit {
public:
    explicit BanImplicit(uint32 index) : m_index(index) {
    }

    void nothing() {};

    uint32 m_index;
};

// 其他类
class Other {
};
// 父类
class Father {
public:
    virtual ~Father() {}
};
// 子类
class Son : public Father {
public:
    ~Son() {}
};

void TestCast() {
    FUNC_TRACE;
    USING_NS_UTILS;
    // 基础测试
    {
        Base data = 1; // 隐式转换
        Base data2 = (Base)1; // 显示转换
        data.nothing();
        data2.nothing();
    }

    // 禁止隐式转换测试
    {
        BanImplicit data(1);
        BanImplicit data2 = (BanImplicit)1;
        //BanImplicit data = 1;
        data.nothing();
        data2.nothing();
    }
    // static_cast(cpp通用转换)
    {
        Father fa;
        Son so;
        int index = 1;
        Father* pFa = static_cast<Father*>(&so);
        Son* pSo = static_cast<Son*>(&fa); // 不安全(父类实体不具有子类的新数据 转换调用可能回引起未知错误)
        void* pAddr = static_cast<void*>(&index);
        // Other* pOt = static_cast<Other*>(&so); // 不可转换,无上下关系
        // cv 限定符不可以被转换掉如: const int => int
        // 支持派生转基类
        // 支持基类转派生 不进行动态检测 易出问题
        // 基础类型转换
    }
    // dynamic_cast(多态)
    {
        Father fa;
        Son so;
        Father* pFa = dynamic_cast<Father*>(&so);
        Son* pSo = dynamic_cast<Son*>(&fa);
        if (pSo == NULL) {
            log_error("pSon == NULL");
        }
        pSo = dynamic_cast<Son*>(pFa);
        if (!pSo) {
            log_warn("指向派生类实体的基类指针才可以转换成派生类指针");
        }
        try {
            Father& fb = fa;
            Son& sb = dynamic_cast<Son&>(fb);
        } catch(const std::exception& e) {
            log_error("dynamic_cast error:%s", e.what());
        }
        // virtual函数支持多态的继承关系类, 只能转换指针或者引用
        // 支持派生类转基类
        // 支持基类转派生(动态类型检测(根据虚函数表))
        // 指针转换失败会返回null, 引用转换会直接抛异常std::bad_cast
    }

    // const_cast(去除cv限定符)
    {
        const Father fa;
        Father* fb = const_cast<Father*>(&fa);
        volatile Son so;
        Son* sb = const_cast<Son*>(&so);
        const uint32& index = 2;
        uint32& num = const_cast<uint32&>(index);
        //Son* sc = const_cast<Son*>(&fa); 只支持同类型转换
        // 支持同类型的指针或者引用的去cv限定符(const volatile)转换
    }

    // reinterpret_cast(个人感觉类似于内存拷贝)
    {
        double d = 12.1;
        uint32* pIndex = reinterpret_cast<uint32*>(&d);
        char* count = reinterpret_cast<char*>(pIndex);
        // 它对原始对象的位模式提供较低层次上的重新解释（即reinterpret），完全复制二进制比特位到目标对象，转换后的值与原始对象无关但比特位一致，前后无精度损失
        // 危险转换 可以将完全无关系的类型互相转换
    }
}

#endif
