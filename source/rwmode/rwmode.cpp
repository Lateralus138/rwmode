//////////////////////////////////////////////////
// RW Mode - Set a files write mode in          //
// Windows; read only or writable. Defaults to  //
// read only.                                   //
// © 2022 Ian Pride - New Pride Software /      //
// Services                                     //
//////////////////////////////////////////////////
#include "Functions.h"
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <regex>
#include <signal.h>
#include <map>
int main(int argc, char* argv[])
{
	signal(SIGINT, signal_ctrl_c_handler);
	DWORD
		dwProcessIdMain,
		dwProcessIdThis = GetCurrentProcessId();
	HWND conHwnd = GetConsoleWindow();
	GetWindowThreadProcessId(conHwnd, &dwProcessIdMain);
	const bool isOwnConWin = (dwProcessIdThis == dwProcessIdMain);
	const std::string
		TITLE = "RW Mode",
		TITLE_ESC = "\x1b]0;" + TITLE + "\007";
	auto waitIfOwned = [isOwnConWin]()
	{
		if (isOwnConWin)
		{
			std::cout << "Press [Enter] to continue... > ";
			std::cin.get();
			std::cout << std::endl;
		}
	};
	std::filesystem::path fileMainPath;
	const std::regex
		RGX_HELP("^/([hH]|[hH][eE][lL][pP]|\\?)$"),
		RGX_FILE("^/([fF]|[fF][iI][lL][eE])$"),
		RGX_TOGGLE("^/([tT]|[tT][oO][gG][gG][lL][eE])$"),
		RGX_READ("^/([rR]|[rR][eE][aD][dD])$"),
		RGX_WRITE("^/([wW]|[wW][rR][iI][tT][eE])$");
	const std::vector <std::string> BOX =
	{
		u8"\u250C", u8"\u2500", u8"\u2510", u8"\u2502",
		u8"\u2514", u8"\u2518", u8"\u251C", u8"\u2524"
	};
	const std::string CR = u8"\u00A9";
	std::string box_line;
	for (auto i = 0; i < 48; ++i)
	{
		box_line += BOX[1];
	}
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	std::cout << TITLE_ESC;
	if (isOwnConWin)
	{
		SetWindowTextA(conHwnd, TITLE.c_str());
	}
	bool
		isFileRegularFile = false,
		isReadOnly = false;
	std::map<std::string, int> modes
	{
		{"Verbose",	0},
		{"Toggle",	1},
		{"Read",		2},
		{"Write",		3}
	};	int mode = modes["Verbose"];
	if (argc > 1)
	{
		bool skip = false;
		std::string thisArg;
		for (auto i = 1; i < argc; i++)
		{
			if (skip)
			{
				skip = false;
				continue;
			}
			thisArg = argv[i];
			if (std::regex_match(thisArg, RGX_HELP))
			{
				std::string help_message;
				help_message.append
				(
					'\n' +
					BOX[0] + box_line + BOX[2] + '\n' +
					BOX[3] + "                    RW Mode                     " + BOX[3] + '\n' +
					BOX[6] + box_line + BOX[7] + '\n' +
					BOX[3] + " Read, toggle, or set the Read/Write mode of a  " + BOX[3] + '\n' +
					BOX[3] + " file in Windows.                               " + BOX[3] + '\n' +
					BOX[6] + box_line + BOX[7] + '\n' +
					BOX[3] + " rwmode [/H] [/F FILE] [/T] [/R] [/W] FILE      " + BOX[3] + '\n' +
					BOX[6] + box_line + BOX[7] + '\n' +
					BOX[3] + " /h,/HELP,/?     This help screen.              " + BOX[3] + '\n' +
					BOX[3] + " /f,/FILE        Path to the file. This switch  " + BOX[3] + '\n' +
					BOX[3] + "                 is not necessary as the path   " + BOX[3] + '\n' +
					BOX[3] + "                 can be passed as a normal      " + BOX[3] + '\n' +
					BOX[3] + "                 argument.                      " + BOX[3] + '\n' +
					BOX[3] + " /t,/TOGGLE      Toggle a file's R/W mode.      " + BOX[3] + '\n' +
					BOX[3] + " /r,/READ        Set a file to Read Only.       " + BOX[3] + '\n' +
					BOX[3] + " /w,/WRITE       Set a file to Writeable.       " + BOX[3] + '\n' +
					BOX[6] + box_line + BOX[7] + '\n' +
					BOX[3] + " " + CR + " 2022 Ian Pride - New Pride Software/Services " + BOX[3] + '\n' +
					BOX[4] + box_line + BOX[5] + "\n\n"
				);
				std::cout << help_message;
				waitIfOwned();
				return 0;
			}
			if (std::regex_match(thisArg, RGX_FILE))
			{
				skip = true;
				if ((i + 1) < argc)
				{
					std::string nextArg = argv[i + 1];
					if (nextArg.empty())
					{
						std::cerr << "Argument provided for '" << thisArg << "' is empty.\n\n";
						waitIfOwned();
						return 2;
					}
					fileMainPath = std::filesystem::path(nextArg);
				}
				else
				{
					std::cerr << "No argument provided for '" << thisArg << "'.\n\n";
					waitIfOwned();
					return 1;
				}
				continue;
			}
			if (std::regex_match(thisArg, RGX_TOGGLE))
			{
				mode = modes["Toggle"];
				continue;
			}
			if (std::regex_match(thisArg, RGX_READ))
			{
				mode = modes["Read"];
				continue;
			}
			if (std::regex_match(thisArg, RGX_WRITE))
			{
				mode = modes["Write"];
				continue;
			}
			if (!thisArg.empty())
			{
				fileMainPath = std::filesystem::path(thisArg);
			}
		}
	}
	std::error_code ec;
	isFileRegularFile = std::filesystem::is_regular_file(fileMainPath, ec);
	if (!isFileRegularFile)
	{
		const int ecValue = (int)ec.value();
		if (ecValue > 0)
		{
			std::cerr << '[' << ecValue << "]: " << ec.message() << '\n';
		}
		std::cerr << fileMainPath << " is not a regular file.\n";
		waitIfOwned();
		return 3;
	}
	auto file = fileMainPath.wstring();
	DWORD
		fileAttributes = GetFileAttributesW(file.c_str()),
		fileAttributesFlipped;
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		std::cerr << "Invalid file attributes for " << fileMainPath << '\n';
		waitIfOwned();
		return 4;
	}
	isReadOnly = (fileAttributes & FILE_ATTRIBUTE_READONLY);
	fileAttributesFlipped =
		isReadOnly ?
		(fileAttributes + FILE_ATTRIBUTE_READONLY) :
		(fileAttributes - FILE_ATTRIBUTE_READONLY);
	std::cout << fileMainPath << " is currently: [" << (isReadOnly ? "Read Only" : "Writeable") << "].\n";
	int setFile = 0;
	switch (mode)
	{
	case 1:
		std::cout << "Attempting to toggle the Read/Write mode of " << fileMainPath << '\n';
		setFile = SetFileAttributesW(file.c_str(), fileAttributesFlipped);
		if (setFile == 0)
		{
			std::cerr << GetLastErrorString();
			return setFile;
		}
		fileAttributes = GetFileAttributesW(file.c_str());
		if (fileAttributes == INVALID_FILE_ATTRIBUTES)
		{
			std::cerr << "Invalid file attributes for " << fileMainPath << '\n';
			waitIfOwned();
			return 5;
		}
		isReadOnly = (fileAttributes & FILE_ATTRIBUTE_READONLY);
		std::cout << fileMainPath << " is currently: [" << (isReadOnly ? "Read Only" : "Writeable") << "].\n";
		waitIfOwned();
		return 0;
		break;
	case 2:
		if (!isReadOnly)
		{
			std::cout << "Attempting to set the Read/Write mode of " << fileMainPath << " to [Read Only].\n";
			setFile = SetFileAttributesW(file.c_str(), fileAttributesFlipped);
			if (setFile == 0)
			{
				std::cerr << GetLastErrorString();
				return setFile;
			}
			fileAttributes = GetFileAttributesW(file.c_str());
			if (fileAttributes == INVALID_FILE_ATTRIBUTES)
			{
				std::cerr << "Invalid file attributes for " << fileMainPath << '\n';
				waitIfOwned();
				return 6;
			}
			isReadOnly = (fileAttributes & FILE_ATTRIBUTE_READONLY);
			std::cout << fileMainPath << " is currently: [" << (isReadOnly ? "Read Only" : "Writeable") << "].\n";
		}
		else
		{
			std::cout << fileMainPath << " is already [Read Only] and does not need to be set.\n";
		}
		waitIfOwned();
		return 0;
		break;
	case 3:
		if (isReadOnly)
		{
			std::cout << "Attempting to set the Read/Write mode of " << fileMainPath << " to [Writeable].\n";
			setFile = SetFileAttributesW(file.c_str(), fileAttributesFlipped);
			if (setFile == 0)
			{
				std::cerr << GetLastErrorString();
				return setFile;
			}
			fileAttributes = GetFileAttributesW(file.c_str());
			if (fileAttributes == INVALID_FILE_ATTRIBUTES)
			{
				std::cerr << "Invalid file attributes for " << fileMainPath << '\n';
				waitIfOwned();
				return 7;
			}
			isReadOnly = (fileAttributes & FILE_ATTRIBUTE_READONLY);
			std::cout << fileMainPath << " is currently: [" << (isReadOnly ? "Read Only" : "Writeable") << "].\n";
		}
		else
		{
			std::cout << fileMainPath << " is already [Writeable] and does not need to be set.\n";
		}
		waitIfOwned();
		return 0;
		break;
	default:
		break;
	}
	waitIfOwned();
	return 0;
}