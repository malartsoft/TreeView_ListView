#pragma once
#include "stdafx.h"
#include <tchar.h>

#define BUFFER_LEN 105 
#define IDI_FLOPPY 4
#define IDI_USB 5
#define IDI_HDD 6
#define IDI_CD  7 

class Drive
{
private:
	TCHAR **m_pszDrive;
	TCHAR **m_pszVolName;
	TCHAR **m_pszDisplayName;
	int *m_nIconIndex;
	int m_nCount;

public:
	Drive();
	~Drive();

	TCHAR* GetDriveName(const int &i);
	TCHAR* GetDisplayName(const int &i);
	int GetCount();
	int GetIconIndex(const int &i);
	void GetSystemDrives();

};
