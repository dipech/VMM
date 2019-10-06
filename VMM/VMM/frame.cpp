#include "frame.h"
#include "functions.h"

float Frame::ComputeFramePosition(HWND *_hwnd, Vertex *_vertices, D3DXVECTOR3 _pos, IDirect3DDevice9* _Device, int _modelSize, float _currentMinimalRayLength)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(*_hwnd, &pt);
	D3DXVECTOR3 CursorPos;
	CursorPos.x = (float)pt.x;
	CursorPos.y = (float)pt.y;

	struct Ray
	{
		D3DXVECTOR3 _origin;
		D3DXVECTOR3 _direction;
	}ray;

	float rayLength = 9999999.0f;

	RECT clientRect;
	GetClientRect(*_hwnd, &clientRect);

	ray._direction = MakeRay(D3DX_PI/2, (float)( clientRect.bottom )/(float)( clientRect.right ), (WORD)CursorPos.x, (WORD)CursorPos.y, rayLength, _Device, clientRect.right, clientRect.bottom);
	ray._origin = _pos;

	float x1, y1, z1, x2, y2, z2, x3, y3, z3;
	int j = 0, i = 0;
	int xNear = (int)rayLength, yNear = (int)rayLength, zNear = (int)rayLength;
	D3DXVECTOR3 PointIntersection;

	float t_minimum = 0;
	if (_currentMinimalRayLength>=0)
	{
		t_minimum = rayLength;
	}
	else
	{
		t_minimum = _currentMinimalRayLength;
	}

	while (i<_modelSize)
	{
		D3DXVECTOR3 point1, point2, point3, normal;
		point1.x = _vertices[i].x;
		point1.y = _vertices[i].y;
		point1.z = _vertices[i].z;

		point2.x = _vertices[i+1].x;
		point2.y = _vertices[i+1].y;
		point2.z = _vertices[i+1].z;

		point3.x = _vertices[i+2].x;
		point3.y = _vertices[i+2].y;
		point3.z = _vertices[i+2].z;

		ComputeNormal(&point1, &point2, &point3, &normal);
		D3DXPLANE Plane;
		D3DXPlaneFromPoints(&Plane, &point1, &point2, &point3);

		float t = -( D3DXVec3Dot(&normal, &ray._origin)+Plane.d )/D3DXVec3Dot(&normal, &ray._direction);

		if (t>0&&t<t_minimum)
		{
			D3DXVECTOR3 PointIntersection = ray._origin+t*ray._direction;
			x1 = _vertices[i].x;
			y1 = _vertices[i].y;
			z1 = _vertices[i].z;

			x2 = _vertices[i+1].x;
			y2 = _vertices[i+1].y;
			z2 = _vertices[i+1].z;

			x3 = _vertices[i+2].x;
			y3 = _vertices[i+2].y;
			z3 = _vertices[i+2].z;

			if (( x1==x2 && x1==x3-1 )&&( y1==y2-1&&y1==y3-1 )&&( z1==z2 && z1==z3 ))
			{
				if (( PointIntersection.x>x1 && PointIntersection.x < x3 )&&( PointIntersection.y > y1 && PointIntersection.y<y2 ))
				{
					if (_pos.z<z1)
					{
						xNear = (int)( x1+0.5f );
						yNear = (int)( y1+0.5f );
						zNear = (int)( z1-0.5f );
						t_minimum = t;
						directionOfIntersection = FROM_FRONT_TO_BACK;
					}
				}
			}
			if (( x1==x2 && x1==x3+1 )&&( y1==y2-1&&y1==y3-1 )&&( z1==z2 && z1==z3 ))
			{
				if (( PointIntersection.x>x3 && PointIntersection.x < x1 )&&( PointIntersection.y > y1 && PointIntersection.y < y2 ))
				{
					if (_pos.z > z1)
					{
						xNear = (int)( x1-0.5f );
						yNear = (int)( y1+0.5f );
						zNear = (int)( z1+0.5f );
						t_minimum = t;
						directionOfIntersection = FROM_BACK_TO_FRONT;
					}
				}
			}
			if (( x1==x2 && x1==x3 )&&( y1==y2-1&&y1==y3-1 )&&( z1==z2 && z1==z3+1 ))
			{
				if (( PointIntersection.z>z3 && PointIntersection.z < z1 )&&( PointIntersection.y > y1 && PointIntersection.y<y2 ))
				{
					if (_pos.x<x1)
					{
						xNear = (int)( x1-0.5f );
						yNear = (int)( y1+0.5f );
						zNear = (int)( z1-0.5 );
						t_minimum = t;
						directionOfIntersection = FROM_RIGHT_TO_LEFT;
					}
				}
			}
			if (( x1==x2 && x1==x3 )&&( y1==y2-1&&y1==y3-1 )&&( z1==z2 && z1==z3-1 ))
			{
				if (( PointIntersection.z>z1 && PointIntersection.z < z3 )&&( PointIntersection.y > y1 && PointIntersection.y < y2 ))
				{
					if (_pos.x > x1)
					{
						xNear = (int)( x1+0.5f );
						yNear = (int)( y1+0.5f );
						zNear = (int)( z1+0.5f );
						t_minimum = t;
						directionOfIntersection = FROM_LEFT_TO_RIGHT;
					}
				}
			}
			if (( x1==x2 && x1==x3-1 )&&( y1==y2 && y1==y3 )&&( z1==z2-1&&z1==z3-1 ))
			{
				if (( PointIntersection.x>x1 && PointIntersection.x < x3 )&&( PointIntersection.z > z1 && PointIntersection.z < z2 ))
				{
					if (_pos.y > y1)
					{
						xNear = (int)( x1+0.5f );
						yNear = (int)( y1+0.5f );
						zNear = (int)( z1+0.5f );
						t_minimum = t;
						directionOfIntersection = FROM_BOTTOM_TO_TOP;
					}
				}
			}
			if (( x1==x2 && x1==x3-1 )&&( y1==y2 && y1==y3 )&&( z1==z2+1&&z1==z3+1 ))
			{
				if (( PointIntersection.x>x2 && PointIntersection.x < x3 )&&( PointIntersection.z > z2 && PointIntersection.z<z1 ))
				{
					if (_pos.y<y1)
					{
						xNear = (int)( x1+0.5f );
						yNear = (int)( y1-0.5f );
						zNear = (int)( z1-0.5f );
						t_minimum = t;
						directionOfIntersection = FROM_TOP_TO_BOTTOM;
					}
				}
			}
		}
		i += 6;
	}

	if (xNear!=rayLength && yNear!=rayLength && zNear!=rayLength)
	{
		x = xNear;
		y = yNear;
		z = zNear;
	}
	return t_minimum;
}
Frame::Frame()
{
	x = y = z = maxDimension;
	xExtraSize = yExtraSize = zExtraSize = xMove = yMove = zMove = 0;
	isVisible = true;
}
int Frame::GetX()
{
	return x;
}
int Frame::GetY()
{
	return y;
}
int Frame::GetZ()
{
	return z;
}
void Frame::SetX(int _x)
{
	x = _x;
}
void Frame::SetY(int _y)
{
	y = _y;
}
void Frame::SetZ(int _z)
{
	z = _z;
}
int Frame::GetExtraSizeX()
{
	return xExtraSize;
}
int Frame::GetExtraSizeY()
{
	return yExtraSize;
}
int Frame::GetExtraSizeZ()
{
	return zExtraSize;
}
void Frame::SetExtraSizeX(int _xExtraSize)
{
	xExtraSize = _xExtraSize;
}
void Frame::SetExtraSizeY(int _yExtraSize)
{
	yExtraSize = _yExtraSize;
}
void Frame::SetExtraSizeZ(int _zExtraSize)
{
	zExtraSize = _zExtraSize;
}
void Frame::IncreazeExtraSizeX()
{
	xExtraSize++;
}
void Frame::IncreazeExtraSizeY()
{
	yExtraSize++;
}
void Frame::IncreazeExtraSizeZ()
{
	zExtraSize++;
}
void Frame::DecreazeExtraSizeX()
{
	if (xExtraSize > 0)
	{
		xExtraSize--;
	}	
}
void Frame::DecreazeExtraSizeY()
{
	if (yExtraSize>0)
	{
		yExtraSize--;
	}
}
void Frame::DecreazeExtraSizeZ()
{
	if (zExtraSize>0)
	{
		zExtraSize--;
	}
}
int Frame::GetMoveX()
{
	return xMove;
}
int Frame::GetMoveY()
{
	return yMove;
}
int Frame::GetMoveZ()
{
	return zMove;
}
void Frame::SetMoveX(int _xMove)
{
	xMove = _xMove;
}
void Frame::SetMoveY(int _yMove)
{
	yMove = _yMove;
}
void Frame::SetMoveZ(int _zMove)
{
	zMove = _zMove;
}
void Frame::IncreazeMoveX()
{
	xMove++;
}
void Frame::IncreazeMoveY()
{
	yMove++;
}
void Frame::IncreazeMoveZ()
{
	zMove++;
}
void Frame::DecreazeMoveX()
{
	xMove--;
}
void Frame::DecreazeMoveY()
{
	yMove--;
}
void Frame::DecreazeMoveZ()
{
	zMove--;
}
void Frame::SetFramePos(int _x, int _y, int _z)
{
	x = _x;
	y = _y;
	z = _z;
}
void Frame::RestoreFrameResizeAndMove()
{
	SetExtraSizeX(0);
	SetExtraSizeY(0);
	SetExtraSizeZ(0);
	SetMoveX(0);
	SetMoveY(0);
	SetMoveZ(0);
}
void Frame::ShowFrame()
{
	isVisible = true;
}
void Frame::HideFrame()
{
	isVisible = false;
}
bool Frame::IsVisible()
{
	return isVisible;
}
void Frame::TransformCoordinatesDependsDirectionOfIntersection(int *_x, int *_y, int *_z)
{
	switch (directionOfIntersection)
	{
		case Frame::DirectionOfIntersection::FROM_FRONT_TO_BACK:
		{
			(*_z)++;
			break;
		}
		case Frame::DirectionOfIntersection::FROM_BACK_TO_FRONT:
		{
			(*_z)--;
			break;
		}
		case Frame::DirectionOfIntersection::FROM_LEFT_TO_RIGHT:
		{
			(*_x)--;
			break;
		}
		case Frame::DirectionOfIntersection::FROM_RIGHT_TO_LEFT:
		{
			(*_x)++;
			break;
		}
		case Frame::DirectionOfIntersection::FROM_TOP_TO_BOTTOM:
		{
			(*_y)++;
			break;
		}
		case Frame::DirectionOfIntersection::FROM_BOTTOM_TO_TOP:
		{
			(*_y)--;
			break;
		}
	}
}