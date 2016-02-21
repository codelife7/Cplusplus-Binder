#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

char* GetBaseName(string path) {
	string str = path.substr(path.find_last_of("\\") + 1);
	string name = str.substr(0, str.find_last_of("."));
	string ext = str.substr(str.find_last_of(".") + 1);
	str = name + "*" + ext;

	char *filename = new char[str.length() + 1];
	strcpy(filename, str.c_str());
	return filename;
}

bool AddResource(const char* filePath, const char* stub)
{
    HANDLE hfile;
    DWORD dwfilesize, dwbytesread;
    LPBYTE lpbuffer;
	char *filename = GetBaseName(filePath);

    hfile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE) return false;
    
    dwfilesize = GetFileSize(hfile, NULL);
    lpbuffer = new BYTE[dwfilesize]; // Create a buffer the size of the file
    if (ReadFile(hfile, lpbuffer, dwfilesize, &dwbytesread, NULL) == FALSE) //parameters = handle to file to be read, buffer to put data in, max num bytes to read, num that receieves bytes read, optional)
    {
        cout << "\nERROR: Could not read file\n";
        delete [] lpbuffer;
        CloseHandle(hfile);
        return false;
    }
    else
    {
        HANDLE hresource;
        hresource = BeginUpdateResource(stub, FALSE); // File to update resource, FALSE specifies not to delete filename's existing resources

		if (hresource == NULL)
        {
            cout << "\nERROR: Could not begin updating the resource\n";
            delete [] lpbuffer;
            CloseHandle(hfile);
            return false;
        }
        else
        {
			// Alter first 10 bytes
			for (int i=0; i < 10; i++) {
				lpbuffer[i] = lpbuffer[i] - 1;
			}

			if (UpdateResource(hresource, // Handle created by BeginUpdateResource
								   RT_RCDATA, // Resource type - raw data
								   filename, // Creates resource ID
								   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
								   (LPVOID) lpbuffer, // Pointer to byte array containing the file data
								   dwfilesize) == TRUE)
			{
                EndUpdateResource(hresource, FALSE); // This commits the changes made by update resource
                delete[] lpbuffer;
                CloseHandle(hfile);
                return true;

            }
            else
            {
                EndUpdateResource(hresource, TRUE); // DISCARDS the changes made by UpdateResource b/c UpdateResource failed
                delete [] lpbuffer;
                CloseHandle(hfile);
                return false;
            }
        }

    }


}

int main()
{
	
	char* source = new char[MAX_PATH];
    char* stub = new char[MAX_PATH];
	
	cout << "Path of the stub file: ";
    cin.getline(stub, MAX_PATH);
	cout << endl;
	
	cout << "Files to bind:" << endl;
    
	cin.getline(source, MAX_PATH);
    if (AddResource(source, stub) == true) {
		cout << "File Added." << endl;
    }

	cin.getline(source, MAX_PATH);
    if (AddResource(source, stub) == true) {
		cout << "File Added." << endl;
    }

    system("pause");

    delete[] source;
    delete[] stub;

    return 0;
}