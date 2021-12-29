// GameThread.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <process.h>    // 윈도우용 스레드 헤더
#include <thread>       // 범용 스레드 헤더
#include <functional>
#include <Windows.h>

/*
 * __stdcall 호출 규칙은 Win32 API 함수를 호출하는 데 사용됩니다.
 * 호출 수신자가 스택을 정리하므로 컴파일러는 vararg 함수를 __cdecl로 만듭니다.
 * 이 호출 규칙을 사용하는 함수에는 함수 프로토타입이 필요합니다.
 * 여기서 호출 수신자란 피 호출자(Callee)를 말합니다.
 *
 * total = Mul(a, b);				-> Mul을 호출하기에 호출자. Caller
 * int Mul(int a, int b) { ... }	-> 호출되기 때문에 피 호출자. Callee
 * __stdcall은 피 호출자가 stack을 정리합니다.
 *
 *
 * _beginthreadex의 매개변수 _beginthreadex_proc_type는 이렇게 정의되어 있다
 * unsigned (__stdcall* _beginthreadex_proc_type)(void*);
 */
static unsigned __stdcall ThreadFunc(void* arg)
{
	while (true)
	{
		std::cout << "window Thread Process" << std::endl;
		//Sleep(1);	// Windows.h 함수
	}
}

void Func()
{
	while (true)
	{
		std::cout << "std Thread Process" << std::endl;
		//Sleep(1);
	}
}

void TestFunc0(int a) { }

class A
{
public:
	A()
	{
		
	}

	A(void (*Ptr)())
	{

	}

	A(void (*Ptr)(int))
	{

	}

	A(void (*Ptr)(char))
	{

	}

	A(void (*Ptr)(char, int))
	{

	}

};

int main()
{
	// void 함수 포인터
	A newA0 = A();
	A newA1 = A(Func);
	
	// 윈도우의 스레드 함수
	//HANDLE ThreadHandle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, ThreadFunc, nullptr, 0, nullptr));

	// std:: -> 스탠다드 라이브러리
	// C++가 빌드 되는 곳이라면 std를 사용해서 짠 코드는
	// 리눅스건 윈도우건 동일하게 작동한다

	// thread 클래스에 들어가는 함수는 정확히 무엇때문에 그냥 집어넣어도 동작하는 것이였는가?
	std::thread NewThread0 = std::thread(Func);					// 일반 함수
	std::thread NewThread1 = std::thread(TestFunc0);				// 매개변수를 받는 함수
	std::thread NewThread2 = std::thread(std::bind(Func));	// std::bind

	std::forward;

	// std::bind
	// bind란 함수의 일부 매개 변수를 고정 값으로 세팅한 후, 한 번 Wrapping 하여 사용할 수 있게 해주는 함수

	// 스레드란 무엇인가?
	// 동시에 2개 이상의 함수가 돌아가는 것
	// 운영체제에게 요청하여 main 함수가 메인 스레드에서 돌고있지만
	// 내가 요청한 Func( ) 함수도 다른 스레드에서 같이 돌려준다
	std::cout << "main thread" << std::endl;

	// @param
	// HANDLE hHandle		 : Event Object Handle
	// DWORD  dwMilliseconds : time-out interval.
	//						   INFINITE로 설정할 경우, 무한정 기다린다.
	//
	// main 함수가 종료됐더라도 해당 스레드가 종료될 때 까지 기다린다
	// 따라서 실제로 돌려보면 INFINITE로 설정했고 해당 스레드는 while(true)로 돌기 때문에
	// 프로그램이 종료되지 않는다.
	//WaitForSingleObject(ThreadHandle, INFINITE);


	// 우아한 종료. Graceful Shutdown
	// 메인 스택은 이미 끝나서 사라지게 되지만
	// 기존에 돌려놨던 스레드는 계속해서 돌아간다
	// 스레드는 기본적으로 자신이 만들어졌던 프로그램에 귀속되고 영향을 받으므로
	// main( ) 함수가 종료되서 프로그램이 종료되었을 때
	// 남아있는 스레드가 있다면 에러가 나게 된다
	NewThread0.join();
}