#ifndef DIRECTX_CLASS
#define DIRECTX_CLASS

#pragma comment (lib, "D3d9.lib" )
#pragma comment (lib, "D3dx9.lib" )
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#define DIRECTINPUT_VERSION 0x0800

#include <d3d9.h>
#include <d3dx9.h>
#include <D3dx9math.h>
#include <dinput.h>
#include <D3dx9core.h>

#include "frame.h"


struct Directx 
{
	IDirect3D9*			  d3d9Interface;
	IDirectInput8*		  dInputInterface;
	IDirect3DDevice9*     d3dDevice;
	IDirectInputDevice8*  dInputKeyboard;
	IDirectInputDevice8*  dInputMouse;
	ID3DXFont*			  d3dFont;
	D3DXFONT_DESC         d3dFontDesc;
	D3DPRESENT_PARAMETERS d3dPresentParameters;

	enum MouseButton
	{
		LEFT,
		RIGHT,
		MIDDLE,
	}mouseButton;

	IDirect3DVertexBuffer9 *vbForModels;
	IDirect3DVertexBuffer9 *vbLinesForFrame;
	IDirect3DVertexBuffer9 *vbVerticesForFrame;
	IDirect3DVertexBuffer9 *vbForWireframe;
	IDirect3DVertexBuffer9 *vbForContour;
	IDirect3DVertexBuffer9 *vbForLines;
	IDirect3DVertexBuffer9 *vbForClipboard;

	Directx();
	RC_RESULT Init(HWND _mainWindow, HWND _renderTarget, RECT _renderTargetRect, HINSTANCE _hInstance);
	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* _dir, D3DXCOLOR _color, float _power);
	void SetLight();
	DIMOUSESTATE GetMouseState();
	void GetKeyboardState(char *_keyboardStates);
	void Render(IDirect3DVertexBuffer9 *_vb, UINT _vertexCounter);
	void DrawFrameSizes(Frame & frame);
	void Release();
};

#endif /*DIRECTX_CLASS*/