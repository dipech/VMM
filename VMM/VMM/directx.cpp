#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "defines.h"
#include "directx.h"

Directx::Directx()
{
	d3dDevice = NULL;
}

RC_RESULT Directx::Init(HWND _mainWindow, HWND _renderTarget, RECT _renderTargetRect, HINSTANCE _hInstance)
{
	d3d9Interface = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d9Interface == NULL)
	{
		return RC_ERROR;
	}

	DWORD multiSampleQuality = 0;
	d3d9Interface->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_A8R8G8B8,true,D3DMULTISAMPLE_NONMASKABLE,&multiSampleQuality);

	ZeroMemory(&d3dPresentParameters,sizeof(D3DPRESENT_PARAMETERS));
	d3dPresentParameters.BackBufferWidth = _renderTargetRect.right;
	d3dPresentParameters.BackBufferHeight = _renderTargetRect.bottom;
	d3dPresentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dPresentParameters.BackBufferCount = 1;
	d3dPresentParameters.MultiSampleType=D3DMULTISAMPLE_NONMASKABLE;
	d3dPresentParameters.MultiSampleQuality = multiSampleQuality-1;
	d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dPresentParameters.hDeviceWindow = _renderTarget;	
	d3dPresentParameters.Windowed = true;
	d3dPresentParameters.EnableAutoDepthStencil = 1;
	d3dPresentParameters.Flags = 0;
	d3dPresentParameters.FullScreen_RefreshRateInHz =  D3DPRESENT_RATE_DEFAULT;
	d3dPresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	

	D3DCAPS9 caps;
	d3d9Interface->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	int vertexProcessingType = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vertexProcessingType = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vertexProcessingType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	HRESULT hResult = d3d9Interface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,_renderTarget,
		vertexProcessingType,
		&d3dPresentParameters, &d3dDevice);

	if (hResult != D3D_OK)
	{
		return RC_ERROR;
	}

	d3dDevice->SetRenderState(D3DRS_LIGHTING, true);
	d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,DWORD(TRUE) ); 
	d3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE,DWORD(TRUE) ); 
	d3dDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE );
	d3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);


	hResult = DirectInput8Create(_hInstance,DIRECTINPUT_VERSION, IID_IDirectInput8,(void**)&dInputInterface, NULL);
	if (hResult != DI_OK)
	{
		return RC_ERROR;
	}

	hResult = dInputInterface->CreateDevice(GUID_SysKeyboard,&dInputKeyboard,NULL);
	dInputKeyboard->SetDataFormat(&c_dfDIKeyboard);
	dInputKeyboard->SetCooperativeLevel(_mainWindow,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	dInputKeyboard->Acquire();

	if (hResult != DI_OK)
	{
		return RC_ERROR;
	}

	hResult =  dInputInterface->CreateDevice(GUID_SysMouse, &dInputMouse, NULL);
	dInputMouse->SetDataFormat(&c_dfDIMouse);
	dInputMouse->SetCooperativeLevel(_mainWindow,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	dInputMouse->Acquire();

	if (hResult != DI_OK)
	{
		return RC_ERROR;
	}

	d3dFontDesc.Height = 20;
	d3dFontDesc.Width = 0;
	d3dFontDesc.Weight = 600;
	d3dFontDesc.MipLevels = D3DX_DEFAULT;
	d3dFontDesc.Italic = 0;
	d3dFontDesc.CharSet = DEFAULT_CHARSET;
	d3dFontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	d3dFontDesc.Quality = CLEARTYPE_QUALITY;
	d3dFontDesc.PitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
	wchar_t FaceName[] = L"Times New Roman";
	memcpy(d3dFontDesc.FaceName, &FaceName, sizeof(FaceName));

	hResult = D3DXCreateFontIndirect(d3dDevice, &d3dFontDesc, &d3dFont);
	if (hResult!=D3D_OK)
	{
		return RC_ERROR;
	}

	SetLight();

	return RC_SUCCESS;
}

void Directx::SetLight()
{
	D3DXVECTOR3 dirFromFrontToBack  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DLIGHT9 light_1 = InitDirectionalLight(&dirFromFrontToBack, D3DCOLOR_XRGB(255, 255, 255), 0.95f);
	d3dDevice->SetLight(1, &light_1);
	d3dDevice->LightEnable(1, true);

	D3DXVECTOR3 dirFromUpToDown  = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	D3DLIGHT9 light_2 = InitDirectionalLight(&dirFromUpToDown, D3DCOLOR_XRGB(255, 255, 255), 0.85f);
	d3dDevice->SetLight(2, &light_2);
	d3dDevice->LightEnable(2, true);

	D3DXVECTOR3 dirFromRightToLeft  = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
	D3DLIGHT9 light_3 = InitDirectionalLight(&dirFromRightToLeft, D3DCOLOR_XRGB(255, 255, 255), 0.75f);
	d3dDevice->SetLight(3, &light_3);
	d3dDevice->LightEnable(3, true);

	D3DXVECTOR3 dirFromLeftToRight  = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	D3DLIGHT9 light_4 = InitDirectionalLight(&dirFromLeftToRight, D3DCOLOR_XRGB(255, 255, 255), 0.65f);	
	d3dDevice->SetLight(4, &light_4);
	d3dDevice->LightEnable(4, true);

	D3DXVECTOR3 dirFromBackToFront  = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	D3DLIGHT9 light_5 = InitDirectionalLight(&dirFromBackToFront, D3DCOLOR_XRGB(255, 255, 255), 0.55f);	
	d3dDevice->SetLight(5, &light_5);
	d3dDevice->LightEnable(5, true);

	D3DXVECTOR3 DirFromDownToUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DLIGHT9 light_6 = InitDirectionalLight(&DirFromDownToUp, D3DCOLOR_XRGB(255, 255, 255), 0.45f);
	d3dDevice->SetLight(6, &light_6);
	d3dDevice->LightEnable(6, true);
}

D3DLIGHT9 Directx::InitDirectionalLight(D3DXVECTOR3* _dir, D3DXCOLOR _color, float _power)
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = _color * 0.4f * _power;
	light.Diffuse   = _color * _power;
	light.Specular  = _color * 0.6f * _power;
	light.Direction = *_dir;

	return light;
}

void Directx::Render(IDirect3DVertexBuffer9 *_vb, UINT _vertexCounter)
{
	
}

void Directx::DrawFrameSizes(Frame & frame)
{
	RECT textCoordinates;
	textCoordinates.left = 5;
	textCoordinates.right = d3dPresentParameters.BackBufferWidth;
	textCoordinates.top = d3dPresentParameters.BackBufferHeight - 20;
	textCoordinates.bottom = d3dPresentParameters.BackBufferHeight;

	char buffer[MAX_PATH] = "";
	int bufferLength = sprintf_s(buffer, "X:%i  Y:%i  Z:%i",frame.GetExtraSizeX()+1,frame.GetExtraSizeY()+1,frame.GetExtraSizeZ()+1);
		d3dFont->DrawText(NULL,buffer,bufferLength,&textCoordinates,DT_TOP|DT_LEFT,0xff000000);
}

void Directx::Release()
{
	if(d3dDevice != NULL)
	{
		d3dDevice->Release();
	}
	if(dInputKeyboard != NULL)
	{
		dInputKeyboard->Release();
	}
	if(dInputMouse != NULL)
	{
		dInputMouse->Release();
	}
	if(dInputInterface != NULL)
	{
		dInputInterface->Release();
	}
	if(d3d9Interface != NULL)
	{
		d3d9Interface->Release();
	}
	if (d3dFont!=NULL)
	{
		d3dFont->Release();
	}
}