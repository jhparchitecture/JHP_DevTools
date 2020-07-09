// *****************************************************************************
// Source code for the JHP_DevTools Add-On
// API Development Kit 22; Mac/Win
//
// Namespaces:			Contact person:
//		-None-
//
// [SG compatible] - Yes
// *****************************************************************************

#include 	"APIEnvir.h"

// ---------------------------------- Includes ---------------------------------

#include	"ACAPinc.h"									// also includes APIdefs.h
#include	"JHP_DevToolsResIDs.hpp"
#include	<Lmcons.h>
#include	<Commdlg.h>
#include	<iostream>
#include	<string>
#include    <sstream>
#include    <strsafe.h>

// =============================================================================
//
// Required functions
//
// =============================================================================

// ~~~~~~~~~~~~~~~
// returns username in upper case
// ~~~~~~~~~~~~~~~
GS::UniString JHP_UserName()
{
	char strUserName[UNLEN + 1];
	DWORD intUserSize = UNLEN + 1;
	GetUserName(strUserName, &intUserSize);
	return GS::UniString(strUserName).ToUpperCase();
}

// Usage:  printf("Filename = %s\n", ExtractFileName("c:\\some\\dir\\hello.exe").c_str());
std::string ExtractFileName(const std::string& fullPath)
{
	const size_t lastSlashIndex = fullPath.find_last_of("/\\");
	return fullPath.substr(lastSlashIndex + 1);
}

void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

// ~~~~~~~~~~~~~~~
// Copy SOURCE .apx to USER .apx and create archive
// ~~~~~~~~~~~~~~~

char* strAddOn_Source = "D:\\ArchicadDevelopment\\JHP_DevTools\\Build\x64\\Release";
char* strAddOn_Destination = "O:\\CAD\\ArchiCAD\\bin_23";

// Usage:  printf("Filename = %s\n", ExtractFileName("c:\\some\\dir\\hello.exe").c_str());
void JHP_Publish()
{
	HWND hwnd = nullptr;
	char szFileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = TEXT("ArchiCAD Add-On (*.apx)\0*.apx\0All Files (*.*)\0*.*\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	ofn.lpstrInitialDir = strAddOn_Source;
	ofn.lpstrDefExt = "apx";

	if (GetOpenFileName(&ofn))
		// if (IFileDialog(&ofn))
	{
		std::string fname = (ExtractFileName(szFileName));


		char dpath[MAX_PATH] = "O:\\CAD\\ArchiCAD\\bin_23\\";
		strcat(dpath, fname.c_str());

		DWORD attr = GetFileAttributes(dpath);
		if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
		{
			//  Does not exist.  No archive and copy.
			CopyFile(szFileName, dpath, FALSE);
			return;
		}
		else
		{
			// Does exist.  Archive and copy if OK.
			const int result = MessageBox(NULL, "Destination file already exists.\n\nDo you want to overwrite?", "Confirm Overwrite", MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING);
			switch (result)
			{
				//			case IDYES:
				//				break;
			case IDNO:
				return;
			case IDCANCEL:
				return;
			}
		}
		char apath[MAX_PATH] = "O:\\CAD\\ArchiCAD\\bin_23\\Archive\\";
		strcat(apath, fname.c_str());

		int i = 0;
		char cpath[MAX_PATH] = "O:\\CAD\\ArchiCAD\\bin_23\\Archive\\";
		strcat(cpath, fname.c_str());
		char *str = new char[3];
		itoa(i, str, 10);
		strcat(cpath, "___0");
		strcat(cpath, str);

		bool ifexist = TRUE;
		attr = GetFileAttributes(cpath);
		while (ifexist == TRUE)
			if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
				ifexist = FALSE;   //  does not exist
			else
			{
				//				ifexist = TRUE;  // does exist
				char cpath[MAX_PATH] = "O:\\CAD\\ArchiCAD\\bin_23\\Archive\\";
				strcat(cpath, fname.c_str());
				char *str = new char[3];
				itoa(i, str, 10);
				if (i < 10)
					strcat(cpath, "___0");
				else
					strcat(cpath, "___");

				strcat(cpath, str);
				attr = GetFileAttributes(cpath);
				i++;

			}

		if (i != 0)
			itoa(--i, str, 10);
		itoa(i, str, 10);
		if (i < 10)
			strcat(apath, "___0");
		else
			strcat(apath, "___");

		strcat(apath, str);

		CopyFile(dpath, apath, FALSE);

		ifexist = FALSE;
		attr = GetFileAttributes(apath);
		while (ifexist == FALSE)
			if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
			{
				ifexist = FALSE;   //  does not exist
				attr = GetFileAttributes(apath);
			}
			else
				ifexist = TRUE;  // does exist

		CopyFile(szFileName, dpath, FALSE);
	}
	return;
}

// =============================================================================
//
// Main function
//
// =============================================================================

GSErrCode	__ACENV_CALL	MenuCommandHandler(const API_MenuParams *params)
{
	return ACAPI_CallUndoableCommand("JHP_DevTools Menu Command", [&]() -> GSErrCode {
		switch (params->menuItemRef.menuResID) {
			case JHP_DEVTOOLS_MENU_STRINGSID: {
				switch (params->menuItemRef.itemIndex) {
					case 1: {
						GS::UniString message = "User ";
						GS::UniString uname = (JHP_UserName());
						message += uname;

						if (uname == "CGILMER" || uname == "CFRANKLIN") {
							//							message +=  " is a verified user.";
							JHP_Publish();
							//							break;
						}
						else {
							message += " is NOT a verified user.";
							ACAPI_WriteReport(message, true);
						}
						break;
					}
				}
				break;
			}
		}
		return NoError;
	});
}

//------------------------------------------------------
// Dependency definitions
//------------------------------------------------------
API_AddonType	__ACENV_CALL	CheckEnvironment (API_EnvirParams* envir)
{
	RSGetIndString (&envir->addOnInfo.name, JHP_DEVTOOLS_ABOUT_STRINGSID, 1, ACAPI_GetOwnResModule ());
	RSGetIndString (&envir->addOnInfo.description, JHP_DEVTOOLS_ABOUT_STRINGSID, 2, ACAPI_GetOwnResModule ());

	return APIAddon_Normal; 
	//return APIAddon_Preload; //!Change to APIAddon_Preload to allow listening for events!
}		
// CheckEnvironment


//------------------------------------------------------
// Interface definitions
//------------------------------------------------------
GSErrCode	__ACENV_CALL	RegisterInterface (void)
{
	GSErrCode err = NoError;
	err = ACAPI_Register_Menu (JHP_DEVTOOLS_MENU_STRINGSID, 0, MenuCode_UserDef, MenuFlag_Default);

	return NoError;
}		
// RegisterInterface


// -----------------------------------------------------------------------------
// Initialize
//		called after the Add-On has been loaded into memory
// -----------------------------------------------------------------------------

GSErrCode __ACENV_CALL	Initialize (void)
{
	GSErrCode err = ACAPI_Install_MenuHandler (JHP_DEVTOOLS_MENU_STRINGSID, MenuCommandHandler);
	
	return err;
}		
// Initialize


// -----------------------------------------------------------------------------
// FreeData
//		called when the Add-On is going to be unloaded
// -----------------------------------------------------------------------------

GSErrCode __ACENV_CALL	FreeData (void)
{
	return NoError;
}		
// FreeData