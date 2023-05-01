#pragma once


#include <wrl.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <Windows.Graphics.Capture.Interop.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <opencv2/core/directx.hpp>	



class WindowsCaptureBase
{

public:
	WindowsCaptureBase() { Init(); }

	bool SetCaptureTarget(const HWND& hwnd);
	void SetFps(const int& fps) { FPS = fps; TimeCur = 1000 / fps; }
	cv::Mat GetCaptureImage();
private:
	bool Init();
	// capture opencv mat;
	cv::Mat  resMat;
	cv::ocl::Context  m_oclCtx;

	int FPS = 60;
	float TimeCur = 1000.0 / 60;
	//  set mutex   in   func"OnFrameArrived"  to help get cv::mat;
	std::mutex ImageMutex;

	//// set a bool to change the Directx2d source save
	bool SaveTexture2d_bool = true;


	// directx3d 
	winrt::com_ptr<ID3D11Device> d3dDevice{ nullptr };
	winrt::com_ptr< ID3D11DeviceContext>  d3dContext{ nullptr };
	winrt::com_ptr<IDXGISwapChain1> m_swapChain{ nullptr };

	winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device{};

	//  WindowsCapture core
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
	winrt::Windows::Graphics::SizeInt32 ItemSize{};
	winrt::Windows::Graphics::SizeInt32 LastSize{};
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker m_frameArrived;
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };

	void OnFrameArrived(
		winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
		winrt::Windows::Foundation::IInspectable const& args);

};



inline auto
CreateDXGISwapChain(
	winrt::com_ptr<ID3D11Device> const& device,
	const DXGI_SWAP_CHAIN_DESC1* desc)
{
	auto dxgiDevice = device.as<IDXGIDevice2>();
	winrt::com_ptr<IDXGIAdapter> adapter;
	winrt::check_hresult(dxgiDevice->GetParent(winrt::guid_of<IDXGIAdapter>(), adapter.put_void()));
	winrt::com_ptr<IDXGIFactory2> factory;
	winrt::check_hresult(adapter->GetParent(winrt::guid_of<IDXGIFactory2>(), factory.put_void()));

	winrt::com_ptr<IDXGISwapChain1> swapchain;
	winrt::check_hresult(factory->CreateSwapChainForComposition(
		device.get(),
		desc,
		nullptr,
		swapchain.put()));

	return swapchain;
}

inline auto
CreateDXGISwapChain(
	winrt::com_ptr<ID3D11Device> const& device,
	uint32_t width,
	uint32_t height,
	DXGI_FORMAT format,
	uint32_t bufferCount)
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Format = format;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferCount = bufferCount;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

	return CreateDXGISwapChain(device, &desc);
}


