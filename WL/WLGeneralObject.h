#pragma once

#include "dxstdafx.h"
#include "..\XMesh.h"

class GeneralObject 
{
protected:

	XMesh* pMesh;
	D3DXMATRIX m_mWorldMatrix;
	D3DXVECTOR3 m_vVelocity;

public:

	GeneralObject();
	virtual ~GeneralObject() = 0 {};

	virtual void Render();
	virtual void Update(float timeDelta){};

	inline void SetMatrix(const D3DXMATRIX& matrix) { m_mWorldMatrix = matrix; }
	inline D3DXMATRIX GetViewMatrix() { return m_mWorldMatrix; }
	D3DXVECTOR3 GetPosition();
	void SetPosition(const D3DXVECTOR3& pos);	
	D3DXVECTOR3 GetVelocity();
	void SetVelocity(const D3DXVECTOR3& pos);

	virtual HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	virtual void OnResetDevice(){};
	virtual void OnLostDevice(){};
	virtual void OnDestroyDevice(){};
};
