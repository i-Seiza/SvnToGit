// SvnToGit.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <Windows.h>
#include <Shlobj.h>
#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <errno.h>  
#include "SvnToGit.h"

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


//////////////////////////////////////////////
// コマンドを叩いて、結果を待機する
BOOL runcmdproc(LPSTR cmd, LPCTSTR lpCurrentDirectory)
{
	STARTUPINFO  si;
	PROCESS_INFORMATION pi;
	DWORD ret;
	HANDLE hWndmain;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	ret = CreateProcess(NULL, cmd, NULL, NULL, FALSE,
		CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,
		NULL, lpCurrentDirectory, &si, &pi);
	hWndmain = pi.hProcess;
	CloseHandle(pi.hThread);
	WaitForSingleObject(hWndmain, INFINITE);
	CloseHandle(hWndmain);
	return (ret);
}


BOOL GitSvn(LPCTSTR lpCurrentDirectory)
{
	_TCHAR param[] = "git svn fetch";
	// param.Format("cmd /c takeown /f \"%s\" && icacls \"%s\" /grant administrators:F", filename, filename);
	if (runcmdproc(param, lpCurrentDirectory))
		return TRUE;
	else
		return FALSE;
}
BOOL CheckFolder(LPCTSTR lpCurrentDirectory)
{

	FILE * fp;
	if ((fp = _popen("dir", "r")) == NULL) {
		//書き込みもしくは読み込みモードのどちらかでしか開けません
		printf("err");

		//エラー処理
		return false;
	}
	else {
		//注意　コンソールではすべての文字を取得してからでないと書き込めません
		char buff[128] = "";

		//標準出力を取得
		while (!feof(fp))
		{
			if (fgets(buff, 128, fp) != NULL) printf(buff);
		}

		//標準入力にコマンドを書き込む
		//fprintf(fp,"%s","dir\r\n");
		//fflush(fp);//バッファをフラッシュ
		//書き込みモードの標準出力は自分のコンソールに表示されます

		fclose(fp);
	}

	return true;
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
	GitSvn(dir);

	// ファイルが増えたかチェック
	CheckFolder(dir);

	return 0;
}