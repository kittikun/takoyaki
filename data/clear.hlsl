// Clear screen

cbuffer ModelViewProjectionCB : register(b0)
{
	float4x4 mat;
	float4x4 matViewProjection;
};

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.position = mul(matViewProjection, input.position);
    
    return output;
}

PS_OUTPUT PS_Main(VS_OUTPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT)0;
    
    output.color = float4(0, 1, 0, 1);
    
    return output;
}