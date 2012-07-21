#ifndef _WLHDRSUN_H_
#define _WLHDRSUN_H_

#include "dxstdafx.h"
#include "glaredefd3d.h"
#include "WL.h"

//-----------------------------------------------------------------------------
// Constants and custom types
//-----------------------------------------------------------------------------
#define MAX_SAMPLES           16      // Maximum number of texture grabs

#define NUM_LIGHTS            2       // Number of lights in the scene

#define EMISSIVE_COEFFICIENT  39.78f  // Emissive color multiplier for each lumen
                                      // of light intensity                                    
#define NUM_TONEMAP_TEXTURES  4       // Number of stages in the 4x4 down-scaling 
                                      // of average luminance textures
#define NUM_STAR_TEXTURES     12      // Number of textures used for the star
                                      // post-processing effect
#define NUM_BLOOM_TEXTURES    3       // Number of textures used for the bloom
                                      // post-processing effect
                                    
// Texture coordinate rectangle
struct CoordRect
{
    float fLeftU, fTopV;
    float fRightU, fBottomV;
};


// Screen quad vertex format
struct ScreenVertex
{
    D3DXVECTOR4 p; // position
    D3DXVECTOR2 t; // texture coordinate

    static const DWORD FVF;
};

class HDRSun 
{
public:
	//
	//	Constructor
	//
	HDRSun(	float x, float y, float z,					// The World Coordinates of the sun
			float radius, int stacks, int slices,		// Sun's sphere geometry info
			D3DXMATRIX mProjection,						// Camera Projection Matrix, will be set in the shader
			float BloomScale = 3.0f,					// Scale of the bloom effect
			float StarScale = 0.5f,						// Scale of the star effect
			int LightIntensity = 52,					// Light intensity of the sun, must be 0..100
			float BloomIntensity = 28.0f);				// Intensity for the bloom effect

	//
	//	Destructor
	//
	~HDRSun();

	//
	//	Render the sun
	//
	void Draw(D3DXMATRIX mView);

	//
	//	Handle device changes
	//
	HRESULT OnCreateDevice();
	HRESULT OnResetDevice();
	void OnLostDevice();
	void OnDestroyDevice();
	void ResetMatrices();

private:
	//--------------------------------------------------------------------------------------
	// Private member variables
	//--------------------------------------------------------------------------------------
	IDirect3DDevice9*   m_pd3dDevice;					// D3D Device object

	ID3DXEffect*        m_pEffect;						// D3DX effect interface
	D3DFORMAT           m_LuminanceFormat;				// Format to use for luminance map

	PDIRECT3DSURFACE9	m_pFloatMSRT;					// Multi-Sample float render target
	PDIRECT3DSURFACE9	m_pFloatMSDS;					// Depth Stencil surface for the float RT
	PDIRECT3DTEXTURE9	m_pTexScene;					// HDR render target containing the scene
	PDIRECT3DTEXTURE9	m_pTexSceneScaled;				// Scaled copy of the HDR scene
	PDIRECT3DTEXTURE9	m_pTexBrightPass;				// Bright-pass filtered copy of the scene
	PDIRECT3DTEXTURE9	m_pTexAdaptedLuminanceCur;		// The luminance that the user is currenly adapted to
	PDIRECT3DTEXTURE9	m_pTexAdaptedLuminanceLast;		// The luminance that the user is currenly adapted to
	PDIRECT3DTEXTURE9	m_pTexStarSource;				// Star effect source texture
	PDIRECT3DTEXTURE9	m_pTexBloomSource;				// Bloom effect source texture


	PDIRECT3DTEXTURE9	m_apTexBloom[NUM_BLOOM_TEXTURES];		// Blooming effect working textures
	PDIRECT3DTEXTURE9	m_apTexStar[NUM_STAR_TEXTURES];			// Star effect working textures
	PDIRECT3DTEXTURE9	m_apTexToneMap[NUM_TONEMAP_TEXTURES];	// Log average luminance samples 
																// from the HDR render target

	LPD3DXMESH			m_pmeshSphere;					// Representation of point light

	CGlareDef			m_GlareDef;						// Glare defintion
	EGLARELIBTYPE		m_eGlareType;					// Enumerated glare type

	D3DXVECTOR4			m_avLightPosition;				// Light positions in world space
	D3DXVECTOR4			m_avLightIntensity;				// Light floating point intensities
	int					m_nLightLogIntensity;			// Light intensities on a log scale
	int					m_nLightMantissa;				// Mantissa of the light intensity

	DWORD				m_dwCropWidth;					// Width of the cropped scene texture
	DWORD				m_dwCropHeight;					// Height of the cropped scene texture

	float				m_fKeyValue;					// Middle gray key value for tone mapping

	bool				m_bToneMap;						// True when scene is to be tone mapped            
	bool				m_bBlueShift;					// True when blue shift is to be factored in
	bool				m_bAdaptationInvalid;			// True when adaptation level needs refreshing
	bool				m_bUseMultiSampleFloat16;		// True when using multisampling on a floating point back buffer
	D3DMULTISAMPLE_TYPE m_MaxMultiSampleType;			// Non-Zero when m_bUseMultiSampleFloat16 is true
	DWORD				m_dwMultiSampleQuality;			// Non-Zero when we have multisampling on a float backbuffer
	bool				m_bSupportsD16;
	bool				m_bSupportsD32;
	bool				m_bSupportsD24X8;

	D3DXMATRIX			m_mView;
	D3DXMATRIX			m_mProjection;
	float				m_fRadius;						// Sun's sphere radius
	int					m_fStacks;
	int					m_fSlices;
	float				m_fBloomScale;
	float				m_fStarScale;


private:
	//--------------------------------------------------------------------------------------
	// Private methods
	//--------------------------------------------------------------------------------------

	// Post-processing source textures creation
	HRESULT Scene_To_SceneScaled();
	HRESULT SceneScaled_To_BrightPass();
	HRESULT BrightPass_To_StarSource();
	HRESULT StarSource_To_BloomSource();

	// Post-processing helper functions
	HRESULT GetTextureRect( PDIRECT3DTEXTURE9 pTexture, RECT* pRect );
	HRESULT GetTextureCoords( PDIRECT3DTEXTURE9 pTexSrc, RECT* pRectSrc, PDIRECT3DTEXTURE9 pTexDest, RECT* pRectDest, CoordRect* pCoords );

	// Sample offset calculation. These offsets are passed to corresponding
	// pixel shaders.
	HRESULT GetSampleOffsets_GaussBlur5x5(DWORD dwD3DTexWidth, DWORD dwD3DTexHeight, D3DXVECTOR2* avTexCoordOffset, D3DXVECTOR4* avSampleWeights, FLOAT fMultiplier = 1.0f );
	HRESULT GetSampleOffsets_Bloom(DWORD dwD3DTexSize, float afTexCoordOffset[15], D3DXVECTOR4* avColorWeight, float fDeviation, FLOAT fMultiplier=1.0f);    
	HRESULT GetSampleOffsets_Star(DWORD dwD3DTexSize, float afTexCoordOffset[15], D3DXVECTOR4* avColorWeight, float fDeviation);    
	HRESULT GetSampleOffsets_DownScale4x4( DWORD dwWidth, DWORD dwHeight, D3DXVECTOR2 avSampleOffsets[] );
	HRESULT GetSampleOffsets_DownScale2x2( DWORD dwWidth, DWORD dwHeight, D3DXVECTOR2 avSampleOffsets[] );

	// Tone mapping and post-process lighting effects
	HRESULT MeasureLuminance();
	HRESULT CalculateAdaptation();
	HRESULT RenderStar();
	HRESULT RenderBloom();

	// Methods to control scene lights
	HRESULT AdjustLight(bool bIncrement); 
	HRESULT RefreshLights();

	HRESULT RenderScene(D3DXMATRIX mView);
	void    RenderText();
	HRESULT ClearTexture( LPDIRECT3DTEXTURE9 pTexture );

	VOID    DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV);
	VOID    DrawFullScreenQuad(CoordRect c) { DrawFullScreenQuad( c.fLeftU, c.fTopV, c.fRightU, c.fBottomV ); }

	inline float GaussianDistribution( float x, float y, float rho )
	{
		float g = 1.0f / sqrtf( 2.0f * D3DX_PI * rho * rho );
		g *= expf( -(x*x + y*y)/(2*rho*rho) );

		return g;
	}

	inline bool SUCCESS(HRESULT hr) { return (hr >= 0); }
	inline bool FAILURE(HRESULT hr) { return (hr < 0); }
//	inline void VERIFY(HRESULT hr) { if( FAILURE(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L"Failed Verification", true ); } }
	
};


#endif // _WLHDRSUN_H_