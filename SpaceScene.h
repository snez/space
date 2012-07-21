#ifndef	SPACESCENE_H
#define SPACESCENE_H

#include "WL\WL.h"
#include "XMesh.h"
#include "Particle.h"

class SpaceScene
{	
private:
	IDirect3DDevice9*	m_pd3dDevice;
	Starmap *			m_Stars;				// Starmap
	HDRSun *			m_Sun;
	GeneralObject**		m_Objects;
 
	int					m_NumberOfObjects;
	D3DXMATRIX			m_mViewCoordinates;
	D3DXMATRIX			m_mProjection;
	float				m_fAspectRatio;
	float				m_fFarPlane;
	float				m_fNearPlane;
	float				m_fFov;
	float				m_fCamRotateAngle;			// Starting rotation angle
	bool				m_bPaused;
	D3DXVECTOR3			cameraPos;
	D3DXVECTOR3			m_vRotationPoint;
	float				m_fDistanceX;
	float				m_fDistanceY;
	float				m_fCameraHeight;
	float				m_fRotateDelay;
	const D3DXVECTOR3	CameraPosition(float timeDelta);

	int					m_iCameraMode;

public:

	SpaceScene();
	~SpaceScene();
	void Render(float timeDelta);
	void Update(float timeDelta);
	void Zoom(float amount);
	inline void Pause() { m_bPaused = !m_bPaused; }	// Pause the scene animation
	inline bool Paused() const { return m_bPaused; }
	void SetCameraMode(int mode);
	inline void CameraRotateAngle(bool right = true) 
	{
		if (right)
		{
			m_fCamRotateAngle += D3DX_PI/32.0f;
			cameraPos = CameraPosition(0);
		}
		else
		{
			m_fCamRotateAngle -= D3DX_PI/32.0f;
			cameraPos = CameraPosition(0);
		}
	}

	// Device Changes
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);
	void OnResetDevice(IDirect3DDevice9* pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();

};

#endif	// SPACESCENE_H
