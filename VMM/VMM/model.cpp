#include <windows.h>

#include "model.h"
#include "frame.h"

Model::Model()
{
	voxels = nullptr;
	ChangeModelSize(1,1,1);
	AllocateMemory(xSize,ySize,zSize);
}
void Model::IncreaseModelOnX()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp,xSize,ySize,zSize);
	DoCopyOfVoxelsArray(&temp,voxels,xSize,ySize,zSize,xSize,ySize,zSize);
	DeallocateMemory();
	AllocateMemory(xSize+1,ySize,zSize);
	DoCopyOfVoxelsArray(&voxels,temp,xSize+1,ySize,zSize,xSize,ySize,zSize);	
	DeallocateMemory(temp,xSize,ySize,zSize);
	ChangeModelSize(xSize+1,ySize,zSize);
}
void Model::IncreaseModelOnY()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);
	DeallocateMemory();
	AllocateMemory(xSize, ySize+1, zSize);
	DoCopyOfVoxelsArray(&voxels, temp, xSize, ySize+1, zSize, xSize, ySize, zSize);
	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(xSize, ySize+1, zSize);
}
void Model::IncreaseModelOnZ()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);
	DeallocateMemory();
	AllocateMemory(xSize, ySize, zSize+1);
	DoCopyOfVoxelsArray(&voxels, temp, xSize, ySize, zSize+1, xSize, ySize, zSize);
	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(xSize, ySize, zSize+1);
}
void Model::DecreaseModelOnX()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);
	DeallocateMemory();
	AllocateMemory(xSize-1, ySize, zSize);
	DoCopyOfVoxelsArray(&voxels, temp, xSize-1, ySize, zSize, xSize, ySize, zSize);
	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(xSize-1, ySize, zSize);
}
void Model::DecreaseModelOnY()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);
	DeallocateMemory();
	AllocateMemory(xSize, ySize-1, zSize);
	DoCopyOfVoxelsArray(&voxels, temp, xSize, ySize-1, zSize, xSize, ySize, zSize);
	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(xSize, ySize-1, zSize);
}
void Model::DecreaseModelOnZ()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);
	DeallocateMemory();
	AllocateMemory(xSize, ySize, zSize-1);
	DoCopyOfVoxelsArray(&voxels, temp, xSize, ySize, zSize-1, xSize, ySize, zSize);
	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(xSize, ySize, zSize-1);
}
void Model::IncreaseModelCoordinateOnX()
{
	for (int x=xSize-1; x>-1; x--)
	{
		for (int y=0; y<ySize; y++)
		{
			for (int z=0; z<zSize; z++)
			{
				if (x!=0)
				{
					voxels[y][x][z] = voxels[y][x-1][z];
				}
				else
				{
					voxels[y][x][z] = 0;
				}
			}
		}
	}
}
void Model::IncreaseModelCoordinateOnY()
{
	for (int x = 0; x<xSize; x++)
	{
		for (int y = ySize-1; y>-1; y--)
		{
			for (int z = 0; z<zSize; z++)
			{
				if (y!=0)
				{
					voxels[y][x][z] = voxels[y-1][x][z];
				}
				else
				{
					voxels[y][x][z] = 0;
				}
			}
		}
	}
}
void Model::IncreaseModelCoordinateOnZ()
{
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = zSize-1; z>-1; z--)
			{
				if (z!=0)
				{
					voxels[y][x][z] = voxels[y][x][z-1];
				}
				else
				{
					voxels[y][x][z] = 0;
				}
			}
		}
	}
}
void Model::DecreaseModelCoordinateOnX()
{
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = 0; z<zSize; z++)
			{
				if (x!=xSize-1)
				{
					voxels[y][x][z] = voxels[y][x+1][z];
				}
				else
				{
					voxels[y][x][z] = 0;
				}
			}
		}
	}
}
void Model::DecreaseModelCoordinateOnY()
{
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = 0; z<zSize; z++)
			{
				if (y!=ySize-1)
				{
					voxels[y][x][z] = voxels[y+1][x][z];
				}
				else
				{
					voxels[y][x][z] = 0;
				}
			}
		}
	}
}
void Model::DecreaseModelCoordinateOnZ()
{
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = 0; z<zSize; z++)
			{
				if (z!=zSize-1)
				{
					voxels[y][x][z] = voxels[y][x][z+1];
				}
				else
				{
					voxels[y][x][z] = 0;
				}
			}
		}
	}
}
void Model::RotateModelAroundXClockwise()
{
 	DWORD ***temp = NULL;
	AllocateMemory(&temp,xSize,ySize,zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);

	DeallocateMemory();
	AllocateMemory(xSize,zSize,ySize);	
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = 0; z<zSize; z++)
			{
				voxels[zSize-1-z][x][y] = temp[y][x][z];
			}
		}
	}

	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(xSize,zSize,ySize);
}
void Model::RotateModelAroundYClockwise()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);

	DeallocateMemory();
	AllocateMemory(zSize, ySize, xSize);
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = 0; z<zSize; z++)
			{
				voxels[y][z][xSize-1-x] = temp[y][x][z];
			}
		}
	}

	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(zSize, ySize, xSize);
}
void Model::RotateModelAroundZClockwise()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);

	DeallocateMemory();
	AllocateMemory(ySize, xSize, zSize);
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = 0; z<zSize; z++)
			{
				voxels[xSize-1-x][y][z] = temp[y][x][z];
			}
		}
	}

	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(ySize, xSize, zSize);
}
void Model::RotateModelAroundXCounterclockwise()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);

	DeallocateMemory();
	AllocateMemory(xSize, zSize, ySize);
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = 0; z<zSize; z++)
			{
				voxels[z][x][ySize-1-y] = temp[y][x][z];
			}
		}
	}

	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(xSize, zSize, ySize);
}
void Model::RotateModelAroundYCounterclockwise()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);

	DeallocateMemory();
	AllocateMemory(zSize, ySize, xSize);
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = 0; z<zSize; z++)
			{
				voxels[y][zSize-1-z][x] = temp[y][x][z];
			}
		}
	}

	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(zSize, ySize, xSize);
}
void Model::RotateModelAroundZCounterclockwise()
{
	DWORD ***temp = NULL;
	AllocateMemory(&temp, xSize, ySize, zSize);
	DoCopyOfVoxelsArray(&temp, voxels, xSize, ySize, zSize, xSize, ySize, zSize);

	DeallocateMemory();
	AllocateMemory(ySize, xSize, zSize);
	for (int x = 0; x<xSize; x++)
	{
		for (int y = 0; y<ySize; y++)
		{
			for (int z = 0; z<zSize; z++)
			{
				voxels[x][ySize-1-y][z] = temp[y][x][z];
			}
		}
	}

	DeallocateMemory(temp, xSize, ySize, zSize);
	ChangeModelSize(ySize, xSize, zSize);
}
void Model::AllocateMemory(int _xSize, int _ySize, int _zSize)
{
	DeallocateMemory();

	voxels = new DWORD **[_ySize];
	for (int y=0; y<_ySize; y++)
	{
		voxels[y] = new DWORD *[_xSize];
		for (int x=0; x<_xSize; x++)
		{
			voxels[y][x] = new DWORD [_zSize];
		}
	}
	
	CleanModel(_xSize, _ySize, _zSize);
}
void Model::AllocateMemory(DWORD ****_arr, int _xSize, int _ySize, int _zSize)
{
	DeallocateMemory((*_arr), _xSize, _ySize, _zSize);

	(*_arr) = new DWORD **[_ySize];
	for (int y=0; y<_ySize; y++)
	{
		(*_arr)[y] = new DWORD *[_xSize];
		for (int x=0; x<_xSize; x++)
		{
			(*_arr)[y][x] = new DWORD [_zSize];
		}
	}

	CleanModel((*_arr), _xSize, _ySize, _zSize);
}
void Model::ReallocateMemory(int _newSizeX, int _newSizeY, int _newSizeZ)
{
	DWORD ***temporary_buffer = NULL;
	AllocateMemory(&temporary_buffer,xSize,ySize,zSize);
	DoCopyOfVoxelsArray(&temporary_buffer,voxels,xSize,ySize,zSize,xSize,ySize,zSize);
	AllocateMemory(_newSizeX, _newSizeY, _newSizeZ);
	DoCopyOfVoxelsArray(&voxels,temporary_buffer,_newSizeX,_newSizeY,_newSizeZ,xSize,ySize,zSize);
	DeallocateMemory(temporary_buffer,xSize,ySize,zSize);
	ChangeModelSize(_newSizeX,_newSizeY,_newSizeZ);
}
void Model::DeallocateMemory()
{
	if (voxels != NULL)
	{
		for (int y=0; y<ySize; y++)
		{		
			for (int x=0; x<xSize; x++)
				delete[] voxels[y][x];
			delete [] voxels[y];
		}
		delete [] voxels;
		voxels=nullptr;
	}
}
void Model::DeallocateMemory(DWORD ***_arr, int _xSize, int _ySize, int _zSize)
{
	if (_arr != nullptr)
	{
		for (int y=0; y<_ySize; y++)
		{		
			for (int x=0; x<_xSize; x++)
				delete[] _arr[y][x];
			delete [] _arr[y];
		}
		delete [] _arr;
		_arr=nullptr;
	}
}
void Model::CleanModel(int _xSize, int _ySize, int _zSize)
{
	for (int y=0; y<_ySize; y++)
	{
		for (int x=0; x<_xSize; x++)
		{
			for (int z=0; z<_zSize; z++)
			{
				voxels[y][x][z]=0;
			}
		}
	}
}
void Model::CleanModel(DWORD ***_arr, int _xSize, int _ySize, int _zSize)
{
	for (int y=0; y<_ySize; y++)
	{
		for (int x=0; x<_xSize; x++)
		{
			for (int z=0; z<_zSize; z++)
			{
				_arr[y][x][z]=0;
			}
		}
	}
}
void Model::DoCopyOfVoxelsArray(DWORD ****_dst, DWORD ***_src, int _xSizeDst, int _ySizeDst, int _zSizeDst, int _xSizeSrc, int _ySizeSrc, int _zSizeSrc)
{
	int xMin=0, yMin=0, zMin=0;

	if (_xSizeDst < _xSizeSrc)
	{
		xMin = _xSizeDst;
	}
	else
	{
		xMin = _xSizeSrc;
	}

	if (_ySizeDst<_ySizeSrc)
	{
		yMin = _ySizeDst;
	}
	else
	{
		yMin = _ySizeSrc;
	}

	if (_zSizeDst<_zSizeSrc)
	{
		zMin = _zSizeDst;
	}
	else
	{
		zMin = _zSizeSrc;
	}

	for (int y=0; y<yMin; y++)
	{
		for (int x=0; x<xMin; x++)
		{
			for (int z=0; z<zMin; z++)
			{
				(*_dst)[y][x][z]=_src[y][x][z];
			}
		}
	}
}
void Model::AddVoxel(int _x, int _y, int _z, DWORD _c)
{
	if (_x > -1 && _y > -1 && _z > -1 && _x < xSize && _y < ySize && _z < zSize)
	{
		voxels[_y][_x][_z] = _c;
	}
}
void Model::DeleteVoxel(int _x, int _y, int _z)
{
	if (IsCoordinateCorrect(_x,_y,_z))
	{
		voxels[_y][_x][_z] = 0;
	}
}
void Model::FillVoxels(int _x, int _y, int _z, DWORD _color, DWORD _previousColor)
{
	for (int x=-1; x<2; x++)
	{
		for (int y = -1; y<2; y++)
		{
			for (int z = -1; z<2; z++)
			{
				if (!(x==0 && y==0 && z==0))
				{
					if (IsCoordinateCorrect(_x+x, _y+y, _z+z))
					{
						if (GetVoxelColor(_x+x, _y+y, _z+z)==_previousColor && GetVoxelColor(_x+x, _y+y, _z+z) != 0)
						{
							BrushVoxel(_x+x, _y+y, _z+z, _color);
							FillVoxels(_x+x, _y+y, _z+z, _color, _previousColor);
						}
					}
				}
			}
		}
	}		
}
void Model::BrushVoxel(int _x, int _y, int _z, DWORD _c)
{
	if (IsCoordinateCorrect(_x,_y,_z))
	{
		DeleteVoxel(_x,_y,_z);
		AddVoxel(_x,_y,_z,_c);
	}	
}
void Model::GetVoxelsArray(DWORD ****_arr, int *xSize_, int *ySize_, int *zSize_)
{
	AllocateMemory(_arr,xSize,ySize,zSize);
	*xSize_ = xSize;
	*ySize_ = ySize;
	*zSize_ = zSize;
	DoCopyOfVoxelsArray(_arr,voxels,xSize,ySize,zSize,xSize,ySize,zSize);
}
void Model::ChangeModelSize(int _x, int _y, int _z)
{
	xSize = _x;
	ySize = _y;
	zSize = _z;
}
int Model::GetModelSizeX()
{
	return xSize;
}
int Model::GetModelSizeY()
{
	return ySize;
}
int Model::GetModelSizeZ()
{
	return zSize;
}
DWORD Model::GetVoxelColor(int _x, int _y, int _z)
{
	if(IsCoordinateCorrect(_x, _y, _z))
	{
		return voxels[_y][_x][_z];
	}
	else
	{
		return 0;
	}
}
bool Model::IsCoordinateCorrect(int _x, int _y, int _z)
{
	if (_x > -1 && _y > -1 && _z > -1 && _x < xSize && _y < ySize && _z < zSize)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Model::SetVoxelsArray(DWORD ***_arr, int _xSize, int _ySize, int _zSize)
{
	AllocateMemory(_xSize, _ySize, _zSize);	
	ChangeModelSize(_xSize, _ySize, _zSize);
	for (int y = 0; y < _ySize; y++)
	{
		for (int x = 0; x < _xSize; x++)
		{
			for (int z = 0; z < _zSize; z++)
			{
				if (_arr[y][x][z] != 0)
				{
					AddVoxel(x, y, z, _arr[y][x][z]);
				}
			}
		}
	}	
}/*
void Model::ChangeCopiedVoxelsSize(int _x, int _y, int _z)
{
	xCopiedVoxelsSize = _x;
	yCopiedVoxelsSize = _y;
	zCopiedVoxelsSize = _z;
}

bool Model::IsCopiedVoxelsCoordinateCorrect(int _x, int _y, int _z)
{
	if (_x>-1&&_y>-1&&_z>-1&&_x<xCopiedVoxelsSize && _y<yCopiedVoxelsSize && _z<zCopiedVoxelsSize)
	{
		return true;
	}
	else
	{
		return false;
	}
}
DWORD Model::GetVoxelColor(DWORD****_arr, int _x, int _y, int _z)
{
	if (IsCopiedVoxelsCoordinateCorrect(_x, _y, _z))
	{
		return copiedVoxels[_y][_x][_z];
	}
	else
	{
		return 0;
	}
}*/

/*
for (int x = 0; x<xSize; x++)
{
for (int y = 0; y<ySize; y++)
{
for (int z = 0; z<zSize; z++)
{

}
}
}
*/