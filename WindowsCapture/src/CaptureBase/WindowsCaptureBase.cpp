#include "WindowsCaptureBase.h"

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
	// m_oclCtx = cv::directx::ocl::initializeContextFromD3D11Device(d3dDevice.get());
}
