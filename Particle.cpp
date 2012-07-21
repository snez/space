#include "dxstdafx.h"
#include ".\particle.h"

const DWORD ParticleVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;	// Position, Color, Texture

Particle::Particle()
{
	alive = false;
	size = sizeDelta = life = 0;
	color = colorDelta = D3DXCOLOR(0,0,0,0);
	velocity = position = D3DXVECTOR3(0,0,0);	
}

void Particle::Init(D3DXVECTOR3& position,D3DXVECTOR3& velocity,D3DXCOLOR& color,D3DXCOLOR& colorDelta,float life,float size,float sizeDelta)
{
	alive = true;
	this->position = position;
	this->velocity = velocity;
	this->color = color;
	this->colorDelta = colorDelta;
	this->life = life;
	this->size = size;
	this->sizeDelta = sizeDelta;
}

void Particle::Update(float timeDelta)
{
	if (!alive)
		return;

	if ( (life -= timeDelta) <= 0)
	{
		alive = false;
		return;
	}

	position += velocity * timeDelta;
	color += colorDelta * timeDelta; 
	size += sizeDelta * timeDelta;
}


//--------------------------------------------------------------------------------------//


ParticleSystem::ParticleSystem(D3DXVECTOR3 position, float emPitch, float emYaw, float pitchVar, float yawVar, int numParticles)
{
	emitter = position;
	particleCount = min( MAX_PARTICLES, numParticles );
	
	// pitch must be between -90 and 90 degrees
	pitch = emPitch * DEG_TO_RAD;
	Limit(&pitch, -90.0f * DEG_TO_RAD, 90.0f * DEG_TO_RAD);

	// pitch must be between -360 and 360 degrees
	yaw = emYaw * DEG_TO_RAD;
	Limit(&yaw,  -360.0f * DEG_TO_RAD, 360.0f * DEG_TO_RAD);

	// pitchVariation must be between 0 and 180 degrees
	pitchVariation = pitchVar * DEG_TO_RAD;
	Limit(&pitchVariation, 0, 180.0f * DEG_TO_RAD);

	// yawVariation must be between 0 and 360 degrees
	yawVariation = yawVar * DEG_TO_RAD; 
	Limit(&yawVariation, 0, 360.0f * DEG_TO_RAD);

	particleBuffer = new Particle*[particleCount];
	for (int i = 0; i < particleCount; i++)
		particleBuffer[i] = new Particle();

	aliveParticles = 0;
	colorStart = colorEnd = colorStartVar = colorEndVar = D3DXCOLOR(0,0,0,0);
	maxLife = minLife = 0;
	minVelocity = maxVelocity = 0;
	particleSize = particleSizeVar = 0;
	emitting = false;

	device = DXUTGetD3DDevice();
	pTexture = NULL;

	// create the vertex buffer
	device->CreateVertexBuffer(particleCount * (4 * sizeof( ParticleVertex )), 0, ParticleVertex::FVF, D3DPOOL_MANAGED, &vb, 0);
	srand( (unsigned)time( NULL ) );
}

ParticleSystem::~ParticleSystem()
{
	for(int i = 0; i < particleCount; i++)
		SAFE_DELETE(particleBuffer[i]);
	SAFE_DELETE_ARRAY(particleBuffer);

	SAFE_RELEASE(vb);
	SAFE_RELEASE(pTexture);
}

void ParticleSystem::SetColor(const D3DXCOLOR& start, const D3DXCOLOR& startVar, const D3DXCOLOR& end, const D3DXCOLOR& endVar)
{
	colorStart = start;
	colorEnd = end;
	colorStartVar = startVar;
	colorEndVar = endVar;
}

void ParticleSystem::SetLife(float min,float max)
{
	minLife = min;
	maxLife = max;
}

void ParticleSystem::SetSize(float startSize,float endSize)
{
	particleSize = startSize;
	particleSizeVar = (endSize - startSize);
}

void ParticleSystem::SetTexture(LPCWSTR name)
{
	if (FAILED( D3DXCreateTextureFromFile(device, name, &pTexture) ))
		SAFE_RELEASE(pTexture);
}

void ParticleSystem::SetPosition(D3DXVECTOR3& pos)
{
	emitter = pos;
}

void ParticleSystem::Start() 
{ 
	emitting = true; 
}

void ParticleSystem::Stop()
{
	emitting = false;
}

void ParticleSystem::SetVelocity(float min, float max)
{
	minVelocity = min;
	maxVelocity = max;
}

inline float ParticleSystem::GetRandomNum(float min, float max)
{
	return (( (float) rand() / (float) RAND_MAX ) * (max - min)) + min;
}

inline void ParticleSystem::Limit(float* x, float min, float max)
{
	*x = (*x < min) ? min : (*x < max) ? *x : max;
}

void ParticleSystem::RotationToDirection(float pitch,float yaw,D3DXVECTOR3& direction)
{
	direction.x = -sin(yaw) * cos(pitch);
	direction.y = sin(pitch);
	direction.z = cos(pitch) * cos(yaw);
}

void ParticleSystem::InitParticle(Particle* p)
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	D3DXCOLOR colorS;
	D3DXCOLOR colorE;
	D3DXCOLOR colorDelta;

	position = emitter;

	float partPitch = (GetRandomNum(-0.5f,0.5f) * pitchVariation) + pitch;
	float partYaw = (GetRandomNum(-0.5f,0.5f) * yawVariation) + yaw;

	RotationToDirection(partPitch,partYaw,velocity);

	float r = GetRandomNum(minVelocity,maxVelocity);
	velocity *= r;

	float life = GetRandomNum(minLife,maxLife);

	colorS = colorStart + colorStartVar * GetRandomNum();
	colorE = colorEnd + colorEndVar * GetRandomNum();

	Limit(&colorS.r);
	Limit(&colorS.g);
	Limit(&colorS.b);
	Limit(&colorS.a);

	Limit(&colorE.r);
	Limit(&colorE.g);
	Limit(&colorE.b);
	Limit(&colorE.a);

	colorDelta = (colorE - colorS)  / life;

	p->Init(position,velocity,colorS,colorDelta,life,particleSize,particleSizeVar / life);
}

void ParticleSystem::Update(float timeDelta)
{
	if (!emitting && aliveParticles <= 0)
		return;

	aliveParticles = 0;
	for (int i = 0; i < particleCount; i++)
	{
		if (particleBuffer[i]->IsAlive())
			particleBuffer[i]->Update(timeDelta);

		if (particleBuffer[i]->IsAlive())
			aliveParticles++;
		else if (emitting)
		{
			InitParticle(particleBuffer[i]);
			aliveParticles++;
		}
	}
}

void ParticleSystem::Render()
{
	if (!emitting && aliveParticles <= 0)
		return;

	device->SetVertexShader(NULL);
	device->SetPixelShader(NULL);
	
	DWORD states[5];
	// save current render states
	device->GetRenderState(D3DRS_LIGHTING, &states[0]);
	device->GetRenderState(D3DRS_ALPHABLENDENABLE, &states[1]);
	device->GetRenderState(D3DRS_SRCBLEND, &states[2]);
	device->GetRenderState(D3DRS_DESTBLEND, &states[3]);
	device->GetRenderState(D3DRS_ZWRITEENABLE, &states[4]);
	
	// set new render states
	device->SetRenderState(D3DRS_LIGHTING, false);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
	device->SetRenderState(D3DRS_ZWRITEENABLE, false);

	device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); 
    device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); 
	device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

	// set vertex buffer
	device->SetStreamSource(0, vb, 0, sizeof(ParticleVertex));
	device->SetFVF(ParticleVertex::FVF);
	
	ParticleVertex* vertices;
	vb->Lock(0, 0, (void**)&vertices, 0); // lock the entire buffer
	
	D3DXMATRIX matViewOld;
    device->GetTransform(D3DTS_VIEW, &matViewOld);
	D3DXMATRIX IdentityMatrix;
	D3DXMatrixIdentity(&IdentityMatrix);
    device->SetTransform(D3DTS_VIEW, &IdentityMatrix);
	device->SetTransform(D3DTS_WORLD, &IdentityMatrix);

	for (int i = 0, j = 0; i < particleCount; i++)
	{
		if (particleBuffer[i]->IsAlive())
		{
			D3DXCOLOR color =  particleBuffer[i]->GetColor();
			D3DCOLOR c = (DWORD) color;
			float halfParticleSize = particleBuffer[i]->GetSize() / 2.0f;
			D3DXVECTOR4 tPos; // Transformed position

			// Apply the view matrix to the position vector
			D3DXVec3Transform(&tPos,&particleBuffer[i]->GetPosition(),&matViewOld);

			// create a textured quad
			vertices[4*j]   = ParticleVertex( tPos.x - halfParticleSize,tPos.y - halfParticleSize,tPos.z,c,0.0f,1.0f);
			vertices[4*j+1] = ParticleVertex( tPos.x - halfParticleSize,tPos.y + halfParticleSize,tPos.z,c,0.0f,0.0f);
			vertices[4*j+2] = ParticleVertex( tPos.x + halfParticleSize,tPos.y - halfParticleSize,tPos.z,c,1.0f,1.0f);
			vertices[4*j+3] = ParticleVertex( tPos.x + halfParticleSize,tPos.y + halfParticleSize,tPos.z,c,1.0f,0.0f);
			j++;
		}
	}

	vb->Unlock(); // unlock when done accessing the buffer

	device->SetTexture(0, pTexture);

	for (int i = 0; i < aliveParticles; i++)
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4*i,2);

	device->SetTransform(D3DTS_VIEW, &matViewOld);

	// restore render states
	device->SetRenderState(D3DRS_LIGHTING, states[0]);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, states[1]);
	device->SetRenderState(D3DRS_SRCBLEND, states[2]);
	device->SetRenderState(D3DRS_DESTBLEND, states[3]);
	device->SetRenderState(D3DRS_ZWRITEENABLE, states[4]);
}