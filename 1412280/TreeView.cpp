
#include "TreeView.h"


#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

TreeView::TreeView()
{
	hInst = NULL;
	hParent = NULL;
	hTreeView = NULL;
	ID = 0;
}

TreeView::~TreeView()
{
	DestroyWindow(hTreeView);
}

//create tree view
void TreeView::Create(HWND parentHwnd, long ID, HINSTANCE hParentInst, int nWidth, int nHeight)
{
	InitCommonControls();
	TreeView::hParent = parentHwnd;
	TreeView::hInst = hParentInst;
	TreeView::hTreeView = CreateWindowEx(0, WC_TREEVIEW, _T("Tree View"),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_VSCROLL | WS_TABSTOP | TVS_HASLINES | 
		TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CW_USEDEFAULT, 0, nWidth, nHeight, parentHwnd,
		(HMENU)ID, hParentInst, NULL);
	TreeView::ID = ID;
}

LPCWSTR TreeView::GetPath(HTREEITEM hItem)
{
	TVITEMEX tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(hTreeView, &tv);
	return (LPCWSTR)tv.lParam;
}

LPCWSTR TreeView::GetCurPath()
{
	return GetPath(GetCurSel());
}

HTREEITEM TreeView::GetCurSel()
{
	return TreeView_GetNextItem(hTreeView, NULL, TVGN_CARET);
}

HTREEITEM TreeView::GetMyComputer()
{
	return TreeView_GetRoot(hTreeView);
}

/*****************************************************************************************/

void TreeView::LoadMyComputer(Drive *drive)
{
	TV_INSERTSTRUCT tvInsert;
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;

	//My Computer
	tvInsert.hParent = NULL;
	tvInsert.item.pszText = _T("This PC");
	tvInsert.item.lParam = (LPARAM)_T("This PC");
	HTREEITEM hMyComputer = TreeView_InsertItem(hTreeView, &tvInsert);

	//Desktop
	tvInsert.hParent = hMyComputer;
	tvInsert.item.pszText = _T("Desktop");
	tvInsert.item.lParam = (LPARAM)_T("Desktop");
	TreeView_InsertItem(hTreeView, &tvInsert);

	//Documents
	tvInsert.item.pszText = _T("Documents");
	tvInsert.item.lParam = (LPARAM)_T("Documents");
	TreeView_InsertItem(hTreeView, &tvInsert);

	//Downloads
	tvInsert.item.pszText = _T("Downloads");
	tvInsert.item.lParam = (LPARAM)_T("Downloads");
	TreeView_InsertItem(hTreeView, &tvInsert);

	//Music
	tvInsert.item.pszText = _T("Music");
	tvInsert.item.lParam = (LPARAM)_T("Music");
	TreeView_InsertItem(hTreeView, &tvInsert);

	//Pictures
	tvInsert.item.pszText = _T("Pictures");
	tvInsert.item.lParam = (LPARAM)_T("Pictures");
	TreeView_InsertItem(hTreeView, &tvInsert);

	//Videos
	tvInsert.item.pszText = _T("Videos");
	tvInsert.item.lParam = (LPARAM)_T("Videos");
	TreeView_InsertItem(hTreeView, &tvInsert);

	for (int i = 0; i < drive->GetCount(); ++i)
	{
		tvInsert.hParent = hMyComputer; 
		tvInsert.item.pszText = drive->GetDisplayName(i);
		tvInsert.item.lParam = (LPARAM)drive->GetDriveName(i);
		HTREEITEM hDrive = TreeView_InsertItem(hTreeView, &tvInsert);

		//Preload
		if ((drive->GetIconIndex(i) == IDI_HDD) || (drive->GetIconIndex(i) == IDI_USB))
		{
			tvInsert.hParent = hDrive;
			tvInsert.item.pszText = _T("PreLoad");
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(hTreeView, &tvInsert);
		}
	}

	 //default show list disk and first folder
	TreeView_Expand(hTreeView, hMyComputer, TVE_EXPAND);
	TreeView_SelectItem(hTreeView, hMyComputer);
}

//load child folder or file  of father folder 
void TreeView::LoadChild(HTREEITEM &hParent, LPCWSTR path, BOOL bShowHiddenSystem)
{
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, path);

	StrCat(buffer, _T("\\*"));

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;


	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);
	BOOL bFound = 1;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	TCHAR * folderPath;
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			tvInsert.item.pszText = fd.cFileName;
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];

			StrCpy(folderPath, path);
			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));
			StrCat(folderPath, fd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;
			HTREEITEM hItem = TreeView_InsertItem(hTreeView, &tvInsert);
			//Preload
			PreLoad(hItem);
		}

		bFound = FindNextFileW(hFile, &fd);
	}
}

//when click '+' or '-'
void TreeView::PreloadExpanding(HTREEITEM hPrev, HTREEITEM hCurSel)
{
	if (hCurSel == GetMyComputer())
		return;

	HTREEITEM hCurSelChild = TreeView_GetChild(hTreeView, hCurSel);

	if (!StrCmp(GetPath(hCurSelChild), _T("PreLoad")))
	{
		TreeView_DeleteItem(hTreeView, hCurSelChild);
		LoadChild(hCurSel, GetPath(hCurSel));
	}
}

void TreeView::PreLoad(HTREEITEM hItem)
{
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, GetPath(hItem));

	if (wcslen(buffer) == 3)
	{
		// is disk soft
		if (StrStr(buffer, _T("A:")) || StrStr(buffer, _T("B:")))
			return;
	}
	else
		StrCat(buffer, _T("\\"));

	StrCat(buffer, _T("*"));

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	BOOL bFound = true;

	//find folder
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			TV_INSERTSTRUCT tvInsert;
			tvInsert.hParent = hItem;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
			tvInsert.item.pszText = NULL;
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(hTreeView, &tvInsert);
			bFound = FALSE;
		}
		else
			bFound = FindNextFileW(hFile, &fd);
	}
}