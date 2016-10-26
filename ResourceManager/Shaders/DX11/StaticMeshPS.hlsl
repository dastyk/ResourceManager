struct VS_OUT
{
	float4 posH : SV_POSITION;
	float4 posVS : POSITION;
	float3 normalVS : NORMAL;
	float2 tex : TEXCOORD;
};
cbuffer PerFrameBuffer : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gViewProj;
	float4x4 gInvView;
	float4x4 gInvViewProj;
	float4 gCamPos;
}

struct PS_OUT
{
	float4 Diffuse : SV_TARGET0;
	float4 Normal : SV_TARGET1;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D Roughness : register(t2);
Texture2D Metallic : register(t3);

SamplerState AniSam : register(s0);

PS_OUT main(VS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	
	output.Diffuse = DiffuseMap.Sample(AniSam, input.tex);
	output.Diffuse.a = 1.0f;
	output.Normal = float4(normalize(input.normalVS), 0.0f);
	return output;
}