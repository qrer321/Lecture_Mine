#pragma once
#include <GameServerBase/GameServerObjectBase.h>

// �뵵 : 
// �з� :
// ÷�� : 
class TestClass : public GameServerObjectBase
{
private: // Member Var

public: // Default
	TestClass();
	~TestClass();

	TestClass(const TestClass& _Other) = delete;
	TestClass(TestClass&& _Other) noexcept;

protected:
	TestClass& operator=(const TestClass& _Other) = delete;
	TestClass& operator=(TestClass&& _Other) = delete;

private:

public: // Member Function
};

