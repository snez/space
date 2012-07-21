#include "SpaceScene.h"

SpaceScene::SpaceScene() 
{
	m_iCameraMode = 0;
	m_pd3dDevice = DXUTGetD3DDevice();
	m_fFarPlane = 1000.0f;
	m_fNearPlane = 1.0f;
	m_fFov = 0;
	m_Stars = NULL;
	m_Sun = NULL;
	m_fCamRotateAngle = D3DX_PI*2;	// Starting rotation angle
	m_bPaused = false;
	m_vRotationPoint = D3DXVECTOR3(0,0,0);
	m_fDistanceX = 0.0f;
	m_fDistanceY = 0.0f;
	m_fCameraHeight = 0.0f;
	m_fRotateDelay = 0.0f;
	// Allocate space
	m_NumberOfObjects = 4;
	m_Objects = new GeneralObject*[m_NumberOfObjects];

	m_Objects[0] = new Planet(L"data\\models\\moon.x");
	m_Objects[1] = new Planet(L"data\\models\\venus.x", L"data\\fx\\glow.fx",0.5f,0.2f,0.2f,0.20f);
	m_Objects[2] = new Comet(L"data\\models\\comet.x");
	m_Objects[3] = new Spaceship(L"data\\models\\bigship1.x");

	D3DXMATRIX** m_ObjectWorldMatrices;	// World matrices for each object.
	m_ObjectWorldMatrices = new D3DXMATRIX*[m_NumberOfObjects];
	
	for (int i = 0; i < m_NumberOfObjects; i++)
		m_ObjectWorldMatrices[i] = new D3DXMATRIX();

	D3DXMatrixTranslation(m_ObjectWorldMatrices[0], 0.0f, 0.0f, -15.0f);	
	D3DXMatrixIdentity(m_ObjectWorldMatrices[1]);
	D3DXMatrixScaling(m_ObjectWorldMatrices[1], 5.0f, 5.0f, 5.0f);	
	D3DXMatrixTranslation(m_ObjectWorldMatrices[2], -150.0f, 50.0f, 400.0f);
	D3DXMatrixTranslation(m_ObjectWorldMatrices[3], 100.0f, 0.0f, 16.0f);

	//D3DXMATRIX matA;
	//D3DXMATRIX matA1;
	//D3DXMatrixIdentity(&matA);
	//D3DXMatrixScaling(&matA, 0.1f, 0.1f, 0.1f);
	//D3DXMatrixTranslation(&matA1, 100.0f, 0.0f, 16.0f);
	//D3DXMatrixMultiply(m_ObjectWorldMatrices[3],&matA,&matA1);

	for (int i = 0; i < m_NumberOfObjects; i++) // initialize matrixes
		m_Objects[i]->SetMatrix(*m_ObjectWorldMatrices[i]);

	for(int i = 0; i < m_NumberOfObjects; i++)
		delete m_ObjectWorldMatrices[i];
	delete[] m_ObjectWorldMatrices;

	// Set up the projection and D3D states
	OnResetDevice(m_pd3dDevice);

	//	Starmap
	m_Stars = new Starmap(m_fFarPlane, 70600, 10, 200);
	// Sun
	m_Sun = new HDRSun(250.0f, 0.0f, 250.0f, 80.0f, 32, 20, m_mProjection);
	SetCameraMode(0);
}


SpaceScene::~SpaceScene()
{
	for(int i = 0; i < m_NumberOfObjects; i++)
		delete m_Objects[i];

	delete m_Stars;
	delete m_Sun;
	delete[] m_Objects;	
}

void SpaceScene::SetCameraMode(int mode)
{
	if (mode < 0 || mode > 2)
		return;
	static bool firstTime[3] = {true,true,true};
	static float angle[3];

	angle[m_iCameraMode] = m_fCamRotateAngle;

	m_iCameraMode = mode;
	D3DXVECTOR3 position = D3DXVECTOR3(0,0,0);
	switch(mode)
	{
		case 0: 
			{
				m_fDistanceX = 90.0f;
				m_fDistanceY = 75.0f;
				m_fFov = D3DX_PI * 0.12f;
				m_fCameraHeight = 0.0f;
				m_fCamRotateAngle = D3DX_PI*2;
				
				if (!firstTime[0])
					m_fCamRotateAngle = angle[mode];
				else
					firstTime[0] = false;
				m_vRotationPoint = D3DXVECTOR3(0.0f,0.0f,0.0f);
				m_fRotateDelay = 30.0f;
				break;
			}
		case 1:
			{
				m_fDistanceX = 60.0f;
				m_fDistanceY = 60.0f;
				m_fCameraHeight = 10.0f;
				m_fFov = D3DX_PI * 0.32f;
				position = m_Objects[3]->GetPosition();
				if (firstTime[1])
				{
                   m_fCamRotateAngle = 70 * DEG_TO_RAD;
				   firstTime[1] = false;
				}
				else
					m_fCamRotateAngle = angle[mode];
				
				m_vRotationPoint = m_Objects[3]->GetPosition();
				m_fRotateDelay = 45.0f;
				break;
			}
		case 2:
			{
				m_fDistanceX = 120.0f;
				m_fDistanceY = 120.0f;
				m_fCameraHeight = 0.0f;
				m_fFov = D3DX_PI * 0.22f;
				position = m_Objects[2]->GetPosition();
				if (firstTime[2])
				{
                   m_fCamRotateAngle = 115 * DEG_TO_RAD;
				   firstTime[2] = false;
				}
				else
					m_fCamRotateAngle = angle[mode];
				m_vRotationPoint = position;
				m_fRotateDelay = 70.0f;
				break;
			}
	};

	D3DXVECTOR3 target(m_vRotationPoint.x, m_vRotationPoint.y, m_vRotationPoint.z);

	// the worlds up vector
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&m_mViewCoordinates, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &m_mViewCoordinates);

	const D3DSURFACE_DESC* pBackBufferSurfaceDesc = DXUTGetBackBufferSurfaceDesc();
	m_fAspectRatio = float(pBackBufferSurfaceDesc->Width) / float(pBackBufferSurfaceDesc->Height);
	D3DXMatrixPerspectiveFovLH(
			&m_mProjection,
			m_fFov,
			m_fAspectRatio,
			m_fNearPlane,
			m_fFarPlane);

	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_mProjection);
	if (m_Sun)
		m_Sun->ResetMatrices();
}

void SpaceScene::Render(float timeDelta)
{
	// Draw skybox 
	Device->SetRenderState(D3DRS_WRAP0, 0); 
	m_Stars->draw(cameraPos);
	m_Sun->Draw(m_mViewCoordinates);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//	Set the looping textures RenderState as described in
	//	http://groups.google.com/group/microsoft.public.win32.programmer.directx.graphics/browse_thread/thread/68e1a6e41541ffd5/860fdd0f25f5b142
	//Device->SetRenderState(D3DRS_WRAP0, D3DWRAPCOORD_0); 

	switch(m_iCameraMode)
	{
		case 0: 
			for(int i = 0; i < m_NumberOfObjects-1; i++)
			{
				m_Objects[i]->Render();
			}
			break;
		case 1: 
			m_Objects[3]->Render();
			break;
		case 2: 
			for(int i = 0; i < m_NumberOfObjects-1; i++)
			{
				m_Objects[i]->Render();
			}
			break;
	};


}

void SpaceScene::Update(float timeDelta)
{
	for(int i = 0; i < m_NumberOfObjects; i++)
		m_Objects[i]->Update(timeDelta);

	// Recalculate Camera Position and m_mViewCoordinates
	//if (m_iCameraMode == 0)	
		cameraPos = CameraPosition(timeDelta);
}

const D3DXVECTOR3 SpaceScene::CameraPosition(float timeDelta)
{
	// Animate the camera:
	// The camera will circle around the center of the scene.  We use the
	// sin and cos functions to generate points on the circle, then scale them
	// by 10 to further the radius.  In addition the camera will move up and down
	// as it circles about the scene.
	
	//static float cameraHeightDirection = 5.0f;
	
	D3DXVECTOR3 position(	cosf(m_fCamRotateAngle) * m_fDistanceX + m_vRotationPoint.x , 
							m_fCameraHeight + m_vRotationPoint.y, 
							sinf(m_fCamRotateAngle) * m_fDistanceY + m_vRotationPoint.z);

	// the camera is targetted at the origin of the world
	D3DXVECTOR3 target(m_vRotationPoint.x, m_vRotationPoint.y, m_vRotationPoint.z);

	// the worlds up vector
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&m_mViewCoordinates, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &m_mViewCoordinates);

	// compute the position for the next frame
	m_fCamRotateAngle += timeDelta/ m_fRotateDelay;
	if( m_fCamRotateAngle >= 6.28f )
		m_fCamRotateAngle = 0.0f;

	//// compute the height of the camera for the next frame
	//cameraHeight += cameraHeightDirection * timeDelta;
	//if( cameraHeight > 0.0f )
	//	cameraHeightDirection -= timeDelta;
	//else
	//	cameraHeightDirection += timeDelta;


	return position;

}


void SpaceScene::Zoom(float amount)
{
	if (((m_fFov + amount) > 1.0f) || ((m_fFov + amount) < 0.0f))
		return;
	else 
		m_fFov += amount;

    const D3DSURFACE_DESC* pBackBufferSurfaceDesc = DXUTGetBackBufferSurfaceDesc();
	m_fAspectRatio = float(pBackBufferSurfaceDesc->Width) / float(pBackBufferSurfaceDesc->Height);
	D3DXMatrixPerspectiveFovLH(
			&m_mProjection,
			m_fFov,
			m_fAspectRatio,
			m_fNearPlane,
			m_fFarPlane);

	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_mProjection);
	if (m_Sun)
		m_Sun->ResetMatrices();
}

void SpaceScene::OnResetDevice(IDirect3DDevice9* pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;

    const D3DSURFACE_DESC* pBackBufferSurfaceDesc = DXUTGetBackBufferSurfaceDesc();
	m_fAspectRatio = float(pBackBufferSurfaceDesc->Width) / float(pBackBufferSurfaceDesc->Height);
	D3DXMatrixPerspectiveFovLH(
			&m_mProjection,
			m_fFov,
			m_fAspectRatio,
			m_fNearPlane,
			m_fFarPlane);

	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_mProjection);

	// Switch to wireframe mode.
	//m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// Set Phong shading
	m_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_PHONG);

	//	Enable Lighting
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);

	//	Initialize and register a light
	D3DLIGHT9 dirLight = WL::InitDirectionalLight(		// Directional Light
		-1.0f, 0.0f, -1.0f,								// Direction
		WL::WHITE);										// Color

	m_pd3dDevice->SetLight(0,				// element in the light list to set, range is 0-maxlights
					&dirLight);				// address of the D3DLIGHT9 structure to set
	m_pd3dDevice->LightEnable(0, true);		// Enable light 0

	//	Enable automatic vertex normalization inside the pipeline
	//	Optimization: DirectX Normalization is not necessary if it is correctly performed in the game's custom classes
	//	It seems that up to this point normalization is correctly performed
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);

	//	Disable backface culling
	//m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Set Texture Filter States.
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//
	//	DO NOT DELETE THE STENCIL BUFFER COMMENTS. FOR FUTURE REFERENCE!
	//

	// Enable the stencil buffer
	//m_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, true);

	// Set the stencil buffer reference value to 1
	//m_pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0x1);

	// Set the stencil mask to hide the 16 leftmost bits
	//m_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0x0000ffff);

	// The stencil buffer is true when (ref & mask) > (value & mask)
	//m_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC,	D3DCMP_GREATER);

	// If the stencil test fails, zero the pixel value
	//m_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_ZERO);

	if (m_Sun)
		m_Sun->OnResetDevice();

	cameraPos = CameraPosition(0);

}

HRESULT SpaceScene::OnCreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
	HRESULT hr;

	m_pd3dDevice = pd3dDevice;

	for (int i = 0; i < m_NumberOfObjects; i++)
		if (m_Objects[i])
			m_Objects[i]->OnCreateDevice(m_pd3dDevice);

	if (m_Sun)
		hr = m_Sun->OnCreateDevice();

	return hr;
}

void SpaceScene::OnLostDevice()
{
	for (int i = 0; i < m_NumberOfObjects; i++)
		if (m_Objects[i])
			m_Objects[i]->OnLostDevice();

	if (m_Sun)
		m_Sun->OnLostDevice();
	
}

void SpaceScene::OnDestroyDevice()
{
	for (int i = 0; i < m_NumberOfObjects; i++)
		if (m_Objects[i])
			m_Objects[i]->OnDestroyDevice();

	if (m_Sun)
		m_Sun->OnDestroyDevice();

}


