struct PointLight
{
	float3 position;
	float range;
	float3 color;
	float intensity;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

cbuffer LightBuffer : register(b0)
{
	PointLight pointLights[512];
	uint pointLightCount;
}

cbuffer PerFrameBuffer : register(b1)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gViewProj;
	float4x4 gInvView;
	float4x4 gInvViewProj;
	float4x4 gInvProj;
	float4 gCamPos; //The w-component is just padding
}

SamplerState AniSam : register(s0);

Texture2D Diffuse : register(t0);
Texture2D Normal : register(t1);
Texture2D Depth : register(t2);


float4 main(VS_OUT input) : SV_TARGET
{
	
	//float depth = Depth.Sample(AniSam, input.tex).r;
	//return float4(depth.xxx, 1.0f);
	//float x = input.tex.x * 2.0f - 1.0f;
	//float y = (1.0f - input.tex.y) * 2.0f - 1.0f;
	//float4 projPos = float4(x, y, depth, 1.0f);
	//
	//float4 posVS = mul(projPos, gInvProj);
	//posVS.xyz /= posVS.w;
	float3 normal = Normal.Sample(AniSam, input.tex).rgb;
	//normal = normalize((normal * 2.0f) - 1.0f);
	float3 diffuse = Diffuse.Sample(AniSam, input.tex).rgb;
	
	float3 lightDir = float3(-10.0f, -15.0f, 5.0f);
	float3 lightCol = float3(1.0f, 1.0f, 0.70f);
	lightDir = -normalize(lightDir);
	lightDir = mul(float4(lightDir, 0.0f), gView).xyz;
	float intensity = 4.0f;
	float NdL = dot(normal, lightDir);
	float3 fincol;
	if (NdL > 0.0f)
		fincol = intensity * NdL * lightCol * diffuse + diffuse * 0.35f;
	else
		fincol = diffuse * 0.35f;

	return float4((normal + 1.0f) / 2.0f, 1.0f);
	
}