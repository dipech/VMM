#include "camera.h"
#include "defines.h"

#include "functions.h"

Camera::Camera()
{
	cameraType = MOUSE;

	xSizeOfModel = 1;
	ySizeOfModel = 1;
	zSizeOfModel = 1;

	pos = D3DXVECTOR3(0,0,-3.5f);	
	right = D3DXVECTOR3(1,0,0);
	up    = D3DXVECTOR3(0,1,0);
	look  = D3DXVECTOR3(0,0,1);

	radius = 3.5f;
	azimuth = azimuthAngleValueAtFrontPosition;
	zenith = 0.0f;

	pointToLook = D3DXVECTOR3(0,0,0);
	defaultRadius = 3.5f;
	radiusChangeRate = 1.0f;
	moveSpeed = 0.01f;
	rotateSpeed = 0.007f;

	azimuthID = FRONT;
	zenithID = MIDDLE;
}
void Camera::MoveForward()
{
	pos += D3DXVECTOR3(look.x, 0, look.z)*moveSpeed;
}
void Camera::MoveBackward()
{
	pos -= D3DXVECTOR3( look.x, 0, look.z )*moveSpeed;
}
void Camera::MoveRight()
{
	pos += D3DXVECTOR3( right.x, 0, right.z )*moveSpeed;
}
void Camera::MoveLeft()
{
	pos -= D3DXVECTOR3( right.x, 0, right.z )*moveSpeed;
}
void Camera::MoveUp()
{
	pos += D3DXVECTOR3( 0, 1, 0 )*moveSpeed;
}
void Camera::MoveDown()
{
	pos -= D3DXVECTOR3( 0, 1, 0 )*moveSpeed;
}
void Camera::AddToAzimuth(float _addToAzimuth)
{
	azimuth += _addToAzimuth*rotateSpeed*70;
	if(azimuth>maxAzimuthAngle)
	{
		azimuth=minAzimuthAngle;
	}
	if(azimuth<minAzimuthAngle)
	{
		azimuth=maxAzimuthAngle;
	}
}
void Camera::AddToZenith(float _addToZenith)
{
	zenith += _addToZenith*rotateSpeed*70;
	if(zenith>maxZenithAngle)
	{
		zenith=maxZenithAngle;
	}
	if(zenith<minZenithAngle)
	{
		zenith=minZenithAngle;
	}
}
void Camera::AddToRadius(float _addToRadius)
{
	radius += _addToRadius;
}
D3DXVECTOR3 Camera::GetPos()
{
	return pos;
}
D3DXVECTOR3 Camera::GetLook()
{
	return look;
}
D3DXVECTOR3 Camera::GetUp()
{
	return up;
}
D3DXVECTOR3 Camera::GetRight()
{
	return right;
}
float Camera::GetRadius()
{
	return radius;
}
UINT Camera::GetCameraType()
{
	return cameraType;
}
void Camera::ComputePos()
{
	pos.x = pointToLook.x + radius * cos(zenith*D3DX_PI/180.0f) * cos(azimuth*D3DX_PI/180.0f);
	pos.y = pointToLook.y + radius * sin(zenith*D3DX_PI/180.0f);
	pos.z = pointToLook.z + radius * cos(zenith*D3DX_PI/180.0f) * sin(azimuth*D3DX_PI/180.0f);
}
D3DXVECTOR3 Camera::GetPointToLook()
{
	return pointToLook;
}
void Camera::RestoreCameraPosition()
{
switch(azimuthID)
{
case FRONT:
	{
		switch(zenithID)
		{
			case TOP:
			{
				pos = D3DXVECTOR3(( xSizeOfModel-1 )/2.0f, ( ySizeOfModel-1 )/2.0f+defaultRadius, ( zSizeOfModel-1 )/2.0f);
				right = D3DXVECTOR3(1, 0, 0);
				up = D3DXVECTOR3(0, 0, 1);
				look = D3DXVECTOR3(0, -1, 0);

				switch (cameraType)
				{
				case WASD_SPACE_CONTROL:
				{
					azimuth = 0.0f;
					zenith = 0.0f;
					break;
				}
				case MOUSE:
				{
					azimuth = azimuthAngleValueAtFrontPosition;
					zenith = zenithAngleValueAtTopPosition;
					break;
				}
			}
		break;
		}
		case MIDDLE:
			{
				pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f, (ySizeOfModel-1)/2.0f, (zSizeOfModel-1)/2.0f-defaultRadius);
				right = D3DXVECTOR3(1,0,0);
				up    = D3DXVECTOR3(0,1,0);
				look  = D3DXVECTOR3(0,0,1);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtFrontPosition;
						zenith = zenithAngleValueAtMiddlePosition;
						break;
					}
				}					
				break;
			}
		case BOTTOM:
			{
										pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f, (ySizeOfModel-1)/2.0f-defaultRadius, (zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(1,0,0);
				up    = D3DXVECTOR3(0,0,-1);
				look  = D3DXVECTOR3(0,1,0);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtFrontPosition;
						zenith = zenithAngleValueAtBottomPosition;
						break;
					}
				}					
				break;
			}
		}
		break;
	}
case BACK:
	{
		switch(zenithID)
		{
		case TOP:
			{
				pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f, defaultRadius+(ySizeOfModel-1)/2.0f, (zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(-1,0,0);
				up    = D3DXVECTOR3(0,0,-1);
				look  = D3DXVECTOR3(0,-1,0);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtBackPosition;
						zenith = zenithAngleValueAtTopPosition;
						break;
					}
				}					
				break;
			}
		case MIDDLE:
			{
				pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f, (ySizeOfModel-1)/2.0f, defaultRadius+(zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(-1,0,0);
				up    = D3DXVECTOR3(0,1,0);
				look  = D3DXVECTOR3(0,0,-1);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtBackPosition;
						zenith = zenithAngleValueAtMiddlePosition;
						break;
					}
				}					
				break;
			}
		case BOTTOM:
			{
				pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f, (ySizeOfModel-1)/2.0f-defaultRadius, (zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(-1,0,0);
				up    = D3DXVECTOR3(0,0,0);
				look  = D3DXVECTOR3(0,1,0);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtBackPosition;
						zenith = zenithAngleValueAtBottomPosition;
						break;
					}
				}					
				break;
			}
		}
		break;
	}
case LEFT:
	{
		switch(zenithID)
		{
		case TOP:
			{
				pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f, (ySizeOfModel-1)/2.0f+defaultRadius, (zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(0,0,-1);
				up    = D3DXVECTOR3(0,0,0);
				look  = D3DXVECTOR3(0,-1,0);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtLeftPosition;
						zenith = zenithAngleValueAtTopPosition;
						break;
					}
				}					
				break;
			}
		case MIDDLE:
			{
				pos=D3DXVECTOR3(-defaultRadius+(xSizeOfModel-1)/2.0f, (ySizeOfModel-1)/2.0f, (zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(0,0,-1);
				up    = D3DXVECTOR3(0,1,0);
				look  = D3DXVECTOR3(1,0,0);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtLeftPosition;
						zenith = zenithAngleValueAtMiddlePosition;
						break;
					}
				}					
				break;
			}
		case BOTTOM:
			{
				pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f, (ySizeOfModel-1)/2.0f-defaultRadius, (zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(0,0,-1);
				up    = D3DXVECTOR3(0,0,0);
				look  = D3DXVECTOR3(0,1,0);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtLeftPosition;
						zenith = zenithAngleValueAtBottomPosition;
						break;
					}
				}					
				break;
			}
		}
		break;
	}
case RIGHT:
	{
		switch(zenithID)
		{
		case TOP:
			{
				pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f, (ySizeOfModel-1)/2.0f+defaultRadius, (zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(0,0,1);
				up    = D3DXVECTOR3(0,0,0);
				look  = D3DXVECTOR3(0,-1,0);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtRightPosition;
						zenith = zenithAngleValueAtTopPosition;
						break;
					}
				}					
				break;
			}
		case MIDDLE:
			{
				pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f+defaultRadius, (ySizeOfModel-1)/2.0f, (zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(0,0,1);
				up    = D3DXVECTOR3(0,1,0);
				look  = D3DXVECTOR3(-1,0,0);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtRightPosition;
						zenith = zenithAngleValueAtMiddlePosition;
						break;
					}
				}
				break;
			}
		case BOTTOM:
			{
				pos=D3DXVECTOR3((xSizeOfModel-1)/2.0f, (ySizeOfModel-1)/2.0f-defaultRadius, (zSizeOfModel-1)/2.0f);
				right = D3DXVECTOR3(0,0,1);
				up    = D3DXVECTOR3(0,0,0);
				look  = D3DXVECTOR3(0,1,0);

				switch(cameraType)
				{
				case WASD_SPACE_CONTROL:
					{
						azimuth = 0.0f;
						zenith = 0.0f;
						break;
					}
				case MOUSE:
					{
						azimuth = azimuthAngleValueAtRightPosition;
						zenith = zenithAngleValueAtBottomPosition;
						break;
					}
				}					
				break;
			}
		}
		break;
	}
}	
	radius = defaultRadius;	
}
float Camera::GetAzimuth()
{
	return azimuth;
}
float Camera::GetZenith()
{
	return zenith;
}
void Camera::SetAzimuth(float _azimuth)
{
	azimuth = _azimuth*rotateSpeed;
}
void Camera::SetZenith(float _zenith)
{
	zenith = _zenith*rotateSpeed;
}
void Camera::SetPos(D3DXVECTOR3 _pos)
{
	pos = _pos;
}
void Camera::SetLook(D3DXVECTOR3 _look)
{
	look = _look;
}
void Camera::SetUp(D3DXVECTOR3 _up)
{
	up = _up;
}
void Camera::SetRight(D3DXVECTOR3 _right)
{
	right = _right;
}
void Camera::SetCameraType(UINT _cameraType)
{
	cameraType = (CameraType)_cameraType;
}
void Camera::SetAzimuthID(UINT _azimuth)
{
	azimuthID = (AzimuthID)_azimuth;
}
void Camera::SetZenithID(UINT _zenith)
{
	zenithID = (ZenithID)_zenith;
}
UINT Camera::GetAzimuthID()
{
	return azimuthID;
}
UINT Camera::GetZenithID()
{
	return zenithID;
}
void Camera::SetCameraSettingsDependsTheModelSizes(int _xSize, int _ySize, int _zSize)
{
	xSizeOfModel = _xSize;
	ySizeOfModel = _ySize;
	zSizeOfModel = _zSize;

	pointToLook.x = (_xSize-1) / 2.0f;
	pointToLook.y = (_ySize-1) / 2.0f;
	pointToLook.z = (_zSize-1) / 2.0f;
	
	int maxModelSize = _xSize;
	if (maxModelSize < _ySize)
	{
		maxModelSize = _ySize;
	}
	if (maxModelSize < _zSize)
	{
		maxModelSize = _zSize;
	}

	float k = 1.1547f;// tan(30)*2;
	float x = 4.04157f/2.0f; // proportion
	defaultRadius = x*maxModelSize / k;
	//radius = defaultRadius;
	
	int maxModelDimension = 200;
	if ( maxModelSize > maxModelDimension )
	{
		maxModelSize = maxModelDimension;
	}
	//after ScaleValue() addToRadius will be between A and B
	radiusChangeRate = ScaleValue((float)maxModelSize, 1.0f,(float)maxModelDimension,1.0f,10.0f);
	//after ScaleValue() moveSpeed will be between A and B
	moveSpeed = ScaleValue((float)maxModelSize,1.0f,(float)maxModelDimension,0.03f,1.0f);
	Sleep(0);
}
float Camera::GetRateOfChangeRadius()
{
	return radiusChangeRate;
}
void Camera::GetAzimuthalDirectionsOfLook(UINT *_leftOrRight, UINT *_frontOrBack)
{
	if (cameraType == CameraType::MOUSE)
	{
		if ((azimuth>90.0f && azimuth<=270.000f))
		{
			*_leftOrRight = FROM_LEFT_TO_RIGHT;
		}
		else
		{
			*_leftOrRight = FROM_RIGHT_TO_LEFT;
		}
		if ((azimuth>0.0f && azimuth<=180.0f))
		{
			*_frontOrBack = FROM_BACK_TO_FRONT;
		}
		else
		{
			*_frontOrBack = FROM_FRONT_TO_BACK;
		}

	}
	else
	{
		float angle = (float)(atan(fabs(look.x)/(-1.0)*fabs(look.z))*57.2957795);

		if (look.x<0.0f && look.z>0.0f)
		{
			angle += 90.0f;
		}
		else if (look.x<0.0f && look.z<0.0f)
		{
			angle += 180.0f;
		}
		else if (look.x>0.0f && look.z<0.0f)
		{
			angle += 270.0f;
		}

		angle += 90.0f;
		if (angle>360.0f)
		{
			angle = angle - 90.0f;
		}

		if ((angle>90.0f && angle<=270.000f))
		{
			*_leftOrRight = FROM_RIGHT_TO_LEFT;
		}
		else
		{
			*_leftOrRight = FROM_LEFT_TO_RIGHT;
		}
		if ((angle>0.0f && angle<=180.0f))
		{
			*_frontOrBack = FROM_FRONT_TO_BACK;
		}
		else
		{
			*_frontOrBack = FROM_BACK_TO_FRONT;
		}
	}
}
UINT Camera::GetZenithalDirectionOfLook()
{
	if (cameraType==CameraType::MOUSE)
	{
		if (zenith >-90.0f && zenith<=0.0f)
		{
			return FROM_BOTTOM_TO_UP;
		} 
		else
		{
			return FROM_TOP_TO_BOTTOM;
		}
	}
	else
	{
		float angle = (float)(atan((look.x*look.x + look.z*look.z)/look.y)*57.2957795);
		if (angle>0.0f)
		{
			return FROM_BOTTOM_TO_UP;
		}
		else
		{
			return FROM_TOP_TO_BOTTOM;
		}
	}
}
void Camera::GetAzimuthalDirectionsOfLook(float *_angle)
{
	if (cameraType==CameraType::MOUSE)
	{
		*_angle = azimuth;
	}
	else
	{
		float angle = 0.0f;
		if (look.x>0 && look.z>0)
		{
			angle = (float)(atan(fabs(look.z)/fabs(look.x))*57.2957795);
		}
		else if (look.x<0 && look.z>0)
		{
			angle = (float)(atan(fabs(look.x)/fabs(look.z))*57.2957795);
		}
		else if (look.x<0&&look.z<0)
		{
			angle = (float)(atan(fabs(look.z)/fabs(look.x))*57.2957795);
		}
		else if (look.x>0&&look.z<0)
		{
			angle = (float)(atan(fabs(look.x)/fabs(look.z))*57.2957795);
		}

		if (look.x<0.0f && look.z>0.0f)
		{
			angle += 90.0f;
		}
		else if (look.x<0.0f && look.z<0.0f)
		{
			angle += 180.0f;
		}
		else if (look.x>0.0f && look.z<0.0f)
		{
			angle += 270.0f;
		}

		angle += 180.0f;
		if (angle>360.0f)
		{
			angle = angle-360.0f;
		}

		*_angle = angle;
	}
}
void Camera::GetZenithalDirectionOfLook(float *_angle)
{
	if (cameraType==CameraType::MOUSE)
	{
		*_angle = zenith;
	}
	else
	{
		float angle = (float)(atan((look.x*look.x+look.z*look.z)/look.y)*57.2957795);
		*_angle = angle;
	}
}