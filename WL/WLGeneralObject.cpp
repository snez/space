#include "WLGeneralObject.h"

GeneralObject::GeneralObject()
{
	pMesh = NULL;
	m_vVelocity = D3DXVECTOR3(0,0,0);
	D3DXMatrixIdentity(&m_mWorldMatrix);
}

HRESULT GeneralObject::OnCreateDevice(IDirect3DDevice9* pd3dDevice)
{ 
	return D3D_OK; 
}

void GeneralObject::Render()
{
	DXUTGetD3DDevice()->SetTransform(D3DTS_WORLD, &m_mWorldMatrix);

	if (pMesh)
		pMesh->Render();
}

D3DXVECTOR3 GeneralObject::GetPosition()
{
	return D3DXVECTOR3(m_mWorldMatrix._41,m_mWorldMatrix._42,m_mWorldMatrix._43);
}

void GeneralObject::SetPosition(const D3DXVECTOR3& pos)
{
	m_mWorldMatrix._41 = pos.x;
	m_mWorldMatrix._42 = pos.y;
	m_mWorldMatrix._43 = pos.z;
}