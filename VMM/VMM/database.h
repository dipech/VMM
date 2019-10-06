#ifndef DATABASE_CLASS
#define DATABASE_CLASS


#include <windows.h>
#include <vector>
#include <string>

#pragma comment (lib, "shell32.lib")

#include <Shlobj.h>

#include "includes/sqlite3.h"
#include "defines.h"

using namespace std;

class Database
{
private:
	sqlite3 *db;
	bool isAssigned;
	vector<string> modelsNames;
	string openedModelName;
	string pathToDatabaseFolder;

public:
	Database();
	RC_RESULT AssignDatabase(char *_chooseFolderDialogTitle, char *_path);
	RC_RESULT AssignDatabase(char *_chooseFolderDialogTitle);
	void CloseDatabase();
	void InsertModel(HWND _hMainWindow);
	void ExportModel(HWND _hMainWindow);
	void UpdateModel(DWORD ***_voxels, int _xSize, int _ySize, int _zSize);
	void DeleteModel();
	void GetListOfModels();
	void GetModel(DWORD ****voxels_, int *xSize_, int *ySize_, int *zSize_, char *_key);
	bool IsAssigned();
	void SetOpenedModelName(string _name);
	string GetOpenedModelName();
	string GetPathToDatabaseFolder();
	void SetPathToDatabaseFolder(string _path);
	vector<string> GetModelsNames();

private:
	RC_RESULT GetPathToDatabaseFolder(char *folder, char *_chooseFolderDialogTitle);
	void ChangeCurrentModelName();
	void CodeData(unsigned char *_arr, int size);
	void DecodeData(unsigned char *_arr, int size);
};


#endif /*DATABASE_CLASS*/