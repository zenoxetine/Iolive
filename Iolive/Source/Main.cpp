#include <windows.h>
#include "Application.hpp"

#if IOLIVE_DEBUG == 0
INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPWSTR, INT)
{

#else
int main()
{
#endif

	Iolive::Application::Get()->Run();

	Iolive::Application::Release();

	return 0;
}