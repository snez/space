// This is a struct that represents a vertex with a color property to be placed in a vertex buffer
// Author: snez, 26 August 2006

#ifndef	__WLVERTEX_H__
#define __WLVERTEX_H__

#include <d3d9.h>
#include <d3dx9.h>

struct Vertex {
	float _x, _y, _z;		// Vertex Position
	float _nx, _ny, _nz;	// Vertex Normal
	float _Tu, _Tv;			// Texture Coordinates
	static const DWORD FVF;	// Flexible Vertex Format

	// Default Constructor
	Vertex(){
		_x = _y = _z = 0.0f;
		_nx = _ny = _nz = 0.0f;
	}

	// Constructor
	Vertex(const float& x, const float& y, const float& z, const float& nx, const float& ny, const float& nz) 
	{
		_x = x;
		_y = y;
		_z = z;
		_nx = nx;
		_ny = ny;
		_nz = nz;
	}

	// Copy Constructor
	Vertex(const Vertex& v) 
	{
		_x = v._x;
		_y = v._y;
		_z = v._z;
		_nx = v._nx;
		_ny = v._ny;
		_nz = v._nz;
	}

	// You can scale this Vertex
	const Vertex operator*(const float& m);
	Vertex& operator*=(const float& m);

	// You can normalize this Vertex
	Vertex& normalize();
};

//
//	Specialize the Vertex to a SphereVertex, by adding a couple of sensible sphere operations
//
struct SphereVertex : public Vertex {

	// Default Constructor
	SphereVertex(){
		_x = _y = _z = 0.0f;
		_nx = _ny = _nz = 0.0f;
	}

	// Constructor for sphere vertices only
	SphereVertex(const float& x, const float& y, const float& z);

	// You can add/subtract to this Vertex
	const SphereVertex operator+(const SphereVertex& vertex);
	SphereVertex& operator+=(const SphereVertex& vertex);
	const SphereVertex operator-(const SphereVertex& vertex);
	SphereVertex& operator-=(const SphereVertex& vertex);
};

//
// A vertex that contains only position and texture
//
struct VertexPT
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex0;

	VertexPT();
	VertexPT(float x, float y, float z, float u, float v);
	VertexPT(const D3DXVECTOR3& p, const D3DXVECTOR2& uv);

	static IDirect3DVertexDeclaration9* DECL;	// Shader Vertex Declaration
	static const DWORD FVF;						// Flexible Vertex Format

};

//
// A color vertex that shall only be used when lighting is disabled in the pipeline
//

struct VertexPC {
	float _x, _y, _z;
	D3DCOLOR c;
	static const DWORD FVF;

	// Default Constructor
	VertexPC(){
		_x = _y = _z = 0.0f;
		c = (D3DCOLOR)0;
	}

	// Constructor
	VertexPC(const float& x, const float& y, const float& z, const D3DCOLOR& color) 
	{
		_x = x;
		_y = y;
		_z = z;
		c = color;
	}

	// Constructor
	VertexPC(const VertexPC& v, const D3DCOLOR& color) 
	{
		_x = v._x;
		_y = v._y;
		_z = v._z;
		c = color;
	}

	// Copy Constructor
	VertexPC(const VertexPC& v) 
	{
		_x = v._x;
		_y = v._y;
		_z = v._z;
		c = v.c ;
	}
	
};
#endif	// __WLVERTEX_H__