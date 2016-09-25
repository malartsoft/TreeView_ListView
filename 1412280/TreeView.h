#pragma once
#include "stdafx.h"
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include "Drive.h"

#define IDI_HDD 6
#define IDI_USB 5
#define MAX_PATH_LEN 10240

class TreeView {
	HINSTANCE hInst;
	HWND		hParent;
	HWND		hTreeView;
	int			ID;
public:
	TreeView();
	~TreeView();

	void Create(HWND parentHwnd, long ID, HINSTANCE hParentInst, int nWidth, int nHeight);

	LPCWSTR		GetPath(HTREEITEM hItem);
	LPCWSTR		GetCurPath();
	HTREEITEM	GetCurSel();

	HTREEITEM	GetMyComputer();

	void LoadMyComputer(Drive *drive);
	void LoadChild(HTREEITEM &hParent, LPCWSTR path, BOOL bShowHiddenSystem = FALSE);
	void PreloadExpanding(HTREEITEM hPrev, HTREEITEM hCurSel);
	void PreLoad(HTREEITEM hItem);
};