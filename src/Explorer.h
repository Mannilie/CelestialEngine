/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Explorer.h
@date: 23/07/2015
@author: Emmanuel Vaccaro
@brief: Allows for opening up and interacting
with the windows explorer.
===============================================*/

#ifndef _EXPLORER_H_
#define _EXPLORER_H_

// Other
#include <Windows.h>
#include <ShlObj.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
using std::string;

class Explorer {
public:
	static Explorer* instance;

	Explorer();
	~Explorer();
	static void Create();
	static void Shutdown();
	static void	OpenFileExplorer();
	static string OpenFileDialog(char* _filter = "All Files (*.*)\0*.*\0");
private:
	BROWSEINFO browseInfo;
	char pszBuffer[MAX_PATH];
	LPITEMIDLIST pidl;
	LPMALLOC lpMalloc;
};

#endif // _EXPLORER_H_