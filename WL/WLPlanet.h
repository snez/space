#pragma once

#include "dxstdafx.h"
#include "..\XMesh.h"
#include "WL.h"

class Planet : public GeneralObject 
{
private:

	IDirect3DDevice9* m_pd3dDevice;
	XMesh* m_pMesh;

	// Effect Variables
	ID3DXEffect* m_pEffect;
	DWORD dwShaderFlags;
	D3DXHANDLE              m_hWorld;               // Handle to world matrix
	D3DXHANDLE              m_hView;                // Handle to view matrix
	D3DXHANDLE              m_hProj;                // Handle to projection matrix
	D3DXHANDLE				m_hLightDir;			// Handle to light direction vector3
	D3DXHANDLE				m_hTexture;				// Handle to a texture
	D3DXHANDLE				m_hThickness;			// Handle to glow effect thickness
	D3DXHANDLE				m_hTechnique;			// Handle to a shader technique
	D3DXHANDLE				m_hAmbientColor;		// The color of the planet atmosphere
	D3DXHANDLE				m_hBias;				// Bias for how much the atmosphere exceeds the 90 degree cutoff (0.0f-1.0f)
	float					m_fThickness;			// The thickness of the planet's atmosphere
	int						m_iDetail;				// The amount of layers to draw for the atmosphere

public:

	Planet(LPCWSTR xfile, LPCWSTR fxfile = NULL, float R = 0.5f, float G = 0.5f, float B = 0.5f, 
		   float thickness = 0.1f, int detail = 7, float Bias = 0.2f);

	~Planet();
	void Render();
	void OnResetDevice();
	void OnLostDevice();
	void OnDestroyDevice();
};