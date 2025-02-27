/**
* Copyright (C) 2022 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

#include "d3d9.h"
#include "Utils\Utils.h"

HWND DeviceWindow = nullptr;
LONG BufferWidth = 0, BufferHeight = 0;

// For AntiAliasing
bool DeviceMultiSampleFlag = false;
D3DMULTISAMPLE_TYPE DeviceMultiSampleType = D3DMULTISAMPLE_NONE;
DWORD DeviceMultiSampleQuality = 0;

void AdjustWindow(HWND MainhWnd, LONG displayWidth, LONG displayHeight);

HRESULT m_IDirect3D9Ex::QueryInterface(REFIID riid, void** ppvObj)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if ((riid == IID_IUnknown || riid == WrapperID) && ppvObj)
	{
		AddRef();

		*ppvObj = this;

		return D3D_OK;
	}

	return ProxyInterface->QueryInterface(riid, ppvObj);
}

ULONG m_IDirect3D9Ex::AddRef()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->AddRef();
}

ULONG m_IDirect3D9Ex::Release()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	ULONG ref = ProxyInterface->Release();

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

void m_IDirect3D9Ex::LogAdapterNames()
{
#ifndef DEBUG
	static bool RunOnce = true;
	if (!RunOnce)
	{
		return;
	}
	RunOnce = false;
#endif // DEBUG
	UINT Adapter = ProxyInterface->GetAdapterCount();
	for (UINT x = 0; x < Adapter; x++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier = {};
		if (SUCCEEDED(ProxyInterface->GetAdapterIdentifier(x, NULL, &Identifier)))
		{
			Logging::Log() << __FUNCTION__ << " Adapter: " << x << " " << Identifier.DeviceName << " " << Identifier.Description;
		}
	}
}

HRESULT m_IDirect3D9Ex::EnumAdapterModes(THIS_ UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

UINT m_IDirect3D9Ex::GetAdapterCount()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterCount();
}

HRESULT m_IDirect3D9Ex::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE *pMode)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT m_IDirect3D9Ex::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT m_IDirect3D9Ex::GetAdapterModeCount(THIS_ UINT Adapter, D3DFORMAT Format)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterModeCount(Adapter, Format);
}

HMONITOR m_IDirect3D9Ex::GetAdapterMonitor(UINT Adapter)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterMonitor(Adapter);
}

HRESULT m_IDirect3D9Ex::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9 *pCaps)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HRESULT m_IDirect3D9Ex::RegisterSoftwareDevice(void *pInitializeFunction)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->RegisterSoftwareDevice(pInitializeFunction);
}

HRESULT m_IDirect3D9Ex::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT m_IDirect3D9Ex::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT m_IDirect3D9Ex::CheckDeviceMultiSampleType(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (Config.EnableWindowMode)
	{
		Windowed = true;
	}

	return ProxyInterface->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT m_IDirect3D9Ex::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (Config.EnableWindowMode)
	{
		Windowed = true;
	}

	return ProxyInterface->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed);
}

HRESULT m_IDirect3D9Ex::CheckDeviceFormatConversion(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

template <typename T>
HRESULT m_IDirect3D9Ex::CreateDeviceT(D3DPRESENT_PARAMETERS& d3dpp, bool& MultiSampleFlag, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, T ppReturnedDeviceInterface)
{
	if (!pPresentationParameters || !ppReturnedDeviceInterface)
	{
		return D3DERR_INVALIDCALL;
	}

	BehaviorFlags = UpdateBehaviorFlags(BehaviorFlags);

	// Create new d3d9 device
	HRESULT hr = D3DERR_INVALIDCALL;

	// Check fullscreen
	bool ForceFullscreen = TestResolution(Adapter, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);

	// Setup presentation parameters
	CopyMemory(&d3dpp, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	UpdatePresentParameter(&d3dpp, hFocusWindow, ForceFullscreen, true);

	// Check for AntiAliasing
	if (Config.AntiAliasing != 0)
	{
		DWORD QualityLevels = 0;

		// Check AntiAliasing quality
		for (int x = min(16, Config.AntiAliasing); x > 0; x--)
		{
			if (SUCCEEDED(ProxyInterface->CheckDeviceMultiSampleType(Adapter,
				DeviceType, (d3dpp.BackBufferFormat) ? d3dpp.BackBufferFormat : D3DFMT_A8R8G8B8, d3dpp.Windowed,
				(D3DMULTISAMPLE_TYPE)x, &QualityLevels)) ||
				SUCCEEDED(ProxyInterface->CheckDeviceMultiSampleType(Adapter,
					DeviceType, d3dpp.AutoDepthStencilFormat, d3dpp.Windowed,
					(D3DMULTISAMPLE_TYPE)x, &QualityLevels)))
			{
				// Update Present Parameter for Multisample
				UpdatePresentParameterForMultisample(&d3dpp, (D3DMULTISAMPLE_TYPE)x, (QualityLevels > 0) ? QualityLevels - 1 : 0);

				// Create Device
				hr = CreateDeviceT(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &d3dpp, (d3dpp.Windowed) ? nullptr : pFullscreenDisplayMode, ppReturnedDeviceInterface);

				// Check if device was created successfully
				if (SUCCEEDED(hr))
				{
					MultiSampleFlag = true;
					(*ppReturnedDeviceInterface)->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
					LOG_LIMIT(3, "Setting MultiSample " << d3dpp.MultiSampleType << " Quality " << d3dpp.MultiSampleQuality);
					break;
				}
			}
		}
		if (FAILED(hr))
		{
			LOG_LIMIT(100, __FUNCTION__ << " Failed to enable AntiAliasing!");
		}
	}

	// Create Device
	if (FAILED(hr))
	{
		// Update presentation parameters
		CopyMemory(&d3dpp, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
		UpdatePresentParameter(&d3dpp, hFocusWindow, ForceFullscreen, false);

		// Create Device
		hr = CreateDeviceT(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &d3dpp, (d3dpp.Windowed) ? nullptr : pFullscreenDisplayMode, ppReturnedDeviceInterface);
	}

	return hr;
}

HRESULT m_IDirect3D9Ex::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!pPresentationParameters || !ppReturnedDeviceInterface)
	{
		return D3DERR_INVALIDCALL;
	}

	bool MultiSampleFlag = false;
	D3DPRESENT_PARAMETERS d3dpp;

	HRESULT hr = CreateDeviceT(d3dpp, MultiSampleFlag, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, nullptr, ppReturnedDeviceInterface);

	if (SUCCEEDED(hr))
	{
		CopyMemory(pPresentationParameters, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));

		*ppReturnedDeviceInterface = new m_IDirect3DDevice9Ex((LPDIRECT3DDEVICE9EX)*ppReturnedDeviceInterface, this, IID_IDirect3DDevice9, pPresentationParameters->MultiSampleType, pPresentationParameters->MultiSampleQuality, MultiSampleFlag);
		return D3D_OK;
	}

	Logging::LogDebug() << __FUNCTION__ << " FAILED! " << (D3DERR)hr << " " << Adapter << " " << DeviceType << " " << hFocusWindow << " " << BehaviorFlags << " " << pPresentationParameters;
	return hr;
}

UINT m_IDirect3D9Ex::GetAdapterModeCountEx(THIS_ UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterfaceEx)
	{
		Logging::Log() << __FUNCTION__ << " Error: Calling extension function from a non-extension device!";
		return NULL;
	}

	return ProxyInterfaceEx->GetAdapterModeCountEx(Adapter, pFilter);
}

HRESULT m_IDirect3D9Ex::EnumAdapterModesEx(THIS_ UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterfaceEx)
	{
		Logging::Log() << __FUNCTION__ << " Error: Calling extension function from a non-extension device!";
		return D3DERR_INVALIDCALL;
	}

	return ProxyInterfaceEx->EnumAdapterModesEx(Adapter, pFilter, Mode, pMode);
}

HRESULT m_IDirect3D9Ex::GetAdapterDisplayModeEx(THIS_ UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterfaceEx)
	{
		Logging::Log() << __FUNCTION__ << " Error: Calling extension function from a non-extension device!";
		return D3DERR_INVALIDCALL;
	}

	return ProxyInterfaceEx->GetAdapterDisplayModeEx(Adapter, pMode, pRotation);
}

HRESULT m_IDirect3D9Ex::CreateDeviceEx(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!pPresentationParameters || !ppReturnedDeviceInterface)
	{
		return D3DERR_INVALIDCALL;
	}

	bool MultiSampleFlag = false;
	D3DPRESENT_PARAMETERS d3dpp;

	HRESULT hr = CreateDeviceT(d3dpp, MultiSampleFlag, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);

	if (SUCCEEDED(hr))
	{
		CopyMemory(pPresentationParameters, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));

		*ppReturnedDeviceInterface = new m_IDirect3DDevice9Ex(*ppReturnedDeviceInterface, this, IID_IDirect3DDevice9Ex, pPresentationParameters->MultiSampleType, pPresentationParameters->MultiSampleQuality, MultiSampleFlag);
		return D3D_OK;
	}

	Logging::LogDebug() << __FUNCTION__ << " FAILED! " << (D3DERR)hr << " " << Adapter << " " << DeviceType << " " << hFocusWindow << " " << BehaviorFlags << " " << pPresentationParameters << " " << pFullscreenDisplayMode;
	return hr;
}

HRESULT m_IDirect3D9Ex::GetAdapterLUID(THIS_ UINT Adapter, LUID * pLUID)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterfaceEx)
	{
		Logging::Log() << __FUNCTION__ << " Error: Calling extension function from a non-extension device!";
		return D3DERR_INVALIDCALL;
	}

	return ProxyInterfaceEx->GetAdapterLUID(Adapter, pLUID);
}

bool m_IDirect3D9Ex::TestResolution(UINT Adapter, DWORD BackBufferWidth, DWORD BackBufferHeight)
{
	if (Config.ForceExclusiveFullscreen && BackBufferWidth && BackBufferHeight)
	{
		// Enumerate modes for format XRGB
		UINT modeCount = ProxyInterface->GetAdapterModeCount(Adapter, D3DFMT_X8R8G8B8);

		D3DDISPLAYMODE d3ddispmode;

		// Loop through all modes looking for our requested resolution
		for (UINT i = 0; i < modeCount; i++)
		{
			// Get display modes here
			ZeroMemory(&d3ddispmode, sizeof(D3DDISPLAYMODE));
			if (FAILED(ProxyInterface->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &d3ddispmode)))
			{
				LOG_LIMIT(100, __FUNCTION__ << " Error: EnumAdapterModes failed");
				break;
			}
			// Check exact height and width match
			if (d3ddispmode.Width == BackBufferWidth && d3ddispmode.Height == BackBufferHeight)
			{
				return true;
			}
		}
	}
	return false;
}

DWORD UpdateBehaviorFlags(DWORD BehaviorFlags)
{
	if (Config.ForceMixedVertexProcessing)
	{
		BehaviorFlags &= ~(D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
		BehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
	}
	else if (BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
	{
		BehaviorFlags &= ~(D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING);
		BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	else if (BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
	{
		BehaviorFlags &= ~(D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
		BehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
	}
	else if (BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		BehaviorFlags &= ~(D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
		BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	return BehaviorFlags;
}

// Update Presentation Parameters
void UpdatePresentParameter(D3DPRESENT_PARAMETERS* pPresentationParameters, HWND hFocusWindow, bool ForceExclusiveFullscreen, bool SetWindow)
{
	if (!pPresentationParameters)
	{
		return;
	}

	// Set vsync
	if (Config.EnableVSync && (Config.ForceVsyncMode || pPresentationParameters->PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE))
	{
		pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else if (Config.ForceVsyncMode)
	{
		pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	// Set windowed mode if enabled
	if (ForceExclusiveFullscreen)
	{
		pPresentationParameters->Windowed = false;
		if (!pPresentationParameters->FullScreen_RefreshRateInHz)
		{
			pPresentationParameters->FullScreen_RefreshRateInHz = Utils::GetRefreshRate(DeviceWindow);
		}
		if (pPresentationParameters->BackBufferFormat == D3DFMT_UNKNOWN)
		{
			pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
		}
	}
	else if (Config.EnableWindowMode)
	{
		pPresentationParameters->Windowed = true;
		pPresentationParameters->FullScreen_RefreshRateInHz = 0;
	}

	// Store last window data
	LONG LastBufferWidth = BufferWidth;
	LONG LastBufferHeight = BufferHeight;
	HWND LastDeviceWindow = DeviceWindow;

	// Get current window data
	if (IsWindow(pPresentationParameters->hDeviceWindow) || IsWindow(DeviceWindow) || IsWindow(hFocusWindow))
	{
		BufferWidth = (pPresentationParameters->BackBufferWidth) ? pPresentationParameters->BackBufferWidth : BufferWidth;
		BufferHeight = (pPresentationParameters->BackBufferHeight) ? pPresentationParameters->BackBufferHeight : BufferHeight;
		DeviceWindow = (IsWindow(hFocusWindow)) ? hFocusWindow :
			(IsWindow(pPresentationParameters->hDeviceWindow)) ? pPresentationParameters->hDeviceWindow :
			DeviceWindow;

		// Check if window is minimized
		if (IsIconic(DeviceWindow))
		{
			ShowWindow(DeviceWindow, SW_RESTORE);

			// Peek messages to help prevent a "Not Responding" window
			Utils::CheckMessageQueue(DeviceWindow);
		}

		// Get window width and height
		if (!BufferWidth || !BufferHeight)
		{
			RECT tempRect;
			GetClientRect(DeviceWindow, &tempRect);
			BufferWidth = tempRect.right;
			BufferHeight = tempRect.bottom;
		}
	}

	// Set window size
	if (SetWindow && pPresentationParameters->Windowed && IsWindow(DeviceWindow))
	{
		bool AnyChange = (LastBufferWidth != BufferWidth || LastBufferHeight != BufferHeight || LastDeviceWindow != DeviceWindow);

		// Adjust window
		RECT Rect;
		GetClientRect(DeviceWindow, &Rect);
		if (AnyChange || Rect.right - Rect.left != BufferWidth || Rect.bottom - Rect.top != BufferHeight)
		{
			AdjustWindow(DeviceWindow, BufferWidth, BufferHeight);
		}

		// Set fullscreen resolution
		if (Config.FullscreenWindowMode && AnyChange)
		{
			// Get monitor info
			MONITORINFOEX infoex = {};
			infoex.cbSize = sizeof(MONITORINFOEX);
			BOOL bRet = GetMonitorInfo(Utils::GetMonitorHandle(DeviceWindow), &infoex);

			// Get resolution list for specified monitor
			DEVMODE newSettings = {};
			newSettings.dmSize = sizeof(newSettings);
			if (EnumDisplaySettings(bRet ? infoex.szDevice : nullptr, ENUM_CURRENT_SETTINGS, &newSettings) != 0)
			{
				newSettings.dmPelsWidth = BufferWidth;
				newSettings.dmPelsHeight = BufferHeight;
				newSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
				ChangeDisplaySettingsEx(bRet ? infoex.szDevice : nullptr, &newSettings, nullptr, CDS_FULLSCREEN, nullptr);

				// Peek messages to help prevent a "Not Responding" window
				Utils::CheckMessageQueue(DeviceWindow);
			}
		}
	}
}

// Set Presentation Parameters for Multisample
void UpdatePresentParameterForMultisample(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD MultiSampleQuality)
{
	if (!pPresentationParameters)
	{
		return;
	}

	pPresentationParameters->MultiSampleType = MultiSampleType;
	pPresentationParameters->MultiSampleQuality = MultiSampleQuality;

	pPresentationParameters->Flags &= ~D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	pPresentationParameters->SwapEffect = D3DSWAPEFFECT_DISCARD;

	if (!pPresentationParameters->EnableAutoDepthStencil)
	{
		pPresentationParameters->EnableAutoDepthStencil = true;
		pPresentationParameters->AutoDepthStencilFormat = D3DFMT_D24S8;
	}

	pPresentationParameters->BackBufferCount = (pPresentationParameters->BackBufferCount) ? pPresentationParameters->BackBufferCount : 1;
}

// Adjusting the window position for WindowMode
void AdjustWindow(HWND MainhWnd, LONG displayWidth, LONG displayHeight)
{
	if (!IsWindow(MainhWnd) || !displayWidth || !displayHeight)
	{
		LOG_LIMIT(100, __FUNCTION__ << " Error: could not set window size, nullptr.");
		return;
	}

	// Set window active and focus
	if (Config.EnableWindowMode)
	{
		if ((GetWindowLong(MainhWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) == 0)
		{
			SetWindowPos(MainhWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			SetWindowPos(MainhWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
		}
		SetForegroundWindow(MainhWnd);
		SetFocus(MainhWnd);
		SetActiveWindow(MainhWnd);
	}

	// Get screen width and height
	LONG screenWidth = 0, screenHeight = 0;
	Utils::GetScreenSize(MainhWnd, screenWidth, screenHeight);
	RECT screenRect = {};
	Utils::GetDesktopRect(MainhWnd, screenRect);

	// Get window style
	LONG lOriginalStyle = GetWindowLong(MainhWnd, GWL_STYLE);
	LONG lStyle = lOriginalStyle | WS_VISIBLE;
	LONG lExStyle = GetWindowLong(MainhWnd, GWL_EXSTYLE);

	// Get new style
	RECT Rect = { 0, 0, displayWidth, displayHeight };
	AdjustWindowRectEx(&Rect, (lStyle | WS_OVERLAPPEDWINDOW) & ~(WS_MAXIMIZEBOX | WS_THICKFRAME), GetMenu(MainhWnd) != NULL, lExStyle);
	if (Config.WindowModeBorder && !Config.FullscreenWindowMode && screenWidth > Rect.right - Rect.left && screenHeight > Rect.bottom - Rect.top)
	{
		lStyle = (lStyle | WS_OVERLAPPEDWINDOW) & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
	}
	else
	{
		lStyle &= ~WS_OVERLAPPEDWINDOW;
	}

	// Set window style
	if (Config.EnableWindowMode || (lOriginalStyle & WS_VISIBLE) == NULL)
	{
		lStyle = (Config.EnableWindowMode) ? lStyle : (lOriginalStyle | WS_VISIBLE);
		SetWindowLong(MainhWnd, GWL_STYLE, lStyle);
		SetWindowPos(MainhWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOSENDCHANGING);
	}

	// Get new window rect
	Rect = { 0, 0, displayWidth, displayHeight };
	AdjustWindowRectEx(&Rect, GetWindowLong(MainhWnd, GWL_STYLE), GetMenu(MainhWnd) != NULL, lExStyle);
	Rect = { 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top };

	// Get upper left window position
	bool SetWindowPositionFlag = Config.FullscreenWindowMode;
	LONG xLoc = 0, yLoc = 0;
	if (Config.SetInitialWindowPosition && !Config.FullscreenWindowMode &&
		(Config.InitialWindowPositionLeft == 0 || Rect.right + (LONG)Config.InitialWindowPositionLeft <= screenWidth) &&
		(Config.InitialWindowPositionTop == 0 || Rect.bottom + (LONG)Config.InitialWindowPositionTop <= screenHeight))
	{
		SetWindowPositionFlag = true;
		xLoc = Config.InitialWindowPositionLeft;
		yLoc = Config.InitialWindowPositionTop;
	}
	else if (Config.EnableWindowMode && !Config.FullscreenWindowMode && screenWidth >= Rect.right && screenHeight >= Rect.bottom)
	{
		SetWindowPositionFlag = true;
		xLoc = (screenWidth - Rect.right) / 2;
		yLoc = (screenHeight - Rect.bottom) / 2;
	}

	// Center and adjust size of window
	if (SetWindowPositionFlag)
	{
		// Use SetWindowPlacement to center and adjust size
		WINDOWPLACEMENT wndpl = {};
		wndpl.length = sizeof(WINDOWPLACEMENT);
		if (GetWindowPlacement(MainhWnd, &wndpl))
		{
			wndpl.rcNormalPosition = { xLoc, yLoc, Rect.right + xLoc, Rect.bottom + yLoc };
			SetWindowPlacement(MainhWnd, &wndpl);
		}
		// Use SetWindowPos to center and adjust size
		else
		{
			SetWindowPos(MainhWnd, HWND_TOP, xLoc, yLoc, Rect.right, Rect.bottom, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSENDCHANGING);
		}
	}

	// Peek messages to help prevent a "Not Responding" window
	Utils::CheckMessageQueue(MainhWnd);
}
