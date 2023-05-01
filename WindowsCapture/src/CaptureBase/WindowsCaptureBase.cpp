#include "WindowsCaptureBase.h"
#include "direct3d11.interop.h"

bool WindowsCaptureBase::Init()
{
	//Create Direct 3D Device
	winrt::check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr, 0, D3D11_SDK_VERSION, d3dDevice.put(), nullptr, nullptr));

	d3dDevice->GetImmediateContext(d3dContext.put());
	d3dContext->Release();

	const auto dxgiDevice = d3dDevice.as<IDXGIDevice>();
	{
		winrt::com_ptr<::IInspectable> inspectable;
		winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectable.put()));
		device = inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();
	}
	m_oclCtx = cv::directx::ocl::initializeContextFromD3D11Device(d3dDevice.get());
	return true;
}

bool WindowsCaptureBase::SetCaptureTarget(const HWND& m_Hwnd)
{
	auto activation_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
	auto interop_factory = activation_factory.as<IGraphicsCaptureItemInterop>();
	interop_factory->CreateForWindow(m_Hwnd, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), reinterpret_cast<void**>(winrt::put_abi(this->m_item)));
	ItemSize = m_item.Size();
	LastSize = ItemSize;
	m_swapChain = CreateDXGISwapChain(
		d3dDevice,
		static_cast<uint32_t>(ItemSize.Width),
		static_cast<uint32_t>(ItemSize.Height),
		static_cast<DXGI_FORMAT>(winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized),
		2);

	// Create framepool, define pixel format (DXGI_FORMAT_B8G8R8A8_UNORM), and frame size. 
	m_framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::CreateFreeThreaded(
		device,
		winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
		2,
		ItemSize);
	m_session = m_framePool.CreateCaptureSession(m_item);

	m_frameArrived = m_framePool.FrameArrived(winrt::auto_revoke, { this, &WindowsCaptureBase::OnFrameArrived });
	m_session.StartCapture();
	return true;
}

cv::Mat WindowsCaptureBase::GetCaptureImage()
{
	std::lock_guard<std::mutex>lck(ImageMutex);
	return resMat.clone();
}

void WindowsCaptureBase::OnFrameArrived(winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender, winrt::Windows::Foundation::IInspectable const& args)
{

	auto	t_start = std::chrono::steady_clock::now();
	auto newSize = false;
	{

		auto frame = sender.TryGetNextFrame();
		auto frameContentSize = frame.ContentSize();
		if (frameContentSize.Width != LastSize.Width ||
			frameContentSize.Height != LastSize.Height)
		{

			newSize = true;
			LastSize = frameContentSize;
			m_swapChain->ResizeBuffers(
				2,
				static_cast<uint32_t>(LastSize.Width),
				static_cast<uint32_t>(LastSize.Height),
				static_cast<DXGI_FORMAT>(winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized),
				0);
		}

		const auto surface = frame.Surface();
		auto access = surface.as<IDirect3DDxgiInterfaceAccess>();
		Microsoft::WRL::ComPtr< ID3D11Texture2D> texture;
		const auto hr = access->GetInterface(winrt::guid_of<ID3D11Texture2D>(), (void**)texture.GetAddressOf());

		if (SaveTexture2d_bool)
		{

			Microsoft::WRL::ComPtr< ID3D11Texture2D> des{ nullptr };
			D3D11_TEXTURE2D_DESC texDesc;
			texture->GetDesc(&texDesc);
			texDesc.Usage = D3D11_USAGE_STAGING;
			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			texDesc.MiscFlags = 0;
			d3dDevice->CreateTexture2D(&texDesc, NULL, &des);

			d3dContext->CopyResource(des.Get(), texture.Get());
			if (des != nullptr)
			{

				std::lock_guard<std::mutex>lck(ImageMutex);

				cv::directx::convertFromD3D11Texture2D(des.Get(), resMat);

			}
		}
	}
	if (m_swapChain != nullptr)
	{
		DXGI_PRESENT_PARAMETERS presentParameters = { 0 };
		m_swapChain->Present1(1, 0, &presentParameters);
	}



	// 毫秒
	auto t_end = std::chrono::steady_clock::now();
	float time_span = std::chrono::duration<double, std::milli>(t_end - t_start).count();


	if (time_span < TimeCur)
	{
		Sleep(TimeCur - time_span);
	}
	if (newSize)
	{
		m_framePool.Recreate(
			device,
			winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
			2,
			LastSize);
	}

}
