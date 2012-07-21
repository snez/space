#include "WLVertex.h"

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;	// Position, Normal, Texture
const DWORD VertexPT::FVF = D3DFVF_XYZ | D3DFVF_TEX1;				// Position, Texture
const DWORD VertexPC::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;			// Position, Color

IDirect3DVertexDeclaration9* VertexPT::DECL      = 0;

//=====================================================
//	Vertex
//=====================================================
const Vertex Vertex::operator*(const float& m)
{
	return Vertex(_x * m, _y * m, _z * m, _nx, _ny, _nz);
}

Vertex& Vertex::operator*=(const float& m)
{
	_x *= m;
	_y *= m;
	_z *= m;
	return *this;
}

// You can normalize this Vertex
Vertex& Vertex::normalize()
{
	float magnitude = sqrt(pow(_x,2)+pow(_y,2)+pow(_z,2));
	_x /= magnitude;
	_y /= magnitude;
	_z /= magnitude;
	_nx = _x;
	_ny = _y;
	_nz = _z;
	return *this;
}

//=====================================================
//	SphereVertex
//=====================================================
const SphereVertex SphereVertex::operator+(const SphereVertex& vertex)
{
	// Compute new Vertex position
	float x = _x + vertex._x;
	float y = _y + vertex._y;
	float z = _z + vertex._z;

	return SphereVertex(x, y, z);	
}

SphereVertex& SphereVertex::operator+=(const SphereVertex& vertex)
{
	_x += vertex._x;
	_y += vertex._y;
	_z += vertex._z;

	// Compute normal
	float magnitude = sqrt(pow(_x,2)+pow(_y,2)+pow(_z,2));
	_nx = _x / magnitude;
	_ny = _y / magnitude;
	_nz = _z / magnitude;
	// Optimization: Normal computation is also performed in normalize() so these can be removed if
	// only used in CreateGeoSphere

	return *this;
}

const SphereVertex SphereVertex::operator-(const SphereVertex& vertex)
{
	// Compute new Vertex position
	float x = _x - vertex._x;
	float y = _y - vertex._y;
	float z = _z - vertex._z;

	return SphereVertex(x, y, z);	
}

SphereVertex& SphereVertex::operator-=(const SphereVertex& vertex)
{
	_x -= vertex._x;
	_y -= vertex._y;
	_z -= vertex._z;

	// Compute normal
	float magnitude = sqrt(pow(_x,2)+pow(_y,2)+pow(_z,2)); 
	_nx = _x / magnitude;
	_ny = _y / magnitude;
	_nz = _z / magnitude;
	// Optimization: Normal computation is also performed in normalize() so these can be removed if
	// only used in CreateGeoSphere

	return *this;
}

// Constructor for sphere vertices only
SphereVertex::SphereVertex(const float& x, const float& y, const float& z)
{
	_x = x;
	_y = y;
	_z = z;

	// Compute normals
	float magnitude = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
	_nx = x / magnitude;
	_ny = y / magnitude;
	_nz = z / magnitude;
	// Optimization: Normal computation is also performed in normalize() which is always performed
	// upon vertex creation in WLCreateGeoSphere

}

//=====================================================
//	VertexPT
//=====================================================

// Default Constructor
VertexPT::VertexPT()
: pos(0.0f, 0.0f, 0.0f), tex0(0.0f, 0.0f)
{
}

// Constructor
VertexPT::VertexPT(float x, float y, float z, float u, float v)
: pos(x, y, z), tex0(u, v)
{
}

// Constructor
VertexPT::VertexPT(const D3DXVECTOR3& p, const D3DXVECTOR2& uv)
: pos(p), tex0(uv)
{
}
