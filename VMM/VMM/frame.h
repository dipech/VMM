#ifndef FRAME_CLASS
#define FRAME_CLASS

#include <windows.h>
#include <d3d9.h>
#include <d3dx9math.h>

#include "structs.h"

class Frame
{
public:
	const int maxDimension = 32767;
	enum DirectionOfIntersection
	{
		FROM_FRONT_TO_BACK,
		FROM_BACK_TO_FRONT,
		FROM_LEFT_TO_RIGHT,
		FROM_RIGHT_TO_LEFT,
		FROM_TOP_TO_BOTTOM,
		FROM_BOTTOM_TO_TOP
	}directionOfIntersection;

private:
	int x;
	int y;
	int z;

	int xExtraSize;
	int yExtraSize;
	int zExtraSize;

	int xMove;
	int yMove;
	int zMove;

	bool isVisible;

public:
	Frame();
	int GetX();
	int GetY();
	int GetZ();
	void SetX(int _x);
	void SetY(int _y);
	void SetZ(int _z);
	int GetExtraSizeX();
	int GetExtraSizeY();
	int GetExtraSizeZ();	
	void SetExtraSizeX(int _xExtraSize);
	void SetExtraSizeY(int _yExtraSize);
	void SetExtraSizeZ(int _zExtraSize);
	void IncreazeExtraSizeX();
	void IncreazeExtraSizeY();
	void IncreazeExtraSizeZ();
	void DecreazeExtraSizeX();
	void DecreazeExtraSizeY();
	void DecreazeExtraSizeZ();
	int GetMoveX();
	int GetMoveY();
	int GetMoveZ();
	void SetMoveX(int _xMove);
	void SetMoveY(int _yMove);
	void SetMoveZ(int _zMove);
	void IncreazeMoveX();
	void IncreazeMoveY();
	void IncreazeMoveZ();
	void DecreazeMoveX();
	void DecreazeMoveY();
	void DecreazeMoveZ();
	void RestoreFrameResizeAndMove();
	void SetFramePos(int _x, int _y, int _z);
	void ShowFrame();
	void HideFrame();
	bool IsVisible();
	void TransformCoordinatesDependsDirectionOfIntersection(int *_x, int *_y, int *_z);
	float ComputeFramePosition(HWND *_hwnd, Vertex _vertices[], D3DXVECTOR3 _pos, IDirect3DDevice9* _Device, int _modelSize, float _currentMinimalRayLength);
};

#endif /*FRAME_CLASS*/