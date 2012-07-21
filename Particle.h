#pragma once

#include <time.h>

class ParticleSystem; // forward declaration

class Particle
{
private:
	
	D3DXCOLOR color;		// Particle color
	D3DXCOLOR colorDelta;	// How much to change the color in one time slice
	D3DXVECTOR3 position;	// Particle position
	D3DXVECTOR3 velocity;	// Particle velocity
	float life;				// Particle remaining life
	float size;				// Current particle size
	float sizeDelta;		// How much to change the size in one time slice
	bool alive;				// Is the particle alive?

public:

	Particle();
	~Particle() {};
	void Init(D3DXVECTOR3& position,D3DXVECTOR3& velocity,D3DXCOLOR& color,D3DXCOLOR& colorDelta,float life,float size,float sizeDelta);
	void Update(float timeDelta);
	inline D3DXVECTOR3& GetPosition() { return position; }
	inline D3DXCOLOR& GetColor() { return color; }
	inline float GetSize() { return size; }
	inline bool IsAlive() { return alive; }
};


//--------------------------------------------------------------------------------------//


#define DEG_TO_RAD ( D3DX_PI/180.f ) // convert from degrees to radians
#define RAD_TO_DEG ( 180.f/D3DX_PI ) // convert from radians to degrees

class ParticleSystem
{
private:

	static const MAX_PARTICLES = 1000;

	D3DXVECTOR3 emitter;			// position of the emitter
	int particleCount;
	int aliveParticles;				// number of alive particles

	float pitch;					// pitch of the emitter
	float yaw;						// yaw of the emitter
	float pitchVariation;			// spread in y axis , around x axis
	float yawVariation;				// spread in x axis , around y axis

	float particleSize;				// starting particle size
	float particleSizeVar;			// how much the size will change

	float maxLife;					// maximum life for a particle
	float minLife;					// minimum life for a particle
	float maxVelocity;				// maximum velocity for a particle
	float minVelocity;				// minimum velocity for a particle
	
	Particle** particleBuffer;		// the particle buffer
	D3DXCOLOR colorStart;			// start color
	D3DXCOLOR colorEnd;				// end color
	D3DXCOLOR colorStartVar;		// start color
	D3DXCOLOR colorEndVar;			// end color

	LPDIRECT3DTEXTURE9 pTexture;	// the texture of the particles
	IDirect3DVertexBuffer9* vb;		// our vertex buffer

	IDirect3DDevice9* device;		// pointer to the device
	bool emitting;					// Is the emitter working?

public:

	ParticleSystem(D3DXVECTOR3 position, float emPitch, float emYaw, float pitchVar, float yawVar, int numParticles);
	~ParticleSystem();
	void Render();
	void Start();
	void Stop();
	bool IsEmitting() { return emitting; }
	void SetColor(const D3DXCOLOR& start, const D3DXCOLOR& startVar, const D3DXCOLOR& end, const D3DXCOLOR& endVar);
	void SetLife(float min,float max);
	void SetSize(float startSize,float endSize);
	void SetVelocity(float min,float max);
	void SetTexture(LPCWSTR name);
	void SetPosition(D3DXVECTOR3& pos);
	inline float GetRandomNum(float min = 0.0f, float max = 1.0f);
	inline void Limit(float* x, float min = 0.0f, float max = 1.0f);
	void RotationToDirection(float pitch,float yaw,D3DXVECTOR3& direction);
	void Update(float timeDelta);
	void InitParticle(Particle* p);
};


//--------------------------------------------------------------------------------------//


struct ParticleVertex 
{
	float x, y, z;			// Vertex Position
	D3DCOLOR c;				// Vertex Color
	float u, v;				// Texture Coordinates
	static const DWORD FVF;	// Flexible Vertex Format

	ParticleVertex(float x,float y,float z,D3DCOLOR c, float u, float v)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->c = c;
		this->u = u;
		this->v = v;
	}

	ParticleVertex(const ParticleVertex& v) 
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->c = v.c;
		this->u = v.u;
		this->v = v.v;
	}
};
