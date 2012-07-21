#include "dxstdafx.h"
#include ".\xmesh.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
XMesh::XMesh()
{
	pMesh = NULL;
	pMaterials = NULL;
	ppTextures = NULL;
	dwNumMaterials = 0L;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
XMesh::~XMesh()
{
    if (pMaterials != NULL) 
        delete[] pMaterials;

    if (ppTextures)
    {
        for( DWORD i = 0; i < dwNumMaterials; i++ )
        {
			SAFE_RELEASE(ppTextures[i]);
        }
        delete[] ppTextures;
    }

	SAFE_RELEASE(pMesh);
}

//-----------------------------------------------------------------------------
// Return a texture
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 XMesh::GetTexture(DWORD num)
{
	if (num < dwNumMaterials)
		return ppTextures[num];
	else
		return NULL;
}

//-----------------------------------------------------------------------------
// Render the object
//-----------------------------------------------------------------------------
void XMesh::Render()
{
	IDirect3DDevice9* device = DXUTGetD3DDevice();

	for (DWORD i = 0; i < dwNumMaterials; i++ )
    {
        // Set the material and texture for this subset
        device->SetMaterial(&pMaterials[i]);
        device->SetTexture(0, ppTextures[i]);
    
        // Draw the mesh subset
        pMesh->DrawSubset(i);
    }
}

//-----------------------------------------------------------------------------
// Load the mesh and build the material and texture arrays
//-----------------------------------------------------------------------------
HRESULT XMesh::LoadFile(LPCWSTR fileName)
{
	IDirect3DDevice9* device = DXUTGetD3DDevice();
    LPD3DXBUFFER pMaterialBuffer;

    // Load the mesh from the specified file
    if (FAILED (D3DXLoadMeshFromX(fileName, D3DXMESH_SYSTEMMEM, device, NULL, 
                                  &pMaterialBuffer, NULL, &dwNumMaterials, &pMesh)) )
    {
		WCHAR outMsg[MAX_PATH] = L"Could not find ";
		StringCchCatW( outMsg, MAX_PATH, fileName );
        MessageBox(NULL, outMsg, L"Error", MB_OK | MB_ICONSTOP);
        return E_FAIL;
    }

    // We need to extract the material properties and texture names from the pMaterialBuffer
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*) pMaterialBuffer->GetBufferPointer();
    pMaterials = new D3DMATERIAL9[dwNumMaterials];

    if ( pMaterials == NULL )
        return E_OUTOFMEMORY;

    ppTextures  = new LPDIRECT3DTEXTURE9[dwNumMaterials];
    
	if( ppTextures == NULL )
        return E_OUTOFMEMORY;

    for ( DWORD i=0; i < dwNumMaterials; i++ )
    {
        // Copy the material
        pMaterials[i] = d3dxMaterials[i].MatD3D;

        WCHAR textureName[MAX_PATH];
        MultiByteToWideChar( CP_ACP, 0,d3dxMaterials[i].pTextureFilename, -1, textureName, MAX_PATH );

        // Set the ambient color for the material (D3DX does not do this)
        pMaterials[i].Ambient = pMaterials[i].Diffuse;

        ppTextures[i] = NULL;
        if ( d3dxMaterials[i].pTextureFilename != NULL && lstrlen(textureName) > 0 )
        {
            // Create the texture
            if ( FAILED (D3DXCreateTextureFromFile( device, textureName, &ppTextures[i])) )
                    MessageBox(NULL, L"Could not find texture map", L"Error", MB_OK | MB_ICONSTOP);
        }
    }
	
	if ( !(pMesh->GetFVF() & D3DFVF_NORMAL) ) // does the mesh have a D3DFVF_NORMAL in its vertex format?
	{
		ID3DXMesh* pTempMesh = 0;
		// clone a new mesh and add D3DFVF_NORMAL to its format
		pMesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, pMesh->GetFVF() | D3DFVF_NORMAL, device, &pTempMesh);
		D3DXComputeNormals( pTempMesh, 0 ); // compute the normals
		pMesh->Release(); // get rid of the old mesh
		pMesh = pTempMesh; // save the new mesh with normals
	}

    // Done with the material buffer
    pMaterialBuffer->Release();

    return S_OK;
}