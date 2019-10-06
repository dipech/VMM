#ifndef STRUCTS
#define STRUCTS

#pragma comment (lib, "Winmm.lib" )

#include <windows.h>

struct Vertex
{
	float x, y, z, nx, ny, nz;
	DWORD c;

	void SetVertex( float _x, float _y, float _z, float _nx, float _ny, float _nz, unsigned long _c )
	{
		x = _x; y = _y; z = _z;
		nx = _nx; ny = _ny; nz = _nz;
		c = _c;
	}
};

struct Point
{
	float x, y, z;
	void SetVertex( float _x, float _y, float _z)
	{
		x = _x; 
		y = _y; 
		z = _z;
	}
};

typedef Point Wireframe;
typedef Vertex Contour;
typedef Vertex Line;
typedef Vertex Wall;

enum TimersID
{
	ADD_VOXEL_LIMITER,
	DELETE_VOXEL_LIMITER,
	FILLING_VOXEL_LIMITER,
	BRUSHING_VOXEL_LIMITER,
	HOTKEY_LIMITER,
	WIREFRAME_INCREASING_X_SPEED_LIMITER,
	WIREFRAME_INCREASING_Y_SPEED_LIMITER,
	WIREFRAME_INCREASING_Z_SPEED_LIMITER,
	COPY_OR_PASTE_VOXELS_LIMITER
};

struct Timer
{
	DWORD startTime;
	DWORD endTime;	

	void StartTiming()
	{
		startTime = timeGetTime();
		endTime = timeGetTime();
	}
	void EndTiming()
	{
		endTime = timeGetTime();
	}
	DWORD GetResult()
	{
		return endTime - startTime;
	}
};

#endif /*STRUCTS*/