//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: WLUtility.cpp
// 
// Author: Christos Constantinou
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "WLUtility.h"

//extern bool loop;
//
//bool WL::InitD3D(
//	HINSTANCE hInstance,
//	int width, int height,
//	bool windowed,
//	D3DDEVTYPE deviceType,
//	IDirect3DDevice9** device)
//{
//	static HWND hwnd = 0;
//	static WNDCLASS wc;
//
//
//	//
//	// Create the main application window.
//	//
//
//	wc.style         = CS_HREDRAW | CS_VREDRAW;
//	wc.lpfnWndProc   = (WNDPROC)WL::WndProc; 
//	wc.cbClsExtra    = 0;
//	wc.cbWndExtra    = 0;
//	wc.hInstance     = hInstance;
//	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
//	wc.hCursor       = LoadCursor(0, IDC_ARROW);
//	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//	wc.lpszMenuName  = 0;
//	wc.lpszClassName = L"Space";
//
//	if( !RegisterClass(&wc) ) 
//	{
//		::MessageBox(0, L"RegisterClass() - FAILED", 0, 0);
//		return false;
//	}
//	
//	hwnd = ::CreateWindow(L"Space", L"Space System", 
//		WS_EX_TOPMOST,
//		0, 0, width, height,
//		0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/); 
//
//	if( !hwnd )
//	{
//		::MessageBox(0, L"CreateWindow() - FAILED", 0, 0);
//		return false;
//	}
//
//	::ShowWindow(hwnd, SW_SHOW);
//	::UpdateWindow(hwnd);
//
//
//	//
//	// Init D3D: 
//	//
//
//	HRESULT hr = 0;
//
//	// Step 1: Create the IDirect3D9 object.
//
//	IDirect3D9* d3d9 = 0;
//    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
//
//    if( !d3d9 )
//	{
//		::MessageBox(0, L"Direct3DCreate9() - FAILED", 0, 0);
//		return false;
//	}
//
//	// Step 2: Check for hardware vp.
//
//	D3DCAPS9 caps;
//	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);
//
//	int vp = 0;
//	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
//		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
//	else
//		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
//
//	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
// 
//	D3DPRESENT_PARAMETERS d3dpp;
//	d3dpp.BackBufferWidth            = width;
//	d3dpp.BackBufferHeight           = height;
//	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
//	d3dpp.BackBufferCount            = 1;
//	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
//	d3dpp.MultiSampleQuality         = 0;
//	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
//	d3dpp.hDeviceWindow              = hwnd;
//	d3dpp.Windowed                   = windowed;
//	d3dpp.EnableAutoDepthStencil     = true; 
//	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
//	d3dpp.Flags                      = 0;
//	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
//	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
//
//	// Step 4: Create the device.
//
//	hr = d3d9->CreateDevice(
//		D3DADAPTER_DEFAULT, // primary adapter
//		deviceType,         // device type
//		hwnd,               // window associated with device
//		vp,                 // vertex processing
//		&d3dpp,             // present parameters
//		device);            // return created device
//
//	if( FAILED(hr) )
//	{
//		// try again using a 16-bit depth buffer
//		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
//		
//		hr = d3d9->CreateDevice(
//			D3DADAPTER_DEFAULT,
//			deviceType,
//			hwnd,
//			vp,
//			&d3dpp,
//			device);
//
//		if( FAILED(hr) )
//		{
//			d3d9->Release(); // done with d3d9 object
//			::MessageBox(0, L"CreateDevice() - FAILED", 0, 0);
//			return false;
//		}
//	}
//
//
//	d3d9->Release(); // done with d3d9 object
//	
//	return true;
//}


//
//	Check device capabilities
//
bool WL::CheckDeviceCaps(IDirect3DDevice9* lpDevice)
{
	D3DCAPS9 caps;
	lpDevice->GetDeviceCaps(&caps);

	if (caps.DevCaps & D3DPRASTERCAPS_WBUFFER) {

		// If W-Buffers are supported, then depth buffering is enabled for W-Buffering:

		lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);

	} else {

		// Otherwise, normal Z-Buffering is enabled:

		lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	}

	return true;
}

//
//	Main Game Loop
//
void WL::EnterMsgLoop( bool (*ptr_display)(float timeDelta) )
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime(); 

	while(WM_QUIT != msg.message)
	{
		if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
        {	
			float currTime  = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;

			ptr_display(timeDelta);

			lastTime = currTime;
        }
    }

}

// Initializes and returns a Directional Light
D3DLIGHT9 WL::InitDirectionalLight(const float& x, const float& y, const float& z, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = color * 0.0f;
	light.Diffuse   = color * 1.0f;
	light.Specular  = color * 0.0f;
	light.Direction.x = x;
	light.Direction.y = y;
	light.Direction.z = z;

	return light;
}

// Initializes and returns a Point Light
D3DLIGHT9 WL::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *color * 0.6f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

// Initializes and returns a Spot Light
D3DLIGHT9 WL::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = *color * 0.0f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Direction = *direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.4f;
	light.Phi          = 0.9f;

	return light;
}

// Initializes and returns a Material
D3DMATERIAL9 WL::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}
