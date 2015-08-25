// Clear screen

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	float4x4 mat;
	float4x4 matViewProjection;
};

struct VS_INPUT
{
    float3 position : Position;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

VS_OUTPUT VS_Main(VS_INPUT in)
{
    VS_OUTPUT out = (VS_OUTPUT)0;
    
    out.position = mul(matViewProjection, in.position);
    
    return out;
}

`PS_OUTPUT PS_Main(VS_INPUT in)
{
    PS_OUTPUT out = (PS_OUTPUT)0;
    
    out.color = float4(0, 1, 0, 1);
    
    return out;
}