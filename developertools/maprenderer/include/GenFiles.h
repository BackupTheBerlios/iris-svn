#ifndef GENFILES_H_INCLUDED
#define GENFILES_H_INCLUDED
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///[GenFiles - General purpose functions for working with files]////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// All functions are static.
/// Using those methods requires linkage with common controls library (comctrl32.lib/shell32lib).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Written by Nir Dremer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>

namespace GenLib
{

struct GenFiles
{
	/* 
		Types of open file dialog boxs.
	*/
	enum FILE_DIALOG_TYPE
	{ 
		OPEN_FILE_DIALOG = 0,
		SAVE_FILE_DIALOG
	};

	/* 
		Openning file browsing window to choose file.
	*/
	static BOOL fileDialog(const enum FILE_DIALOG_TYPE dialogType, const HWND parent, const char *topic, const char *extList, char *result, const int resultSize, const int extraFlags = 0);

	/* 
		Openning folder browsing window to choose file. 
		Requires linkage with ole32.lib. Result is assumed to be at least MAX_PATH size.
	*/
	static BOOL folderDialog(const HWND parent, const char *topic, char *result, const bool allowNew = true, const int extraFlags = 0);

};

};
#endif // #define GENFILES_H_INCLUDED
