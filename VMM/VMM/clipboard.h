#ifndef CLIPBOARD_CLASS
#define CLIPBOARD_CLASS

#include <windows.h>

#include "model.h"

class Clipboard : public Model
{
public:
	void CopyVoxels(int _xStart, int _yStart, int _zStart, int _xEnd, int _yEnd, int _zEnd, Model & model);
	void PasteVoxels(int _xStart, int _yStart, int _zStart, int _xEnd, int _yEnd, int _zEnd, Model & model);	
};

#endif /*CLIPBOARD_CLASS*/