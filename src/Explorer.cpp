#include "Explorer.h"

// Utilities
#include "GLFW_Header.h"
#include "Window.h"

// Other
#include <iostream>

Explorer* Explorer::instance = nullptr;

Explorer::Explorer(){}
Explorer::~Explorer(){}

void Explorer::Create() {
	if (instance == nullptr) {
		instance = new Explorer();
	}
}
void Explorer::Shutdown() {
	delete instance;
}
int CALLBACK BrowseForFolderCallback(HWND _hwnd, UINT _uMsg, LPARAM _lp, LPARAM _pData) {
	char szPath[MAX_PATH];

	switch (_uMsg) {
	case BFFM_INITIALIZED:
		SendMessage(_hwnd, BFFM_SETSELECTION, TRUE, _pData);
		break;
	case BFFM_SELCHANGED:
		if (SHGetPathFromIDList((LPITEMIDLIST)_lp, szPath)) {
			SendMessage(_hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szPath);

		}
		break;
	}
	return 0;
}
void Explorer::OpenFileExplorer() {
	// Initialize COM
	if (CoInitializeEx(0, COINIT_APARTMENTTHREADED) != S_OK) {
		MessageBox(NULL, _T("Error opening browse window"), _T("ERROR"), MB_OK);
		CoUninitialize();
		return;
	}
	// Get a pointer to the shell memory allocator
	if (SHGetMalloc(&instance->lpMalloc) != S_OK) {
		MessageBox(NULL, _T("Error opening browse window"), _T("ERROR"), MB_OK);
		CoUninitialize();
		return;
	}
	BROWSEINFO& bi = instance->browseInfo;
	LPITEMIDLIST& pidl = instance->pidl;
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = instance->pszBuffer;
	bi.lpszTitle = _T("Select a install Directory");
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	if (pidl = SHBrowseForFolder(&bi)) {
		// Copy the path directory to the buffer
		if (SHGetPathFromIDList(pidl, instance->pszBuffer)) {
			// pszBuffer now holds the directory path
			printf(_T("You selected the directory: %s\n"), instance->pszBuffer);
		}
		instance->lpMalloc->Free(pidl);
	}
	instance->lpMalloc->Release();
	CoUninitialize();
}
string Explorer::OpenFileDialog(char* _filter) {
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window(Window::window);
	ofn.lpstrFilter = (LPSTR)_filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = "";
	if (GetOpenFileName(&ofn)) {
		return fileName;
	}
	return string("");
}