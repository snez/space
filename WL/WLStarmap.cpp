#include "WLStarmap.h"

Starmap::Starmap(	
				const float& FarPlane,			// The bigger, the better, but not larger than the far plane!
				const int& AmountOfStars,		// Number of total stars to be created
				const int& LowestIntensity,		// Lowest color value for star brightness, 0-200
				const int& HighestIntensity		// Highest color value for star brightness, 0-200
				)
{
	// Error checking
	if ((HighestIntensity <= 200) && (LowestIntensity >= 0) && (HighestIntensity >= LowestIntensity) && (AmountOfStars > 0) && (FarPlane > 1.0f))
	{
		countStars = AmountOfStars;

		// Temporary variables
		float x, y, z, h;
		int color = 0;
		double max_distance = pow(RAND_MAX,2);	// derived formula from pythagorean theorem + optimizations
		double distance = 0;

		// Create the starmap's vertex buffer
		Device->CreateVertexBuffer(
			AmountOfStars*sizeof(VertexPC),
			D3DUSAGE_WRITEONLY,
			0,
			D3DPOOL_MANAGED,
			&mVB, 0);

		if (mVB)	// Optimization: This check may be removed after development
		{
			VertexPC* v = 0;
			mVB->Lock(0, 0, (void**)&v, 0);

			//
			// Star Coordinates & Color generation algorithm
			//

			// Feed rand()
			srand(unsigned int(time(NULL)));	
			for (int index = 0; index < AmountOfStars; index++)
			{
				do
				{
					// Generate a random float in the range 0 .. RAND_MAX
					x = float(rand());				
					y = float(rand()/((rand()%17)+1));
					z = float(rand());
				
					distance = pow(double(x),2) + pow(double(y),2) + pow(double(z),2);

				} while (distance > max_distance);

				// Now distribute the coordinates to all the sides of the sphere
				if ((int(x) % 2) == 0) x = -x;
				if ((int(y) % 2) == 0) y = -y;
				if ((int(z) % 2) == 0) z = -z;

				// Normalize and scale by FarPlane
				h = sqrt(x*x + y*y + z*z)/(FarPlane-1.0f);
				x /= h;
				y /= h;
				z /= h;

				// Calculate a color
				color = LowestIntensity + (rand() % (HighestIntensity - LowestIntensity + 1));	// Color

				// Add position and color in the vertex buffer
				v[index] = VertexPC(x, y, z, D3DCOLOR_XRGB(color, color + (rand() % 15), color + (rand() % 55)));

			}

			mVB->Unlock();

		}

	}
	else
	{
		mVB = 0;
		countStars = 0;
	}
}

Starmap::~Starmap()
{
	if (mVB){
		mVB->Release();
		mVB = 0;
	}
}

void Starmap::draw(const D3DXVECTOR3& cameraPos)
{
	if ((mVB) && (countStars > 0))
	{
		// No fancy texture wrapping
		Device->SetRenderState(D3DRS_WRAP0, 0); 

		// Disable lighting
		Device->SetRenderState(D3DRS_LIGHTING, false);

		// Don't write starmap to the depth buffer; in this way, 
		// the starmap will never occlude goemtry, which it should not
		// since it is "infinitely" far away.
		Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
		Device->SetRenderState(D3DRS_ZENABLE, false);

		// Have starmap move with the camera, but _not_ rotate with camera.
		D3DXMATRIX W, Save;
		Device->GetTransform( D3DTS_WORLD, &Save );
		D3DXMatrixTranslation(&W, cameraPos.x, cameraPos.y, cameraPos.z);
		Device->SetTransform(D3DTS_WORLD, &W);

		// Switch to fixed pipe (Release the set shader if there is one).
		// Note that FX set shaders.
		Device->SetVertexShader(0);
		Device->SetPixelShader(0);

		Device->SetFVF(VertexPC::FVF);
		Device->SetStreamSource(0, mVB, 0, sizeof(VertexPC));
		Device->SetTexture(0, 0); // Disable textures

		// Draw the stars
		Device->DrawPrimitive(
			D3DPT_POINTLIST,
			0,					// Base Vertex Index
			countStars
			);

		// Restore render states.
		Device->SetTransform(D3DTS_WORLD, &Save);
		Device->SetRenderState(D3DRS_LIGHTING, true);
		Device->SetRenderState(D3DRS_ZWRITEENABLE, true);
		Device->SetRenderState(D3DRS_ZENABLE, true);

	}
}
				 
				 
				 
				 
				 
				 
				 
				 
				 
				 
				 
				 
				 

