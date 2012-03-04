#include <NyxRender\DepthTarget.h>

#include <D3D11.h>

#include <NyxRender\RenderDevice.h>
#include <NyxRender\DXGI\RenderFormat.h>

namespace Nyx
{

int DepthTarget::init(DepthTarget& obj, RenderFormat format, size_t width, size_t height)
{
    NYX_ASSERT(obj._depth_stencil_view == nullptr);
    NYX_ASSERT(obj._shader_resource_view == nullptr);
    NYX_ASSERT(obj._texture == nullptr);

    ID3D11Device* device = RenderDevice::device();

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = to_dxgi(format);
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&desc, nullptr, &obj._texture);
    if (FAILED(hr))
    {
        term(obj);
        return 1;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    dsv_desc.Format = desc.Format;
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Texture2D.MipSlice = 0;

    hr = device->CreateDepthStencilView(obj._texture, &dsv_desc, &obj._depth_stencil_view);
    if (FAILED(hr))
    {
        term(obj);
        return 1;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    srv_desc.Format = desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = desc.MipLevels;

    hr = device->CreateShaderResourceView(obj._texture, &srv_desc, &obj._shader_resource_view);
    if (FAILED(hr))
    {
        term(obj);
        return 1;
    }

    obj._format = format;
    obj._width = width;
    obj._height = height;

    return 0;
}

void DepthTarget::term(DepthTarget& obj)
{
    if (obj._depth_stencil_view != nullptr)
    {
        obj._depth_stencil_view->Release();
        obj._depth_stencil_view = nullptr;
    }

    if (obj._shader_resource_view != nullptr)
    {
        obj._shader_resource_view->Release();
        obj._shader_resource_view = nullptr;
    }

    if (obj._texture != nullptr)
    {
        obj._texture->Release();
        obj._texture = nullptr;
    }
}

} // namespace Nyx
