#pragma once
#include <windows.h>

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "stdafx.h"
#include "Drive.h"

class ListView {
	HINSTANCE	hInst;
	HWND		hParent;
	HWND		hListView;
	int			ID;
public:
	ListView();
	~ListView();
	void Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth,int nHeight, int x );

	HWND	GetHandle();
	LPCWSTR GetCurSelPath();

	void LoadFolderInComputer();
	void LoadMyComputer(Drive *drive);
	void LoadChild(LPCWSTR path, Drive *drive);
	void LoadFileAndFolder(LPCWSTR path);
	void LoadCurSel();
	void DeleteAll();
};