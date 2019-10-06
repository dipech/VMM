#include "clipboard.h"

void Clipboard::CopyVoxels(int _xStart, int _yStart, int _zStart, int _xEnd, int _yEnd, int _zEnd, Model & model)
{
	DeallocateMemory();
	int xNewCopiedVoxelsSize = _xEnd-_xStart;
	int yNewCopiedVoxelsSize = _yEnd-_yStart;
	int zNewCopiedVoxelsSize = _zEnd-_zStart;
	ChangeModelSize(xNewCopiedVoxelsSize, yNewCopiedVoxelsSize, zNewCopiedVoxelsSize);
	voxels = nullptr;
	AllocateMemory(xSize,ySize,zSize);

	for (int x = _xStart, xCopied = 0; x<_xEnd; x++, xCopied++)
	{
		for (int y = _yStart, yCopied = 0; y<_yEnd; y++, yCopied++)
		{
			for (int z = _zStart, zCopied = 0; z<_zEnd; z++, zCopied++)
			{
				if (model.IsCoordinateCorrect(x, y, z))
				{
					voxels[yCopied][xCopied][zCopied] = model.GetVoxelColor(x,y,z);
				}
			}
		}
	}
}
void Clipboard::PasteVoxels(int _xStart, int _yStart, int _zStart, int _xEnd, int _yEnd, int _zEnd, Model & model)
{
	int xCopied = 0;
	int yCopied = 0;
	int zCopied = 0;

	for (int x = _xStart; x<_xEnd; x++)
	{
		for (int y = _yStart; y<_yEnd; y++)
		{
			for (int z = _zStart; z<_zEnd; z++)
			{
				if (model.IsCoordinateCorrect(x, y, z))
				{					
					DWORD color = GetVoxelColor(xCopied, yCopied, zCopied);
					if (color != 0)
					{
						model.DeleteVoxel(x, y, z);
						model.AddVoxel(x, y, z, color);
					}					
				}
				zCopied++;
				if (zCopied==zSize||z==_zEnd-1)
				{
					zCopied = 0;
				}
			}
			yCopied++;
			if (yCopied==ySize||y==_yEnd-1)
			{
				yCopied = 0;
			}
		}
		xCopied++;
		if (xCopied==xSize||x==_xEnd-1)
		{
			xCopied = 0;
		}
	}
}