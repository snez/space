#pragma once

#include "WLGeneralObject.h"
#include "..\Particle.h"

class Comet : public GeneralObject 
{
protected:

	ParticleSystem* partSys;

public:

	Comet(LPCWSTR xfile);
	~Comet();
	
	void Render();
	void Update(float timeDelta);
};