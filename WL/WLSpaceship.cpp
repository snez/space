#include "WLSpaceship.h"

Spaceship::Spaceship(LPCWSTR xfile)
{
	pMesh = new XMesh();
	HRESULT hr = pMesh->LoadFile(xfile);
	if (FAILED(hr))
	{
		pMesh = NULL;
		DXUTTrace(__FILE__,(DWORD)__LINE__, hr, L"Failed to create Spaceship mesh.", true);
	}

	partSys = new ParticleSystem(GetPosition(), 0.0f,0.0f, 30.0f, 30.0f, 200);
	partSys->SetColor(D3DXCOLOR(1.0f,0.8f,0.5f,1.0f), D3DXCOLOR(0.1f,0.1f,0.1f,0.0f), D3DXCOLOR(0.0f,0.0f,1.0f,0.0f), D3DXCOLOR(0.1f,0.1f,0.1f,0.0f));
	partSys->SetLife(0.4f,0.8f);
	partSys->SetSize(2.0f,6.0f);
	partSys->SetVelocity(0,36.0f);
	partSys->SetTexture(L"flare.tga");
	partSys->Start();

	partDust = new ParticleSystem(GetPosition(), 0.0f,0.0f, 40.0f, 40.0f, 1000);
	partDust->SetColor(D3DXCOLOR(0.8f,0.8f,1.0f,1.0f), D3DXCOLOR(0.6f,0.6f,0.0f,0.0f), D3DXCOLOR(0.0f,0.0f,0.0f,0.0f), D3DXCOLOR(0.1f,0.1f,0.1f,0.0f));
	partDust->SetLife(2.0f,10.0f);
	partDust->SetSize(0.0f,4.0f);
	partDust->SetVelocity(50,150.0f);
	partDust->SetTexture(L"spark.tga");
	partDust->Start();

	//m_vVelocity = D3DXVECTOR3(0,0,-1.0f);
	m_vVelocity = D3DXVECTOR3(0,0, 0);
}

Spaceship::~Spaceship()
{
	SAFE_DELETE(pMesh);
	SAFE_DELETE(partSys);
	SAFE_DELETE(partDust);
}

void Spaceship::Render()
{
	GeneralObject::Render();
	partSys->Render();
	partDust->Render();
}

void Spaceship::Update(float timeDelta)
{
	D3DXVECTOR3 pos = GetPosition();
	pos += m_vVelocity * timeDelta;
	SetPosition(pos);
	pos.z += 8.0f;
	partSys->SetPosition(pos);
	partSys->Update(timeDelta);
	pos.z -= 108.0f;
	partDust->SetPosition(pos);
	partDust->Update(timeDelta);	
}