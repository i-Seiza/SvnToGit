// SvnToGit.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <Windows.h>
#include <Shlobj.h>
#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <errno.h>  

int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage(hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
		break;
	case BFFM_SELCHANGED:
		break;
	}
	return 0;
}

BOOL SelectFolder(
	HWND hWnd,
	LPCTSTR lpDefFolder,
	LPTSTR lpSelectPath,
	UINT nFlag,
	LPCTSTR lpTitle)
{
	LPMALLOC pMalloc;
	BOOL bRet = FALSE;

	if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
		BROWSEINFO browsInfo;
		ITEMIDLIST *pIDlist;

		memset(&browsInfo, NULL, sizeof(browsInfo));

		browsInfo.hwndOwner = hWnd;
		browsInfo.pidlRoot = NULL;
		browsInfo.pszDisplayName = lpSelectPath;
		browsInfo.lpszTitle = lpTitle;
		browsInfo.ulFlags = nFlag;
		browsInfo.lpfn = &BrowseCallbackProc;
		browsInfo.lParam = (LPARAM)lpDefFolder;
		browsInfo.iImage = (int)NULL;

		pIDlist = SHBrowseForFolder(&browsInfo);
		if (NULL == pIDlist) {
			strcpy_s(lpSelectPath, MAX_PATH, lpDefFolder);
		}
		else {
			SHGetPathFromIDList(pIDlist, lpSelectPath);
			bRet = TRUE;
			pMalloc->Free(pIDlist);
		}
		pMalloc->Release();
	}
	return bRet;
}




int main()
{
	char dir[MAX_PATH] = { '\0' };

	// フォルダを選択する
	BOOL bRes = SelectFolder(
		/*this->m_hWnd*/nullptr,
		NULL,
		dir,
		BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE,
		""
		);
	// キャンセルの場合は続けない
	if (strlen(dir) == 0)	return 1;
	
	// フォルダ内に対してsvn git fetchをする
	// TODO

    return 0;
}

