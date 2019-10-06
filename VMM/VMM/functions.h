#ifndef FUNCTIONS
#define FUNCTIONS

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <d3dx9math.h>

int GetNumberFromTextPanel(HWND _hwnd);
void SetNumberToTextPanel(HWND _hwnd, int _value);
void CheckOrUncheckMenuItem(HMENU _hMenu, UINT _II);
void SwapCol(DWORD* ptr);
float ScaleValue(float _currentValueToScale,float _minValueOfScalingValue, float _maxValueOfScalingValue, float _minValueOfResultValue, float _maxValueOfResultValue);
void ComputeNormal( D3DXVECTOR3* p0, D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DXVECTOR3* out );
D3DXVECTOR3 MakeRay( float FOV, float AspectRatio, WORD MouseX, WORD MouseY, float range, IDirect3DDevice9* _Device, int _windowWidth, int _windowHeight );
unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA);
void DWORD2RGBA(DWORD color,int *iR, int *iG, int *iB, int *iA);
void Swap(int *a, int *b);
bool IsMenuItemActive(HMENU _hmenu, UINT _II);

#endif /*FUNCTIONS*/