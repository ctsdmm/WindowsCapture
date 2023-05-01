#pragma once
#include "direct3d11.interop.h"
#include <wrl.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <winrt/Windows.Graphics.Capture.h>
class WindowsCaptureBase
{

public:
	bool Init();
	bool SetCaptureTarget(const HWND& hwnd);
private:
	winrt::com_ptr<ID3D11Device> d3dDevice{ nullptr };
	winrt::com_ptr< ID3D11DeviceContext>  d3dContext{ nullptr };
	winrt::com_ptr<IDXGISwapChain1> m_swapChain{ nullptr };

	winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device{};
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
	winrt::Windows::Graphics::SizeInt32 ItemSize{};




