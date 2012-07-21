// Basic Configuration
#define FULLSCREEN		false		// Would you like to run it in fullscreen?
#define WINDOWWIDTH		1150		// Window Width
#define WINDOWHEIGHT	480			// Window Height
#define VSYNC			false
#define ANTIALIASING	4

#include "dxstdafx.h"
#include "resource.h"

// Memory leak detection
#if defined(DEBUG) | defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include "SpaceScene.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont			= NULL;     // Font for drawing text
ID3DXSprite*            g_pSprite		= NULL;     // Sprite for batching draw text calls
IDirect3DDevice9 *		Device			= 0;		// Pointer to the Direct3D device
float					g_AspectRatio	= 0.0f;		// Used when setting the perspective projection
SpaceScene*				scene			= 0;

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
void RenderText();

//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Typically want to skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	// Enable VSYNC
	if (!VSYNC)
		pDeviceSettings->pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Enable Anti-Aliasing
	pDeviceSettings->pp.MultiSampleType = (D3DMULTISAMPLE_TYPE)ANTIALIASING;
	
	// Enable the Stencil Buffer
	//pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // If device doesn't support HW T&L or doesn't support 2.0 vertex shaders in HW 
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(2,0) )
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;                            
        pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning();
    }

    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;
	// Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
						 L"Arial", &g_pFont ) );

	if (scene)
		scene->OnCreateDevice(pd3dDevice, pBackBufferSurfaceDesc);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

	if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );
	if (scene)
		scene->OnResetDevice(pd3dDevice);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	if (scene && !scene->Paused())
		scene->Update(fElapsedTime);
}


//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
	
	// Clear the render target and the zbuffer 
	V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER /*| D3DCLEAR_STENCIL*/, D3DCOLOR_ARGB(0,0, 0, 0), 1.0f, 0) );

	// Render the scene
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		scene->Render(fElapsedTime);
		RenderText();
		V( pd3dDevice->EndScene() );
	}
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
	if( g_pFont )
        g_pFont->OnLostDevice();
	if (scene)
		scene->OnLostDevice();

}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	SAFE_RELEASE( g_pFont );
	if (scene)
		scene->OnDestroyDevice();

}

void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1: DXUTToggleFullScreen(); break;
			case VK_SPACE: if (scene) scene->Pause(); break;
			case VK_RIGHT: if (scene) scene->CameraRotateAngle(true); break;
			case VK_LEFT: if (scene) scene->CameraRotateAngle(false); break;
			case VK_PRIOR : if (scene) scene->Zoom(-0.02f); break;
			case VK_NEXT : if (scene) scene->Zoom(0.02f); break;
			case 49 : if (scene) scene->SetCameraMode(0); break;
			case 50 : if (scene) scene->SetCameraMode(1); break;
			case 51 : if (scene) scene->SetCameraMode(2); break;
        }
    }
}

void CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, 
						 bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext )
{

	if (scene)
	{
		if (nMouseWheelDelta > 0)		
			scene->Zoom(-0.02f);
		else if (nMouseWheelDelta < 0)	
			scene->Zoom(0.02f);
	}

}

void RenderText()
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work fine however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves perf.
    CDXUTTextHelper txtHelper( g_pFont, g_pSprite, 15 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 2, 0 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats(true) ); // Show FPS
	txtHelper.DrawTextLine( DXUTGetDeviceStats() );
    txtHelper.End();
}

void InitApp()
{
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
	DXUTSetCallbackMouse( MouseProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

    // Initialize DXUT and create the desired Win32 window and Direct3D 
    // device for the application. Calling each of these functions is optional, but they
    // allow you to set several options which control the behavior of the framework.
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTCreateWindow( L"Space System" );

	if (FULLSCREEN) {
		g_AspectRatio = float(WINDOWWIDTH)/(WINDOWHEIGHT);	
		// If the resolution is not valid then DXUT will change it to the closest valid
		DXUTCreateDevice( D3DADAPTER_DEFAULT, false, WINDOWWIDTH, WINDOWHEIGHT, IsDeviceAcceptable, ModifyDeviceSettings );
	}
	else
	{
		g_AspectRatio = WINDOWWIDTH/WINDOWHEIGHT;	
		DXUTCreateDevice( D3DADAPTER_DEFAULT, true, WINDOWWIDTH, WINDOWHEIGHT, IsDeviceAcceptable, ModifyDeviceSettings );
	}

	Device = DXUTGetD3DDevice();
	if (Device)
		scene = new SpaceScene();

	//InitApp();

	if(!scene)
	{
		MessageBox(0, L"Scene Initialization Failed", 0, 0);
		return 0;
	} 
	// Pass control to DXUT for handling the message pump and 
	// dispatching render calls. DXUT will call your FrameMove 
	// and FrameRender callback when there is idle time between handling window messages.
	DXUTMainLoop();
	

    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.
	
	delete scene;
	scene = 0;

    return DXUTGetExitCode();
}