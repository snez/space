#include "WLPlanet.h"

// This function is used in the class and is not meant to be visible outside of it
D3DXVECTOR4 operator*(const D3DXVECTOR4& v, const D3DXMATRIX& m){
	return D3DXVECTOR4(	v.x * m._11 + v.y * m._21 + v.z * m._31 + v.w * m._41,
					v.x * m._12 + v.y * m._22 + v.z * m._32 + v.w * m._42,
					v.x * m._13 + v.y * m._23 + v.z * m._33 + v.w * m._43,
					v.x * m._14 + v.y * m._24 + v.z * m._34 + v.w * m._44);
};


Planet::Planet(LPCWSTR xfile, LPCWSTR fxfile /* = NULL */, 
			   float R /* = 0.5f */, float G /* = 0.5f */, float B /* = 0.5f */, 
			   float thickness /* = 0.1f */, int detail /* = 7 */, float Bias /* = 0.2f */)
{
	HRESULT hr;
	m_pd3dDevice = DXUTGetD3DDevice();
	m_pMesh = new XMesh();
	hr = m_pMesh->LoadFile(xfile);
	if (FAILED(hr))
	{
		delete m_pMesh;
		m_pMesh = NULL;
		DXUTTrace(__FILE__,(DWORD)__LINE__, hr, L"Failed to create Planet mesh.", true);
	}	

	//	Create the glow effect
	m_pEffect = NULL;
	dwShaderFlags = 0;
	if (fxfile != NULL)
	{
		m_hWorld = NULL;
		m_hView = NULL;
		m_hProj = NULL;
		m_hLightDir = NULL;

		//dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
		//dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
		//dwShaderFlags |= D3DXSHADER_NO_PRESHADER;
		dwShaderFlags |= D3DXSHADER_DEBUG;

		// Read the D3DX effect file
		hr = D3DXCreateEffectFromFile( 
				m_pd3dDevice, 
				fxfile, 
				NULL, // CONST D3DXMACRO* pDefines,
				NULL, // LPD3DXINCLUDE pInclude,
				dwShaderFlags, 
				NULL, // Shared Pool
				&m_pEffect, 
				NULL );

		if (FAILED(hr))
		{
			SAFE_RELEASE(m_pEffect);
			DXUTTrace(__FILE__,(DWORD)__LINE__, hr, L"Failed to create Planet effect from file.", true);
		} else {
			m_hTechnique = m_pEffect->GetTechniqueByName("TGlowAndTexture");
			m_hWorld = m_pEffect->GetParameterByName( 0, "World" );
			m_hView = m_pEffect->GetParameterByName( 0, "View" );
			m_hProj = m_pEffect->GetParameterByName( 0, "Projection" );
			m_hLightDir = m_pEffect->GetParameterByName( 0, "LightDir" );
			m_hTexture = m_pEffect->GetParameterByName( 0, "Tex0" );
			m_hThickness = m_pEffect->GetParameterByName( 0, "GlowThickness" );
			m_hAmbientColor = m_pEffect->GetParameterByName( 0, "GlowAmbient" );
			m_hBias = m_pEffect->GetParameterByName( 0, "Bias" );

			if (detail < 1) detail = 1;
			m_iDetail = detail;
			D3DXVECTOR4 ambientColor(R/float(m_iDetail),G/float(m_iDetail),B/float(m_iDetail),1.0f);

			if (Bias > 1.0f) Bias = 1.0f;
			m_fThickness = thickness;

			// Update the effect's variables 
			m_pEffect->SetTexture( m_hTexture, m_pMesh->GetTexture(0));
			m_pEffect->SetVector( m_hAmbientColor, &ambientColor);
			m_pEffect->SetFloat( m_hBias, Bias);
			
		}
	}

};

void Planet::Render()
{
	HRESULT hr;
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_mWorldMatrix);

	if (m_pEffect)
	{
        UINT iPass, cPasses;
	    D3DXMATRIX m_mViewMatrix;
		D3DXMATRIX m_mProjMatrix;

		m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

		float factor = 0.0f;
		float tmp_thickness = m_fThickness;
		
		m_pd3dDevice->GetTransform(D3DTS_VIEW, &m_mViewMatrix);
		m_pd3dDevice->GetTransform(D3DTS_PROJECTION, &m_mProjMatrix);

		V( m_pEffect->SetMatrix( m_hWorld, &m_mWorldMatrix ) );			
		V( m_pEffect->SetMatrix( m_hView, &m_mViewMatrix) );
		V( m_pEffect->SetMatrix( m_hProj, &m_mProjMatrix ));

		// Get the light direction
		D3DLIGHT9 dirLight; 
		D3DXVECTOR4 ld(0.0f, 0.0f, 0.0f, 0.0f); 
		m_pd3dDevice->GetLight(0,&dirLight); 
		ld.x = dirLight.Direction.x; 
		ld.y = dirLight.Direction.y; 
		ld.z = dirLight.Direction.z; 
		static bool once = true; 

		// Transform the light direction before passing it to the shader
		static D3DXMATRIX inverse; 
		if (once) { 
			D3DXVECTOR3 position( 0.0f, 0.0f, -1.0f ); 
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f); 
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); 
			D3DXMatrixLookAtLH(&inverse, &position, &target, &up); 
			D3DXMatrixInverse(&inverse, 0, &inverse); 
			once = false; 
		} 
		ld = ld * m_mViewMatrix * inverse;
		V( m_pEffect->SetVector( m_hLightDir, &ld ));

		// Set the initial thickness
		factor = m_fThickness/float(m_iDetail);
		V( m_pEffect->SetFloat( m_hThickness, factor ));

		// Change the technique to add multiple glowing layers
		tmp_thickness = factor;

		V( m_pEffect->SetTechnique("TGlowOnly") );
		for (int i = 1; i <= m_iDetail; i++)
		{
			if (i == m_iDetail)
			{
				m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
				m_pMesh->Render();
				m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
			}

			m_pEffect->Begin(&cPasses, 0);

			V( m_pEffect->SetFloat( m_hThickness, tmp_thickness ));

			for (iPass = 0; iPass < cPasses; iPass++)
			{
				m_pEffect->BeginPass(iPass);
				// Only call CommitChanges if any state changes have happened
				// after BeginPass is called
				//m_pEffect->CommitChanges();

				// Render the mesh with the applied technique
				m_pMesh->Render();

				m_pEffect->EndPass();
			}
			m_pEffect->End();
			tmp_thickness += factor;
		}

		m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	}
	else
	{
		if (m_pMesh) 
		{
			m_pMesh->Render();
		}		
	}

};

void Planet::OnResetDevice()
{
    if( m_pEffect )
        m_pEffect->OnResetDevice();
};

void Planet::OnLostDevice()
{
    if( m_pEffect )
        m_pEffect->OnLostDevice();
};

void Planet::OnDestroyDevice()
{
    SAFE_RELEASE(m_pEffect);
}

Planet::~Planet()
{
	SAFE_RELEASE(m_pEffect);
	SAFE_DELETE(m_pMesh);
}