#include "ListView.h"
#include "resource.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

ListView::ListView()
{
	hInst = NULL;
	hParent = NULL;
	hListView = NULL;
	ID = 0;
}

ListView::~ListView()
{
	DestroyWindow(hListView);
}

void ListView::Create(HWND parentWnd, long ID, HINSTANCE hParentInst,int nWidth, int nHeight,int x)
{
	InitCommonControls();
	ListView::hInst = hParentInst;
	ListView::hParent = parentWnd;
	ListView::hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, _T("List View"),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS,
		x, 0, nWidth, nHeight, hParent, (HMENU)ID, hInst, NULL);
	ListView::ID = ID;
}

HWND ListView::GetHandle()
{
	return hListView;
}

LPCWSTR ListView::GetCurSelPath()
{
	LVITEM lv;
	lv.mask = LVIF_PARAM;
	lv.iItem = ListView_GetSelectionMark(hListView);
	lv.iSubItem = 0;
	ListView_GetItem(hListView, &lv);
	return (LPCWSTR)lv.lParam;
}

void ListView::LoadFolderInComputer()
{
	LV_ITEM lv;
	lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lv.iItem = 0;
	lv.iSubItem = 0;

	lv.pszText = _T("Videos");
	lv.lParam = (LPARAM)_T("Videos");
	ListView_InsertItem(hListView, &lv);

	lv.pszText = _T("Pictures");
	lv.lParam = (LPARAM)_T("Pictures");
	ListView_InsertItem(hListView, &lv);

	lv.pszText = _T("Music");
	lv.lParam = (LPARAM)_T("Music");
	ListView_InsertItem(hListView, &lv);

	lv.pszText = _T("Downloads");
	lv.lParam = (LPARAM)_T("Downloads");
	ListView_InsertItem(hListView, &lv);

	lv.pszText = _T("Documents");
	lv.lParam = (LPARAM)_T("Documents");
	ListView_InsertItem(hListView, &lv);

	lv.pszText = _T("Desktop");
	lv.lParam = (LPARAM)_T("Desktop");
	ListView_InsertItem(hListView, &lv);
}
void ListView::LoadMyComputer(Drive *drive)
{
	LV_ITEM lv;
	for (int i = 0; i < drive->GetCount(); ++i)
	{
		//list item
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lv.iItem = i;

		lv.iSubItem = 0;
		lv.pszText = drive->GetDisplayName(i);
		lv.lParam = (LPARAM)drive->GetDriveName(i);
		ListView_InsertItem(hListView, &lv);
	}
	LoadFolderInComputer();
}

void ListView::LoadChild(LPCWSTR path, Drive *drive)
{
	if (path == NULL)
		return;

	if (!StrCmp(path, _T("This PC"))) {
		LoadMyComputer(drive);
	}
	else
		LoadFileAndFolder(path);
}

void ListView::LoadFileAndFolder(LPCWSTR path)
{
	TCHAR buffer[10240];
	StrCpy(buffer, path);

	if (wcslen(path) == 3) 
		StrCat(buffer, _T("*"));
	else
		StrCat(buffer, _T("\\*"));

	//find file and folder in father folder
	WIN32_FIND_DATA fd;
	HANDLE hFile;
	BOOL bFound = true;
	LV_ITEM lv;

	TCHAR * folderPath;
	int nItemCount = 0;

	//get folder
	hFile = FindFirstFileW(buffer, &fd);
	bFound = TRUE;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN) &&
			(StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(folderPath, path);

			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));

			StrCat(folderPath, fd.cFileName);

			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = nItemCount;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)folderPath;
			ListView_InsertItem(hListView, &lv);

			++nItemCount;
		}

		//find next
		bFound = FindNextFileW(hFile, &fd);
	}

	DWORD folderCount = nItemCount;

	//get file
	TCHAR *filePath;
	DWORD fileSizeCount = 0;
	DWORD fileCount = 0;

	hFile = FindFirstFileW(buffer, &fd);
	bFound = TRUE;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	while (bFound)
	{
		if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))
		{
			filePath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(filePath, path);

			if (wcslen(path) != 3)
				StrCat(filePath, _T("\\"));

			StrCat(filePath, fd.cFileName);

			//show name folder or file
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = nItemCount;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)filePath;
			ListView_InsertItem(hListView, &lv);

			++nItemCount;
			++fileCount;
		}

		//find next
		bFound = FindNextFileW(hFile, &fd);
	}
}

//run file
void ListView::LoadCurSel()
{
	LPCWSTR path = GetCurSelPath();
	if (path == (LPCWSTR)0xcccccccc)
		return;
	WIN32_FIND_DATA fd;
	GetFileAttributesEx(path, GetFileExInfoStandard, &fd);
	//is folder
	if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		ListView_DeleteAllItems(hListView);
		LoadFileAndFolder(path);
	}
	else //is file
		ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWNORMAL);
}

//reset screen of list view
void ListView::DeleteAll()
{
	ListView_DeleteAllItems(hListView);
}
