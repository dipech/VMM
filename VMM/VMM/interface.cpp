#pragma comment(lib, "comctl32.lib")

#include <ShlObj.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

#include "functions.h"
#include "interface.h"

using namespace std;

Interface::Interface()
{
	mainWindowPos.x = 5;
	mainWindowPos.y = 5;

	mainWindowRect.left = mainWindowRect.top = 0;
	mainWindowRect.right = 926;
	mainWindowRect.bottom = 750;

	colorWindowPos.x = 905;
	colorWindowPos.y = 5;

	listViewRect.left = 0;
	listViewRect.top = 22;
	listViewRect.right = 250;
	listViewRect.bottom = 658;

	renderTargetRect.left = 250;
	renderTargetRect.top = 22;
	renderTargetRect.right = 658;
	renderTargetRect.bottom = 658;

	settings.programType = Settings::ProgramType::EMPTY;

	editingMode = ADD_OR_REMOVE_VOXELS;
	isPipette = false;

	for(int i = 0; i < 16; i++)
	{
		dColors[i] = 16777215;
	}
	//color = 16777215;
	color = 16777215;
}

RC_RESULT Interface::Init(WNDPROC _WndProc, HINSTANCE _hInstance, int _nCmdShow, PointersToObjects *_ptrs)
{
	standartCursor = LoadCursorFromFile("./images/Standart.cur");
	pipetteCursor = LoadCursorFromFile("./images/PipetteTool.cur");
	brushToolCursor = LoadCursorFromFile("./images/BrushTool.cur");
	fillToolCursor = LoadCursorFromFile("./images/FillTool.cur");
	frameToolCursor = LoadCursorFromFile("./images/FrameTool.cur");

	HBITMAP hIcon = (HBITMAP)LoadImage(NULL, "./images/icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = _WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = _hInstance;
	wndClass.hIcon = (HICON)hIcon;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "VoxelModelMaker";
	wndClass.hIconSm = NULL;
	if(!RegisterClassEx(&wndClass))
	{
		return RC_ERROR;
	}

	if(settings.programType == Settings::ProgramType::EMPTY)
	{
		mainWindowRect.right -= listViewRect.right;
		colorWindowPos.x -= listViewRect.right;
		renderTargetRect.left -= listViewRect.right;
		listViewRect = {0};
	}

	hMainWindow = CreateWindow("VoxelModelMaker",
		GetProgramText(PTI_WINDOW_TITLE),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX,
		mainWindowPos.x,
		mainWindowPos.y,
		mainWindowRect.right,
		mainWindowRect.bottom,
		NULL, NULL, wndClass.hInstance,
		_ptrs);

	RECT mainWindowRectNew;
	GetWindowRect(hMainWindow, &mainWindowRectNew);
	CreateProgramMenu();
	SetProgramMenu();

	//---GUI Elements---

	InitCommonControls();

	hRenderTarget = CreateWindow("STATIC", " ", WS_CHILD | WS_VISIBLE,
		renderTargetRect.left, renderTargetRect.top,
		renderTargetRect.right, renderTargetRect.bottom,
		hMainWindow, (HMENU)menuMain, wndClass.hInstance, (LPVOID)NULL);

	if(settings.programType == Settings::ProgramType::CUBE_WORLD)
	{
		hListView = CreateWindow(WC_LISTVIEW, "",
			WS_CHILD | LVS_REPORT | WS_BORDER | WS_VSCROLL | WS_VISIBLE,
			listViewRect.left, listViewRect.top,
			listViewRect.right, listViewRect.bottom,
			hMainWindow, (HMENU)menuMain, wndClass.hInstance, NULL);

		ListView_SetExtendedListViewStyle(hListView, ListView_GetExtendedListViewStyle(hListView) | LVS_EX_FULLROWSELECT);

		ShowWindow(hListView, SW_SHOW);

		memset(&listViewColumn, 0, sizeof(listViewColumn));
		listViewColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		listViewColumn.cx = listViewRect.right;
		listViewColumn.pszText = (LPSTR)GetProgramText(PTI_COLUMN_NAME);
		ListView_InsertColumn(hListView, 0, &listViewColumn);

		hSearchTextField = CreateWindow("EDIT", GetProgramText(PTI_FIND_MODEL_STARTING_TEXT),
			WS_BORDER | WS_CHILD | ES_LEFT | WS_VISIBLE,
			5, 1,
			190, 20,
			hMainWindow, (HMENU)II_SEARCH_TEXT_FIELD, wndClass.hInstance, (LPVOID)NULL);

		hSearchButtonReset = CreateWindow("BUTTON", "",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			200, 1,
			20, 20,
			hMainWindow, (HMENU)II_SEARCH_BUTTON_RESET, wndClass.hInstance, NULL);

		hSearchButtonFind = CreateWindow("BUTTON", "",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			230, 1,
			20, 20,
			hMainWindow, (HMENU)II_SEARCH_BUTTON_FIND, wndClass.hInstance, NULL);
	}

	//---

	int offsetX = 5;
	if(settings.programType == Settings::ProgramType::CUBE_WORLD)
	{
		offsetX = 255;
	}

	int plusToOffsetX = 60;

	hLabelModelDimX = CreateWindow("STATIC", "X:",
		WS_CHILD | ES_LEFT | WS_VISIBLE,
		offsetX, 1,
		15, 20,
		hMainWindow, (HMENU)NULL, wndClass.hInstance, (LPVOID)NULL);

	hButtonDecrementX = CreateWindow("BUTTON", "",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
		offsetX += 20, 1,
		20, 20,
		hMainWindow, (HMENU)II_DECREMENT_X, wndClass.hInstance, NULL);

	hTextFieldX = CreateWindow("EDIT", "1",
		WS_BORDER | WS_CHILD | ES_LEFT | WS_VISIBLE,
		offsetX += 25, 1,
		32, 20,
		hMainWindow, (HMENU)II_TEXT_FIELD_X, wndClass.hInstance, (LPVOID)NULL);

	hButtonIncrementX = CreateWindow("BUTTON", "",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
		offsetX += 37, 1,
		20, 20,
		hMainWindow, (HMENU)II_INCREMENT_X, wndClass.hInstance, NULL);

	offsetX += plusToOffsetX;

	hLabelModelDimY = CreateWindow("STATIC", "Y:",
		WS_CHILD | ES_LEFT | WS_VISIBLE,
		offsetX, 1,
		15, 20,
		hMainWindow, (HMENU)NULL, wndClass.hInstance, (LPVOID)NULL);

	hButtonDecrementY = CreateWindow("BUTTON", "",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
		offsetX += 20, 1,
		20, 20,
		hMainWindow, (HMENU)II_DECREMENT_Y, wndClass.hInstance, NULL);

	hTextFieldY = CreateWindow("EDIT", "1",
		WS_BORDER | WS_CHILD | ES_LEFT | WS_VISIBLE,
		offsetX += 25, 1,
		32, 20,
		hMainWindow, (HMENU)II_TEXT_FIELD_Y, wndClass.hInstance, (LPVOID)NULL);

	hButtonIncrementY = CreateWindow("BUTTON", "",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
		offsetX += 37, 1,
		20, 20,
		hMainWindow, (HMENU)II_INCREMENT_Y, wndClass.hInstance, NULL);

	offsetX += plusToOffsetX;

	hLabelModelDimZ = CreateWindow("STATIC", "Z:",
		WS_CHILD | ES_LEFT | WS_VISIBLE,
		offsetX, 1,
		15, 20,
		hMainWindow, (HMENU)NULL, wndClass.hInstance, (LPVOID)NULL);

	hButtonDecrementZ = CreateWindow("BUTTON", "",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
		offsetX += 20, 1,
		20, 20,
		hMainWindow, (HMENU)II_DECREMENT_Z, wndClass.hInstance, NULL);

	hTextFieldZ = CreateWindow("EDIT", "1",
		WS_BORDER | WS_CHILD | ES_LEFT | WS_VISIBLE,
		offsetX += 25, 1,
		32, 20,
		hMainWindow, (HMENU)II_TEXT_FIELD_Z, wndClass.hInstance, (LPVOID)NULL);

	hButtonIncrementZ = CreateWindow("BUTTON", "",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
		offsetX += 37, 1,
		20, 20,
		hMainWindow, (HMENU)II_INCREMENT_Z, wndClass.hInstance, NULL);

	hSubmitSizeChanges = CreateWindow("BUTTON", GetProgramText(PTI_SUBMIT_SIZE_CHANGES),
		WS_CHILD,
		offsetX += 30, 1,
		100, 20,
		hMainWindow, (HMENU)II_SUBMIT_SIZE_CHANGES, wndClass.hInstance, NULL);

	//------------------

	ShowWindow(hMainWindow, _nCmdShow);
	UpdateWindow(hMainWindow);

	HBITMAP hbitmapCross = (HBITMAP)LoadImage(NULL, "./images/cross.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	HBITMAP hbitmapLoupe = (HBITMAP)LoadImage(NULL, "./images/loupe.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);

	SendMessage(hSearchButtonReset, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmapCross);
	SendMessage(hSearchButtonFind, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmapLoupe);

	HBITMAP hbitmapPlusToX = (HBITMAP)LoadImage(NULL, "./images/plusToX.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	HBITMAP hbitmapMinusToX = (HBITMAP)LoadImage(NULL, "./images/minusToX.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	HBITMAP hbitmapPlusToY = (HBITMAP)LoadImage(NULL, "./images/plusToY.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	HBITMAP hbitmapMinusToY = (HBITMAP)LoadImage(NULL, "./images/minusToY.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	HBITMAP hbitmapPlusToZ = (HBITMAP)LoadImage(NULL, "./images/plusToZ.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	HBITMAP hbitmapMinusToZ = (HBITMAP)LoadImage(NULL, "./images/minusToZ.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);

	SendMessage(hButtonIncrementX, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmapPlusToX);
	SendMessage(hButtonDecrementX, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmapMinusToX);
	SendMessage(hButtonIncrementY, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmapPlusToY);
	SendMessage(hButtonDecrementY, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmapMinusToY);
	SendMessage(hButtonIncrementZ, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmapPlusToZ);
	SendMessage(hButtonDecrementZ, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmapMinusToZ);

	return RC_SUCCESS;
}

void Interface::SetProgramText()
{
	switch(language)
	{
		case Language::ENGLISH:
		{
			programText.push_back("Voxel Model Maker");
			programText.push_back("File");
			programText.push_back("Open database...");
			programText.push_back("Save model");
			programText.push_back("New model");
			programText.push_back("Save model to disk");
			programText.push_back("Load model from disk");
			programText.push_back("Insert model in database");
			programText.push_back("Export model from database");
			programText.push_back("Convert .bmp picture to model");
			programText.push_back("Options");
			programText.push_back("Show sizes of model(F1)");
			programText.push_back("Show frame that helping to place voxels(F2)");
			programText.push_back("Show wire frame(F3)");
			programText.push_back("Restore view(E)");
			programText.push_back("Pipette(Q)");
			programText.push_back("Language...");
			programText.push_back("English");
			programText.push_back("Russian");
			programText.push_back("Editing mode");
			programText.push_back("Add/Remove voxels(button 1)");
			programText.push_back("Brushing(button 2)");
			programText.push_back("Filling(button 3)");
			programText.push_back("Control mode");
			programText.push_back("WASD-Space-Ctrl");
			programText.push_back("Mouse");
			programText.push_back("Starting camera position");
			programText.push_back("Horizontal position");
			programText.push_back("Front");
			programText.push_back("Back");
			programText.push_back("Left");
			programText.push_back("Right");
			programText.push_back("Vertical position");
			programText.push_back("Top");
			programText.push_back("Middle");
			programText.push_back("Bottom");
			programText.push_back("Find model...");
			programText.push_back("Names of models");
			programText.push_back("Submit");
			programText.push_back("Choose Cube World folder");
			programText.push_back("Please, restart program to apply changes");
			programText.push_back("Axis settings");
			programText.push_back("Axis dimensions as the model");
			programText.push_back("Axis dimensions are endless");
			programText.push_back("Supports only BMP images which the color depth = 24\r\nYou may quickly convert all to BMP with 24 depth color in Paint");
			programText.push_back("Thanks for use the \"Voxel Model Maker\". \r\nI hope, you like this voxel editor! \r\n \r\nIf you have any suggestions, \r\nor you find a bug, or you want \r\nto ask me a question, here are my contacts: \r\n \r\nemail: dadya.dima@gmail.com \r\nvk: vk.com/grand_daddy \r\n \r\nBest regards, your GranD_Daddy.");
			programText.push_back("Info");
			programText.push_back("Program mode");
			programText.push_back("Standard");
			programText.push_back("Cube World");
			programText.push_back("Rotate object around X clockwise");
			programText.push_back("Rotate object around Y clockwise");
			programText.push_back("Rotate object around Z clockwise");
			programText.push_back("Rotate object around X counterclockwise");
			programText.push_back("Rotate object around Y counterclockwise");
			programText.push_back("Rotate object around Z counterclockwise");
			programText.push_back("Rotate object");
			programText.push_back("Message from author");
			programText.push_back("Control");
			programText.push_back("<<<Camera control type \"Mouse\">>> \r\nRight mouse button + mouse move - rotate model \r\nMouse Wheel - change radius from camera to center of model \r\n \r\n<<<Camera control type \"WASD-SPACE-CTRL\">>> \r\nW,A,S,D - move forward,left,backward,right \r\nSPACE,CTRL - move up,down \r\nRight mouse button + mouse move - change direction of look \r\n \r\n<<<Common for all camera's control types buttons>>> \r\nLeft mouse button, Middle mouse button - add/remove voxel \r\nQ - select a tool \"Pipette\" \r\nE - restore to default distance to model and direction of look \r\nR - restore to default sizes and position of black frame \r\n1 - turn on edition type \"Add or Remove voxel\" \r\n2 - turn on edition type \"Brush voxel\" \r\n3 - turn on edition type \"Fill voxels\" \r\n4 - turn on edition type \"Work with groups of voxels\" \r\nF1 - turn on/turn off display of models sizes \r\nF2 - turn on/turn off display black frame \r\nF3 - turn on/turn off display contours on voxels \r\nShift + C, Shift + V - copy/paste voxels \r\n \r\n<<<In edition type \"Work with groups of voxels\">>> \r\nNUMPAD5,NUMPAD2,NUMPAD1,NUMPAD3 - move black frame forward,backward,left,right \r\nNUMPAD9,NUMPAD6 - move black frame up,down \r\n \r\nArrow Up,Down,Left,Right - resize black frame forward,backward,left,right \r\nPage Up,Down - resize black frame up,down \r\n \r\n<<<In other types>>> \r\nArrow Up,Down,Left,Right - move model forward,backward,left,right \r\nPage Up,Down - move model up,down");
			programText.push_back("Work with groups of voxels(button 4)");
			programText.push_back("There are four editing mode: \r\n \r\n(1) \"Add/remove voxels\": \r\nediting mode, in where user can add or remove voxels. \r\n \r\n(2) \"Brush voxels\": \r\nediting mode, in where user can paint voxels. \r\n \r\n(3) \"Fill voxels\": \r\nediting mode, in where user can paint immediately the huge \r\nquantity of voxels. (Not recommended to paint very huge quantities of voxels. \r\nThe program may crash). \r\n \r\n(4) \"Work with groups voxels\": \r\nediting mode, in where user can increase/decrease and move black frame. \r\nIf user click to left mouse button, then voxels will be added into frame. \r\nIf user click to middle mouse button, then voxels will be removed from frame. \r\nIf user will copy voxels in frame, then user will can paste voxels into frame. \r\n \r\nTo see more informations about the control - see paragraph \" Control \" in menu \"Information\".");
			programText.push_back("About editing modes");
			programText.push_back("Preview clipboard settings");
			programText.push_back("Clipboard is visible");
			programText.push_back("Clipboard is transparent");
			programText.push_back("Export as .STL file");
			break;
		}
		case Language::RUSSIAN:
		{
			programText.push_back("Voxel Model Maker");
			programText.push_back("Файл");
			programText.push_back("Открыть базу данных...");
			programText.push_back("Сохранить модель");
			programText.push_back("Новая модель");
			programText.push_back("Сохранить модель на диск");
			programText.push_back("Загрузить модель с диска");
			programText.push_back("Импортировать модель в базу данных");
			programText.push_back("Экспортировать модель из базы данных");
			programText.push_back("Конвертировать .bmp картинку в модель");
			programText.push_back("Опции");
			programText.push_back("Показывать размеры модели(F1)");
			programText.push_back("Показывать рамку, помогающую размещать воксели(F2)");
			programText.push_back("Показывать контуры вокселя(F3)");
			programText.push_back("Восстановить камеру(E)");
			programText.push_back("Пипетка(Q)");
			programText.push_back("Язык...");
			programText.push_back("Английский");
			programText.push_back("Русский");
			programText.push_back("Режим редактирования");
			programText.push_back("Добавление/удаление вокселей(кнопка 1)");
			programText.push_back("Закрашивание вокселей(кнопка 2)");
			programText.push_back("Заливка вокселей(кнопка 3)");
			programText.push_back("Тип управления");
			programText.push_back("WASD-Space-Ctrl");
			programText.push_back("Мышь");
			programText.push_back("Начальное положение камеры");
			programText.push_back("Горизонтальная позиция");
			programText.push_back("Спереди");
			programText.push_back("Сзади");
			programText.push_back("Слева");
			programText.push_back("Справа");
			programText.push_back("Вертикальная позиция");
			programText.push_back("Сверху");
			programText.push_back("По центру");
			programText.push_back("Снизу");
			programText.push_back("Найти модель...");
			programText.push_back("Названия моделей");
			programText.push_back("Применить");
			programText.push_back("Выберите папку с игрой Cube World");
			programText.push_back("Пожалуйста, перезагрузите программу, чтобы изменения вступили в силу");
			programText.push_back("Настройки осей");
			programText.push_back("Размеры осей совпадают с размерами модели");
			programText.push_back("Размеры осей бесконечны");
			programText.push_back("Поддерживаются только BMP с глубиной цвета = 24\r\nВы можете быстро и легко преобразовать\r\nисходное изображение в нужное\r\nв программе Paint");
			programText.push_back("Спасибо за использование \"Voxel Model Maker\". \r\nНадеюсь, редактор вам нравится! \r\n \r\nЕсли есть пожелания как улучшить редактор, \r\nили вы нашли ошибку, или вы хотите \r\nзадать мне вопрос, то вот мои контакты: \r\n \r\nemail: dadya.dima@gmail.com \r\nvk: vk.com/grand_daddy \r\n \r\nС уважением, ваш GranD_Daddy.");
			programText.push_back("Информация");
			programText.push_back("Режим программы");
			programText.push_back("Стандартный");
			programText.push_back("Cube World");
			programText.push_back("Вращать объект вокруг X по часовой стрелке");
			programText.push_back("Вращать объект вокруг Y по часовой стрелке");
			programText.push_back("Вращать объект вокруг Z по часовой стрелке");
			programText.push_back("Вращать объект вокруг X против часовой стрелки");
			programText.push_back("Вращать объект вокруг Y против часовой стрелки");
			programText.push_back("Вращать объект вокруг Z против часовой стрелки");
			programText.push_back("Вращать объект");
			programText.push_back("Послание от автора");
			programText.push_back("Управление");
			programText.push_back("<<<Режим управления \"Мышь\">>> \r\nПравая кнопка мыши + движение мышью - вращение модели \r\nКолёсико мыши - изменение расстояния от камеры до центра модели \r\n \r\n<<<Режим управления \"WASD-SPACE-CTRL\">>> \r\nW,A,S,D - движение вперед,влево,назад,вправо \r\nSPACE,CTRL - движение вверх,вниз \r\nПравая кнопка мыши + движение мышью - изменить направление взгляда \r\n \r\n<<<Общие для всех типов управления кнопки>>> \r\nЛевая кнопка мыши, Средняя кнопка мыши - поставить/удалить воксель \r\nQ - включить инструмент \"Пипетка\" \r\nE - восстановить позицию и взгляд камеры по умолчанию \r\nR - восстановить первоначальные размеры и позицию черной рамки \r\n1 - включить режим редактирования \"Добавление или удаление вокселей\" \r\n2 - включить режим редактирования \"Закрашивание вокселей\" \r\n3 - включить режим редактирования \"Заливка вокселей\" \r\n4 - включить режим редактирования \"Работа с группами вокселей\" \r\nF1 - включить/отключить отображение размеров модели \r\nF2 - включить/отключить отображение черной рамки, помогающей ставить воксели \r\nF3 - включить/отключить отображение контуров вокруг вокселей \r\nShift + C, Shift + V - копировать/вставить воксели \r\n \r\n<<<В режиме редактирования \"Работа с группами вокселей\">>> \r\nNUMPAD5,NUMPAD2,NUMPAD1,NUMPAD3 - двигать черную рамку по горизонтали \r\nNUMPAD9,NUMPAD6 - двигать черную рамку по вертикали \r\n \r\nArrow Up,Down,Left,Right - изменить размер черной рамки по горизонтали \r\nPage Up,Down - изменить размер черной рамки по вертикали \r\n \r\n<<<В остальных режимах>>> \r\nСтрелка вверх, Стрелка вниз, Стрелка влево, Стрелка вправо - двигать модель по горизонтали \r\nPage Up,Down - двигать модель по вертикали");
			programText.push_back("Работа с группами вокселей(кнопка 4)");
			programText.push_back("В редакторе четыре режима редактирования: \r\n \r\n(1) \"Добавление/удаление вокселей\": \r\nрежим, в котором пользователь может добавлять или удалять воксели. \r\n \r\n(2) \"Закрашивание вокселей\": \r\nрежим, в котором пользователь может менять цвет у вокселей. \r\n \r\n(3) \"Заливка вокселей\": \r\nрежим, в котором пользователь может окрашивать сразу много вокселей одного цвета выбранным цветом. \r\n(Не рекоммендуется заливать сразу большие объемы вокселей. \r\nИначе может произойти аварийное завершение работы программы) \r\n \r\n(4) \"Работа в группами вокселей\": \r\nрежим, в котором пользователь может расширять черную рамку до любых размеров и перемещать рамку.  \r\nЕсли пользователь нажмет на левую пнопку мыши, то рамка изнутри заполнится вокселями выбранного цвета. \r\nЕсли пользоватьль нажмет на правую кнопку мыши, то внутри рамки удалятся все воксели.  \r\nЕсли пользователь скопирует то, что находится в рамке, то пользователь будет иметь возможность вставлять то, что он скопировал ранее в ту область модели, где сейчас находится рамка.  \r\nЕсли размер рамки меньше,чем размер скопированной области, то при вставке скопированная область обрежется. \r\nЕсли размер рамки больше размера скопированной области, то область будет вставляться снова и снова после предыдущих оболастей, пока не достигнет конца рамки или конца размеров модели. \r\n \r\nБолее подробно о кнопках перемещения и расширения рамки, а так же о копировании и вставке областей - смотрите пункт \"Управление\" в меню \"Информация\".");
			programText.push_back("О режимах редактирования");
			programText.push_back("Настройки предпросмотра скопированных вокселей");
			programText.push_back("Скопированные воксели видны");
			programText.push_back("Скопированные воксели прозрачны");
			programText.push_back("Экспортировать как .STL файл");
			break;
		}
	}
}

LPCSTR Interface::GetProgramText(UINT _ProgramTextIndex)
{
	return programText[_ProgramTextIndex].c_str();
}

void Interface::CreateProgramMenu()
{
	menuMain = CreateMenu();
	menuFile = CreatePopupMenu();
	menuOptions = CreatePopupMenu();
	menuEditingMode = CreatePopupMenu();
	menuControlMode = CreatePopupMenu();
	menuInfo = CreatePopupMenu();
	menuProgramMode = CreatePopupMenu();
	menuAxisOptions = CreatePopupMenu();
	menuCopiedVoxelsDisplaySettings = CreatePopupMenu();
	menuLanguage = CreatePopupMenu();
	menuRotateModel = CreatePopupMenu();
	menuStartingCameraPosition = CreatePopupMenu();
	menuCameraVerticalPosition = CreatePopupMenu();
	menuCameraHorizontalPosition = CreatePopupMenu();

	AppendMenu(menuMain, MF_STRING | MF_POPUP, (UINT)menuFile, GetProgramText(PTI_FILE));
	if(settings.programType == Settings::ProgramType::CUBE_WORLD)
	{
		AppendMenu(menuFile, MF_STRING, II_OPEN_DATABASE, GetProgramText(PTI_OPEN_DATABASE));
		AppendMenu(menuFile, MF_STRING, II_SAVE_MODEL, GetProgramText(PTI_SAVE_MODEL));
		AppendMenu(menuFile, MF_SEPARATOR, II_SEPARATOR, "");
	}
	AppendMenu(menuFile, MF_STRING, II_NEW_MODEL_OUTSIDE_DATABASE, GetProgramText(PTI_NEW_MODEL_OUTSIDE_DATABASE));
	AppendMenu(menuFile, MF_STRING, II_SAVE_MODEL_OUTSIDE_DATABASE, GetProgramText(PTI_SAVE_MODEL_OUTSIDE_DATABASE));
	AppendMenu(menuFile, MF_STRING, II_LOAD_MODEL_OUTSIDE_DATABASE, GetProgramText(PTI_LOAD_MODEL_OUTSIDE_DATABASE));
	AppendMenu(menuFile, MF_SEPARATOR, II_SEPARATOR, "");
	if(settings.programType == Settings::ProgramType::CUBE_WORLD)
	{
		AppendMenu(menuFile, MF_STRING, II_INSERT_MODEL, GetProgramText(PTI_INSERT_MODEL));
		AppendMenu(menuFile, MF_STRING, II_EXPORT_MODEL, GetProgramText(PTI_EXPORT_MODEL));
		AppendMenu(menuFile, MF_SEPARATOR, II_SEPARATOR, "");
	}
	AppendMenu(menuFile, MF_STRING, II_CONVERT_PICTURE_TO_MODEL, GetProgramText(PTI_CONVERT_PICTURE_TO_MODEL));
	AppendMenu(menuFile, MF_SEPARATOR, II_SEPARATOR, "");
	AppendMenu(menuFile, MF_STRING, II_EXPORT_AS_STL_FILE, GetProgramText(PTI_EXPORT_AS_STL_FILE));

	EnableMenuItem(menuFile, II_SAVE_MODEL, MF_GRAYED);
	EnableMenuItem(menuFile, II_EXPORT_MODEL, MF_GRAYED);
	EnableMenuItem(menuFile, II_INSERT_MODEL, MF_GRAYED);

	AppendMenu(menuMain, MF_STRING | MF_POPUP, (UINT)menuOptions, GetProgramText(PTI_OPTIONS));
	if(settings.isContourVisible)
	{
		AppendMenu(menuOptions, MF_STRING | MF_CHECKED, II_SIZE_OF_MODEL, GetProgramText(PTI_SIZE_OF_MODEL));
		EnableMenuItem(menuAxisOptions, II_SAVE_MODEL, MF_ENABLED);
	}
	else
	{
		AppendMenu(menuOptions, MF_STRING | MF_UNCHECKED, II_SIZE_OF_MODEL, GetProgramText(PTI_SIZE_OF_MODEL));
		EnableMenuItem(menuAxisOptions, II_SAVE_MODEL, MF_DISABLED);
	}
	if(settings.isFrameVisible)
	{
		AppendMenu(menuOptions, MF_STRING | MF_CHECKED, II_FRAME, GetProgramText(PTI_FRAME));
	}
	else
	{
		AppendMenu(menuOptions, MF_STRING | MF_UNCHECKED, II_FRAME, GetProgramText(PTI_FRAME));
	}
	if(settings.isWireframeVisible)
	{
		AppendMenu(menuOptions, MF_STRING | MF_CHECKED, II_WIREFRAME, GetProgramText(PTI_WIREFRAME));
	}
	else
	{
		AppendMenu(menuOptions, MF_STRING | MF_UNCHECKED, II_WIREFRAME, GetProgramText(PTI_WIREFRAME));
	}
	AppendMenu(menuOptions, MF_STRING | MF_POPUP, (UINT)menuAxisOptions, GetProgramText(PTI_EXTENDED_AXIS));
	if(settings.isExtendedAsix)
	{
		AppendMenu(menuAxisOptions, MF_STRING | MF_UNCHECKED, II_AXIS_SIZE_MODEL, GetProgramText(PTI_AXIS_SIZE_MODEL));
		AppendMenu(menuAxisOptions, MF_STRING | MF_CHECKED, II_AXIS_ENDLESS, GetProgramText(PTI_AXIS_ENDLESS));
	}
	else
	{
		AppendMenu(menuAxisOptions, MF_STRING | MF_CHECKED, II_AXIS_SIZE_MODEL, GetProgramText(PTI_AXIS_SIZE_MODEL));
		AppendMenu(menuAxisOptions, MF_STRING | MF_UNCHECKED, II_AXIS_ENDLESS, GetProgramText(PTI_AXIS_ENDLESS));
	}
	AppendMenu(menuOptions, MF_SEPARATOR, II_SEPARATOR, "");
	AppendMenu(menuOptions, MF_STRING, II_RESTORE_VIEW, GetProgramText(PTI_RESTORE_VIEW));
	AppendMenu(menuOptions, MF_STRING, II_PIPETTE, GetProgramText(PTI_PIPETTE));
	AppendMenu(menuOptions, MF_SEPARATOR, II_SEPARATOR, "");
	AppendMenu(menuOptions, MF_STRING | MF_POPUP, (UINT)menuLanguage, GetProgramText(PTI_LANGUAGE));
	if(language == Language::ENGLISH)
	{
		AppendMenu(menuLanguage, MF_STRING | MF_CHECKED, II_LANGUAGE_ENGLISH, GetProgramText(PTI_LANGUAGE_ENGLISH));
		AppendMenu(menuLanguage, MF_STRING | MF_UNCHECKED, II_LANGUAGE_RUSSIAN, GetProgramText(PTI_LANGUAGE_RUSSIAN));
	}
	if(language == Language::RUSSIAN)
	{
		AppendMenu(menuLanguage, MF_STRING | MF_UNCHECKED, II_LANGUAGE_ENGLISH, GetProgramText(PTI_LANGUAGE_ENGLISH));
		AppendMenu(menuLanguage, MF_STRING | MF_CHECKED, II_LANGUAGE_RUSSIAN, GetProgramText(PTI_LANGUAGE_RUSSIAN));
	}
	AppendMenu(menuOptions, MF_STRING | MF_POPUP, (UINT)menuCopiedVoxelsDisplaySettings, GetProgramText(PTI_COPIED_VOXELS_PREVIEW_SETTING));
	if(settings.isCopiedVoxelsPreviewVisible)
	{
		AppendMenu(menuCopiedVoxelsDisplaySettings, MF_STRING | MF_CHECKED, II_IS_COPIED_VOXELS_PREVIEW, GetProgramText(PTI_IS_COPIED_VOXELS_PREVIEW));
	}
	else
	{
		AppendMenu(menuCopiedVoxelsDisplaySettings, MF_STRING | MF_UNCHECKED, II_IS_COPIED_VOXELS_PREVIEW, GetProgramText(PTI_IS_COPIED_VOXELS_PREVIEW));
	}
	if(settings.isCopiedVoxelsPreviewTransparent)
	{
		AppendMenu(menuCopiedVoxelsDisplaySettings, MF_STRING | MF_CHECKED, II_IS_COPIED_VOXELS_PREVIEW_TRANSPARENT, GetProgramText(PTI_IS_COPIED_VOXELS_PREVIEW_TRANSPARENT));
	}
	else
	{
		AppendMenu(menuCopiedVoxelsDisplaySettings, MF_STRING | MF_UNCHECKED, II_IS_COPIED_VOXELS_PREVIEW_TRANSPARENT, GetProgramText(PTI_IS_COPIED_VOXELS_PREVIEW_TRANSPARENT));
	}
	AppendMenu(menuOptions, MF_SEPARATOR, II_SEPARATOR, "");
	AppendMenu(menuOptions, MF_STRING | MF_POPUP, (UINT)menuProgramMode, GetProgramText(PTI_PROGRAM_MODE));
	if(settings.programType == Settings::ProgramType::EMPTY)
	{
		AppendMenu(menuProgramMode, MF_STRING | MF_CHECKED, II_PROGRAM_TYPE_EMPTY, GetProgramText(PTI_PROGRAM_TYPE_EMPTY));
		AppendMenu(menuProgramMode, MF_STRING | MF_UNCHECKED, II_PROGRAM_TYPE_CUBE_WORLD, GetProgramText(PTI_PROGRAM_TYPE_CUBE_WORLD));
	}
	if(settings.programType == Settings::ProgramType::CUBE_WORLD)
	{
		AppendMenu(menuProgramMode, MF_STRING | MF_UNCHECKED, II_PROGRAM_TYPE_EMPTY, GetProgramText(PTI_PROGRAM_TYPE_EMPTY));
		AppendMenu(menuProgramMode, MF_STRING | MF_CHECKED, II_PROGRAM_TYPE_CUBE_WORLD, GetProgramText(PTI_PROGRAM_TYPE_CUBE_WORLD));
	}

	AppendMenu(menuMain, MF_STRING | MF_POPUP, (UINT)menuEditingMode, GetProgramText(PTI_EDITION_MODE));
	AppendMenu(menuEditingMode, MF_STRING | MF_CHECKED, II_ADD_OR_REMOVE, GetProgramText(PTI_ADD_OR_REMOVE));
	AppendMenu(menuEditingMode, MF_STRING, II_BRUSHING, GetProgramText(PTI_BRUSHING));
	AppendMenu(menuEditingMode, MF_STRING, II_FILLING, GetProgramText(PTI_FILLING));
	AppendMenu(menuEditingMode, MF_STRING, II_WORK_WITH_GROUPS_OF_VOXELS, GetProgramText(PTI_WORK_WITH_GROUPS_OF_VOXELS));

	AppendMenu(menuMain, MF_STRING | MF_POPUP, (UINT)menuRotateModel, GetProgramText(PTI_ROTATE_MODEL));
	AppendMenu(menuRotateModel, MF_STRING, II_ROTATE_MODEL_AROUND_X_CLOCKWISE, GetProgramText(PTI_ROTATE_MODEL_AROUND_X_CLOCKWISE));
	AppendMenu(menuRotateModel, MF_STRING, II_ROTATE_MODEL_AROUND_X_COUNTERCLOCKWISE, GetProgramText(PTI_ROTATE_MODEL_AROUND_X_COUNTERCLOCKWISE));
	AppendMenu(menuRotateModel, MF_STRING, II_ROTATE_MODEL_AROUND_Y_CLOCKWISE, GetProgramText(PTI_ROTATE_MODEL_AROUND_Y_CLOCKWISE));
	AppendMenu(menuRotateModel, MF_STRING, II_ROTATE_MODEL_AROUND_Y_COUNTERCLOCKWISE, GetProgramText(PTI_ROTATE_MODEL_AROUND_Y_COUNTERCLOCKWISE));
	AppendMenu(menuRotateModel, MF_STRING, II_ROTATE_MODEL_AROUND_Z_CLOCKWISE, GetProgramText(PTI_ROTATE_MODEL_AROUND_Z_CLOCKWISE));
	AppendMenu(menuRotateModel, MF_STRING, II_ROTATE_MODEL_AROUND_Z_COUNTERCLOCKWISE, GetProgramText(PTI_ROTATE_MODEL_AROUND_Z_COUNTERCLOCKWISE));

	AppendMenu(menuMain, MF_STRING | MF_POPUP, (UINT)menuControlMode, GetProgramText(PTI_CONTROL_MODE));
	if(settings.cameraType == settings.WASD_SPACE_CTRL)
	{
		AppendMenu(menuControlMode, MF_STRING | MF_CHECKED, II_WASD_SPACE_CTRL, GetProgramText(PTI_WASD_SPACE_CTRL));
		AppendMenu(menuControlMode, MF_STRING | MF_UNCHECKED, II_MOUSE, GetProgramText(PTI_MOUSE));
	}
	else
	{
		AppendMenu(menuControlMode, MF_STRING | MF_UNCHECKED, II_WASD_SPACE_CTRL, GetProgramText(PTI_WASD_SPACE_CTRL));
		AppendMenu(menuControlMode, MF_STRING | MF_CHECKED, II_MOUSE, GetProgramText(PTI_MOUSE));
	}

	AppendMenu(menuMain, MF_STRING | MF_POPUP, (UINT)menuStartingCameraPosition, GetProgramText(PTI_CAMERA_POSITION));
	AppendMenu(menuStartingCameraPosition, MF_STRING | MF_POPUP, (UINT)menuCameraHorizontalPosition, GetProgramText(PTI_HORIZONTAL_POSITION));
	if(settings.cameraStartingAzimuth == Camera::AzimuthID::FRONT)
	{
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_CHECKED, II_FRONT, GetProgramText(PTI_FRONT));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_BACK, GetProgramText(PTI_BACK));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_LEFT, GetProgramText(PTI_LEFT));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_RIGHT, GetProgramText(PTI_RIGHT));
	}
	if(settings.cameraStartingAzimuth == Camera::AzimuthID::BACK)
	{
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_FRONT, GetProgramText(PTI_FRONT));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_CHECKED, II_BACK, GetProgramText(PTI_BACK));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_LEFT, GetProgramText(PTI_LEFT));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_RIGHT, GetProgramText(PTI_RIGHT));
	}
	if(settings.cameraStartingAzimuth == Camera::AzimuthID::LEFT)
	{
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_FRONT, GetProgramText(PTI_FRONT));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_BACK, GetProgramText(PTI_BACK));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_CHECKED, II_LEFT, GetProgramText(PTI_LEFT));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_RIGHT, GetProgramText(PTI_RIGHT));
	}
	if(settings.cameraStartingAzimuth == Camera::AzimuthID::RIGHT)
	{
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_FRONT, GetProgramText(PTI_FRONT));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_BACK, GetProgramText(PTI_BACK));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_UNCHECKED, II_LEFT, GetProgramText(PTI_LEFT));
		AppendMenu(menuCameraHorizontalPosition, MF_STRING | MF_CHECKED, II_RIGHT, GetProgramText(PTI_RIGHT));
	}
	AppendMenu(menuStartingCameraPosition, MF_STRING | MF_POPUP, (UINT)menuCameraVerticalPosition, GetProgramText(PTI_VERTICAL_POSITION));
	if(settings.cameraStartingZenith == Camera::ZenithID::TOP)
	{
		AppendMenu(menuCameraVerticalPosition, MF_STRING | MF_CHECKED, II_TOP, GetProgramText(PTI_TOP));
		AppendMenu(menuCameraVerticalPosition, MF_STRING | MF_UNCHECKED, II_MIDDLE, GetProgramText(PTI_MIDDLE));
		AppendMenu(menuCameraVerticalPosition, MF_STRING | MF_UNCHECKED, II_BOTTOM, GetProgramText(PTI_BOTTOM));
	}
	if(settings.cameraStartingZenith == Camera::ZenithID::MIDDLE)
	{
		AppendMenu(menuCameraVerticalPosition, MF_STRING | MF_UNCHECKED, II_TOP, GetProgramText(PTI_TOP));
		AppendMenu(menuCameraVerticalPosition, MF_STRING | MF_CHECKED, II_MIDDLE, GetProgramText(PTI_MIDDLE));
		AppendMenu(menuCameraVerticalPosition, MF_STRING | MF_UNCHECKED, II_BOTTOM, GetProgramText(PTI_BOTTOM));
	}
	if(settings.cameraStartingZenith == Camera::ZenithID::BOTTOM)
	{
		AppendMenu(menuCameraVerticalPosition, MF_STRING | MF_UNCHECKED, II_TOP, GetProgramText(PTI_TOP));
		AppendMenu(menuCameraVerticalPosition, MF_STRING | MF_UNCHECKED, II_MIDDLE, GetProgramText(PTI_MIDDLE));
		AppendMenu(menuCameraVerticalPosition, MF_STRING | MF_CHECKED, II_BOTTOM, GetProgramText(PTI_BOTTOM));
	}

	AppendMenu(menuMain, MF_STRING | MF_POPUP, (UINT)menuInfo, GetProgramText(PTI_INFO));
	AppendMenu(menuInfo, MF_STRING, II_CONTROL_INFORMATION, GetProgramText(PTI_CONTROL_INFORMATION));
	AppendMenu(menuInfo, MF_STRING, II_EDITING_MODE_INFORMATION, GetProgramText(PTI_EDITING_MODE_INFORMATION));
	AppendMenu(menuInfo, MF_SEPARATOR, II_SEPARATOR, "");
	AppendMenu(menuInfo, MF_STRING, II_FROM_AUTHOR, GetProgramText(PTI_FROM_AUTHOR));
}

void Interface::SetProgramMenu()
{
	SetMenu(hMainWindow, menuMain);
}

void Interface::DestroyProgramMenu()
{
	DestroyMenu(menuMain);
}

void Interface::SaveSettingToFile(Camera & _camera, Database & _database)
{
	ofstream ofs_settings("./configs/settings.cfg");
	if(ofs_settings.is_open())
	{
		int cameraType = _camera.GetCameraType();
		int cameraStartingZenith = _camera.GetZenithID();
		int cameraStartingAzimuth = _camera.GetAzimuthID();
		int lang = language;
		bool isFrameVisible = settings.isFrameVisible;
		bool isContourVisible = settings.isContourVisible;
		bool isWireframeVisible = settings.isWireframeVisible;
		bool isExtendedAsix = settings.isExtendedAsix;
		bool isCopiedVoxelsPreviewVisible = settings.isCopiedVoxelsPreviewVisible;
		bool isCopiedVoxelsPreviewTransparent = settings.isCopiedVoxelsPreviewTransparent;

		int extraSize = 0;
		if(settings.programType == Settings::ProgramType::EMPTY)
		{
			extraSize = 250;//listViewSizeOnX
		}

		ofs_settings << cameraType << endl;
		ofs_settings << cameraStartingZenith << endl;
		ofs_settings << cameraStartingAzimuth << endl;
		ofs_settings << lang << endl;
		ofs_settings << isContourVisible << endl;
		ofs_settings << isFrameVisible << endl;
		ofs_settings << isWireframeVisible << endl;
		ofs_settings << isExtendedAsix << endl;
		ofs_settings << (colorWindowPos.x - 13) + extraSize << endl;
		ofs_settings << (colorWindowPos.y + 38) << endl;
		ofs_settings << mainWindowPos.x << endl;
		ofs_settings << mainWindowPos.y << endl;
		ofs_settings << mainWindowRect.right + extraSize << endl;
		ofs_settings << mainWindowRect.bottom << endl;
		ofs_settings << settings.programType << endl;
		ofs_settings << isCopiedVoxelsPreviewVisible << endl;
		ofs_settings << isCopiedVoxelsPreviewTransparent << endl;
		ofs_settings.close();
	}

	ofstream ofs_palette("./configs/palette.cfg");
	if(ofs_palette.is_open())
	{
		for(int i = 0; i < 16; i++)
		{
			ofs_palette << dColors[i] << endl;
		}
		ofs_palette.close();
	}


	ofstream ofs_databasePath("./configs/databasePath.cfg");
	if(ofs_databasePath.is_open())
	{
		string path = _database.GetPathToDatabaseFolder();
		ofs_databasePath << path;
		ofs_databasePath.close();
	}

}

void Interface::LoadSettingFromFile(Camera & _camera, Database & _database, Frame & frame)
{
	ifstream ifs("./configs/settings.cfg");
	if(ifs.is_open())
	{
		int cameraType;
		int cameraStartingZenith;
		int cameraStartingAzimuth;
		int lang;
		bool isFrameVisible;
		bool isContourVisible;
		bool isWireframeVisible;
		bool isExtendedAsix;
		UINT programType;
		bool isCopiedVoxelsPreviewVisible;
		bool isCopiedVoxelsPreviewTransparent;

		ifs >> cameraType;
		ifs >> cameraStartingZenith;
		ifs >> cameraStartingAzimuth;
		ifs >> lang;
		ifs >> isContourVisible;
		ifs >> isFrameVisible;
		ifs >> isWireframeVisible;
		ifs >> isExtendedAsix;
		ifs >> colorWindowPos.x;
		ifs >> colorWindowPos.y;
		ifs >> mainWindowPos.x;
		ifs >> mainWindowPos.y;
		ifs >> mainWindowRect.right;
		ifs >> mainWindowRect.bottom;
		ifs >> programType;
		ifs >> isCopiedVoxelsPreviewVisible;
		ifs >> isCopiedVoxelsPreviewTransparent;

		_camera.SetCameraType(cameraType);
		settings.cameraType = (Settings::CameraType)cameraType;
		_camera.SetZenithID(cameraStartingZenith);
		settings.cameraStartingZenith = cameraStartingZenith;
		_camera.SetAzimuthID(cameraStartingAzimuth);
		settings.cameraStartingAzimuth = cameraStartingAzimuth;
		language = (Language)lang;
		settings.isContourVisible = isContourVisible;
		settings.isFrameVisible = isFrameVisible;
		if(isFrameVisible)
		{
			frame.ShowFrame();
		}
		else
		{
			frame.HideFrame();
		}
		settings.isWireframeVisible = isWireframeVisible;
		settings.isExtendedAsix = isExtendedAsix;
		if(programType == 0)
		{
			settings.programType = Settings::ProgramType::EMPTY;
		}
		else if(programType == 1)
		{
			settings.programType = Settings::ProgramType::CUBE_WORLD;
		}

		settings.isCopiedVoxelsPreviewTransparent = isCopiedVoxelsPreviewTransparent;
		settings.isCopiedVoxelsPreviewVisible = isCopiedVoxelsPreviewVisible;

		ifs.close();
	}

	FILE *F = NULL;
	fopen_s(&F, "./configs/databasePath.cfg", "r");
	if(F != NULL)
	{
		string pathToDatabaseFolder;

		char path[MAX_PATH];
		fgets(path, MAX_PATH, F);
		pathToDatabaseFolder = path;
		_database.SetPathToDatabaseFolder(pathToDatabaseFolder);
		fclose(F);
	}

	ifstream ifs_palette("./configs/palette.cfg");
	if(ifs_palette.is_open())
	{
		for(int i = 0; i < 16; i++)
		{
			ifs_palette >> dColors[i];
		}
		ifs_palette.close();
	}
}

void Interface::AddItemToListView(char *_item, int _row)
{
	LVITEM lvItem;
	memset(&lvItem, 0, sizeof(LVITEM));
	lvItem.pszText = _item;
	lvItem.iItem = _row;
	lvItem.iSubItem = 0;
	ListView_InsertItem(hListView, &lvItem);
	lvItem.iSubItem = 0;
	lvItem.pszText = _item;
	ListView_SetItemText(hListView, _row, 0, _item);
}

void Interface::AddModelsNamesToListView(Database & _database)
{
	ListView_DeleteAllItems(hListView);

	vector<string> modelsNames = _database.GetModelsNames();

	int i = 0;
	vector<string>::iterator iter = modelsNames.begin();
	while(iter != modelsNames.end())
	{
		AddItemToListView((char*)(*iter).c_str(), i);
		iter++;
		i++;
	}
}

void Interface::AddModelsNamesToListView(Database & _database, string _strToFind)
{
	ListView_DeleteAllItems(hListView);

	vector<string> modelsNames = _database.GetModelsNames();

	int i = 0;
	vector<string>::iterator iter = modelsNames.begin();
	while(iter != modelsNames.end())
	{
		size_t pos = (*iter).find(_strToFind);
		if(pos != std::string::npos)
		{
			AddItemToListView((char*)(*iter).c_str(), i);
			i++;
		}
		iter++;
	}
}

string Interface::GetPathToFolder()
{
	string result = "";
	BROWSEINFO bi = {0};
	bi.lpszTitle = GetProgramText(PTI_СHOOSE_CUBEWORLD_FOLDER);
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if(pidl != 0)
	{
		TCHAR path[MAX_PATH];
		if(SHGetPathFromIDList(pidl, path))
		{
			result = path;
		}
	}

	IMalloc * imalloc = 0;
	if(SUCCEEDED(SHGetMalloc(&imalloc)))
	{
		imalloc->Free(pidl);
		imalloc->Release();
	}
	return result;
}

void Interface::SetModelSizesToTextsFields(int xSize, int ySize, int zSize)
{
	SetNumberToTextPanel(hTextFieldX, xSize);
	SetNumberToTextPanel(hTextFieldY, ySize);
	SetNumberToTextPanel(hTextFieldZ, zSize);
	ShowWindow(hSubmitSizeChanges, SW_HIDE);
}

LRESULT __stdcall WndProcColorWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static Interface *GUI;
	static Model *model;
	static Camera *camera;
	static Database *database;
	static Clipboard *clipboard;
	static Frame *frame;
	static Directx *directx;

	static UINT selectedListViewItemNumber = -1;

	switch(msg)
	{
		case WM_CREATE:
		{
			CREATESTRUCT *crtstr = (CREATESTRUCT *)lParam;
			PointersToObjects *ptrs = (PointersToObjects*)crtstr->lpCreateParams;
			GUI = ptrs->GUI_ptr;
			model = ptrs->Model_ptr;
			camera = ptrs->Camera_ptr;
			database = ptrs->Database_ptr;
			frame = ptrs->Frame_ptr;
			directx = ptrs->Directx_ptr;
			clipboard = ptrs->Clipboard_ptr;
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_SIZE:
		{
			RECT mainWindowRect;
			GetWindowRect(GUI->hMainWindow, &mainWindowRect);
			GUI->mainWindowRect.right = mainWindowRect.right - mainWindowRect.left;
			GUI->mainWindowRect.bottom = mainWindowRect.bottom - mainWindowRect.top;
			RECT clientRect;
			GetClientRect(GUI->hMainWindow, &clientRect);

			GUI->listViewRect.bottom = clientRect.bottom - GUI->listViewRect.top;
			SetWindowPos(GUI->hListView, 0, 0, 0, GUI->listViewRect.right, GUI->listViewRect.bottom, SWP_NOZORDER | SWP_NOMOVE);
			GUI->renderTargetRect.right = clientRect.right - GUI->listViewRect.right;
			GUI->renderTargetRect.bottom = clientRect.bottom - GUI->renderTargetRect.top;
			SetWindowPos(GUI->hRenderTarget, 0, GUI->renderTargetRect.left, GUI->renderTargetRect.top, GUI->renderTargetRect.right, GUI->renderTargetRect.bottom, 0);

			if(directx->d3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				directx->d3dPresentParameters.BackBufferWidth = GUI->renderTargetRect.right;
				directx->d3dPresentParameters.BackBufferHeight = GUI->renderTargetRect.bottom;

				directx->vbForModels->Release();
				directx->vbForContour->Release();
				directx->vbForLines->Release();
				directx->vbLinesForFrame->Release();
				directx->vbForWireframe->Release();
				directx->vbVerticesForFrame->Release();
				directx->vbForClipboard->Release();

				directx->d3dDevice->Reset(&directx->d3dPresentParameters);

				D3DXMATRIX matProj;
				D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 2.0f,
					(float)(directx->d3dPresentParameters.BackBufferWidth) / (float)(directx->d3dPresentParameters.BackBufferHeight),
					0.1f, 1000.0f);
				directx->d3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

				directx->d3dDevice->SetRenderState(D3DRS_LIGHTING, true);
				directx->d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, DWORD(TRUE));
				directx->d3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, DWORD(TRUE));
				directx->d3dDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
				directx->d3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);

				directx->SetLight();

				directx->d3dDevice->CreateVertexBuffer(MAX_VERTICES*sizeof(Vertex), D3DUSAGE_DYNAMIC, (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE), D3DPOOL_DEFAULT, &directx->vbForModels, NULL);
				directx->d3dDevice->CreateVertexBuffer(24 * sizeof(Point), D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &directx->vbLinesForFrame, NULL);
				directx->d3dDevice->CreateVertexBuffer(MAX_WIREFRAMES*sizeof(Wireframe), D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &directx->vbForWireframe, NULL);
				directx->d3dDevice->CreateVertexBuffer(MAX_CONTOURS*sizeof(Contour), D3DUSAGE_DYNAMIC, (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE), D3DPOOL_DEFAULT, &directx->vbForContour, NULL);
				directx->d3dDevice->CreateVertexBuffer(6 * sizeof(Line), D3DUSAGE_DYNAMIC, (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE), D3DPOOL_DEFAULT, &directx->vbForLines, NULL);
				directx->d3dDevice->CreateVertexBuffer(MAX_VERTICES*sizeof(Vertex), D3DUSAGE_DYNAMIC, (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE), D3DPOOL_DEFAULT, &directx->vbForClipboard, NULL);
				directx->d3dDevice->CreateVertexBuffer(36 * sizeof(Vertex), D3DUSAGE_DYNAMIC, (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE), D3DPOOL_DEFAULT, &directx->vbVerticesForFrame, NULL);
			}

		}
		case WM_MOVE:
		{
			RECT rect;
			GetWindowRect(GUI->hMainWindow, &rect);

			GUI->mainWindowPos.x = rect.left/*(int)LOWORD(lParam)*/;
			GUI->mainWindowPos.y = rect.top/*(int)HIWORD(lParam)*/;
			break;
		}
		case WM_MOUSEMOVE:
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(GUI->hRenderTarget, &pt);

			if(pt.x > -1 && pt.x < GUI->renderTargetRect.right && pt.y > -1 && pt.y < GUI->renderTargetRect.bottom)
			{
				HWND hWndFocused = GetFocus();
				if(hWndFocused != GUI->hTextFieldX && hWndFocused != GUI->hTextFieldY && hWndFocused != GUI->hTextFieldZ && hWndFocused != GUI->hSearchTextField)
				{
					SetFocus(GUI->hRenderTarget);
				}
				switch(GUI->editingMode)
				{
					case Interface::EditingMode::ADD_OR_REMOVE_VOXELS:
					{
						if(GUI->isPipette)
						{
							SetCursor(GUI->pipetteCursor);
						}
						else
						{
							SetCursor(GUI->standartCursor);
						}

						break;
					}
					case Interface::EditingMode::BRUSHING:
					{
						if(GUI->isPipette)
						{
							SetCursor(GUI->pipetteCursor);
						}
						else
						{
							SetCursor(GUI->brushToolCursor);
						}
						break;
					}
					case Interface::EditingMode::FILLING:
					{
						if(GUI->isPipette)
						{
							SetCursor(GUI->pipetteCursor);
						}
						else
						{
							SetCursor(GUI->fillToolCursor);
						}
						break;
					}
					case Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS:
					{
						if(GUI->isPipette)
						{
							SetCursor(GUI->pipetteCursor);
						}
						else
						{
							SetCursor(GUI->frameToolCursor);
						}
						break;
					}
				}
			}
			else
			{
				SetCursor(GUI->standartCursor);
			}
			break;
		}
		case WM_NOTIFY:
		{
			LPNMHDR nmhdr;
			ZeroMemory(&nmhdr, sizeof(LPNMHDR));
			nmhdr = (LPNMHDR)lParam;

			//Change color of selected item
			NM_LISTVIEW *nmlv;
			nmlv = (NM_LISTVIEW *)lParam;
			if(nmlv->hdr.code == NM_CUSTOMDRAW)
			{
				NMLVCUSTOMDRAW *nmlvcd;
				nmlvcd = (NMLVCUSTOMDRAW *)lParam;
				if(nmlvcd->nmcd.dwDrawStage == CDDS_PREPAINT)
				{
					return CDRF_NOTIFYITEMDRAW;
				}
			}

			if(nmhdr->code == NM_CLICK)
			{
				LPNMLISTVIEW lvi = (LPNMLISTVIEW)lParam;
				LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN)lParam;

				LPNMKEY lpnmk = (LPNMKEY)lParam;
				if(nmhdr->hwndFrom == GUI->hListView)
				{
					int ic = ListView_GetItemCount(GUI->hListView);
					int n = 0;
					int i = 0;
					for(i = 0; i < ic; i++)
					{
						if(ListView_GetItemState(GUI->hListView, i, LVIS_SELECTED) & LVIS_SELECTED)
						{
							selectedListViewItemNumber = i;

							char itemText[MAX_PATH];
							ZeroMemory(itemText, sizeof(char)*MAX_PATH);
							ListView_GetItemText(GUI->hListView, i, 0, itemText, MAX_PATH);

							DWORD ***voxels = NULL;
							int xSize = 0, ySize = 0, zSize = 0;

							database->GetModel(&voxels, &xSize, &ySize, &zSize, itemText);
							database->SetOpenedModelName(string(itemText));
							model->SetVoxelsArray(voxels, xSize, ySize, zSize);
							camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
							GUI->SetModelSizesToTextsFields(xSize, ySize, zSize);
							camera->RestoreCameraPosition();
							frame->SetFramePos(frame->maxDimension, frame->maxDimension, frame->maxDimension);
						}
					}
				}
			}
			SetFocus(GUI->hRenderTarget);
		}
		case WM_COMMAND:
		{
			int interfaceIndentifier = LOWORD(wParam);
			switch(interfaceIndentifier)
			{
				case II_OPEN_DATABASE:
				{
					string pathToDatabaseFolder = GUI->GetPathToFolder();
					if(pathToDatabaseFolder != "")
					{
						database->AssignDatabase((LPSTR)GUI->GetProgramText(PTI_СHOOSE_CUBEWORLD_FOLDER), (char*)pathToDatabaseFolder.c_str());
						if(database->IsAssigned())
						{
							EnableMenuItem(GUI->menuFile, II_SAVE_MODEL, MF_ENABLED);
							EnableMenuItem(GUI->menuFile, II_EXPORT_MODEL, MF_ENABLED);
							EnableMenuItem(GUI->menuFile, II_INSERT_MODEL, MF_ENABLED);
							database->GetListOfModels();
							GUI->AddModelsNamesToListView(*database);
						}
					}
					break;
				}
				case II_SAVE_MODEL:
				{
					int xSize = 0, ySize = 0, zSize = 0;
					DWORD ***voxels = 0;
					model->GetVoxelsArray(&voxels, &xSize, &ySize, &zSize);
					database->UpdateModel(voxels, xSize, ySize, zSize);
					break;
				}
				case II_LOAD_MODEL_OUTSIDE_DATABASE:
				{
					char szFileName[MAX_PATH] = "";
					OPENFILENAME ofn;
					ZeroMemory(&ofn, sizeof(OPENFILENAME));

					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = GUI->hMainWindow;
					ofn.lpstrFilter = "Voxel Model Files (*.cub)\0*.cub\0All Files (*.*)\0*.*\0";
					ofn.lpstrFile = szFileName;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					ofn.lpstrDefExt = "txt";

					if(GetOpenFileName(&ofn))
					{
						FILE* fileOfModel = NULL;
						fopen_s(&fileOfModel, (ofn).lpstrFile, "rb");

						if(fileOfModel != NULL)
						{
							model->DeallocateMemory();

							int xSize = 0, ySize = 0, zSize = 0;
							fread(&zSize, 4, 1, fileOfModel);
							fread(&xSize, 4, 1, fileOfModel);
							fread(&ySize, 4, 1, fileOfModel);

							model->AllocateMemory(xSize, ySize, zSize);
							model->ChangeModelSize(xSize, ySize, zSize);

							for(int y = 0; y < ySize; y++)
							{
								for(int x = xSize - 1; x > -1; x--)
								{
									for(int z = 0; z < zSize; z++)
									{
										DWORD color;
										fread(&color, 3, 1, fileOfModel);
										SwapCol(&color);
										if(color != 0)
										{
											model->AddVoxel(x, y, z, color);
										}
									}
								}
							}
							camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
							camera->RestoreCameraPosition();
							GUI->SetModelSizesToTextsFields(xSize, ySize, zSize);
							frame->SetFramePos(frame->maxDimension, frame->maxDimension, frame->maxDimension);
							fclose(fileOfModel);
						}
					}
					break;
				}
				case II_SAVE_MODEL_OUTSIDE_DATABASE:
				{
					char szFileName[MAX_PATH] = "";
					if(database->GetOpenedModelName().length() != 0)
					{
						memcpy(&szFileName, database->GetOpenedModelName().c_str(), sizeof(char)*database->GetOpenedModelName().length());
					}

					OPENFILENAME ofn;
					ZeroMemory(&ofn, sizeof(OPENFILENAME));

					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = GUI->hMainWindow;
					ofn.lpstrFilter = "Voxel Model Files (*.cub)\0*.cub\0All Files (*.*)\0*.*\0";
					ofn.lpstrFile = szFileName;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					ofn.lpstrDefExt = "txt";

					if(GetSaveFileName(&ofn))
					{
						FILE *fileOfModel = NULL;
						fopen_s(&fileOfModel, (const char*)ofn.lpstrFile, "wb");

						if(fileOfModel != NULL)
						{
							int xSize = model->GetModelSizeX();
							int ySize = model->GetModelSizeY();
							int zSize = model->GetModelSizeZ();

							fwrite(&zSize, 4, 1, fileOfModel);
							fwrite(&xSize, 4, 1, fileOfModel);
							fwrite(&ySize, 4, 1, fileOfModel);

							for(int y = 0; y < ySize; y++)
							{
								for(int x = 0; x < xSize; x++)
								{
									for(int z = 0; z < zSize; z++)
									{
										DWORD color = model->GetVoxelColor(xSize - x - 1, y, z);
										SwapCol(&color);
										fwrite(&color, 3, 1, fileOfModel);
									}
								}
							}
							fclose(fileOfModel);
						}
					}
					break;
				}
				case II_NEW_MODEL_OUTSIDE_DATABASE:
				{
					model->ReallocateMemory(1, 1, 1);
					model->CleanModel(1, 1, 1);
					//model->AddVoxel(0,0,0,16777215);
					camera->SetCameraSettingsDependsTheModelSizes(1, 1, 1);
					camera->RestoreCameraPosition();
					GUI->SetModelSizesToTextsFields(1, 1, 1);
					frame->SetFramePos(frame->maxDimension, frame->maxDimension, frame->maxDimension);
					break;
				}
				case II_INSERT_MODEL:
				{
					database->InsertModel(GUI->hMainWindow);
					break;
				}
				case II_EXPORT_MODEL:
				{
					database->ExportModel(GUI->hMainWindow);
					break;
				}
				case II_CONVERT_PICTURE_TO_MODEL:
				{
					HANDLE hBmp;
					byte *modelBits = NULL;
					BITMAP Bitmap;

					char szFileName[MAX_PATH] = "";
					OPENFILENAME ofn;
					ZeroMemory(&ofn, sizeof(OPENFILENAME));

					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = GUI->hMainWindow;
					ofn.lpstrFilter = "BMP file (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0";
					ofn.lpstrFile = szFileName;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					ofn.lpstrDefExt = "bmp";

					if(GetOpenFileName(&ofn))
					{
						hBmp = LoadImage(0, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
						GetObject(hBmp, sizeof(BITMAP), &Bitmap);


						if(Bitmap.bmBitsPixel == 24)
						{
							model->AllocateMemory(Bitmap.bmWidth, Bitmap.bmHeight, 1);
							model->ChangeModelSize(Bitmap.bmWidth, Bitmap.bmHeight, 1);

							int colorArray_sizeOfByte = Bitmap.bmHeight*Bitmap.bmWidth * 4;
							int totalPixels = Bitmap.bmHeight*Bitmap.bmWidth;

							modelBits = (byte*)malloc(colorArray_sizeOfByte);
							ZeroMemory(modelBits, colorArray_sizeOfByte);
							modelBits = (byte*)Bitmap.bmBits;

							for(int i = 0, offset = 0, offset_test = 0; offset_test < colorArray_sizeOfByte; i++, offset += 3, offset_test += 4)
							{
								int x = i%Bitmap.bmWidth;
								int y = i / Bitmap.bmWidth;

								byte r = 0, g = 0, b = 0;
								memcpy(&r, modelBits + offset + 0, 1);
								memcpy(&g, modelBits + offset + 1, 1);
								memcpy(&b, modelBits + offset + 2, 1);

								DWORD color = RGBA2DWORD(r, g, b, 1);
								SwapCol(&color);
								model->AddVoxel(x, y, 0, color);

								if(x == Bitmap.bmWidth - 1)
								{
									int extrabytes = (4 - (Bitmap.bmWidth * 3) % 4) % 4;
									offset += extrabytes;
								}
							}
							int xSize = model->GetModelSizeX();
							int ySize = model->GetModelSizeY();
							int zSize = model->GetModelSizeZ();
							camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
							camera->RestoreCameraPosition();
							GUI->SetModelSizesToTextsFields(xSize, ySize, zSize);
							frame->SetFramePos(frame->maxDimension, frame->maxDimension, frame->maxDimension);
						}
						else
						{
							MessageBox(0, GUI->GetProgramText(PTI_TO_CONVERT_BMP_NEED_24_DEPTH_COLOR), "", MB_TOPMOST);
						}
					}
					break;
				}
				case II_SIZE_OF_MODEL:
				{
					CheckOrUncheckMenuItem(GUI->menuOptions, II_SIZE_OF_MODEL);

					DWORD menuState = GetMenuState(GUI->menuOptions, II_SIZE_OF_MODEL, 0);
					if(menuState & MF_CHECKED)
					{
						GUI->settings.isContourVisible = true;
						EnableMenuItem(GUI->menuAxisOptions, II_SAVE_MODEL, MF_ENABLED);
					}
					else
					{
						GUI->settings.isContourVisible = false;
						EnableMenuItem(GUI->menuAxisOptions, II_SAVE_MODEL, MF_DISABLED);
					}
					break;
				}
				case II_FRAME:
				{
					CheckOrUncheckMenuItem(GUI->menuOptions, II_FRAME);

					DWORD menuState = GetMenuState(GUI->menuOptions, II_FRAME, 0);
					if(menuState & MF_CHECKED)
					{
						frame->ShowFrame();
						GUI->settings.isFrameVisible = true;
					}
					else
					{
						frame->HideFrame();
						GUI->settings.isFrameVisible = false;
					}
					break;
				}
				case II_WIREFRAME:
				{
					CheckOrUncheckMenuItem(GUI->menuOptions, II_WIREFRAME);
					DWORD menuState = GetMenuState(GUI->menuOptions, II_WIREFRAME, 0);
					if(menuState & MF_CHECKED)
					{
						GUI->settings.isWireframeVisible = true;
					}
					else
					{
						GUI->settings.isWireframeVisible = false;
					}
					break;
				}
				case II_AXIS_SIZE_MODEL:
				{
					GUI->settings.isExtendedAsix = false;
					CheckMenuItem(GUI->menuAxisOptions, II_AXIS_SIZE_MODEL, MF_CHECKED);
					CheckMenuItem(GUI->menuAxisOptions, II_AXIS_ENDLESS, MF_UNCHECKED);
					break;
				}
				case II_AXIS_ENDLESS:
				{
					GUI->settings.isExtendedAsix = true;
					CheckMenuItem(GUI->menuAxisOptions, II_AXIS_SIZE_MODEL, MF_UNCHECKED);
					CheckMenuItem(GUI->menuAxisOptions, II_AXIS_ENDLESS, MF_CHECKED);
					break;
				}
				case II_RESTORE_VIEW:
				{
					camera->RestoreCameraPosition();
					break;
				}
				case II_LANGUAGE_RUSSIAN:
				{
					CheckMenuItem(GUI->menuLanguage, II_LANGUAGE_RUSSIAN, MF_CHECKED);
					CheckMenuItem(GUI->menuLanguage, II_LANGUAGE_ENGLISH, MF_UNCHECKED);
					GUI->language = Interface::Language::RUSSIAN;
					MessageBox(0, GUI->GetProgramText(PTI_TO_CONFIRM_CHANGES_PLEASE_RESTART), "", MB_TOPMOST);
					break;
				}
				case II_LANGUAGE_ENGLISH:
				{
					CheckMenuItem(GUI->menuLanguage, II_LANGUAGE_RUSSIAN, MF_UNCHECKED);
					CheckMenuItem(GUI->menuLanguage, II_LANGUAGE_ENGLISH, MF_CHECKED);
					GUI->language = Interface::Language::ENGLISH;
					MessageBox(0, GUI->GetProgramText(PTI_TO_CONFIRM_CHANGES_PLEASE_RESTART), "", MB_TOPMOST);
					break;
				}
				case II_IS_COPIED_VOXELS_PREVIEW:
				{
					CheckOrUncheckMenuItem(GUI->menuCopiedVoxelsDisplaySettings, II_IS_COPIED_VOXELS_PREVIEW);
					if(IsMenuItemActive(GUI->menuCopiedVoxelsDisplaySettings, II_IS_COPIED_VOXELS_PREVIEW))
					{
						GUI->settings.isCopiedVoxelsPreviewVisible = true;
					}
					else
					{
						GUI->settings.isCopiedVoxelsPreviewVisible = false;
					}
					break;
				}
				case II_IS_COPIED_VOXELS_PREVIEW_TRANSPARENT:
				{
					CheckOrUncheckMenuItem(GUI->menuCopiedVoxelsDisplaySettings, II_IS_COPIED_VOXELS_PREVIEW_TRANSPARENT);
					if(IsMenuItemActive(GUI->menuCopiedVoxelsDisplaySettings, II_IS_COPIED_VOXELS_PREVIEW_TRANSPARENT))
					{
						GUI->settings.isCopiedVoxelsPreviewTransparent = true;
					}
					else
					{
						GUI->settings.isCopiedVoxelsPreviewTransparent = false;
					}
					break;
				}
				case II_PROGRAM_TYPE_EMPTY:
				{
					CheckMenuItem(GUI->menuOptions, II_PROGRAM_TYPE_EMPTY, MF_CHECKED);
					CheckMenuItem(GUI->menuOptions, II_PROGRAM_TYPE_CUBE_WORLD, MF_UNCHECKED);
					GUI->settings.programType = Interface::Settings::ProgramType::EMPTY;
					MessageBox(0, GUI->GetProgramText(PTI_TO_CONFIRM_CHANGES_PLEASE_RESTART), "", MB_TOPMOST);
					break;
				}
				case II_PROGRAM_TYPE_CUBE_WORLD:
				{
					CheckMenuItem(GUI->menuOptions, II_PROGRAM_TYPE_EMPTY, MF_UNCHECKED);
					CheckMenuItem(GUI->menuOptions, II_PROGRAM_TYPE_CUBE_WORLD, MF_CHECKED);
					GUI->settings.programType = Interface::Settings::ProgramType::CUBE_WORLD;
					MessageBox(0, GUI->GetProgramText(PTI_TO_CONFIRM_CHANGES_PLEASE_RESTART), "", MB_TOPMOST);
					break;
				}
				case II_WASD_SPACE_CTRL:
				{
					CheckMenuItem(GUI->menuControlMode, II_WASD_SPACE_CTRL, MF_CHECKED);
					CheckMenuItem(GUI->menuControlMode, II_MOUSE, MF_UNCHECKED);

					camera->SetCameraType(camera->WASD_SPACE_CONTROL);
					camera->RestoreCameraPosition();
					break;
				}
				case II_MOUSE:
				{
					CheckMenuItem(GUI->menuControlMode, II_MOUSE, MF_CHECKED);
					CheckMenuItem(GUI->menuControlMode, II_WASD_SPACE_CTRL, MF_UNCHECKED);

					camera->SetCameraType(camera->MOUSE);
					camera->RestoreCameraPosition();
					break;
				}
				case II_PIPETTE:
				{
					SetCursor(GUI->pipetteCursor);
					GUI->isPipette = true;
					break;
				}
				case II_ADD_OR_REMOVE:
				{
					CheckMenuItem(GUI->menuEditingMode, II_ADD_OR_REMOVE, MF_CHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_BRUSHING, MF_UNCHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_FILLING, MF_UNCHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_WORK_WITH_GROUPS_OF_VOXELS, MF_UNCHECKED);

					GUI->editingMode = GUI->ADD_OR_REMOVE_VOXELS;
					SetCursor(GUI->standartCursor);
					frame->RestoreFrameResizeAndMove();
					break;
				}
				case II_BRUSHING:
				{
					CheckMenuItem(GUI->menuEditingMode, II_BRUSHING, MF_CHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_ADD_OR_REMOVE, MF_UNCHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_FILLING, MF_UNCHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_WORK_WITH_GROUPS_OF_VOXELS, MF_UNCHECKED);

					GUI->editingMode = GUI->BRUSHING;
					SetCursor(GUI->brushToolCursor);
					frame->RestoreFrameResizeAndMove();
					break;
				}
				case II_FILLING:
				{
					CheckMenuItem(GUI->menuEditingMode, II_FILLING, MF_CHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_ADD_OR_REMOVE, MF_UNCHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_BRUSHING, MF_UNCHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_WORK_WITH_GROUPS_OF_VOXELS, MF_UNCHECKED);

					GUI->editingMode = GUI->FILLING;
					SetCursor(GUI->fillToolCursor);
					frame->RestoreFrameResizeAndMove();
					break;
				}
				case II_WORK_WITH_GROUPS_OF_VOXELS:
				{
					CheckMenuItem(GUI->menuEditingMode, II_FILLING, MF_UNCHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_ADD_OR_REMOVE, MF_UNCHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_BRUSHING, MF_UNCHECKED);
					CheckMenuItem(GUI->menuEditingMode, II_WORK_WITH_GROUPS_OF_VOXELS, MF_CHECKED);

					GUI->editingMode = GUI->WORK_WITH_GROUPS_OF_VOXELS;
					SetCursor(GUI->frameToolCursor);
					break;
				}
				case II_FRONT:
				{
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_FRONT, MF_CHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_BACK, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_LEFT, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_RIGHT, MF_UNCHECKED);

					camera->SetAzimuthID(Camera::AzimuthID::FRONT);
					camera->RestoreCameraPosition();
					break;
				}
				case II_BACK:
				{
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_BACK, MF_CHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_FRONT, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_LEFT, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_RIGHT, MF_UNCHECKED);

					camera->SetAzimuthID(Camera::AzimuthID::BACK);
					camera->RestoreCameraPosition();
					break;
				}
				case II_LEFT:
				{
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_LEFT, MF_CHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_FRONT, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_BACK, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_RIGHT, MF_UNCHECKED);

					camera->SetAzimuthID(Camera::AzimuthID::LEFT);
					camera->RestoreCameraPosition();
					break;
				}
				case II_RIGHT:
				{
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_RIGHT, MF_CHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_FRONT, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_BACK, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraHorizontalPosition, II_LEFT, MF_UNCHECKED);

					camera->SetAzimuthID(Camera::AzimuthID::RIGHT);
					camera->RestoreCameraPosition();
					break;
				}
				case II_TOP:
				{
					CheckMenuItem(GUI->menuCameraVerticalPosition, II_TOP, MF_CHECKED);
					CheckMenuItem(GUI->menuCameraVerticalPosition, II_MIDDLE, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraVerticalPosition, II_BOTTOM, MF_UNCHECKED);

					camera->SetZenithID(Camera::ZenithID::TOP);
					camera->RestoreCameraPosition();
					break;
				}
				case II_MIDDLE:
				{
					CheckMenuItem(GUI->menuCameraVerticalPosition, II_MIDDLE, MF_CHECKED);
					CheckMenuItem(GUI->menuCameraVerticalPosition, II_TOP, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraVerticalPosition, II_BOTTOM, MF_UNCHECKED);

					camera->SetZenithID(Camera::ZenithID::MIDDLE);
					camera->RestoreCameraPosition();
					break;
				}
				case II_BOTTOM:
				{
					CheckMenuItem(GUI->menuCameraVerticalPosition, II_BOTTOM, MF_CHECKED);
					CheckMenuItem(GUI->menuCameraVerticalPosition, II_TOP, MF_UNCHECKED);
					CheckMenuItem(GUI->menuCameraVerticalPosition, II_MIDDLE, MF_UNCHECKED);

					camera->SetZenithID(Camera::ZenithID::BOTTOM);
					camera->RestoreCameraPosition();
					break;
				}
				case II_INCREMENT_X:
				{
					int value = GetNumberFromTextPanel(GUI->hTextFieldX);
					value++;
					SetNumberToTextPanel(GUI->hTextFieldX, value);
					ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
					SetFocus(GUI->hRenderTarget);
					model->IncreaseModelOnX();
					int xSize = model->GetModelSizeX();
					int ySize = model->GetModelSizeY();
					int zSize = model->GetModelSizeZ();
					camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
					break;
				}
				case II_DECREMENT_X:
				{
					int value = GetNumberFromTextPanel(GUI->hTextFieldX);
					value--;
					if(value == 0)
					{
						value = 1;
					}
					else
					{
						model->DecreaseModelOnX();
						int xSize = model->GetModelSizeX();
						int ySize = model->GetModelSizeY();
						int zSize = model->GetModelSizeZ();
						camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
					}
					SetNumberToTextPanel(GUI->hTextFieldX, value);
					ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
					SetFocus(GUI->hRenderTarget);
					break;
				}
				case II_INCREMENT_Y:
				{
					int value = GetNumberFromTextPanel(GUI->hTextFieldY);
					value++;
					SetNumberToTextPanel(GUI->hTextFieldY, value);
					ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
					SetFocus(GUI->hRenderTarget);
					model->IncreaseModelOnY();
					int xSize = model->GetModelSizeX();
					int ySize = model->GetModelSizeY();
					int zSize = model->GetModelSizeZ();
					camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
					break;
				}
				case II_DECREMENT_Y:
				{
					int value = GetNumberFromTextPanel(GUI->hTextFieldY);
					value--;
					if(value == 0)
					{
						value = 1;
					}
					else
					{
						model->DecreaseModelOnY();
						int xSize = model->GetModelSizeX();
						int ySize = model->GetModelSizeY();
						int zSize = model->GetModelSizeZ();
						camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
					}
					SetNumberToTextPanel(GUI->hTextFieldY, value);
					ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
					SetFocus(GUI->hRenderTarget);
					break;
				}
				case II_INCREMENT_Z:
				{
					int value = GetNumberFromTextPanel(GUI->hTextFieldZ);
					value++;
					SetNumberToTextPanel(GUI->hTextFieldZ, value);
					ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
					SetFocus(GUI->hRenderTarget);
					model->IncreaseModelOnZ();
					int xSize = model->GetModelSizeX();
					int ySize = model->GetModelSizeY();
					int zSize = model->GetModelSizeZ();
					camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
					break;
				}
				case II_DECREMENT_Z:
				{
					int value = GetNumberFromTextPanel(GUI->hTextFieldZ);
					value--;
					if(value == 0)
					{
						value = 1;
					}
					else
					{
						model->DecreaseModelOnZ();
						int xSize = model->GetModelSizeX();
						int ySize = model->GetModelSizeY();
						int zSize = model->GetModelSizeZ();
						camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
					}
					SetNumberToTextPanel(GUI->hTextFieldZ, value);
					ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
					SetFocus(GUI->hRenderTarget);
					break;
				}
				case II_TEXT_FIELD_X:
				{
					ShowWindow(GUI->hSubmitSizeChanges, SW_SHOW);
					UpdateWindow(GUI->hMainWindow);
					break;
				}
				case II_TEXT_FIELD_Y:
				{
					ShowWindow(GUI->hSubmitSizeChanges, SW_SHOW);
					UpdateWindow(GUI->hMainWindow);
					break;
				}
				case II_TEXT_FIELD_Z:
				{
					ShowWindow(GUI->hSubmitSizeChanges, SW_SHOW);
					UpdateWindow(GUI->hMainWindow);
					break;
				}
				case II_SEARCH_TEXT_FIELD:
				{
					HWND inFocus = GetFocus();

					if(inFocus == GUI->hSearchTextField)
					{
						char textInEditField[MAX_PATH];
						ZeroMemory(textInEditField, MAX_PATH*sizeof(char));
						SendMessage(GUI->hSearchTextField, WM_GETTEXT, MAX_PATH, (LPARAM)textInEditField);
						if(strcmp(textInEditField, GUI->GetProgramText(PTI_FIND_MODEL_STARTING_TEXT)) == 0)
						{
							SendMessage(GUI->hSearchTextField, WM_SETTEXT, 0, (LPARAM)"");
						}
					}
					break;
				}
				case II_SEARCH_BUTTON_FIND:
				{
					SetFocus(GUI->hRenderTarget);

					char buffer[MAX_PATH];
					ZeroMemory(buffer, sizeof(char)*MAX_PATH);
					SendMessage(GUI->hSearchTextField, WM_GETTEXT, MAX_PATH, (LPARAM)buffer);

					string strToFing = buffer;
					GUI->AddModelsNamesToListView(*database, strToFing);

					break;
				}
				case II_SEARCH_BUTTON_RESET:
				{
					SendMessage(GUI->hSearchTextField, WM_SETTEXT, 0, (LPARAM)GUI->GetProgramText(PTI_FIND_MODEL_STARTING_TEXT));
					GUI->AddModelsNamesToListView(*database);
					SetFocus(GUI->hRenderTarget);
					break;
				}
				case II_SUBMIT_SIZE_CHANGES:
				{
					ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
					UpdateWindow(GUI->hMainWindow);
					SetFocus(GUI->hRenderTarget);

					int xSize = GetNumberFromTextPanel(GUI->hTextFieldX);
					int ySize = GetNumberFromTextPanel(GUI->hTextFieldY);
					int zSize = GetNumberFromTextPanel(GUI->hTextFieldZ);

					if(xSize > 0 && ySize > 0 && zSize > 0)
					{
						model->ReallocateMemory(xSize, ySize, zSize);
						camera->SetCameraSettingsDependsTheModelSizes(xSize, ySize, zSize);
					}
					break;
				}
				case II_FROM_AUTHOR:
				{
					MessageBox(0, GUI->GetProgramText(PTI_MESSAGEBOX_INFO), "", MB_TOPMOST);
					break;
				}
				case II_CONTROL_INFORMATION:
				{
					MessageBox(0, GUI->GetProgramText(PTI_HOTKEYS_INFORMATION_MESSAGEBOX), "", MB_TOPMOST);
					break;
				}
				case II_EDITING_MODE_INFORMATION:
				{
					MessageBox(0, GUI->GetProgramText(PTI_EDITING_MODE_MESSAGEBOX), "", MB_TOPMOST);
					break;
				}
				case II_ROTATE_MODEL_AROUND_X_CLOCKWISE:
				{
					if(GUI->editingMode == Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
					{
						int xFrame = frame->GetExtraSizeX();
						int yFrame = frame->GetExtraSizeY();
						int zFrame = frame->GetExtraSizeZ();
						Swap(&yFrame, &zFrame);
						frame->SetExtraSizeX(xFrame);
						frame->SetExtraSizeY(yFrame);
						frame->SetExtraSizeZ(zFrame);
						clipboard->RotateModelAroundXClockwise();
					}
					else
					{
						int xSize = GetNumberFromTextPanel(GUI->hTextFieldX);
						int ySize = GetNumberFromTextPanel(GUI->hTextFieldY);
						int zSize = GetNumberFromTextPanel(GUI->hTextFieldZ);
						model->RotateModelAroundXClockwise();
						SetNumberToTextPanel(GUI->hTextFieldX, xSize);
						SetNumberToTextPanel(GUI->hTextFieldY, zSize);
						SetNumberToTextPanel(GUI->hTextFieldZ, ySize);
						ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
						camera->SetCameraSettingsDependsTheModelSizes(xSize, zSize, ySize);
					}

					break;
				}
				case II_ROTATE_MODEL_AROUND_Y_CLOCKWISE:
				{
					if(GUI->editingMode == Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
					{
						int xFrame = frame->GetExtraSizeX();
						int yFrame = frame->GetExtraSizeY();
						int zFrame = frame->GetExtraSizeZ();
						Swap(&xFrame, &zFrame);
						frame->SetExtraSizeX(xFrame);
						frame->SetExtraSizeY(yFrame);
						frame->SetExtraSizeZ(zFrame);
						clipboard->RotateModelAroundYClockwise();
					}
					else
					{
						int xSize = GetNumberFromTextPanel(GUI->hTextFieldX);
						int ySize = GetNumberFromTextPanel(GUI->hTextFieldY);
						int zSize = GetNumberFromTextPanel(GUI->hTextFieldZ);
						model->RotateModelAroundYClockwise();
						SetNumberToTextPanel(GUI->hTextFieldX, zSize);
						SetNumberToTextPanel(GUI->hTextFieldY, ySize);
						SetNumberToTextPanel(GUI->hTextFieldZ, xSize);
						ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
						camera->SetCameraSettingsDependsTheModelSizes(zSize, ySize, xSize);
					}

					break;
				}
				case II_ROTATE_MODEL_AROUND_Z_CLOCKWISE:
				{
					if(GUI->editingMode == Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
					{
						int xFrame = frame->GetExtraSizeX();
						int yFrame = frame->GetExtraSizeY();
						int zFrame = frame->GetExtraSizeZ();
						Swap(&yFrame, &xFrame);
						frame->SetExtraSizeX(xFrame);
						frame->SetExtraSizeY(yFrame);
						frame->SetExtraSizeZ(zFrame);
						clipboard->RotateModelAroundZClockwise();
					}
					else
					{
						int xSize = GetNumberFromTextPanel(GUI->hTextFieldX);
						int ySize = GetNumberFromTextPanel(GUI->hTextFieldY);
						int zSize = GetNumberFromTextPanel(GUI->hTextFieldZ);
						model->RotateModelAroundZClockwise();
						SetNumberToTextPanel(GUI->hTextFieldX, ySize);
						SetNumberToTextPanel(GUI->hTextFieldY, xSize);
						SetNumberToTextPanel(GUI->hTextFieldZ, zSize);
						ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
						camera->SetCameraSettingsDependsTheModelSizes(ySize, xSize, zSize);
					}

					break;
				}
				case II_ROTATE_MODEL_AROUND_X_COUNTERCLOCKWISE:
				{
					if(GUI->editingMode == Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
					{
						clipboard->RotateModelAroundXCounterclockwise();
						int xFrame = frame->GetExtraSizeX();
						int yFrame = frame->GetExtraSizeY();
						int zFrame = frame->GetExtraSizeZ();
						Swap(&yFrame, &zFrame);
						frame->SetExtraSizeX(xFrame);
						frame->SetExtraSizeY(yFrame);
						frame->SetExtraSizeZ(zFrame);
					}
					else
					{
						int xSize = GetNumberFromTextPanel(GUI->hTextFieldX);
						int ySize = GetNumberFromTextPanel(GUI->hTextFieldY);
						int zSize = GetNumberFromTextPanel(GUI->hTextFieldZ);
						model->RotateModelAroundXCounterclockwise();
						SetNumberToTextPanel(GUI->hTextFieldX, xSize);
						SetNumberToTextPanel(GUI->hTextFieldY, zSize);
						SetNumberToTextPanel(GUI->hTextFieldZ, ySize);
						ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
						camera->SetCameraSettingsDependsTheModelSizes(xSize, zSize, ySize);
					}

					break;
				}
				case II_ROTATE_MODEL_AROUND_Y_COUNTERCLOCKWISE:
				{
					if(GUI->editingMode == Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
					{
						clipboard->RotateModelAroundYCounterclockwise();
						int xFrame = frame->GetExtraSizeX();
						int yFrame = frame->GetExtraSizeY();
						int zFrame = frame->GetExtraSizeZ();
						Swap(&zFrame, &xFrame);
						frame->SetExtraSizeX(xFrame);
						frame->SetExtraSizeY(yFrame);
						frame->SetExtraSizeZ(zFrame);
					}
					else
					{
						int xSize = GetNumberFromTextPanel(GUI->hTextFieldX);
						int ySize = GetNumberFromTextPanel(GUI->hTextFieldY);
						int zSize = GetNumberFromTextPanel(GUI->hTextFieldZ);
						model->RotateModelAroundYCounterclockwise();
						SetNumberToTextPanel(GUI->hTextFieldX, zSize);
						SetNumberToTextPanel(GUI->hTextFieldY, ySize);
						SetNumberToTextPanel(GUI->hTextFieldZ, xSize);
						ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
						camera->SetCameraSettingsDependsTheModelSizes(zSize, ySize, xSize);
					}

					break;
				}
				case II_ROTATE_MODEL_AROUND_Z_COUNTERCLOCKWISE:
				{
					if(GUI->editingMode == Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
					{
						clipboard->RotateModelAroundZCounterclockwise();
						int xFrame = frame->GetExtraSizeX();
						int yFrame = frame->GetExtraSizeY();
						int zFrame = frame->GetExtraSizeZ();
						Swap(&yFrame, &xFrame);
						frame->SetExtraSizeX(xFrame);
						frame->SetExtraSizeY(yFrame);
						frame->SetExtraSizeZ(zFrame);
					}
					else
					{
						int xSize = GetNumberFromTextPanel(GUI->hTextFieldX);
						int ySize = GetNumberFromTextPanel(GUI->hTextFieldY);
						int zSize = GetNumberFromTextPanel(GUI->hTextFieldZ);
						model->RotateModelAroundZCounterclockwise();
						SetNumberToTextPanel(GUI->hTextFieldX, ySize);
						SetNumberToTextPanel(GUI->hTextFieldY, xSize);
						SetNumberToTextPanel(GUI->hTextFieldZ, zSize);
						ShowWindow(GUI->hSubmitSizeChanges, SW_HIDE);
						camera->SetCameraSettingsDependsTheModelSizes(ySize, xSize, zSize);
					}
					break;
				}
				case II_EXPORT_AS_STL_FILE:
				{					
					char szFileName[MAX_PATH] = "";
					if(database->GetOpenedModelName().length() != 0)
					{
						memcpy(&szFileName, database->GetOpenedModelName().c_str(), sizeof(char)*database->GetOpenedModelName().length());
					}

					OPENFILENAME ofn = {0};
					ZeroMemory(&ofn, sizeof(OPENFILENAME));

					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = GUI->hMainWindow;
					ofn.lpstrFilter = "STL file (*.stl)\0*.stl\0All Files (*.*)\0*.*\0";
					ofn.lpstrFile = szFileName;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					ofn.lpstrDefExt = "txt";

					if(GetSaveFileName(&ofn))
					{
						FILE *fileOfModel = NULL;
						fopen_s(&fileOfModel, (const char*)ofn.lpstrFile, "wb");

						if(fileOfModel != NULL)
						{
							struct Data
							{
								float v1x, v1y, v1z;
								float v2x, v2y, v2z;
								float v3x, v3y, v3z;
								float nx, ny, nz;
							};

							vector<Data> fileData;

							///================================================================
							int xSize = model->GetModelSizeX();
							int ySize = model->GetModelSizeY();
							int zSize = model->GetModelSizeZ();

							for(int x = 0; x < xSize; x++)
							{
								for(int y = 0; y < ySize; y++)
								{
									for(int z = 0; z < zSize; z++)
									{
										DWORD c = model->GetVoxelColor(x, y, z);
										if(c != 0)
										{
											//----find neighborhoods----
											int y_up = y + 1;
											int y_down = y - 1;
											int x_left = x - 1;
											int x_right = x + 1;
											int z_near = z - 1;
											int z_far = z + 1;

											bool isVoxelPlaced_up = false;
											bool isVoxelPlaced_down = false;
											bool isVoxelPlaced_left = false;
											bool isVoxelPlaced_right = false;
											bool isVoxelPlaced_near = false;
											bool isVoxelPlaced_far = false;

											if(model->IsCoordinateCorrect(x, y_up, z))
											{
												if(model->GetVoxelColor(x, y_up, z) != 0)
												{
													isVoxelPlaced_up = true;
												}
											}
											if(model->IsCoordinateCorrect(x, y_down, z))
											{
												if(model->GetVoxelColor(x, y_down, z) != 0)
												{
													isVoxelPlaced_down = true;
												}
											}

											if(model->IsCoordinateCorrect(x_left, y, z))
											{
												if(model->GetVoxelColor(x_left, y, z) != 0)
												{
													isVoxelPlaced_left = true;
												}
											}
											if(model->IsCoordinateCorrect(x_right, y, z))
											{
												if(model->GetVoxelColor(x_right, y, z) != 0)
												{
													isVoxelPlaced_right = true;
												}
											}

											if(model->IsCoordinateCorrect(x, y, z_near))
											{
												if(model->GetVoxelColor(x, y, z_near) != 0)
												{
													isVoxelPlaced_near = true;
												}
											}
											if(model->IsCoordinateCorrect(x, y, z_far))
											{
												if(model->GetVoxelColor(x, y, z_far) != 0)
												{
													isVoxelPlaced_far = true;
												}
											}
											
											if(isVoxelPlaced_up && isVoxelPlaced_down && isVoxelPlaced_left && isVoxelPlaced_right && isVoxelPlaced_near && isVoxelPlaced_far)
											{
												continue;
											}
											else
											{
												if(!isVoxelPlaced_up)
												{
													Data t1 = {0}, t2 = {0};

													t1.nx = 0;
													t1.ny = 1;
													t1.nz = 0;

													t2.nx = 0;
													t2.ny = 1;
													t2.nz = 0;

													t1.v1x = (float)x + 0; t1.v1y = (float)y + 1; t1.v1z = (float)z + 0; 
													t1.v2x = (float)x + 0; t1.v2y = (float)y + 1; t1.v2z = (float)z + 1; 
													t1.v3x = (float)x + 1; t1.v3y = (float)y + 1; t1.v3z = (float)z + 1; 

													t2.v1x = (float)x + 0; t2.v1y = (float)y + 1; t2.v1z = (float)z + 0; 
													t2.v2x = (float)x + 1; t2.v2y = (float)y + 1; t2.v2z = (float)z + 1; 
													t2.v3x = (float)x + 1; t2.v3y = (float)y + 1; t2.v3z = (float)z + 0; 

													fileData.push_back(t1);
													fileData.push_back(t2);
												}
												if(!isVoxelPlaced_down)
												{
													Data t1 = {0}, t2 = {0};

													t1.nx = 0;
													t1.ny = -1;
													t1.nz = 0;

													t2.nx = 0;
													t2.ny = -1;
													t2.nz = 0;

													t1.v1x = (float)x + 0; t1.v1y = (float)y + 0; t1.v1z = (float)z + 1; 
													t1.v2x = (float)x + 0; t1.v2y = (float)y + 0; t1.v2z = (float)z + 0; 
													t1.v3x = (float)x + 1; t1.v3y = (float)y + 0; t1.v3z = (float)z + 0; 

													t2.v1x = (float)x + 0; t2.v1y = (float)y + 0; t2.v1z = (float)z + 1; 
													t2.v2x = (float)x + 1; t2.v2y = (float)y + 0; t2.v2z = (float)z + 0; 
													t2.v3x = (float)x + 1; t2.v3y = (float)y + 0; t2.v3z = (float)z + 1; 

													fileData.push_back(t1);
													fileData.push_back(t2);
												}
												if(!isVoxelPlaced_left)
												{
													Data t1 = {0}, t2 = {0};

													t1.nx = -1;
													t1.ny = 0;
													t1.nz = 0;

													t2.nx = -1;
													t2.ny = 0;
													t2.nz = 0;

													t1.v1x = (float)x + 0; t1.v1y = (float)y + 0; t1.v1z = (float)z + 1; 
													t1.v2x = (float)x + 0; t1.v2y = (float)y + 1; t1.v2z = (float)z + 1; 
													t1.v3x = (float)x + 0; t1.v3y = (float)y + 1; t1.v3z = (float)z + 0; 

													t2.v1x = (float)x + 0; t2.v1y = (float)y + 0; t2.v1z = (float)z + 1; 
													t2.v2x = (float)x + 0; t2.v2y = (float)y + 1; t2.v2z = (float)z + 0; 
													t2.v3x = (float)x + 0; t2.v3y = (float)y + 0; t2.v3z = (float)z + 0; 

													fileData.push_back(t1);
													fileData.push_back(t2);
												}
												if(!isVoxelPlaced_right)
												{
													Data t1 = {0}, t2 = {0};

													t1.nx = 1;
													t1.ny = 0;
													t1.nz = 0;

													t2.nx = 1;
													t2.ny = 0;
													t2.nz = 0;

													t1.v1x = (float)x + 1; t1.v1y = (float)y + 0; t1.v1z = (float)z + 0; 
													t1.v2x = (float)x + 1; t1.v2y = (float)y + 1; t1.v2z = (float)z + 0; 
													t1.v3x = (float)x + 1; t1.v3y = (float)y + 1; t1.v3z = (float)z + 1; 

													t2.v1x = (float)x + 1; t2.v1y = (float)y + 0; t2.v1z = (float)z + 0; 
													t2.v2x = (float)x + 1; t2.v2y = (float)y + 1; t2.v2z = (float)z + 1; 
													t2.v3x = (float)x + 1; t2.v3y = (float)y + 0; t2.v3z = (float)z + 1; 

													fileData.push_back(t1);
													fileData.push_back(t2);
												}
												if(!isVoxelPlaced_near)
												{
													Data t1 = {0}, t2 = {0};

													t1.nx = 0;
													t1.ny = 0;
													t1.nz = -1;

													t2.nx = 0;
													t2.ny = 0;
													t2.nz = -1;

													t1.v1x = (float)x + 0; t1.v1y = (float)y + 0; t1.v1z = (float)z + 0; 
													t1.v2x = (float)x + 0; t1.v2y = (float)y + 1; t1.v2z = (float)z + 0; 
													t1.v3x = (float)x + 1; t1.v3y = (float)y + 1; t1.v3z = (float)z + 0; 

													t2.v1x = (float)x + 0; t2.v1y = (float)y + 0; t2.v1z = (float)z + 0; 
													t2.v2x = (float)x + 1; t2.v2y = (float)y + 1; t2.v2z = (float)z + 0; 
													t2.v3x = (float)x + 1; t2.v3y = (float)y + 0; t2.v3z = (float)z + 0; 

													fileData.push_back(t1);
													fileData.push_back(t2);
												}
												if(!isVoxelPlaced_far)
												{
													Data t1 = {0}, t2 = {0};

													t1.nx = 0;
													t1.ny = 0;
													t1.nz = 1;

													t2.nx = 0;
													t2.ny = 0;
													t2.nz = 1;

													t1.v1x = (float)x + 1; t1.v1y = (float)y + 0; t1.v1z = (float)z + 1; 
													t1.v2x = (float)x + 1; t1.v2y = (float)y + 1; t1.v2z = (float)z + 1; 
													t1.v3x = (float)x + 0; t1.v3y = (float)y + 1; t1.v3z = (float)z + 1; 

													t2.v1x = (float)x + 1; t2.v1y = (float)y + 0; t2.v1z = (float)z + 1; 
													t2.v2x = (float)x + 0; t2.v2y = (float)y + 1; t2.v2z = (float)z + 1; 
													t2.v3x = (float)x + 0; t2.v3y = (float)y + 0; t2.v3z = (float)z + 1; 

													fileData.push_back(t1);
													fileData.push_back(t2);
												}
											}
										}
									}
								}
							}

							/*fwrite(&zSize, 4, 1, fileOfModel);
							fwrite(&xSize, 4, 1, fileOfModel);
							fwrite(&ySize, 4, 1, fileOfModel);

							for(int y = 0; y < ySize; y++)
							{
								for(int x = 0; x < xSize; x++)
								{
									for(int z = 0; z < zSize; z++)
									{
										DWORD color = model->GetVoxelColor(xSize - x - 1, y, z);
										SwapCol(&color);
										fwrite(&color, 3, 1, fileOfModel);
									}
								}
							}*/

							uint8_t name[80] = "";
							uint32_t numTriangles = (uint32_t)fileData.size();
							uint16_t atrribByteCount = 0;

							fwrite(name, sizeof(uint8_t), 80, fileOfModel);
							fwrite(&numTriangles, sizeof(uint32_t), 1, fileOfModel);

							for(int i = 0; i < fileData.size(); i++)
							{
								fwrite(&fileData[i].nx, sizeof(float), 1, fileOfModel);
								fwrite(&fileData[i].ny, sizeof(float), 1, fileOfModel);
								fwrite(&fileData[i].nz, sizeof(float), 1, fileOfModel);

								fwrite(&fileData[i].v1x, sizeof(float), 1, fileOfModel);
								fwrite(&fileData[i].v1y, sizeof(float), 1, fileOfModel);
								fwrite(&fileData[i].v1z, sizeof(float), 1, fileOfModel);

								fwrite(&fileData[i].v2x, sizeof(float), 1, fileOfModel);
								fwrite(&fileData[i].v2y, sizeof(float), 1, fileOfModel);
								fwrite(&fileData[i].v2z, sizeof(float), 1, fileOfModel);

								fwrite(&fileData[i].v3x, sizeof(float), 1, fileOfModel);
								fwrite(&fileData[i].v3y, sizeof(float), 1, fileOfModel);
								fwrite(&fileData[i].v3z, sizeof(float), 1, fileOfModel);

								fwrite(&atrribByteCount, sizeof(uint16_t), 1, fileOfModel);
							}

							///================================================================
							fclose(fileOfModel);
						}
					}
					break;
				}
			}
			break;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}