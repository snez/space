#pragma once

#include "WLGeneralObject.h"
#include "..\Particle.h"

class Spaceship : public GeneralObject 
{
protected:

	ParticleSystem* partSys;
	ParticleSystem* partDust;

public:

	Spaceship(LPCWSTR xfile);
	~Spaceship();

	void Render();
	void Update(float timeDelta);
};