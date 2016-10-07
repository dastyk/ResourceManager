#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#define GBUFFER_COUNT 4
#define SAFE_RELEASE(x) {if(x){ x->Release(); x = nullptr;}};
#define MAX_INSTANCES 32 //If you change this, also change it in InstancedStaticMeshVS.hlsl

#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include <map>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include <mutex>

#include "Structs.h"
#include "IGraphics.h"
#include "SM_GUID.h"
#include "MeshData.h"

enum VertexShaders
{
	VS_STATIC_MESHES,
	VS_STATIC_MESHES_INSTANCED,
	VS_FULLSCREEN,
	VS_COUNT
};

enum GeometryShaders
{
	GS_COUNT
};

enum PixelShaders
{
	PS_STATIC_MESHES,
	PS_FINAL,
	PS_COUNT
};

enum ComputeShaders
{
	CS_TILED_DEFERRED,
	CS_COUNT
};

enum InputLayouts
{
	IL_PNT_VERTEX,
	LAYOUT_COUNT
};

enum RenderTargets
{
	DIFFUSE,
	NORMAL,
	RT_COUNT
};

enum ConstantBuffers
{
	CB_PER_FRAME,
	CB_PER_OBJECT,
	CB_PER_INSTANCE,
	CB_LIGHTBUFFER,
	CB_COUNT
};

enum Samplers
{
	ANISO,
	SAM_COUNT
};

enum RasterizerStates
{
	RS_CULL_NONE,
	RS_CULL_BACK,
	RS_WIREFRAME,
	RS_COUNT
};


struct DepthBuffer
{
	ID3D11DepthStencilView*   DSV         = nullptr;
	ID3D11DepthStencilView*   DSVReadOnly = nullptr;
	ID3D11ShaderResourceView* SRV         = nullptr;
	ID3D11Texture2D*          DSB         = nullptr;
	ID3D11Resource*           Texture     = nullptr;
};

class Direct3D11 : public IGraphics
{
private:
	ID3D11Device*                       _device = nullptr;
	ID3D11DeviceContext*                _deviceContext = nullptr;
	IDXGISwapChain*                     _swapChain = nullptr;
	ID3D11RenderTargetView*             _backbufferRTV = nullptr;

	DepthBuffer                         _depth;


	ID3D11VertexShader*     _vertexShaders[VertexShaders::VS_COUNT] = { nullptr };
	ID3D11PixelShader*      _pixelShaders[PixelShaders::PS_COUNT] = { nullptr };
	ID3D11ComputeShader*    _computeShaders[ComputeShaders::CS_COUNT] = { nullptr };
	
	ID3D11RenderTargetView*   _renderTargetViews[RenderTargets::RT_COUNT] = { nullptr };
	ID3D11ShaderResourceView* _shaderResourceViews[RenderTargets::RT_COUNT] = { nullptr }; //related to deferred
	ID3D11Texture2D*          _renderTargetTextures[RenderTargets::RT_COUNT] = { nullptr };
	
	struct BufferInfo
	{
		BufferInfo()
		{
			buffer = nullptr;
			count = 0;
		}
		BufferInfo(ID3D11Buffer* buf, size_t size)
		{
			buffer = buf;
			count = size;
		}
		ID3D11Buffer* buffer;
		size_t count;
	};

	struct RenderBatches
	{
		void Clear()
		{
			meshes.clear();
		}
		struct MeshBatch
		{
			MeshBatch() {};
			MeshBatch(SM_GUID guid)
			{
				mesh = guid;
			}
			struct TextureBatch
			{
				TextureBatch() {};
				TextureBatch(SM_GUID guid)
				{
					texture = guid;
				}
				SM_GUID texture;
				std::vector<DirectX::XMFLOAT4X4> transforms;
			};
			SM_GUID mesh;
			std::vector<TextureBatch> textures;
		};
		std::vector<MeshBatch> meshes;
	};
	std::mutex _bufferLock;
	std::mutex _textureLock;
	RenderBatches _renderBatches;
	
	std::map<uint64_t, BufferInfo> _vertexBuffers;
	std::map<uint64_t, BufferInfo> _indexBuffers;
	std::map<uint64_t, ID3D11ShaderResourceView*> _textures;
	ID3D11Buffer* _constantBuffers[ConstantBuffers::CB_COUNT] = { nullptr };
	ID3D11InputLayout* _inputLayouts[InputLayouts::LAYOUT_COUNT] = { nullptr };
	ID3D11SamplerState* _samplerStates[Samplers::SAM_COUNT] = { nullptr };
	ID3D11RasterizerState* _rasterizerStates[RasterizerStates::RS_COUNT] = { nullptr };
	

	void _CreateShadersAndInputLayouts();
	void _CreateDepthBuffer();
	void _CreateSamplerState();
	void _CreateViewPort();
	void _CreateRasterizerState();
	void _CreateConstantBuffers();

	ID3D11Buffer* _CreateVertexBuffer(MeshData::Vertex* vertexData, uint32_t vertexCount);
	ID3D11Buffer* _CreateIndexBuffer(uint32_t* indexData, uint32_t indexCount);

	ID3D11ShaderResourceView* _CreateDDSTexture(const void* data, size_t size);
	ID3D11ShaderResourceView* _CreateWICTexture(const void* data, size_t size);


public:
	Direct3D11();
	virtual ~Direct3D11();
	ID3D11Device* GetDevice() { return _device; }
	ID3D11DeviceContext* GetDeviceContext() { return _deviceContext; }

	


	//Inherited from graphics interface
	virtual void Draw();
//	virtual void CreateBuffer(Resource* resource);
	virtual void CreateMeshBuffers(Resource& r);
	virtual void CreateShaderResource(Resource& resource);
	virtual void NotifyDelete(SM_GUID guid);
	virtual void AddToRenderQueue(const GameObject& gameObject);

	
};


#endif

