#ifndef TEST_SYNTAX_TEST_VIRTUAL
#define TEST_SYNTAX_TEST_VIRTUAL


#include <stdio.h>

// 虚表相关测试
// 内存布局
// GCC 查看内存布局方式 g++ -fdump-class-hierarchy file
// GDB 查看类内内存布局 p 类对象

class BaseVirtual {
public:
	virtual void show() {
		printf("BaseVirtual.show\n");
	}
	virtual void name() {
		printf("BaseVirtual.name\n");
	}
	void display() {
		printf("BaseVirtual.display\n");
	}
};

class BaseNode {
public:
	virtual void name() {
		printf("BaseNode.name\n");
	}
	virtual void age() {
		printf("BaseNode.age\n");
	}
};

class VirtualB : public BaseVirtual, BaseNode {
public:
	// 重写
	virtual void show() {
		printf("VirtualB.show\n");
	}
	virtual void age() {
		printf("VirtualB.age\n");
	}
	void name() {
		printf("VirtualB.name\n");
	}
};

class VirtualA : public BaseVirtual {
public:
	// 重写
	virtual void show() {
		printf("VirtualA.show\n");
	}
	// 父类隐藏
	void display() {
		printf("VirtualA.display\n");
	}
};

// 二次继承
class VirtualAa : virtual public VirtualA {
public:
	virtual void show() {
		printf("VirtualAa.show\n");
	}
};

void TestVirtual() { 
	{
		BaseVirtual base;
		VirtualA va;
		BaseVirtual* pBase = &va;
		pBase->show();   	// 利用虚表 掉用子类接口 
		pBase->name();   	// 子类未实现 利用虚表 掉用父类接口
		pBase->display();	// 非虚函数 掉用父类接口
	}
	{
		VirtualB vb;
		vb.name();          // 多继承同名函数会冲突 子类必须实现代码 
	}
	{
		VirtualAa vaa;
		vaa.show();
	}

	{
		// 多继承之菱形继承
		// 虚继承
		class BaseData {
		public:
			virtual void show() {}
			int32_t mA = 0;
		};
		class DataA : virtual public BaseData {
		public:
			virtual void showB() {}
			int32_t mB = 0;
		};
		class DataB : virtual public BaseData {
		public:
			virtual void showC() {}
			int32_t mC = 0;
		};

		class DataC : virtual public DataA, DataB {
		public:
		};

		DataC dc;
		dc.mA = 2;
		dc.show();
	}
}


#endif
