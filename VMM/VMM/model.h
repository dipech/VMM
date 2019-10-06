#ifndef MODEL_CLASS
#define MODEL_CLASS

class Model
{
protected:
	DWORD ***voxels;	
	int xSize;
	int ySize;
	int zSize;

public:
	Model();
	void IncreaseModelOnX();
	void IncreaseModelOnY();
	void IncreaseModelOnZ();
	void DecreaseModelOnX();
	void DecreaseModelOnY();
	void DecreaseModelOnZ();
	void IncreaseModelCoordinateOnX();
	void IncreaseModelCoordinateOnY();
	void IncreaseModelCoordinateOnZ();
	void DecreaseModelCoordinateOnX();
	void DecreaseModelCoordinateOnY();
	void DecreaseModelCoordinateOnZ();
	void RotateModelAroundXClockwise();
	void RotateModelAroundXCounterclockwise();
	void RotateModelAroundYClockwise();
	void RotateModelAroundYCounterclockwise();
	void RotateModelAroundZClockwise();
	void RotateModelAroundZCounterclockwise();
	void AddVoxel(int _x, int _y, int _z, DWORD _c);
	void DeleteVoxel(int _x, int _y, int _z);
	void FillVoxels(int _x, int _y, int _z, DWORD _color, DWORD _previousColor);
	void BrushVoxel(int _x, int _y, int _z, DWORD _c);
	void GetVoxelsArray(DWORD ****_arr, int *xSize_, int *ySize_, int *zSize_);
	void ChangeModelSize(int _x, int _y, int _z);
	void ChangeCopiedVoxelsSize(int _x, int _y, int _z);
	int GetModelSizeX();
	int GetModelSizeY();
	int GetModelSizeZ();
	DWORD GetVoxelColor(int _x, int _y, int _z);
	bool IsCoordinateCorrect(int _x, int _y, int _z);
	void SetVoxelsArray(DWORD ***_arr, int _xSize, int _ySize, int _zSize);
	void DoCopyOfVoxelsArray(DWORD ****_dst, DWORD ***_src, int _xSizeDst, int _ySizeDst, int _ZSizeDst, int _xSizeSrc, int _ySizeSrc, int _ZSizeSrc);
	void AllocateMemory(int _xSize, int _ySize, int _zSize);
	void AllocateMemory(DWORD ****_arr, int _xSize, int _ySize, int _zSize);
	void ReallocateMemory(int _newSizeX, int _newSizeY, int _newSizeZ);
	void DeallocateMemory();
	void DeallocateMemory(DWORD ***_arr, int _xSize, int _ySize, int _zSize);
	void CleanModel(int _xSize, int _ySize, int _zSize);
	void CleanModel(DWORD ***_arr, int _xSize, int _ySize, int _zSize);	
};



#endif /*MODEL_CLASS*/