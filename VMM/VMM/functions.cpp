#include "functions.h"

int GetNumberFromTextPanel(HWND _hwnd)
{
	int value;
	char buffer[MAX_PATH];
	SendMessage(_hwnd, WM_GETTEXT, MAX_PATH, (LPARAM)buffer);
	value = atoi(buffer);
	return value;
}
void SetNumberToTextPanel(HWND _hwnd, int _value)
{
	char buffer[MAX_PATH];
	int bufferLength = sprintf_s(buffer, "%d", _value);
	SendMessage(_hwnd, WM_SETTEXT, bufferLength, (LPARAM)buffer);
}

void CheckOrUncheckMenuItem(HMENU _hMenu, UINT _II)
{
	DWORD menuState = GetMenuState(_hMenu, _II, 0);
	if (menuState & MF_CHECKED)
	{
		CheckMenuItem(_hMenu, _II, MF_UNCHECKED);
	}
	else
	{
		CheckMenuItem(_hMenu, _II, MF_CHECKED);
	}
}

void SwapCol(DWORD* ptr)
{
	*ptr = ( *ptr&0x000000FF )<<16|( *ptr&0x0000FF00 )|( *ptr&0x00FF0000 )>>16;
}

float ScaleValue(float _currentValueToScale, float _minValueOfScalingValue, float _maxValueOfScalingValue, float _minValueOfResultValue, float _maxValueOfResultValue)
{
	float ratio = ( _maxValueOfResultValue-_minValueOfResultValue )/( _maxValueOfScalingValue-_minValueOfScalingValue );
	float result = _minValueOfResultValue+ratio*( _currentValueToScale-_minValueOfScalingValue );
	return result;
}

void ComputeNormal(D3DXVECTOR3* p0, D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DXVECTOR3* out)
{
	D3DXVECTOR3 u = *p1-*p0;
	D3DXVECTOR3 v = *p2-*p0;
	D3DXVec3Cross(out, &u, &v);
	D3DXVec3Normalize(out, out);
}

D3DXVECTOR3 MakeRay(float FOV, float AspectRatio, WORD MouseX, WORD MouseY, float range, IDirect3DDevice9* _Device, int _windowWidth, int _windowHeight)
{
	D3DXVECTOR3 LineEnd, CameraSpacePos;
	float NMouseX, NMouseY, det;
	D3DXMATRIX matView;
	NMouseX = (float)( ( MouseX-(float)_windowWidth/2.0 )/( (float)_windowWidth/2.0 ) );
	NMouseY = (float)( ( MouseY-(float)_windowHeight/2.0 )/( (float)_windowHeight/2.0 )*( -1 ) );
	CameraSpacePos.y = (float)( NMouseY*tan(FOV/2.0) );
	CameraSpacePos.x = (float)( ( NMouseX/AspectRatio )*tan(FOV/2.0) );
	LineEnd.x = range*CameraSpacePos.x;
	LineEnd.y = range*CameraSpacePos.y;
	LineEnd.z = range;
	_Device->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, &det, &matView);
	D3DXVec3TransformCoord(&LineEnd, &LineEnd, &matView);
	return LineEnd;
}

void Swap(int *a, int *b)
{
	*a = *a + *b;
	*b = *a - *b;
	*a = *a - *b;
}

unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA)
{
	return (((((iA<<8)+iR)<<8)+iG)<<8)+iB;
}

void DWORD2RGBA(DWORD color, int *iR, int *iG, int *iB, int *iA)
{
	*iA = color & 255;
	*iR = (color>>8) & 255;
	*iG = (color>>16) & 255;
	*iB = (color>>24) & 255;
}

bool IsMenuItemActive(HMENU _hmenu, UINT _II)
{
	DWORD menuState = GetMenuState(_hmenu, _II, 0);
	if (menuState & MF_CHECKED)
	{
		return true;
	}
	else
	{
		return false;
	}
}