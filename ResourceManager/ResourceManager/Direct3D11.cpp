#include "Direct3D11.h"
#include "Core.h"
#include "DebugLogger.h"
#include "Structs.h"
#include <exception>
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include <string>
#include <sstream>
#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

Direct3D11::Direct3D11()
{
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	const Core* core = Core::GetInstance();
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = core->GetWindow()->GetHandle();
	scd.SampleDesc.Count = 1; 
	scd.Windowed = TRUE; 
	
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, &scd, &_swapChain, &_device, NULL, &_deviceContext);
	if (FAILED(hr))
		throw std::exception("Failed to create device and swapchain");

	////Set up GBuffers for deferred shading
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = core->GetWindow()->GetWidth();
	td.Height = core->GetWindow()->GetHeight();
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (int i = 0; i < RenderTargets::RT_COUNT; ++i)
	{
		_device->CreateTexture2D(&td, nullptr, &_renderTargetTextures[i]);
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = td.Format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;

	for (int i = 0; i < RenderTargets::RT_COUNT; ++i)
	{
		_device->CreateRenderTargetView(_renderTargetTextures[i], &rtvd, &_renderTargetViews[i]);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	for (int i = 0; i < RenderTargets::RT_COUNT; ++i)
	{
		_device->CreateShaderResourceView(_renderTargetTextures[i], &srvd, &_shaderResourceViews[i]);
	}

	ID3D11Buffer* backbuffer;
	rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
	_device->CreateRenderTargetView(backbuffer, &rtvd, &_backbufferRTV);
	SAFE_RELEASE(backbuffer);

	_CreateShadersAndInputLayouts();
	_CreateDepthBuffer();
	_CreateSamplerState();
	_CreateViewPort();
	_CreateRasterizerState();
	_CreateConstantBuffers();
}

Direct3D11::~Direct3D11()
{

	for (auto &i : _vertexShaders)
	{
		SAFE_RELEASE(i);
	}

	for (auto &i : _pixelShaders)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _computeShaders)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _renderTargetViews)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _renderTargetTextures)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _shaderResourceViews)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _vertexBuffers)
	{
		SAFE_RELEASE(i.second.buffer);
	}
	for (auto &i : _indexBuffers)
	{
		SAFE_RELEASE(i.second.buffer);
	}
	for (auto &i : _inputLayouts)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _samplerStates)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _constantBuffers)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _rasterizerStates)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _textures)
	{
		SAFE_RELEASE(i.second);
	}

	SAFE_RELEASE(_depth.DSB);
	SAFE_RELEASE(_depth.DSV);
	SAFE_RELEASE(_depth.DSVReadOnly);
	SAFE_RELEASE(_depth.SRV);
	SAFE_RELEASE(_depth.Texture);

	SAFE_RELEASE(_backbufferRTV);
	SAFE_RELEASE(_swapChain);
	SAFE_RELEASE(_deviceContext);
	if (_device)
	{
		uint32_t refCount = _device->Release();
		if (refCount > 0)
		{
			DebugLogger::GetInstance()->AddMsg("Unreleased com objects: " + refCount);
		}
	}
}

ID3D11Buffer* Direct3D11::_CreateVertexBuffer(MeshData::Vertex* vertexData, uint32_t vertexCount)
{
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(PNTVertex) * vertexCount;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &vertexData[0];
	ID3D11Buffer* buffer = nullptr;
	HRESULT hr = _device->CreateBuffer(&bd, &data, &buffer);
	if (FAILED(hr))
	{
		DebugLogger::GetInstance()->AddMsg("Failed to create vertex buffer");
		throw std::exception("Failed to create vertex buffer");
	}
	return buffer;
}

ID3D11Buffer* Direct3D11::_CreateIndexBuffer(uint32_t * indexData, uint32_t indexCount)
{
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = sizeof(unsigned) * indexCount;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &indexData[0];
	ID3D11Buffer* buffer = nullptr;
	HRESULT hr = _device->CreateBuffer(&bd, &data, &buffer);
	if (FAILED(hr))
	{
		DebugLogger::GetInstance()->AddMsg("Failed to create index buffer");
		throw std::exception("Failed to create index buffer");
	}
	return buffer;
}

/* REWRITE TO CREATE FROM MEMORY */
ID3D11ShaderResourceView* Direct3D11::_CreateDDSTexture(const void* data, size_t size)
{
	ID3D11ShaderResourceView* srv = nullptr;

	HRESULT hr = CreateDDSTextureFromMemory(_device, (uint8_t*)data, size, nullptr, &srv);
	if (FAILED(hr))
	{
		return nullptr;
	}
	return srv;
}

ID3D11ShaderResourceView * Direct3D11::_CreateWICTexture(const void * data, size_t size)
{
	ID3D11ShaderResourceView* srv = nullptr;
	HRESULT hr = CreateWICTextureFromMemory(_device, (uint8_t*)data, size, nullptr, &srv);
	if (FAILED(hr))
	{
		return nullptr;
	}
	return srv;

}

void Direct3D11::Draw()
{
	const Core* core = Core::GetInstance();
	float clearColor[] = { 0.0f,1.0f,0.0f,0.0f };


	for (auto &rtv : _renderTargetViews)
	{
		_deviceContext->ClearRenderTargetView(rtv, clearColor);
	}
	_deviceContext->ClearRenderTargetView(_backbufferRTV, clearColor);
	_deviceContext->ClearDepthStencilView(_depth.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//	_deviceContext->ClearDepthStencilView(_depth.DSVReadOnly, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11ShaderResourceView* nullSRVS[RenderTargets::RT_COUNT + 1] = { nullptr };
	_deviceContext->PSSetShaderResources(0, RenderTargets::RT_COUNT + 1, nullSRVS);

	_deviceContext->IASetInputLayout(_inputLayouts[InputLayouts::IL_PNT_VERTEX]);
	UINT stride = sizeof(PNTVertex);
	UINT offset = 0;
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->VSSetShader(_vertexShaders[VertexShaders::VS_STATIC_MESHES], nullptr, 0);
	_deviceContext->PSSetShader(_pixelShaders[PixelShaders::PS_STATIC_MESHES], nullptr, 0);
	_deviceContext->OMSetRenderTargets(RenderTargets::RT_COUNT, &_renderTargetViews[0], _depth.DSV);
	
	PerFrameBuffer pfb;
	core->GetCameraManager()->FillPerFrameBuffer(pfb);
	D3D11_MAPPED_SUBRESOURCE mappedSubres;
	_deviceContext->Map(_constantBuffers[ConstantBuffers::CB_PER_FRAME], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubres);
	memcpy(mappedSubres.pData, &pfb, sizeof(pfb));
	_deviceContext->Unmap(_constantBuffers[ConstantBuffers::CB_PER_FRAME], 0);
	_deviceContext->VSSetConstantBuffers(0, 1, &_constantBuffers[ConstantBuffers::CB_PER_FRAME]);
	
	XMMATRIX view = core->GetCameraManager()->GetView();
	XMMATRIX proj = core->GetCameraManager()->GetProj();

	for (auto &meshes : _renderBatches.meshes)
	{
		_deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffers[meshes.mesh].buffer, &stride, &offset);
		_deviceContext->IASetIndexBuffer(_indexBuffers[meshes.mesh].buffer, DXGI_FORMAT_R32_UINT, 0);
		for (auto &textures : meshes.textures)
		{
			_deviceContext->PSSetShaderResources(0, 1, &_textures[textures.texture.data]);
			PerObjectBuffer pob;
			for (auto &transforms : textures.transforms)
			{
				XMMATRIX world = XMLoadFloat4x4(&transforms);
				XMStoreFloat4x4(&pob.World, XMMatrixTranspose(world));
				XMStoreFloat4x4(&pob.WorldView, XMMatrixTranspose(world * view));
				XMStoreFloat4x4(&pob.WorldViewInvTrp, XMMatrixInverse(nullptr,world * view));
				XMStoreFloat4x4(&pob.WVP, XMMatrixTranspose(world * view * proj));

				D3D11_MAPPED_SUBRESOURCE msr;
				_deviceContext->Map(_constantBuffers[ConstantBuffers::CB_PER_OBJECT], 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
				memcpy(msr.pData, &pob, sizeof(PerObjectBuffer));
				_deviceContext->Unmap(_constantBuffers[ConstantBuffers::CB_PER_OBJECT], 0);
				_deviceContext->VSSetConstantBuffers(1, 1, &_constantBuffers[ConstantBuffers::CB_PER_OBJECT]);

				//_deviceContext->Draw(_vertexBuffers[meshes.mesh].count, 0);
				_deviceContext->DrawIndexed(_indexBuffers[meshes.mesh].count, 0, 0);
			}

		}

	}

	_deviceContext->IASetInputLayout(nullptr);
	_deviceContext->VSSetShader(_vertexShaders[VertexShaders::VS_FULLSCREEN], nullptr, 0);
	_deviceContext->PSSetShader(_pixelShaders[PixelShaders::PS_FINAL], nullptr, 0);
	ID3D11RenderTargetView* nullRTVs[RenderTargets::RT_COUNT + 1] = { nullptr };
	nullRTVs[0] = _backbufferRTV;
	_deviceContext->OMSetRenderTargets(RenderTargets::RT_COUNT + 1, nullRTVs, nullptr);
	_deviceContext->PSSetShaderResources(0, RenderTargets::RT_COUNT, &_shaderResourceViews[0]);
	_deviceContext->PSSetShaderResources(RenderTargets::RT_COUNT, 1, &_depth.SRV);

	_deviceContext->PSSetConstantBuffers(1, 1, &_constantBuffers[ConstantBuffers::CB_PER_FRAME]);

	_deviceContext->Draw(3, 0);
	

	_swapChain->Present(0, 0);
	_renderBatches.Clear();
}

//void Direct3D11::CreateBuffer(Resource * resource)
//{
//	resource->registerObserver(this);
//	const Resource::ResourceType type = resource->GetResourceType();
//	if (type == Resource::ResourceType::MESH_PNT)
//	{
//		const PNTMeshData* meshdata = (PNTMeshData*)resource->GetProcessedData();
//		_vertexBuffers[resource->GetGUID().data] = _CreateVertexBuffer(meshdata->vertices, meshdata->vertexCount);
//		if(meshdata->indices != nullptr)
//			_indexBuffers[resource->GetGUID().data] = _CreateIndexBuffer(meshdata->indices, meshdata->indexCount);
//	}
//	else if (type == Resource::ResourceType::TEXTURE_DDS)
//	{
//		const TextureData* texdata = (TextureData*)resource->GetProcessedData();
//		_textures[resource->GetGUID()] = _CreateDDSTexture(texdata->data, texdata->size);
//	}
//	else if (type & (Resource::ResourceType::TEXTURE_PNG | Resource::ResourceType::TEXTURE_JPG))
//	{
//		const TextureData* texdata = (TextureData*)resource->GetProcessedData();
//		_textures[resource->GetGUID()] = _CreateWICTexture(texdata->data, texdata->size);
//	}
//
//	
//	
//}

void Direct3D11::CreateMeshBuffers(Resource& r)
{
	MeshData::MeshData* pdata = (MeshData::MeshData*)r.GetData();
	uint64_t guid = r.GetGUID().data;
	_bufferLock.lock();
	auto& got = _vertexBuffers.find(guid);
	if (got == _vertexBuffers.end())
	{
		
		_vertexBuffers[guid] = BufferInfo(_CreateVertexBuffer(pdata->vertices, pdata->NumVertices), pdata->NumVertices);
		_indexBuffers[guid] = BufferInfo(_CreateIndexBuffer(pdata->Indices, pdata->IndexCount), pdata->IndexCount);
		r.registerObserver(this);
	
	}
	else
	{
		DebugLogger::GetInstance()->AddMsg("Tried to create mesh buffers for the same resource while it already existed, GUID: " + guid);
	}
	_bufferLock.unlock();
}

void Direct3D11::CreateShaderResource(Resource& resource)
{
	_textureLock.lock();
	auto& got = _textures.find(resource.GetGUID().data);
	if (got == _textures.end())
	{
		
		TextureData* td = (TextureData*)resource.GetData();
		_textures[resource.GetGUID().data] = _CreateWICTexture(td->data, td->size);
		resource.registerObserver(this);
		
	}
	else
	{
		DebugLogger::GetInstance()->AddMsg("Tried to create shader resource view while it already existed, GUID: " + resource.GetGUID().data);
	}
	_textureLock.unlock();
}

void Direct3D11::NotifyDelete(SM_GUID guid)
{
	_bufferLock.lock();
	auto& find = _vertexBuffers.find(guid);
	if (find != _vertexBuffers.end())
	{
		auto& findIndexBuffer = _indexBuffers.find(guid);
		if (findIndexBuffer != _indexBuffers.end())
			SAFE_RELEASE(findIndexBuffer->second.buffer);
		SAFE_RELEASE(find->second.buffer);
		_vertexBuffers.erase(guid);
	}
	_bufferLock.unlock();
	_textureLock.lock();
	auto& got = _textures.find(guid);
	if (got != _textures.end())
	{
		SAFE_RELEASE(got->second);
		_textures.erase(guid);
	}
	_textureLock.unlock();
}

void Direct3D11::AddToRenderQueue(const GameObject & gameObject)
{
	int32_t meshIndex = -1;
	for (uint32_t i = 0; i < _renderBatches.meshes.size(); i++)
	{
		if (gameObject.mesh == _renderBatches.meshes[i].mesh)
		{
			meshIndex = i;
			break;
		}
	}
	int32_t textureIndex = -1;
	if (meshIndex >= 0)
	{
		for (uint32_t i = 0; i < _renderBatches.meshes[meshIndex].textures.size(); i++)
		{
			if (gameObject.texture == _renderBatches.meshes[meshIndex].textures[i].texture)
			{
				textureIndex = i;
				break;
			}
		}
	}
	else
	{
		_renderBatches.meshes.push_back(RenderBatches::MeshBatch(gameObject.mesh));
		meshIndex = _renderBatches.meshes.size() - 1;
	}

	if (textureIndex >= 0)
	{
		_renderBatches.meshes[meshIndex].textures[textureIndex].transforms.push_back(gameObject.transform);
	}
	else
	{
		_renderBatches.meshes[meshIndex].textures.push_back(RenderBatches::MeshBatch::TextureBatch(gameObject.texture));
		_renderBatches.meshes[meshIndex].textures.back().transforms.push_back(gameObject.transform);
	}
}


void Direct3D11::_CreateShadersAndInputLayouts()
{
	ID3DBlob* pVS;
	D3DCompileFromFile(L"Shaders/DX11/StaticMeshVS.hlsl",nullptr,nullptr,"main","vs_4_0",
		NULL, NULL, &pVS, nullptr);
	_device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &_vertexShaders[VertexShaders::VS_STATIC_MESHES]);

	D3D11_INPUT_ELEMENT_DESC id[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	_device->CreateInputLayout(id, ARRAYSIZE(id), pVS->GetBufferPointer(), pVS->GetBufferSize(), &_inputLayouts[InputLayouts::IL_PNT_VERTEX]);
	SAFE_RELEASE(pVS);
	HRESULT hr;
	D3DCompileFromFile(L"Shaders/DX11/FinalVS.hlsl", nullptr, nullptr, "main", "vs_4_0",
		NULL, NULL, &pVS, nullptr);
	_device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &_vertexShaders[VertexShaders::VS_FULLSCREEN]);
	SAFE_RELEASE(pVS);

	D3DCompileFromFile(L"Shaders/DX11/StaticMeshPS.hlsl", nullptr, nullptr, "main", "ps_4_0",
		NULL, NULL, &pVS, nullptr);
	_device->CreatePixelShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &_pixelShaders[PixelShaders::PS_STATIC_MESHES]);
	SAFE_RELEASE(pVS);

	D3DCompileFromFile(L"Shaders/DX11/FinalPS.hlsl", nullptr, nullptr, "main", "ps_4_0",
		NULL, NULL, &pVS, nullptr);
	_device->CreatePixelShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &_pixelShaders[PixelShaders::PS_FINAL]);
	SAFE_RELEASE(pVS);

	hr = D3DCompileFromFile(L"Shaders/DX11/InstancedStaticMeshVS.hlsl", nullptr, nullptr, "main", "vs_5_0",
		NULL, NULL, &pVS, nullptr);
	_device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &_vertexShaders[VertexShaders::VS_STATIC_MESHES_INSTANCED]);
	SAFE_RELEASE(pVS);


}

void Direct3D11::_CreateDepthBuffer()
{
	const Core* core = Core::GetInstance();
	D3D11_TEXTURE2D_DESC dsd;
	ZeroMemory(&dsd, sizeof(dsd));
	dsd.Width = core->GetWindow()->GetWidth();
	dsd.Height = core->GetWindow()->GetHeight();
	dsd.MipLevels = 1;
	dsd.ArraySize = 1;
	dsd.Format = DXGI_FORMAT_R24G8_TYPELESS;
	dsd.SampleDesc.Count = 1;
	dsd.SampleDesc.Quality = 0;
	dsd.Usage = D3D11_USAGE_DEFAULT;
	dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dsd.CPUAccessFlags = 0;
	dsd.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = -1;

	
	_device->CreateTexture2D(&dsd, 0, &_depth.DSB);
	_device->CreateDepthStencilView(_depth.DSB, &dsvd, &_depth.DSV);
	_device->CreateShaderResourceView(_depth.DSB, &srvd, &_depth.SRV);

	dsvd.Flags |= D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
	_device->CreateDepthStencilView(_depth.DSB, &dsvd, &_depth.DSVReadOnly);

}

void Direct3D11::_CreateSamplerState()
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MaxAnisotropy = 16;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MinLOD = -FLT_MAX;
	sd.MaxLOD = FLT_MAX;
	sd.MipLODBias = 0.0f;
	_device->CreateSamplerState(&sd, &_samplerStates[Samplers::ANISO]);
	_deviceContext->PSSetSamplers(0, 1, &_samplerStates[Samplers::ANISO]);
}

void Direct3D11::_CreateViewPort()
{
	const Core* core = Core::GetInstance();
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)core->GetWindow()->GetWidth();
	vp.Height = (FLOAT)core->GetWindow()->GetHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	_deviceContext->RSSetViewports(1, &vp);
}

void Direct3D11::_CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rd;
	rd.AntialiasedLineEnable = false;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = true;
	rd.DepthBias = false;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthClipEnable = true;
	rd.DepthBiasClamp = 0.0f;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;
	
	HRESULT hr;
	hr =_device->CreateRasterizerState(&rd, &_rasterizerStates[RasterizerStates::RS_CULL_BACK]);

	rd.CullMode = D3D11_CULL_NONE;
	hr = _device->CreateRasterizerState(&rd, &_rasterizerStates[RasterizerStates::RS_CULL_NONE]);

	rd.FillMode = D3D11_FILL_WIREFRAME;
	//rd.CullMode = D3D11_CULL_BACK;
	hr = _device->CreateRasterizerState(&rd, &_rasterizerStates[RasterizerStates::RS_WIREFRAME]);

	_deviceContext->RSSetState(_rasterizerStates[RasterizerStates::RS_CULL_NONE]);
}

void Direct3D11::_CreateConstantBuffers()
{
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.StructureByteStride = 0;
	bd.MiscFlags = 0;

	bd.ByteWidth = sizeof(PerFrameBuffer);
	_device->CreateBuffer(&bd, nullptr, &_constantBuffers[ConstantBuffers::CB_PER_FRAME]);

	bd.ByteWidth = sizeof(PerObjectBuffer);
	_device->CreateBuffer(&bd, nullptr, &_constantBuffers[ConstantBuffers::CB_PER_OBJECT]);

	bd.ByteWidth = sizeof(PerObjectBuffer) * MAX_INSTANCES;
	hr = _device->CreateBuffer(&bd, nullptr, &_constantBuffers[ConstantBuffers::CB_PER_INSTANCE]);


}
