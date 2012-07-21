#include "WLComet.h"

Comet::Comet(LPCWSTR xfile)
{
	pMesh = new XMesh();
	HRESULT hr = pMesh->LoadFile(xfile);
	if (FAILED(hr))
	{
		pMesh = NULL;
		DXUTTrace(__FILE__,(DWORD)__LINE__, hr, L"Failed to create Comet mesh.", true);
	}

	partSys = new ParticleSystem(GetPosition(), 30.0f,-25.0f, 30.0f, 30.0f, 500);
	partSys->SetColor(D3DXCOLOR(1.0f,1.0f,0.0f,1.0f), D3DXCOLOR(0.6f,0.6f,0.6f,0.0f), D3DXCOLOR(1.0f,0.0f,0.0f,0.0f), D3DXCOLOR(0.1f,0.1f,0.1f,0.0f));
	partSys->SetLife(1.0f,20.0f);
	partSys->SetSize(3.0f,15.0f);
	partSys->SetVelocity(2.0f,6.0f);
	partSys->SetTexture(L"spark.tga");
	partSys->Start();
	
	m_vVelocity = D3DXVECTOR3(-0.2f,-0.2f,0);
}

Comet::~Comet()
{
	SAFE_DELETE(pMesh);
	SAFE_DELETE(partSys);
}

void Comet::Render()
{
	GeneralObject::Render();
	partSys->Render();
}

void Comet::Update(float timeDelta)
{
	D3DXMATRIX matTemp;
	D3DXMATRIX matRot;

	D3DXMatrixIdentity(&matRot);
	D3DXMatrixRotationX(&matTemp, timeDelta * (10 * DEG_TO_RAD) );
    D3DXMatrixMultiply(&matRot, &matRot, &matTemp);
	D3DXMatrixRotationX(&matTemp, timeDelta * (10 * DEG_TO_RAD));
    D3DXMatrixMultiply(&matRot, &matRot, &matTemp);

	D3DXMatrixMultiply(&m_mWorldMatrix, &matRot, &m_mWorldMatrix);

	D3DXVECTOR3 pos = GetPosition();
	pos += m_vVelocity * timeDelta;
	SetPosition(pos);
	partSys->SetPosition(pos);
	partSys->Update(timeDelta);
}