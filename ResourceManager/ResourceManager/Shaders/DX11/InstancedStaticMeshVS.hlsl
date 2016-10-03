struct VS_IN
{
	float3 pos : POSITION;
	float3 nor : NORMAL;
	float2 tex : TEXCOORD;
	uint instanceID : SV_InstanceID;
};

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
	float4 gCamPos; //The w-component is just padding
}

struct PerObject
{
	float4x4 gWVP;
	float4x4 gWorldViewInvTrp;
	float4x4 gWorld;
	float4x4 gWorldView;
};

cbuffer ObjectBuffer : register(b1)
{
	PerObject gPOB[128];
}


VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.posH = mul(float4(input.pos, 1.0f), gPOB[input.instanceID].gWVP);
	output.posVS = mul(float4(input.pos, 1.0f), gPOB[input.instanceID].gWorldView);
	output.tex = input.tex;
	output.normalVS = mul(float4(input.nor,0.0f), gPOB[input.instanceID].gWorldViewInvTrp);
	return output;
}