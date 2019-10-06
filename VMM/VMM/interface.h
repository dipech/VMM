#ifndef INTERFACE_CLASS
#define INTERFACE_CLASS

#include <windows.h>
#include <vector>
#include <string>
#include <ShlObj.h>

#include "defines.h"
#include "camera.h"
#include "model.h"
#include "database.h"
#include "frame.h"
#include "directx.h"
#include "clipboard.h"

using namespace std;

LRESULT __stdcall WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WndProcColorWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct Interface;

struct PointersToObjects
{
	Interface *GUI_ptr;
	Model *Model_ptr;
	Camera *Camera_ptr;
	Database *Database_ptr;
	Clipboard *Clipboard_ptr;
	Frame *Frame_ptr;
	Directx *Directx_ptr;
};

struct Interface
{	
	//------Data------
	//WNDCLASS
	WNDCLASSEX wndClass;
	//HWNDs
	HWND hMainWindow;
	HWND hColorWindow;
	HWND hListView;
	LVCOLUMN listViewColumn;
	LVITEM listViewItem;
	HWND hRenderTarget;
	HWND hSearchTextField;
	HWND hSearchButtonFind;
	HWND hSearchButtonReset;
	HWND hLabelModelDimX;
	HWND hLabelModelDimY;
	HWND hLabelModelDimZ;
	HWND hButtonIncrementX;
	HWND hButtonIncrementY;
	HWND hButtonIncrementZ;
	HWND hButtonDecrementX;
	HWND hButtonDecrementY;
	HWND hButtonDecrementZ;
	HWND hTextFieldX;
	HWND hTextFieldY;
	HWND hTextFieldZ;
	HWND hSubmitSizeChanges;
	//MENUs
	HMENU menuMain;
	HMENU menuFile;
	HMENU menuOptions;
	HMENU menuLanguage;
	HMENU menuAxisOptions;
	HMENU menuCopiedVoxelsDisplaySettings;
	HMENU menuEditingMode;
	HMENU menuControlMode;
	HMENU menuRotateModel;
	HMENU menuInfo;
	HMENU menuProgramMode;
	HMENU menuStartingCameraPosition;
	HMENU menuCameraVerticalPosition;
	HMENU menuCameraHorizontalPosition;
	//SIZEs
	POINT mainWindowPos;
	POINT colorWindowPos;
	RECT mainWindowRect;
	RECT listViewRect;
	RECT renderTargetRect;
	//CURSORs
	HCURSOR standartCursor;
	HCURSOR pipetteCursor;
	HCURSOR brushToolCursor;
	HCURSOR fillToolCursor;
	HCURSOR frameToolCursor;
	//TEXTs
	vector<string> programText;
	//settings
	struct Settings
	{
		enum CameraType
		{
			WASD_SPACE_CTRL,
			MOUSE
		}cameraType;
		int cameraStartingZenith;
		int cameraStartingAzimuth;
		bool isFrameVisible;
		bool isContourVisible;
		bool isWireframeVisible;
		bool isExtendedAsix;
		bool isCopiedVoxelsPreviewVisible;
		bool isCopiedVoxelsPreviewTransparent;
		enum ProgramType
		{
			EMPTY,
			CUBE_WORLD
		}programType;
	}settings;

	enum EditingMode
	{
		ADD_OR_REMOVE_VOXELS,
		BRUSHING,
		FILLING,
		WORK_WITH_GROUPS_OF_VOXELS,
	}editingMode;
	enum Language
	{
		ENGLISH,
		RUSSIAN
	}language;
	bool isPipette;

	DWORD color;

	COLORREF dColors[16];

	//------Methods------	
	Interface();
	RC_RESULT Init(WNDPROC _WndProc, HINSTANCE _hInstance, int _nCmdShow,PointersToObjects *_ptrs);
	void SetProgramText();
	LPCSTR GetProgramText(UINT _index);
	void CreateProgramMenu();
	void SetProgramMenu();
	void DestroyProgramMenu();
	void SaveSettingToFile(Camera & _camera, Database & _database);
	void LoadSettingFromFile(Camera & _camera, Database & _database, Frame & frame);
	void AddItemToListView(char *_item, int _row);
	void AddModelsNamesToListView(Database & _database);
	void AddModelsNamesToListView(Database & _database, string _strToFind);
	void SetModelSizesToTextsFields(int xSize, int ySize, int zSize);
	string GetPathToFolder();
};

#endif /*INTERFACE_CLASS*/