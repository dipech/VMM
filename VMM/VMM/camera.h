#ifndef CAMERA_CLASS
#define CAMERA_CLASS

#include <d3dx9.h>

class Camera
{
public:
	//Control Types.
	enum CameraType
	{
		WASD_SPACE_CONTROL,
		MOUSE
	}cameraType;
	//Azimuth states
	enum AzimuthID
	{
		FRONT,
		BACK,
		LEFT,
		RIGHT
	}azimuthID;
	//Zenith states
	enum ZenithID
	{
		TOP,
		MIDDLE,
		BOTTOM
	}zenithID;
	//Azimuthal Direction of Look
	enum AzimuthalDirectionOfLook
	{
		FROM_FRONT_TO_BACK,
		FROM_BACK_TO_FRONT,
		FROM_RIGHT_TO_LEFT,
		FROM_LEFT_TO_RIGHT
	};
	//Zenithal Direction of Look
	enum ZenithalDirectionOfLook
	{
		FROM_TOP_TO_BOTTOM,
		FROM_BOTTOM_TO_UP
	};
	
private:
	//"WASD-SPACE-CTRL" control type. Cartesian coordinate system.
	D3DXVECTOR3 pos;
	D3DXVECTOR3 look;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;

	//"MOUSE" control type. Spherical coordinate system.
	float azimuth;
	float zenith;
	float radius;
	
	float defaultRadius;
	int xSizeOfModel;
	int ySizeOfModel;
	int zSizeOfModel;
	float radiusChangeRate;
	float moveSpeed;
	float rotateSpeed;
	D3DXVECTOR3 pointToLook;
	
	//constants
	const float maxAzimuthAngle = 359.999f;
	const float minAzimuthAngle = 0.000f;
	const float maxZenithAngle = 89.999f;
	const float minZenithAngle = -89.999f;

	const float zenithAngleValueAtTopPosition = maxZenithAngle;
	const float zenithAngleValueAtMiddlePosition = 0.000f;
	const float zenithAngleValueAtBottomPosition = minZenithAngle;

	const float azimuthAngleValueAtFrontPosition = 269.999f;
	const float azimuthAngleValueAtBackPosition = 89.999f;
	const float azimuthAngleValueAtLeftPosition = 179.999f;
	const float azimuthAngleValueAtRightPosition = 0.000f;

public:
	Camera();
	void MoveForward();
	void MoveBackward();
	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();
	D3DXVECTOR3 GetPos();
	D3DXVECTOR3 GetLook();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();
	void SetPos(D3DXVECTOR3);
	void SetLook(D3DXVECTOR3);
	void SetUp(D3DXVECTOR3);
	void SetRight(D3DXVECTOR3);

	void AddToAzimuth(float _addToAzimuth);
	void AddToZenith(float _addToZenith);
	void AddToRadius(float _addToRadius);
	D3DXVECTOR3 GetPointToLook();
	void ComputePos();
	float GetRadius();
	float GetAzimuth();
	float GetZenith();
	void SetAzimuth(float);
	void SetZenith(float);

	void SetCameraSettingsDependsTheModelSizes(int _xSize, int _ySize, int zSize);

	void RestoreCameraPosition();
	UINT GetCameraType();
	void SetCameraType(UINT);
	void SetAzimuthID(UINT _azimuthID);
	void SetZenithID(UINT _zenithID);
	UINT GetAzimuthID();
	UINT GetZenithID();
	float GetRateOfChangeRadius();
	void GetAzimuthalDirectionsOfLook(UINT *_leftOrRight, UINT *_frontOrBack);
	UINT GetZenithalDirectionOfLook();
	void GetAzimuthalDirectionsOfLook(float *_angle);
	void GetZenithalDirectionOfLook(float *_angle);
};


#endif /*CAMERA_CLASS*/