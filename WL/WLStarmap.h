#ifndef	_WLSTARMAP_H_
#define	_WLSTARMAP_H_

#include <d3dx9.h>
#include <time.h>
#include <math.h>
#include <cstdlib>
#include "WLVertex.h"

// Global variable used in the drawing
extern IDirect3DDevice9*	Device;

// The starmap object
class Starmap
{
public:
	Starmap(	const float& FarPlane,			// The far plane that is set in the projection matrix
				const int& AmountOfStars,		// Number of total stars to be created
												// The maximum number of primitives allowed is determined by 
												// checking the MaxPrimitiveCount member of the D3DCAPS9 structure. 
				const int& LowestIntensity,		// Lowest color value for star brightness, 0-200
				const int& HighestIntensity		// Highest color value for star brightness, 0-200
			);
	~Starmap();
	
	// This function draws the Starmap in the scene
	void draw(const D3DXVECTOR3& cameraPos);

private:
	IDirect3DVertexBuffer9* mVB;
	int countStars;					// If the starmap fails to initialize, this is set to 0

};

#endif	// _WLSTARMAP_H_