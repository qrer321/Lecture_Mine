#pragma once
#include <GameServerBase/GameServerObjectBase.h>

// 용도 : 
// 분류 :
// 첨언 : 
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

