#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <d3d9.h>

#include "defines.h"
#include "directx.h"
#include "interface.h"
#include "structs.h"
#include "camera.h"
#include "model.h"
#include "database.h"
#include "frame.h"
#include "functions.h"
#include "clipboard.h"

UINT_PTR CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	static POINT * cwPos;
	switch (uiMsg)
	{
		case WM_INITDIALOG:
		{
			CHOOSECOLOR *cc = (CHOOSECOLOR *)lParam;
			cwPos = (POINT*)cc->lpTemplateName;
		}
		case WM_MOVE:
		{
			int xPos = (int)LOWORD(lParam);
			int yPos = (int)HIWORD(lParam);

			if (xPos <1000 && yPos < 1000)
			{
				cwPos->x = xPos-22;
				cwPos->y = yPos-76;
			}			

			break;
		}
	}
	return 0;
}

DWORD WINAPI PickColor(void* Param)
{
	static POINT colorWindowPos;

	Interface *GUI = NULL;
	PointersToObjects *ptrs = (PointersToObjects*)Param;
	GUI = ptrs->GUI_ptr;
	bool ifFirstIteration = true;

	while (true)
	{

		MSG msg;
		ZeroMemory(&msg, sizeof( MSG ));
		if (PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE))
		{
			break;
		}
		else
		{
			CHOOSECOLOR	cc;
			ZeroMemory(&cc, sizeof( CHOOSECOLOR ));
			WNDCLASSEX wcPickColor = { 0 };
			wcPickColor.cbSize = sizeof(WNDCLASSEX);
			wcPickColor.lpfnWndProc = (WNDPROC)WndProcColorWindow;
			wcPickColor.style = CS_HREDRAW|CS_VREDRAW;
			wcPickColor.hInstance = 0;
			wcPickColor.lpszClassName = "ColorClass";
			wcPickColor.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcPickColor.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
			wcPickColor.lpszMenuName = NULL;
			if (!RegisterClassEx(&wcPickColor))
			{
				
			}
			if (ifFirstIteration)
			{
				colorWindowPos.x = GUI->colorWindowPos.x+13;
				colorWindowPos.y = GUI->colorWindowPos.y-38;
				ifFirstIteration = false;
			}
			
			GUI->hColorWindow = CreateWindow("ColorClass", "",
						 WS_OVERLAPPEDWINDOW,
						 colorWindowPos.x, colorWindowPos.y, CW_USEDEFAULT, CW_USEDEFAULT,
						 NULL, NULL, NULL, NULL);					

			cc.Flags = CC_RGBINIT|CC_ENABLEHOOK;
			cc.hwndOwner = GUI->hColorWindow;
			cc.lpCustColors = GUI->dColors;
			cc.lpTemplateName = (LPCSTR)&colorWindowPos;
			cc.lpfnHook = (LPCCHOOKPROC)CCHookProc;			
			cc.lStructSize = sizeof( CHOOSECOLOR );
			cc.rgbResult = RGB(255, 255, 255);

			if (ChooseColor(&cc))
			{
				HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
				DWORD rgbCurrent = cc.rgbResult;
				SwapCol(&rgbCurrent);
				if (rgbCurrent==0)
				{
					rgbCurrent = 1; 
				}
				GUI->color = rgbCurrent;
				GUI->colorWindowPos = colorWindowPos;				
				SetForegroundWindow(GUI->hMainWindow);
				DeleteObject(hBrush);
			}
		}
	}
	return 0;
}

void KeyboardAndMouseProcessing(Directx & _directx, Camera & _camera, Clipboard & clipboard, Model & model, Frame & frame, Interface & GUI, Timer * timers)
{
	char keyboardState[MAX_KEYBOARD_STATES];
	ZeroMemory(keyboardState, sizeof(char)*MAX_KEYBOARD_STATES);
	HRESULT hResult = _directx.dInputKeyboard->GetDeviceState(sizeof(keyboardState), keyboardState);
	if (hResult!=DI_OK)
	{
		_directx.dInputKeyboard->Acquire();
	}

	DIMOUSESTATE mouseState;
	ZeroMemory(&mouseState, sizeof(DIMOUSESTATE));
	hResult = _directx.dInputMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
	if (hResult!=DI_OK)
	{
		_directx.dInputMouse->Acquire();
	}

	if (keyboardState[DIK_W])
	{
		_camera.MoveForward();
	}
	if (keyboardState[DIK_A])
	{
		_camera.MoveLeft();
	}
	if (keyboardState[DIK_S])
	{
		_camera.MoveBackward();
	}
	if (keyboardState[DIK_D])
	{
		_camera.MoveRight();
	}
	if (keyboardState[DIK_LCONTROL])
	{
		_camera.MoveDown();
	}
	if (keyboardState[DIK_SPACE])
	{
		_camera.MoveUp();
	}

	if (keyboardState[DIK_E])
	{
		_camera.RestoreCameraPosition();
	}
	if (keyboardState[DIK_Q])
	{
		SendMessage(GUI.hMainWindow, WM_COMMAND, II_PIPETTE, (LPARAM)GUI.menuEditingMode);
	}
	if (keyboardState[DIK_R])
	{
		frame.RestoreFrameResizeAndMove();
	}

	if (keyboardState[DIK_LSHIFT]&&keyboardState[DIK_C])
	{
		if (timers[TimersID::COPY_OR_PASTE_VOXELS_LIMITER].GetResult()>300.0f)
		{
			clipboard.CopyVoxels(frame.GetX()+frame.GetMoveX(), frame.GetY()+frame.GetMoveY(), frame.GetZ()+frame.GetMoveZ(), frame.GetX()+frame.GetExtraSizeX()+frame.GetMoveX()+1, frame.GetY()+frame.GetExtraSizeY()+frame.GetMoveY()+1, frame.GetZ()+frame.GetExtraSizeZ()+frame.GetMoveZ()+1, model);
			timers[TimersID::COPY_OR_PASTE_VOXELS_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_LSHIFT]&&keyboardState[DIK_V])
	{
		if (timers[TimersID::COPY_OR_PASTE_VOXELS_LIMITER].GetResult()>300.0f)
		{
			clipboard.PasteVoxels(frame.GetX()+frame.GetMoveX(), frame.GetY()+frame.GetMoveY(), frame.GetZ()+frame.GetMoveZ(), frame.GetX()+frame.GetExtraSizeX()+frame.GetMoveX()+1, frame.GetY()+frame.GetExtraSizeY()+frame.GetMoveY()+1, frame.GetZ()+frame.GetExtraSizeZ()+frame.GetMoveZ()+1, model);			
			timers[TimersID::COPY_OR_PASTE_VOXELS_LIMITER].StartTiming();
		}
	}

	if (keyboardState[DIK_1])
	{
		HWND hWnd = GetFocus();
		if (hWnd==GUI.hRenderTarget)
		{
			SendMessage(GUI.hMainWindow, WM_COMMAND, II_ADD_OR_REMOVE, (LPARAM)GUI.menuEditingMode);
			frame.RestoreFrameResizeAndMove();
		}
	}
	if (keyboardState[DIK_2])
	{
		HWND hWnd = GetFocus();
		if (hWnd==GUI.hRenderTarget)
		{
			SendMessage(GUI.hMainWindow, WM_COMMAND, II_BRUSHING, (LPARAM)GUI.menuEditingMode);
			frame.RestoreFrameResizeAndMove();
		}
	}
	if (keyboardState[DIK_3])
	{
		HWND hWnd = GetFocus();
		if (hWnd==GUI.hRenderTarget)
		{
			SendMessage(GUI.hMainWindow, WM_COMMAND, II_FILLING, (LPARAM)GUI.menuEditingMode);
			frame.RestoreFrameResizeAndMove();
		}
	}
	if (keyboardState[DIK_4])
	{
		HWND hWnd = GetFocus();
		if (hWnd==GUI.hRenderTarget)
		{
			SendMessage(GUI.hMainWindow, WM_COMMAND, II_WORK_WITH_GROUPS_OF_VOXELS, (LPARAM)GUI.menuEditingMode);
		}
	}

	if (keyboardState[DIK_F1])
	{
		if (timers[TimersID::HOTKEY_LIMITER].GetResult()>300.0f)
		{
			SendMessage(GUI.hMainWindow, WM_COMMAND, II_SIZE_OF_MODEL, (LPARAM)GUI.menuOptions);
			timers[TimersID::HOTKEY_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_F2])
	{
		if (timers[TimersID::HOTKEY_LIMITER].GetResult()>300.0f)
		{
			SendMessage(GUI.hMainWindow, WM_COMMAND, II_FRAME, (LPARAM)GUI.menuOptions);
			timers[TimersID::HOTKEY_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_F3])
	{
		if (timers[TimersID::HOTKEY_LIMITER].GetResult()>300.0f)
		{
			SendMessage(GUI.hMainWindow, WM_COMMAND, II_WIREFRAME, (LPARAM)GUI.menuOptions);
			timers[TimersID::HOTKEY_LIMITER].StartTiming();
		}
	}

	float modelMoveSpeedLimiter = 150.0f;
	if (keyboardState[DIK_RIGHT])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_X_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			float angle = 0.0f;
			_camera.GetAzimuthalDirectionsOfLook(&angle);
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					frame.IncreazeExtraSizeZ();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					frame.IncreazeExtraSizeX();
					frame.DecreazeMoveX();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					frame.IncreazeExtraSizeZ();
					frame.DecreazeMoveZ();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					frame.IncreazeExtraSizeX();
				}
			}
			else
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					model.IncreaseModelCoordinateOnZ();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					model.DecreaseModelCoordinateOnX();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					model.DecreaseModelCoordinateOnZ();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					model.IncreaseModelCoordinateOnX();
				}
			}
			timers[TimersID::WIREFRAME_INCREASING_X_SPEED_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_LEFT])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_X_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			float angle = 0.0f;
			_camera.GetAzimuthalDirectionsOfLook(&angle);
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					frame.DecreazeExtraSizeZ();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					if (frame.GetExtraSizeX()!=0)
					{
						frame.DecreazeExtraSizeX();
						frame.IncreazeMoveX();
					}
				}
				if (angle>135.0f && angle<=225.0f)
				{
					if (frame.GetExtraSizeZ()!=0)
					{
						frame.DecreazeExtraSizeZ();
						frame.IncreazeMoveZ();
					}
				}
				if (angle>225.0f && angle<=315.0f)
				{
					frame.DecreazeExtraSizeX();
				}
			}
			else
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					model.DecreaseModelCoordinateOnZ();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					model.IncreaseModelCoordinateOnX();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					model.IncreaseModelCoordinateOnZ();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					model.DecreaseModelCoordinateOnX();
				}
			}
			timers[TimersID::WIREFRAME_INCREASING_X_SPEED_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_UP])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_Z_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			float angle = 0.0f;
			_camera.GetAzimuthalDirectionsOfLook(&angle);
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					frame.IncreazeExtraSizeX();
					frame.DecreazeMoveX();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					frame.IncreazeExtraSizeZ();
					frame.DecreazeMoveZ();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					frame.IncreazeExtraSizeX();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					frame.IncreazeExtraSizeZ();
				}
			}
			else
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					model.DecreaseModelCoordinateOnX();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					model.DecreaseModelCoordinateOnZ();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					model.IncreaseModelCoordinateOnX();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					model.IncreaseModelCoordinateOnZ();
				}
			}
			timers[TimersID::WIREFRAME_INCREASING_Z_SPEED_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_DOWN])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_Z_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			float angle = 0.0f;
			_camera.GetAzimuthalDirectionsOfLook(&angle);
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					if (frame.GetExtraSizeX()!=0)
					{
						frame.DecreazeExtraSizeX();
						frame.IncreazeMoveX();
					}
				}
				if (angle>45.0f && angle<=135.0f)
				{
					if (frame.GetExtraSizeZ()!=0)
					{
						frame.DecreazeExtraSizeZ();
						frame.IncreazeMoveZ();
					}
				}
				if (angle>135.0f && angle<=225.0f)
				{
					frame.DecreazeExtraSizeX();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					frame.DecreazeExtraSizeZ();
				}
			}
			else
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					model.IncreaseModelCoordinateOnX();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					model.IncreaseModelCoordinateOnZ();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					model.DecreaseModelCoordinateOnX();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					model.DecreaseModelCoordinateOnZ();
				}
			}
			timers[TimersID::WIREFRAME_INCREASING_Z_SPEED_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_PRIOR])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_Y_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				frame.IncreazeExtraSizeY();
			}
			else
			{
				model.IncreaseModelCoordinateOnY();
			}
			timers[TimersID::WIREFRAME_INCREASING_Y_SPEED_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_NEXT])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_Y_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				frame.DecreazeExtraSizeY();
			}
			else
			{
				model.DecreaseModelCoordinateOnY();
			}
			timers[TimersID::WIREFRAME_INCREASING_Y_SPEED_LIMITER].StartTiming();
		}
	}

	if (keyboardState[DIK_NUMPAD5])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_Z_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			float angle = 0.0f;
			_camera.GetAzimuthalDirectionsOfLook(&angle);
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					frame.DecreazeMoveX();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					frame.DecreazeMoveZ();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					frame.IncreazeMoveX();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					frame.IncreazeMoveZ();
				}
			}
			timers[TimersID::WIREFRAME_INCREASING_Z_SPEED_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_NUMPAD2])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_Z_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			float angle = 0.0f;
			_camera.GetAzimuthalDirectionsOfLook(&angle);
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					frame.IncreazeMoveX();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					frame.IncreazeMoveZ();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					frame.DecreazeMoveX();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					frame.DecreazeMoveZ();
				}
			}
			timers[TimersID::WIREFRAME_INCREASING_Z_SPEED_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_NUMPAD1])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_X_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			float angle = 0.0f;
			_camera.GetAzimuthalDirectionsOfLook(&angle);
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					frame.DecreazeMoveZ();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					frame.IncreazeMoveX();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					frame.IncreazeMoveZ();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					frame.DecreazeMoveX();
				}
			}
			timers[TimersID::WIREFRAME_INCREASING_X_SPEED_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_NUMPAD3])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_X_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			float angle = 0.0f;
			_camera.GetAzimuthalDirectionsOfLook(&angle);
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				if ((angle>315.0f && angle<360.0f)||(angle>=0.0f && angle<=45.0f))
				{
					frame.IncreazeMoveZ();
				}
				if (angle>45.0f && angle<=135.0f)
				{
					frame.DecreazeMoveX();
				}
				if (angle>135.0f && angle<=225.0f)
				{
					frame.DecreazeMoveZ();
				}
				if (angle>225.0f && angle<=315.0f)
				{
					frame.IncreazeMoveX();
				}
			}
			timers[TimersID::WIREFRAME_INCREASING_X_SPEED_LIMITER].StartTiming();
		}
	}
	if (keyboardState[DIK_NUMPAD9])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_Y_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				frame.IncreazeMoveY();
				timers[TimersID::WIREFRAME_INCREASING_Y_SPEED_LIMITER].StartTiming();
			}
		}
	}
	if (keyboardState[DIK_NUMPAD6])
	{
		if (timers[TimersID::WIREFRAME_INCREASING_Y_SPEED_LIMITER].GetResult()>modelMoveSpeedLimiter)
		{
			if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
			{
				frame.DecreazeMoveY();
				timers[TimersID::WIREFRAME_INCREASING_Y_SPEED_LIMITER].StartTiming();
			}
		}
	}
	
	if (mouseState.rgbButtons[Directx::MouseButton::LEFT])
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(GUI.hRenderTarget, &pt);

		if (pt.x>-1&&pt.x < GUI.renderTargetRect.right && pt.y > -1&&pt.y<GUI.renderTargetRect.bottom)
		{
			SetFocus(GUI.hRenderTarget);
			if (GUI.isPipette==true)
			{
				int x = frame.GetX();
				int y = frame.GetY();
				int z = frame.GetZ();
				frame.TransformCoordinatesDependsDirectionOfIntersection(&x, &y, &z);
				GUI.color = model.GetVoxelColor(x, y, z);
				GUI.isPipette = false;
				timers[TimersID::ADD_VOXEL_LIMITER].StartTiming();
			}
			else
			{
				switch (GUI.editingMode)
				{
					case Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS:
					{
						if (timers[TimersID::ADD_VOXEL_LIMITER].GetResult()>200.0f)
						{
							if (!mouseState.rgbButtons[Directx::MouseButton::RIGHT])
							{
								for (int x = frame.GetX()+frame.GetMoveX(); x<frame.GetX()+frame.GetExtraSizeX()+frame.GetMoveX()+1; x++)
								{
									for (int y = frame.GetY()+frame.GetMoveY(); y<frame.GetY()+frame.GetExtraSizeY()+frame.GetMoveY()+1; y++)
									{
										for (int z = frame.GetZ()+frame.GetMoveZ(); z<frame.GetZ()+frame.GetExtraSizeZ()+frame.GetMoveZ()+1; z++)
										{
											model.AddVoxel(x, y, z, GUI.color);
										}
									}
								}
								timers[TimersID::ADD_VOXEL_LIMITER].StartTiming();
							}
						}
						SetCursor(GUI.frameToolCursor);
						break;
					}
					case Interface::EditingMode::ADD_OR_REMOVE_VOXELS:
					{
						if (timers[TimersID::ADD_VOXEL_LIMITER].GetResult()>200.0f)
						{
							if (!mouseState.rgbButtons[Directx::MouseButton::RIGHT])
							{								
								model.AddVoxel(frame.GetX(), frame.GetY(), frame.GetZ(), GUI.color);
								timers[TimersID::ADD_VOXEL_LIMITER].StartTiming();
							}																			 
						}
						SetCursor(GUI.standartCursor);
						break;
					}
					case Interface::EditingMode::FILLING:
					{
															if (timers[TimersID::FILLING_VOXEL_LIMITER].GetResult()>300.0f)
															{
																int x = frame.GetX();
																int y = frame.GetY();
																int z = frame.GetZ();
																frame.TransformCoordinatesDependsDirectionOfIntersection(&x, &y, &z);
																if (GUI.color!=model.GetVoxelColor(x, y, z))
																{
																	DWORD prevColor = model.GetVoxelColor(x, y, z);
																	model.BrushVoxel(x, y, z, GUI.color);
																	model.FillVoxels(x, y, z, GUI.color, prevColor);
																	timers[TimersID::FILLING_VOXEL_LIMITER].StartTiming();
																}
															}
															SetCursor(GUI.fillToolCursor);
															break;
					}
					case Interface::EditingMode::BRUSHING:
					{
															 if (timers[TimersID::BRUSHING_VOXEL_LIMITER].GetResult()>0.5f)
															 {
																 int x = frame.GetX();
																 int y = frame.GetY();
																 int z = frame.GetZ();
																 frame.TransformCoordinatesDependsDirectionOfIntersection(&x, &y, &z);
																 model.BrushVoxel(x, y, z, GUI.color);
																 timers[TimersID::BRUSHING_VOXEL_LIMITER].StartTiming();
															 }
															 SetCursor(GUI.brushToolCursor);
															 break;
					}
				}
			}
		}
	}

	if (mouseState.rgbButtons[Directx::MouseButton::RIGHT])
	{
		SetFocus(GUI.hRenderTarget);
		switch (_camera.GetCameraType())
		{		
			case Camera::CameraType::WASD_SPACE_CONTROL:
			{
				_camera.SetAzimuth((float)mouseState.lY);
				_camera.SetZenith((float)mouseState.lX);
				break;
			}
			case Camera::CameraType::MOUSE:
			{
				_camera.AddToAzimuth((float)(-1.0f)*mouseState.lX);
				_camera.AddToZenith((float)mouseState.lY);
				break;
			}
		}		
	}
	else
	{
		if (_camera.GetCameraType() == Camera::CameraType::WASD_SPACE_CONTROL)
		{
			_camera.SetAzimuth(0.0f);
			_camera.SetZenith(0.0f);			
		}		
	}

	if (mouseState.rgbButtons[Directx::MouseButton::MIDDLE])
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(GUI.hRenderTarget, &pt);

		if (pt.x>-1&&pt.x < GUI.renderTargetRect.right && pt.y > -1&&pt.y<GUI.renderTargetRect.bottom)
		{
			SetFocus(GUI.hRenderTarget);
			if (timers[TimersID::DELETE_VOXEL_LIMITER].GetResult() > 150.0f)
			{
				if (GUI.editingMode == Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
				{
					for (int x = frame.GetX()+frame.GetMoveX(); x<frame.GetX()+frame.GetExtraSizeX()+frame.GetMoveX()+1; x++)
					{
						for (int y = frame.GetY()+frame.GetMoveY(); y<frame.GetY()+frame.GetExtraSizeY()+frame.GetMoveY()+1; y++)
						{
							for (int z = frame.GetZ()+frame.GetMoveZ(); z<frame.GetZ()+frame.GetExtraSizeZ()+frame.GetMoveZ()+1; z++)
							{
								model.DeleteVoxel(x, y, z);
							}
						}
					}
					timers[TimersID::DELETE_VOXEL_LIMITER].StartTiming();
				}
				else
				{
					int xFrame = frame.GetX();
					int yFrame = frame.GetY();
					int zFrame = frame.GetZ();
					frame.TransformCoordinatesDependsDirectionOfIntersection(&xFrame, &yFrame, &zFrame);
					model.DeleteVoxel(xFrame, yFrame, zFrame);
				}		
				timers[TimersID::DELETE_VOXEL_LIMITER].StartTiming();
			}			
		}
	}

	if (mouseState.lZ && _camera.GetCameraType() == Camera::CameraType::MOUSE)
	{
		HWND hWnd = GetFocus();
		if (hWnd == GUI.hRenderTarget)
		{
			float addToRadius = _camera.GetRateOfChangeRadius();
			int dims_lZ_last = 0;
			int dims_lZ_now = mouseState.lZ;
			if (dims_lZ_last>dims_lZ_now)
			{
				_camera.AddToRadius(addToRadius);
			}
			else
			{
				if (_camera.GetRadius()-addToRadius>1.0f)
				{
					_camera.AddToRadius(-addToRadius);
				}
			}
		}		
	}
}

void CameraTransform(Directx & _directx, Camera & _camera)
{
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/2.0f,
		(float)(_directx.d3dPresentParameters.BackBufferWidth) / (float)(_directx.d3dPresentParameters.BackBufferHeight), 
		0.1f, 1000.0f );
	_directx.d3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX V;
	switch (_camera.cameraType)
	{
		case Camera::CameraType::WASD_SPACE_CONTROL:
		{
			D3DXMATRIX T;
			D3DXVECTOR3 right = _camera.GetRight();
			D3DXVECTOR3 pos = _camera.GetPos();
			D3DXVECTOR3 up = _camera.GetUp();
			D3DXVECTOR3 look = _camera.GetLook();

			D3DXMatrixRotationAxis(&T, &right, _camera.GetAzimuth());

			D3DXVec3TransformCoord(&up, &up, &T);
			D3DXVec3TransformCoord(&look, &look, &T);

			D3DXMatrixRotationY(&T, _camera.GetZenith());

			D3DXVec3TransformCoord(&right, &right, &T);
			D3DXVec3TransformCoord(&look, &look, &T);

			D3DXVec3Normalize(&look, &look);

			D3DXVec3Cross(&up, &look, &right);
			D3DXVec3Normalize(&up, &up);

			D3DXVec3Cross(&right, &up, &look);
			D3DXVec3Normalize(&right, &right);

			float x = -D3DXVec3Dot(&right, &pos);
			float y = -D3DXVec3Dot(&up, &pos);
			float z = -D3DXVec3Dot(&look, &pos);

			V._11 = right.x; V._12 = up.x; V._13 = look.x; V._14 = 0.0f;
			V._21 = right.y; V._22 = up.y; V._23 = look.y; V._24 = 0.0f;
			V._31 = right.z; V._32 = up.z; V._33 = look.z; V._34 = 0.0f;
			V._41 = x; V._42 = y; V._43 = z; V._44 = 1.0f;

			_camera.SetRight(right);
			_camera.SetPos(pos);
			_camera.SetUp(up);
			_camera.SetLook(look);
			break;
		}
		case Camera::CameraType::MOUSE:
		{
			D3DXVECTOR3 pos, pointToLook;
			_camera.ComputePos();
			pos = _camera.GetPos();
			pointToLook = _camera.GetPointToLook();
			D3DXMatrixLookAtLH(&V, &pos, &pointToLook, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			break;
		}
	}
	_directx.d3dDevice->SetTransform(D3DTS_VIEW, &V);
}

bool FillModelBuffer(IDirect3DVertexBuffer9 *_vb, Vertex *_vertices, Model & _model, int *_xStart, int *_yStart, int *_zStart, UINT *_vertexCounter)
{
	for (int y = *_yStart; y<_model.GetModelSizeY(); y++)
	{
		for (int x = *_xStart; x<_model.GetModelSizeX(); x++)
		{
			for (int z = *_zStart; z<_model.GetModelSizeZ(); z++)
			{
				DWORD c = _model.GetVoxelColor(x, y, z);
				if (c!=0)
				{
					//----find neighborhoods----
					int y_up = y+1;
					int y_down = y-1;
					int x_left = x-1;
					int x_right = x+1;
					int z_near = z-1;
					int z_far = z+1;

					bool isVoxelPlaced_up = false;
					bool isVoxelPlaced_down = false;
					bool isVoxelPlaced_left = false;
					bool isVoxelPlaced_right = false;
					bool isVoxelPlaced_near = false;
					bool isVoxelPlaced_far = false;

					if (_model.IsCoordinateCorrect(x, y_up, z))
					{
						if (_model.GetVoxelColor(x, y_up, z)!=0)
						{
							isVoxelPlaced_up = true;
						}
					}
					if (_model.IsCoordinateCorrect(x, y_down, z))
					{
						if (_model.GetVoxelColor(x, y_down, z)!=0)
						{
							isVoxelPlaced_down = true;
						}
					}

					if (_model.IsCoordinateCorrect(x_left, y, z))
					{
						if (_model.GetVoxelColor(x_left, y, z)!=0)
						{
							isVoxelPlaced_left = true;
						}
					}
					if (_model.IsCoordinateCorrect(x_right, y, z))
					{
						if (_model.GetVoxelColor(x_right, y, z)!=0)
						{
							isVoxelPlaced_right = true;
						}
					}

					if (_model.IsCoordinateCorrect(x, y, z_near))
					{
						if (_model.GetVoxelColor(x, y, z_near)!=0)
						{
							isVoxelPlaced_near = true;
						}
					}
					if (_model.IsCoordinateCorrect(x, y, z_far))
					{
						if (_model.GetVoxelColor(x, y, z_far)!=0)
						{
							isVoxelPlaced_far = true;
						}
					}

					//----filling buffers----
					if (isVoxelPlaced_up && isVoxelPlaced_down && isVoxelPlaced_left && isVoxelPlaced_right && isVoxelPlaced_near && isVoxelPlaced_far)
					{
						continue;
					}
					else
					{
						if (!isVoxelPlaced_up)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = 0.00000000f; ny = 0.99999994f; nz = 0.00000000f;
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y+0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y+0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y+0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y+0.5f, (float)z-0.5f, nx, ny, nz, c);
							if (( *_vertexCounter )==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_down)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = 0.00000000f; ny = -0.99999994f; nz = 0.00000000f;
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y-0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y-0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y-0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y-0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y-0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y-0.5f, (float)z+0.5f, nx, ny, nz, c);
							if (( *_vertexCounter )==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_left)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = -0.99999994f; ny = 0.00000000f; nz = 0.00000000f;
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y-0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y+0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y+0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y-0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y+0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y-0.5f, (float)z-0.5f, nx, ny, nz, c);
							if (( *_vertexCounter )==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_right)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = 0.99999994f; ny = 0.00000000f; nz = 0.00000000f;
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y-0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y+0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y-0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y-0.5f, (float)z+0.5f, nx, ny, nz, c);
							if (( *_vertexCounter )==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_near)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = 0.00000000f; ny = 0.00000000f; nz = -0.99999994f;
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y-0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y+0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y+0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y-0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y+0.5f, (float)z-0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y-0.5f, (float)z-0.5f, nx, ny, nz, c);
							if (( *_vertexCounter )==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_far)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = 0.00000000f; ny = -0.00000000f; nz = 0.99999994f;
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y-0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y+0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x+0.5f, (float)y-0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y+0.5f, (float)z+0.5f, nx, ny, nz, c);
							_vertices[( *_vertexCounter )++].SetVertex((float)x-0.5f, (float)y-0.5f, (float)z+0.5f, nx, ny, nz, c);
							if (( *_vertexCounter )==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
					}
				}
				if (x==_model.GetModelSizeX()-1&&y==_model.GetModelSizeY()-1&&z==_model.GetModelSizeZ()-1)
				{
					*_xStart = 0;
					*_yStart = 0;
					*_zStart = 0;

					return true;
				}
				if (x==_model.GetModelSizeX()-1&&*_xStart!=0)
				{
					*_xStart = 0;
				}
				if (y==_model.GetModelSizeY()-1&&*_yStart!=0)
				{
					*_yStart = 0;
				}
				if (z==_model.GetModelSizeZ()-1&&*_zStart!=0)
				{
					*_zStart = 0;
				}
			}
		}
	}

	return false;
}

bool FillClipboardBuffer(IDirect3DVertexBuffer9 *_vb, Vertex *_vertices, Clipboard & _clipboard, int *_xStart, int *_yStart, int *_zStart, UINT *_vertexCounter, Frame & frame)
{
	for (int y = *_yStart; y<_clipboard.GetModelSizeY(); y++)
	{
		for (int x = *_xStart; x<_clipboard.GetModelSizeX(); x++)
		{
			for (int z = *_zStart; z<_clipboard.GetModelSizeZ(); z++)
			{
				DWORD c = _clipboard.GetVoxelColor(x, y, z);
				int r = 0, g = 0, b = 0, a = 0;
				DWORD2RGBA(c, &g, &r, &a, &b);
				c = D3DCOLOR_ARGB(128, r, g, b);
				DWORD emptyVoxel = D3DCOLOR_ARGB(128, 0, 0, 0);

				if (c!=emptyVoxel)
				{
					//----find neighborhoods----
					int y_up = y+1;
					int y_down = y-1;
					int x_left = x-1;
					int x_right = x+1;
					int z_near = z-1;
					int z_far = z+1;

					bool isVoxelPlaced_up = false;
					bool isVoxelPlaced_down = false;
					bool isVoxelPlaced_left = false;
					bool isVoxelPlaced_right = false;
					bool isVoxelPlaced_near = false;
					bool isVoxelPlaced_far = false;

					if (_clipboard.IsCoordinateCorrect(x, y_up, z))
					{
						if (_clipboard.GetVoxelColor(x, y_up, z)!=0)
						{
							isVoxelPlaced_up = true;
						}
					}
					if (_clipboard.IsCoordinateCorrect(x, y_down, z))
					{
						if (_clipboard.GetVoxelColor(x, y_down, z)!=0)
						{
							isVoxelPlaced_down = true;
						}
					}

					if (_clipboard.IsCoordinateCorrect(x_left, y, z))
					{
						if (_clipboard.GetVoxelColor(x_left, y, z)!=0)
						{
							isVoxelPlaced_left = true;
						}
					}
					if (_clipboard.IsCoordinateCorrect(x_right, y, z))
					{
						if (_clipboard.GetVoxelColor(x_right, y, z)!=0)
						{
							isVoxelPlaced_right = true;
						}
					}

					if (_clipboard.IsCoordinateCorrect(x, y, z_near))
					{
						if (_clipboard.GetVoxelColor(x, y, z_near)!=0)
						{
							isVoxelPlaced_near = true;
						}
					}
					if (_clipboard.IsCoordinateCorrect(x, y, z_far))
					{
						if (_clipboard.GetVoxelColor(x, y, z_far)!=0)
						{
							isVoxelPlaced_far = true;
						}
					}

					//----filling buffers----
					if (isVoxelPlaced_up && isVoxelPlaced_down && isVoxelPlaced_left && isVoxelPlaced_right && isVoxelPlaced_near && isVoxelPlaced_far)
					{
						continue;
					}
					else
					{
						int xExtra = frame.GetX()+frame.GetMoveX();
						int yExtra = frame.GetY()+frame.GetMoveY();
						int zExtra = frame.GetZ()+frame.GetMoveZ();

						if (!isVoxelPlaced_up)
						{							
							float nx = 0, ny = 0, nz = 0;
							nx = 0.00000000f; ny = 0.99999994f; nz = 0.00000000f;
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra+0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra+0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra+0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra+0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra+0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra+0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							if ((*_vertexCounter)==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_down)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = 0.00000000f; ny = -0.99999994f; nz = 0.00000000f;
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra-0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra-0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra-0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra-0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra-0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra-0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							if ((*_vertexCounter)==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_left)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = -0.99999994f; ny = 0.00000000f; nz = 0.00000000f;
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra-0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra+0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra+0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra-0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra+0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra-0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							if ((*_vertexCounter)==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_right)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = 0.99999994f; ny = 0.00000000f; nz = 0.00000000f;
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra-0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra+0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra+0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra-0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra+0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra-0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							if ((*_vertexCounter)==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_near)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = 0.00000000f; ny = 0.00000000f; nz = -0.99999994f;
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra-0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra+0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra+0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra-0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra+0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra-0.5f, (float)z+zExtra-0.5f, nx, ny, nz, c);
							if ((*_vertexCounter)==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
						if (!isVoxelPlaced_far)
						{
							float nx = 0, ny = 0, nz = 0;
							nx = 0.00000000f; ny = -0.00000000f; nz = 0.99999994f;
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra-0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra+0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra+0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra+0.5f, (float)y+yExtra-0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra+0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							_vertices[(*_vertexCounter)++].SetVertex((float)x+xExtra-0.5f, (float)y+yExtra-0.5f, (float)z+zExtra+0.5f, nx, ny, nz, c);
							if ((*_vertexCounter)==MAX_VERTICES)
							{
								*_xStart = x;
								*_yStart = y;
								*_zStart = z;
								return false;
							}
						}
					}
				}
				if (x==_clipboard.GetModelSizeX()-1&&y==_clipboard.GetModelSizeY()-1&&z==_clipboard.GetModelSizeZ()-1)
				{
					*_xStart = 0;
					*_yStart = 0;
					*_zStart = 0;

					return true;
				}
				if (x==_clipboard.GetModelSizeX()-1&&*_xStart!=0)
				{
					*_xStart = 0;
				}
				if (y==_clipboard.GetModelSizeY()-1&&*_yStart!=0)
				{
					*_yStart = 0;
				}
				if (z==_clipboard.GetModelSizeZ()-1&&*_zStart!=0)
				{
					*_zStart = 0;
				}
			}
		}
	}

	return false;
}

void FillWireframeBuffer(IDirect3DVertexBuffer9 *_vb, Vertex *_vertices, Wireframe *_wireframes, UINT _vertexCount, UINT *_wireframesCount)
{
	for (int i=0; i<(int)_vertexCount; i+=3)
	{
		_wireframes[(*_wireframesCount)++].SetVertex(_vertices[i+0].x, _vertices[i+0].y, _vertices[i+0].z);
		_wireframes[(*_wireframesCount)++].SetVertex(_vertices[i+1].x, _vertices[i+1].y, _vertices[i+1].z);

		_wireframes[(*_wireframesCount)++].SetVertex(_vertices[i+1].x, _vertices[i+1].y, _vertices[i+1].z);
		_wireframes[(*_wireframesCount)++].SetVertex(_vertices[i+2].x, _vertices[i+2].y, _vertices[i+2].z);

		_wireframes[(*_wireframesCount)++].SetVertex(_vertices[i+2].x, _vertices[i+2].y, _vertices[i+2].z);
		_wireframes[(*_wireframesCount)++].SetVertex(_vertices[i+0].x, _vertices[i+0].y, _vertices[i+0].z);
	}

	Wireframe* vb_wireframes;
	_vb->Lock(0, 0, (void**)&vb_wireframes, D3DLOCK_DISCARD);
	memcpy(vb_wireframes, _wireframes, (*_wireframesCount)*sizeof( Wireframe ));
	_vb->Unlock();
}

void FillContourBuffer(IDirect3DVertexBuffer9 *vbForContour,Contour * vContour, UINT *contourCounter, int _xSize, int _ySize, int _zSize, Camera & camera)
{
	float nx=1.0f, ny=0.0f, nz=0.0f;
	unsigned long color = 16777215;

	UINT topOrBottom = camera.GetZenithalDirectionOfLook();
	UINT leftOrRightDir = 0, frontOrBackDir = 0;
	camera.GetAzimuthalDirectionsOfLook(&leftOrRightDir,&frontOrBackDir);

	if (leftOrRightDir == Camera::AzimuthalDirectionOfLook::FROM_RIGHT_TO_LEFT)
	{
		//Z - Y
		for (int y = 1; y<_ySize+1; y++)
		{
			vContour[(*contourCounter)++].SetVertex(-0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(-0.5f, y-0.5f, _zSize-0.5f, nx, ny, nz, color);
		}
		for (int z = 1; z<_zSize+1; z++)
		{
			vContour[(*contourCounter)++].SetVertex(-0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(-0.5f, _ySize-0.5f, z-0.5f, nx, ny, nz, color);
		}
	}
	if (leftOrRightDir == Camera::AzimuthalDirectionOfLook::FROM_LEFT_TO_RIGHT)
	{
		//Z - Y
		for (int y = 0; y<_ySize+1; y++)
		{
			vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, y-0.5f, _zSize-0.5f, nx, ny, nz, color);
		}
		for (int z = 0; z<_zSize+1; z++)
		{
			vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, _ySize-0.5f, z-0.5f, nx, ny, nz, color);
		}
	}
	if (frontOrBackDir == Camera::AzimuthalDirectionOfLook::FROM_BACK_TO_FRONT)
	{
		//X - Y
		for (int x = 1; x<_xSize+1; x++)
		{
			vContour[(*contourCounter)++].SetVertex(x-0.5f, -0.5f, -0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(x-0.5f, _ySize-0.5f, -0.5f, nx, ny, nz, color);
		}
		for (int y = 1; y<_ySize+1; y++)
		{
			vContour[(*contourCounter)++].SetVertex(-0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
		}
	}
	if (frontOrBackDir == Camera::AzimuthalDirectionOfLook::FROM_FRONT_TO_BACK)
	{
		//X - Y
		for (int x = 0; x<_xSize+1; x++)
		{
			vContour[(*contourCounter)++].SetVertex(x-0.5f, -0.5f, _zSize-0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(x-0.5f, _ySize-0.5f, _zSize-0.5f, nx, ny, nz, color);
		}
		for (int y = 0; y<_ySize+1; y++)
		{
			vContour[(*contourCounter)++].SetVertex(-0.5f, y-0.5f, _zSize-0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, y-0.5f, _zSize-0.5f, nx, ny, nz, color);
		}
	}

	if (topOrBottom == Camera::ZenithalDirectionOfLook::FROM_BOTTOM_TO_UP)
	{
		// X - Z
		for (int x=0; x<_xSize+1; x++)
		{
			vContour[(*contourCounter)++].SetVertex(x-0.5f, _ySize-0.5f, -0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(x-0.5f, _ySize-0.5f, _zSize-0.5f, nx, ny, nz, color);
		}
		for (int z=0; z<_zSize+1; z++)
		{
			vContour[(*contourCounter)++].SetVertex(-0.5f, _ySize-0.5f, z-0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, _ySize-0.5f, z-0.5f, nx, ny, nz, color);
		}
	}
	if (topOrBottom == Camera::ZenithalDirectionOfLook::FROM_TOP_TO_BOTTOM)
	{
		// X - Z
		for (int x=1; x<_xSize+1; x++)
		{
			vContour[(*contourCounter)++].SetVertex(x-0.5f, -0.5f, -0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(x-0.5f, -0.5f, _zSize-0.5f, nx, ny, nz, color);
		}
		for (int z=1; z<_zSize+1; z++)
		{
			vContour[(*contourCounter)++].SetVertex(-0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
			vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
		}
	}
	/*// X - Z
	for (int x=1; x<_xSize+1; x++)
	{
		vContour[(*contourCounter)++].SetVertex(x-0.5f, -0.5f, -0.5f, nx, ny, nz, color);
		vContour[(*contourCounter)++].SetVertex(x-0.5f, -0.5f, _zSize-0.5f, nx, ny, nz, color);
	}
	for (int z = 1; z<_zSize+1; z++)
	{
		vContour[(*contourCounter)++].SetVertex(-0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
		vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
	}
	//X - Y
	for (int x = 1; x<_xSize+1; x++)
	{
		vContour[(*contourCounter)++].SetVertex(x-0.5f, -0.5f, -0.5f, nx, ny, nz, color);
		vContour[(*contourCounter)++].SetVertex(x-0.5f, _ySize-0.5f, -0.5f, nx, ny, nz, color);
	}
	for (int y = 1; y<_ySize+1; y++)
	{
		vContour[(*contourCounter)++].SetVertex(-0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
		vContour[(*contourCounter)++].SetVertex(_xSize-0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
	}
	//Z - Y
	for (int y = 1; y<_ySize+1; y++)
	{
		vContour[(*contourCounter)++].SetVertex(-0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
		vContour[(*contourCounter)++].SetVertex(-0.5f, y-0.5f, _zSize-0.5f, nx, ny, nz, color);
	}
	for (int z = 1; z<_zSize+1; z++)
	{
		vContour[(*contourCounter)++].SetVertex(-0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
		vContour[(*contourCounter)++].SetVertex(-0.5f, _ySize-0.5f, z-0.5f, nx, ny, nz, color);
	}
*/

	Contour* vb_contours;
	vbForContour->Lock(0, 0, (void**)&vb_contours, D3DLOCK_DISCARD);
	memcpy(vb_contours, vContour, (*contourCounter)*sizeof(Contour));
	vbForContour->Unlock();
}

void FillWallArray(Wall * vWall, UINT *wallCounter, int _xSize, int _ySize, int _zSize, Camera & camera)
{
	float nx = 0.0f, ny = 0.0f, nz = 0.0f;
	unsigned long color = 0;

	UINT topOrBottom = camera.GetZenithalDirectionOfLook();
	UINT leftOrRightDir = 0, frontOrBackDir = 0;
	camera.GetAzimuthalDirectionsOfLook(&leftOrRightDir, &frontOrBackDir);

	if (leftOrRightDir==Camera::AzimuthalDirectionOfLook::FROM_RIGHT_TO_LEFT)
	{
		for (int z = 0; z<_zSize; z++)
		{
			for (int y = 0; y<_ySize; y++)
			{
				vWall[(*wallCounter)++].SetVertex(-0.5f, y-0.5f, z-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(-0.5f, y+0.5f, z-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(-0.5f, y+0.5f, z+0.5f, nx, ny, nz, color);

				vWall[(*wallCounter)++].SetVertex(-0.5f, y-0.5f, z-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(-0.5f, y+0.5f, z+0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(-0.5f, y-0.5f, z+0.5f, nx, ny, nz, color);
			}
		}
	}
	if (leftOrRightDir==Camera::AzimuthalDirectionOfLook::FROM_LEFT_TO_RIGHT)
	{
		for (int z = 0; z<_zSize; z++)
		{
			for (int y = 0; y<_ySize; y++)
			{
				vWall[(*wallCounter)++].SetVertex(_xSize-0.5f, y-0.5f, z+0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(_xSize-0.5f, y+0.5f, z+0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(_xSize-0.5f, y+0.5f, z-0.5f, nx, ny, nz, color);

				vWall[(*wallCounter)++].SetVertex(_xSize-0.5f, y-0.5f, z+0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(_xSize-0.5f, y+0.5f, z-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(_xSize-0.5f, y-0.5f, z-0.5f, nx, ny, nz, color);
			}
		}
	}
	if (frontOrBackDir==Camera::AzimuthalDirectionOfLook::FROM_BACK_TO_FRONT)
	{
		for (int x = 0; x<_xSize; x++)
		{
			for (int y = 0; y<_ySize; y++)
			{
				vWall[(*wallCounter)++].SetVertex(x+0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x+0.5f, y+0.5f, -0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x-0.5f, y+0.5f, -0.5f, nx, ny, nz, color);

				vWall[(*wallCounter)++].SetVertex(x+0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x-0.5f, y+0.5f, -0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x-0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
			}
		}
	}
	if (frontOrBackDir==Camera::AzimuthalDirectionOfLook::FROM_FRONT_TO_BACK)
	{
		for (int x = 0; x<_xSize; x++)
		{
			for (int y = 0; y<_ySize; y++)
			{
				vWall[(*wallCounter)++].SetVertex(x-0.5f, y-0.5f, _zSize-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x-0.5f, y+0.5f, _zSize-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x+0.5f, y+0.5f, _zSize-0.5f, nx, ny, nz, color);

				vWall[(*wallCounter)++].SetVertex(x-0.5f, y-0.5f, _zSize-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x+0.5f, y+0.5f, _zSize-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x+0.5f, y-0.5f, _zSize-0.5f, nx, ny, nz, color);
			}
		}
	}

	if (topOrBottom==Camera::ZenithalDirectionOfLook::FROM_BOTTOM_TO_UP)
	{
		for (int x = 0; x<_xSize; x++)
		{
			for (int z = 0; z<_zSize; z++)
			{
				vWall[(*wallCounter)++].SetVertex(x-0.5f, _ySize-0.5f, z+0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x-0.5f, _ySize-0.5f, z-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x+0.5f, _ySize-0.5f, z-0.5f, nx, ny, nz, color);

				vWall[(*wallCounter)++].SetVertex(x-0.5f, _ySize-0.5f, z+0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x+0.5f, _ySize-0.5f, z-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x-0.5f, _ySize-0.5f, z+0.5f, nx, ny, nz, color);				
			}
		}
	}
	if (topOrBottom==Camera::ZenithalDirectionOfLook::FROM_TOP_TO_BOTTOM)
	{
		for (int x = 0; x<_xSize; x++)
		{
			for (int z = 0; z<_zSize; z++)
			{
				vWall[(*wallCounter)++].SetVertex(x-0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x-0.5f, -0.5f, z+0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x+0.5f, -0.5f, z+0.5f, nx, ny, nz, color);

				vWall[(*wallCounter)++].SetVertex(x-0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x+0.5f, -0.5f, z+0.5f, nx, ny, nz, color);
				vWall[(*wallCounter)++].SetVertex(x+0.5f, -0.5f, z-0.5f, nx, ny, nz, color);
			}
		}
	}
	/*
	for (int x = 0; x<_xSize; x++)
	{
		for (int y = 0; y<_ySize; y++)
		{
			vFloor[(*floorCounter)++].SetVertex(x+0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
			vFloor[(*floorCounter)++].SetVertex(x+0.5f, y+0.5f, -0.5f, nx, ny, nz, color);
			vFloor[(*floorCounter)++].SetVertex(x-0.5f, y+0.5f, -0.5f, nx, ny, nz, color);

			vFloor[(*floorCounter)++].SetVertex(x+0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
			vFloor[(*floorCounter)++].SetVertex(x-0.5f, y+0.5f, -0.5f, nx, ny, nz, color);
			vFloor[(*floorCounter)++].SetVertex(x-0.5f, y-0.5f, -0.5f, nx, ny, nz, color);
		}
	}

	for (int z = 0; z<_zSize; z++)
	{
		for (int y = 0; y<_ySize; y++)
		{
			vFloor[(*floorCounter)++].SetVertex(-0.5f, y-0.5f, z-0.5f, nx, ny, nz, color);
			vFloor[(*floorCounter)++].SetVertex(-0.5f, y+0.5f, z-0.5f, nx, ny, nz, color);
			vFloor[(*floorCounter)++].SetVertex(-0.5f, y+0.5f, z+0.5f, nx, ny, nz, color);

			vFloor[(*floorCounter)++].SetVertex(-0.5f, y-0.5f, z-0.5f, nx, ny, nz, color);
			vFloor[(*floorCounter)++].SetVertex(-0.5f, y+0.5f, z+0.5f, nx, ny, nz, color);
			vFloor[(*floorCounter)++].SetVertex(-0.5f, y-0.5f, z+0.5f, nx, ny, nz, color);
		}
	}
	*/
}

void Render( IDirect3DVertexBuffer9 * vbForModels,IDirect3DVertexBuffer9 * vbLinesForFrame,IDirect3DVertexBuffer9 * vbForWireframes,IDirect3DVertexBuffer9 * vbForContour,IDirect3DVertexBuffer9 * vbForLines,IDirect3DVertexBuffer9 * vbForClipboard,IDirect3DVertexBuffer9 * vbVerticesForFrame, Model model, Clipboard & clipboard, Directx &directx, Frame & frame, HWND _hRenderTarget, Camera & camera, bool isWireframeVisible, bool isContourVisible, bool isAxisExtended, Interface & GUI)
{
	int xSize = model.GetModelSizeX();
	int ySize = model.GetModelSizeY();
	int zSize = model.GetModelSizeZ();

	UINT maxVertices = 36*xSize*ySize*zSize;
	if (maxVertices>MAX_VERTICES)
	{
		maxVertices = MAX_VERTICES;
	}
	Vertex *vModel = new Vertex[maxVertices];

	UINT maxVerticesForClipboard = 36*(frame.GetExtraSizeX()+1)*(frame.GetExtraSizeY()+1)*(frame.GetExtraSizeZ()+1);
	if (maxVerticesForClipboard>MAX_VERTICES)
	{
		maxVerticesForClipboard = MAX_VERTICES;
	}
	Vertex *vClipboard = new Vertex[maxVerticesForClipboard];

	UINT maxWireframes = 72*xSize*ySize*zSize;
	if (maxWireframes>MAX_WIREFRAMES)
	{
		maxWireframes = MAX_WIREFRAMES;
	}
	Wireframe *vWireframe = new Wireframe[maxWireframes];

	UINT maxContours = 8*(xSize+ySize+zSize);
	if (maxContours>MAX_CONTOURS)
	{
		maxContours = MAX_CONTOURS;
	}
	Contour *vContour = new Contour[maxContours];
	
	UINT maxWalls = 6*(xSize*ySize + xSize*zSize + ySize*zSize);
	Wall * vWall = new Wall[maxWalls];

	int xStartingVoxelNumber = 0;
	int yStartingVoxelNumber = 0;
	int zStartingVoxelNumber = 0;
	UINT vertexCounter = 0;
	UINT clipboardCounter = 0;
	UINT wireframesCounter = 0;
	UINT contourCounter = 0;
	UINT floorCounter = 0;
	UINT renderCounter = 0;
	float currentLowestRayLength = 0;

	directx.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(235, 235, 255), 1.0f, 0);	

	bool isModelWasRendered = false;
	while (!isModelWasRendered)
	{
		vertexCounter = 0;
		wireframesCounter = 0;
		contourCounter = 0;
		floorCounter = 0;
		ZeroMemory(vModel, maxVertices*sizeof(Vertex));
		ZeroMemory(vWireframe, maxWireframes*sizeof(Wireframe));
		ZeroMemory(vContour, maxContours*sizeof(Contour));
		ZeroMemory(vWall, maxWalls*sizeof(Wall));

		isModelWasRendered = FillModelBuffer(vbForModels,vModel, model,&xStartingVoxelNumber,&yStartingVoxelNumber,&zStartingVoxelNumber,&vertexCounter);				

		Vertex* vb_vertices;
		vbForModels->Lock(0, 0, (void**)&vb_vertices, D3DLOCK_DISCARD);
		memcpy(vb_vertices, (vModel), vertexCounter*sizeof(Vertex));
		vbForModels->Unlock();

		directx.d3dDevice->SetStreamSource(0, vbForModels, 0, sizeof(Vertex));
		directx.d3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE);
		directx.d3dDevice->BeginScene();
		directx.d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vertexCounter / 3);		

		if (isContourVisible)
		{
			FillContourBuffer(vbForContour, vContour, &contourCounter, model.GetModelSizeX(), model.GetModelSizeY(), model.GetModelSizeZ(), camera);

			directx.d3dDevice->SetStreamSource(0, vbForContour, 0, sizeof(Contour));
			directx.d3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, contourCounter/2);

			float red = 16711680; 
			float green = 65280;
			float blue = 255;

			float linesExtLength = 0.0f;
			if (isAxisExtended == true)
			{
				linesExtLength = 10000.0f;
			}			

			Line lines[6];
			lines[0].SetVertex(-0.5f,-0.5f,-0.5f, 1.0f,0.0f,0.0f, (unsigned long)red);
			lines[1].SetVertex(-0.5f+model.GetModelSizeX()+linesExtLength,-0.5f,-0.5f, 1.0f,0.0f,0.0f, (unsigned long)red);

			lines[2].SetVertex(-0.5f,-0.5f,-0.5f, 1.0f,0.0f,0.0f, (unsigned long)blue);
			lines[3].SetVertex(-0.5f,-0.5f+model.GetModelSizeY()+linesExtLength,-0.5f, 1.0f,0.0f,0.0f, (unsigned long)blue);

			lines[4].SetVertex(-0.5f,-0.5f,-0.5f, 1.0f,0.0f,0.0f, (unsigned long)green);
			lines[5].SetVertex(-0.5f,-0.5f,-0.5f+model.GetModelSizeZ()+linesExtLength, 1.0f,0.0f,0.0f, (unsigned long)green);

			Line* vb_lines;
			vbForLines->Lock(0, 0, (void**)&vb_lines, D3DLOCK_DISCARD);
			memcpy(vb_lines, lines, 6*sizeof(Line));
			vbForLines->Unlock();

			directx.d3dDevice->SetStreamSource(0, vbForLines, 0, sizeof(Line));
			directx.d3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 3);

			FillWallArray(vWall, &floorCounter, model.GetModelSizeX(), model.GetModelSizeY(), model.GetModelSizeZ(), camera);
			if (renderCounter == 0)
			{
				currentLowestRayLength = frame.ComputeFramePosition(&_hRenderTarget, vWall, camera.GetPos(), directx.d3dDevice, floorCounter, currentLowestRayLength);
			}			
		}

		currentLowestRayLength = frame.ComputeFramePosition( &_hRenderTarget, vModel, camera.GetPos( ), directx.d3dDevice, vertexCounter, currentLowestRayLength);

		if (isWireframeVisible)
		{
			FillWireframeBuffer(vbForWireframes, vModel, vWireframe, vertexCounter, &wireframesCounter);

			directx.d3dDevice->SetStreamSource(0, vbForWireframes, 0, sizeof(Wireframe));
			directx.d3dDevice->SetFVF(D3DFVF_XYZ);
			directx.d3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, wireframesCounter/2);
		}
		renderCounter++;
	}
	
	if (frame.IsVisible())
	{
		Point points[] =
		{
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},

			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},

			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()-0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()-0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()-0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
			{(float)(frame.GetX()+frame.GetMoveX()+frame.GetExtraSizeX()+0.5f), (float)(frame.GetY()+frame.GetMoveY()+frame.GetExtraSizeY()+0.5f), (float)(frame.GetZ()+frame.GetMoveZ()+frame.GetExtraSizeZ()+0.5f)},
		};

		Point* vb_points;
		vbLinesForFrame->Lock(0, 0, (void**)&vb_points, D3DLOCK_DISCARD);
		memcpy(vb_points, points, sizeof(points));
		vbLinesForFrame->Unlock();

		directx.d3dDevice->SetStreamSource(0, vbLinesForFrame, 0, sizeof(Point));
		directx.d3dDevice->SetFVF(D3DFVF_XYZ);
		directx.d3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 12);

		if (frame.GetExtraSizeX()!=0||frame.GetExtraSizeY()!=0||frame.GetExtraSizeZ()!=0)
		{
			directx.DrawFrameSizes(frame);
		}

		if (GUI.editingMode==Interface::EditingMode::WORK_WITH_GROUPS_OF_VOXELS)
		{
			if (IsMenuItemActive(GUI.menuCopiedVoxelsDisplaySettings,II_IS_COPIED_VOXELS_PREVIEW))
			{
				xStartingVoxelNumber = 0;
				yStartingVoxelNumber = 0;
				zStartingVoxelNumber = 0;

				Clipboard clipboardToRender;
				clipboardToRender.DeallocateMemory();
				clipboardToRender.AllocateMemory(frame.GetExtraSizeX()+1, frame.GetExtraSizeY()+1, frame.GetExtraSizeZ()+1);
				clipboardToRender.ChangeModelSize(frame.GetExtraSizeX()+1, frame.GetExtraSizeY()+1, frame.GetExtraSizeZ()+1);
				clipboard.PasteVoxels(0, 0, 0, frame.GetExtraSizeX()+1, frame.GetExtraSizeY()+1, frame.GetExtraSizeZ()+1, clipboardToRender);

				if (IsMenuItemActive(GUI.menuCopiedVoxelsDisplaySettings,II_IS_COPIED_VOXELS_PREVIEW_TRANSPARENT))
				{
					directx.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					directx.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					directx.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				}

				bool isClipboardWasRendered = false;
				while (!isClipboardWasRendered)
				{
					clipboardCounter = 0;
					ZeroMemory(vClipboard, maxVerticesForClipboard*sizeof(Vertex));

					isClipboardWasRendered = FillClipboardBuffer(vbForClipboard, vClipboard, clipboardToRender, &xStartingVoxelNumber, &yStartingVoxelNumber, &zStartingVoxelNumber, &clipboardCounter, frame);

					Vertex* vb_ClipboardVertices;
					vbForClipboard->Lock(0, 0, (void**)&vb_ClipboardVertices, D3DLOCK_DISCARD);
					memcpy(vb_ClipboardVertices, (vClipboard), clipboardCounter*sizeof(Vertex));
					vbForClipboard->Unlock();

					directx.d3dDevice->SetStreamSource(0, vbForClipboard, 0, sizeof(Vertex));
					directx.d3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE);
					directx.d3dDevice->BeginScene();
					directx.d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, clipboardCounter/3);
				}
				clipboardToRender.DeallocateMemory();

				if (IsMenuItemActive(GUI.menuCopiedVoxelsDisplaySettings,II_IS_COPIED_VOXELS_PREVIEW_TRANSPARENT))
				{
					directx.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				}
			}
		}
		else
		{
			DWORD color = GUI.color;
			int r=0,g=0,b=0,a=0;
			DWORD2RGBA(color,&g,&r,&a,&b);
			color = D3DCOLOR_ARGB(128,r,g,b);

			Vertex vertices[36] = {0};
			int verticeCounter = 0;

			//front
			float nx = 0.00000000f, ny = 0.00000000f, nz = -0.99999994f;
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f,frame.GetY()-0.5f,frame.GetZ()-0.5f,nx,ny,nz,color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f,frame.GetY()+0.5f,frame.GetZ()-0.5f,nx,ny,nz,color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f,frame.GetY()+0.5f,frame.GetZ()-0.5f,nx,ny,nz,color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f,frame.GetY()-0.5f,frame.GetZ()-0.5f,nx,ny,nz,color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f,frame.GetY()+0.5f,frame.GetZ()-0.5f,nx,ny,nz,color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f,frame.GetY()-0.5f,frame.GetZ()-0.5f,nx,ny,nz,color);

			//back
			nx = 0.00000000f, ny = 0.00000000f, nz = 0.99999994f;
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()-0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()+0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()+0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()-0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()+0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()-0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);

			//left
			nx = -0.99999994f, ny = 0.00000000f, nz = 0.00000000f;
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()-0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()+0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()+0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()-0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()+0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()-0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);

			//right
			nx = 0.99999994f, ny = 0.00000000f, nz = 0.00000000f;
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()-0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()+0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()+0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()-0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()+0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()-0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);

			//top
			nx = 0.00000000f, ny = 0.99999994f, nz = 0.00000000f;
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()+0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()+0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()+0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()+0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()+0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()+0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);

			//bottom
			nx = 0.00000000f, ny = -0.99999994f, nz = 0.00000000f;
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()-0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()-0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()-0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()-0.5f, frame.GetY()-0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()-0.5f, frame.GetZ()-0.5f, nx, ny, nz, color);
			vertices[verticeCounter++].SetVertex(frame.GetX()+0.5f, frame.GetY()-0.5f, frame.GetZ()+0.5f, nx, ny, nz, color);


			Vertex* vb_vertices;
			vbVerticesForFrame->Lock(0, 0, (void**)&vb_vertices, D3DLOCK_DISCARD);
			memcpy(vb_vertices, vertices, sizeof(vertices));
			vbVerticesForFrame->Unlock();

			directx.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			directx.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			directx.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);			

			directx.d3dDevice->SetStreamSource(0, vbVerticesForFrame, 0, sizeof(Vertex));
			directx.d3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE);
			directx.d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);

			directx.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}

	directx.d3dDevice->EndScene();
	directx.d3dDevice->Present(NULL, NULL, NULL, NULL);


	delete[] vModel;
	vModel = nullptr;
	delete[] vWireframe;
	vWireframe = nullptr;
	delete[] vContour;
	vContour = nullptr;
	delete[] vWall;
	vWall = nullptr;
	delete[] vClipboard;
	vClipboard = nullptr;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Interface GUI;
	Directx directx;
	Model model;
	Clipboard clipboard;
	Camera camera;
	Database database;
	Frame frame;

	GUI.LoadSettingFromFile(camera, database,frame);
	GUI.SetProgramText();

	PointersToObjects ptrs;
	ptrs.GUI_ptr = &GUI;
	ptrs.Model_ptr = &model;
	ptrs.Camera_ptr = &camera;
	ptrs.Database_ptr = &database;
	ptrs.Frame_ptr = &frame;
	ptrs.Clipboard_ptr = &clipboard;
	ptrs.Directx_ptr = &directx;
	PointersToObjects *prtToPtrs = &ptrs;

	if (GUI.Init(WndProc, hInstance, nCmdShow, prtToPtrs)==RC_ERROR)
	{
		MessageBox(0, "Cannot create GUI", "Error", 0);
		return 0;
	}	

	database.AssignDatabase((LPSTR)GUI.GetProgramText(PTI_ÑHOOSE_CUBEWORLD_FOLDER));
	if (database.IsAssigned())
	{
		EnableMenuItem(GUI.menuFile, II_SAVE_MODEL, MF_ENABLED);
		EnableMenuItem(GUI.menuFile, II_EXPORT_MODEL, MF_ENABLED);
		EnableMenuItem(GUI.menuFile, II_INSERT_MODEL, MF_ENABLED);
		database.GetListOfModels();
		GUI.AddModelsNamesToListView(database);
	}

	if (directx.Init(GUI.hMainWindow, GUI.hRenderTarget, GUI.renderTargetRect, GUI.wndClass.hInstance)==RC_ERROR)
	{
		MessageBox(0, "Cannot create DirectX devices", "Error", 0);
		return 0;
	}

	DWORD pickColorThread;
	HANDLE pickColorHandle = CreateThread(NULL, 0, PickColor, prtToPtrs, 0, &pickColorThread);
	
	directx.d3dDevice->CreateVertexBuffer(MAX_VERTICES*sizeof( Vertex ), D3DUSAGE_DYNAMIC, ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE ), D3DPOOL_DEFAULT, &directx.vbForModels, NULL);
	directx.d3dDevice->CreateVertexBuffer(24*sizeof( Point ), D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &directx.vbLinesForFrame, NULL);
	directx.d3dDevice->CreateVertexBuffer(36*sizeof( Vertex ), D3DUSAGE_DYNAMIC, (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE), D3DPOOL_DEFAULT, &directx.vbVerticesForFrame, NULL);
	directx.d3dDevice->CreateVertexBuffer(MAX_WIREFRAMES*sizeof( Wireframe ), D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &directx.vbForWireframe, NULL);
	directx.d3dDevice->CreateVertexBuffer(MAX_CONTOURS*sizeof( Contour ), D3DUSAGE_DYNAMIC, ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE ), D3DPOOL_DEFAULT, &directx.vbForContour, NULL);
	directx.d3dDevice->CreateVertexBuffer(6*sizeof( Line ), D3DUSAGE_DYNAMIC, ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE ), D3DPOOL_DEFAULT, &directx.vbForLines, NULL);
	directx.d3dDevice->CreateVertexBuffer(MAX_VERTICES*sizeof( Vertex ), D3DUSAGE_DYNAMIC, ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE ), D3DPOOL_DEFAULT, &directx.vbForClipboard, NULL);
	camera.SetCameraSettingsDependsTheModelSizes(1, 1, 1);
	camera.RestoreCameraPosition();

	const int timersCount = 9;
	Timer timers[timersCount];

	for (int i=0; i<timersCount; i++)
	{
		timers[i].StartTiming();
	}

	while (true)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		for (int i = 0; i<timersCount; i++)
		{
			timers[i].EndTiming();
		}

		KeyboardAndMouseProcessing(directx, camera, clipboard, model, frame, GUI, timers);

		CameraTransform(directx, camera);

		Render(directx.vbForModels, directx.vbLinesForFrame, directx.vbForWireframe, directx.vbForContour, directx.vbForLines, directx.vbForClipboard, directx.vbVerticesForFrame,
			model, clipboard, directx, frame, GUI.hRenderTarget, camera, 
			GUI.settings.isWireframeVisible,GUI.settings.isContourVisible,GUI.settings.isExtendedAsix, GUI);
	}

	PostThreadMessage(pickColorThread, WM_QUIT, 0, 0);
	WaitForSingleObject(pickColorHandle, INFINITE);
	CloseHandle(pickColorHandle);

	GUI.SaveSettingToFile(camera, database);
	directx.Release();
	GUI.DestroyProgramMenu();
	return 0;
}