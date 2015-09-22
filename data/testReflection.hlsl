// Just a test for d3dcompile

#ifndef CLEAR_HLSL
#define CLEAR_HLSL

#include "global.hlsl"

cbuffer CBClear
{
    float3 clearColor;
};

struct Toto
{
    float4 titi;
};

Buffer testB;
ByteAddressBuffer testByteBuffer;
StructuredBuffer<Toto> testSB;
Texture1D<float3> tex1d;
Texture2D<float3> tex2d;
Texture3D<float3> tex3d;
Texture1DArray tex1dA;
Texture2DArray tex2dA;
TextureCube texCube;
TextureCubeArray TexcubeA;

RWBuffer<uint> testRWB;
RWByteAddressBuffer testByteBufferRWB;
RWStructuredBuffer<Toto> testRWSB;
RWTexture1D<float3> testTex1dRW;
RWTexture2D<float3> testTex2dRW;
RWTexture3D<float3> testTex3dRW;
RWTexture1DArray<float3> testTex1dRWA;
RWTexture2DArray<float3> testTex2dRWA;

SamplerState SSTest;

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
    
    
    uint val = testB[0] + testByteBuffer.Load(0);
    testRWB[0] = val; 
    testByteBufferRWB.Store(0, val);

    float4 pos = testSB[0].titi;
    testRWSB[0].titi = pos;
    
    float3 col = tex1d.Sample(SSTest, 0.f) + tex2d.Sample(SSTest, float2(0, 0)) + tex3d.Sample(SSTest, float3(0, 0, 0));
    col += tex1dA.Sample(SSTest, float2(0, 0)) + tex2dA.Sample(SSTest, float3(0, 0, 0)); 
    testTex1dRW[0] = col;
    testTex2dRW[uint2(0, 0)] = col;
    testTex3dRW[uint3(0, 0, 0)] = col;
    testTex1dRWA[uint2(0, 0)] = col;
    testTex2dRWA[uint3(0, 0, 0)] = col;
    
    output.color = float4(clearColor, 1);
    
    return output;
}

#endif