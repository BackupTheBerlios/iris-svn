#include "GenFiles.h"
#include <objbase.h>
#include <Shlobj.h>

#define BIF_NONEWFOLDERBUTTON 0x0200
namespace GenLib
{

BOOL GenFiles::fileDialog(const enum FILE_DIALOG_TYPE dialogType, const HWND parent, const char *topic, const char *extList, char *result, const int resultSize, const int extraFlags)
{
	OPENFILENAME fileName;
	ZeroMemory(&fileName, sizeof(OPENFILENAME));

	fileName.lStructSize = sizeof(OPENFILENAME);
	fileName.hwndOwner = parent;
	//fileName.hInstance = 0; // ZeroMemory does the work.
	fileName.lpstrFilter = extList;
	fileName.lpstrCustomFilter = NULL;
	//fileName.nMaxCustFilter = 0; // ZeroMemory does the work.
	fileName.nFilterIndex = (DWORD) -1;
	fileName.lpstrFile = result;
	fileName.nMaxFile = resultSize;
	fileName.lpstrInitialDir = NULL;
	fileName.lpstrTitle = topic;
	fileName.Flags = OFN_EXPLORER + OFN_ENABLESIZING + OFN_HIDEREADONLY | extraFlags;
	
	if (dialogType == OPEN_FILE_DIALOG)
	{
		//fileName.Flags = fileName.Flags | OFN_FILEMUSTEXIST + OFN_PATHMUSTEXIST;
		return GetOpenFileName(&fileName);
	}

	// dialogType == SAVE_FILE_DIALOG)
	return GetSaveFileName(&fileName);
}

BOOL GenFiles::folderDialog(const HWND parent, const char *topic, char *result, const bool allowNew, const int extraFlags)
{
	// CoInitialize must be called to support BIF_USENEWUI.
	OleInitialize(NULL);

	LPMALLOC pMalloc = NULL;
	LPITEMIDLIST pidl = NULL;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	
	// set the bi's default values
	bi.hwndOwner = parent;
	bi.lpszTitle = topic;
	
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | extraFlags;
	if (!allowNew) 
	{
		bi.ulFlags = bi.ulFlags | BIF_NONEWFOLDERBUTTON;
	}

	pidl = SHBrowseForFolder(&bi);

	if(pidl != NULL)
	{
		SHGetPathFromIDList(pidl, result);
		
		// free memory
		if (FAILED(SHGetMalloc(&pMalloc)))
		{
			throw "SHGetMalloc Failed!";
		}
		pMalloc->Free(pidl);  
		pMalloc->Release();
		
		OleUninitialize();
		return (BOOL)strlen(result);
	}
	OleUninitialize();
	return FALSE;
}

};
