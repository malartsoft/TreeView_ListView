// 1412280.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "1412280.h"
#include "Drive.h"
#include "TreeView.h"
#include "ListView.h"
#define MAX_LOADSTRING 100

ListView listView;
Drive *drive;
TreeView treeView;

//flag created
BOOL started = FALSE; 


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

												// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void OnDestroy(HWND hwnd);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY1412280, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY1412280));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY1412280));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY1412280);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, L"Tree View - List View", WS_OVERLAPPEDWINDOW,
		0, 0, 1376, 730, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *tree)
{
	int nCurSelIndex;
	LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)tree;

	switch (tree->code)
	{
		//when click into '+' or '-' in treeview
	case TVN_ITEMEXPANDING:
		treeView.PreloadExpanding(lpnmTree->itemOld.hItem, lpnmTree->itemNew.hItem);
		break;

		//when click item in list view
	case TVN_SELCHANGED:
		listView.DeleteAll();
		listView.LoadChild(treeView.GetCurPath(), drive);
		break;

		//run file
	case NM_DBLCLK:
		if (tree->hwndFrom == listView.GetHandle())
			listView.LoadCurSel();
		break;
	}
	return 0;
}
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {

	switch (id)
	{
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwndCtl, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hwnd);
		break;
	}
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	RECT main;
	GetWindowRect(hwnd, &main);

	//get drives in system
	drive = new Drive();
	drive->GetSystemDrives();

	//create place to show tree view
	treeView.Create(hwnd, IDC_TREEVIEW, hInst, main.right / 3, main.bottom);

	//show tree view
	treeView.LoadMyComputer(drive);

	//create place to show list view
	listView.Create(hwnd, IDC_LISTVIEW, hInst, (main.right - main.left) * 2 / 3 + 1, main.bottom, main.right / 3);

	//show tree view
	listView.LoadMyComputer(drive);

	return TRUE;
}