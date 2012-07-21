#pragma once

class XMesh
{
private:

	LPD3DXMESH              pMesh;			// Our mesh object in sysmem
	D3DMATERIAL9*           pMaterials;		// Materials for our mesh
	LPDIRECT3DTEXTURE9*     ppTextures;		// Textures for our mesh
	DWORD                   dwNumMaterials; // Number of mesh materials

public:

	XMesh();
	~XMesh();
	void Render();
	LPDIRECT3DTEXTURE9 GetTexture(DWORD num);
	HRESULT LoadFile(LPCWSTR fileName);
};
