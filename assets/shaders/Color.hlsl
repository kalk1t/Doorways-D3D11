Texture2D gDiffuseMap : register(t0);
SamplerState gSampler : register(s0);


cbuffer cbPerObject : register(b0)
{
    row_major float4x4 gWorldViewProj;
    row_major float4x4 gWorld;
    row_major float4x4 gWorldInvTranspose;
    float4 gMaterialDiffuse;
    float4 gTexTransform;

    float gEmissiveStrength;
    float3 gPadding;
};

cbuffer cbPerFrame : register(b1)
{
    float4 gLightDirection;
    float4 gLightColor;
    float4 gAmbientColor;
};

struct VertexIn
{
    float3 PosL     : POSITION;
    float3 NormalL  : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
    float4 PosH     : SV_POSITION;
    float3 NormalW  : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    vout.NormalW = mul(float4(vin.NormalL, 0.0f), gWorldInvTranspose).xyz;
    vout.TexCoord = vin.TexCoord * gTexTransform.xy + gTexTransform.zw;

    return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
    float3 normalW = normalize(pin.NormalW);

    // gLightDirection is the direction the light travels.
    // We negate it to get the direction from the surface toward the light.
    float3 lightVector = normalize(-gLightDirection.xyz);

    float diffuseAmount = saturate(dot(normalW, lightVector));

    float4 textureColor = gDiffuseMap.Sample(gSampler, pin.TexCoord);

    float4 surfaceColor = textureColor * gMaterialDiffuse;

    float4 ambient = gAmbientColor * surfaceColor;
    float4 diffuse = diffuseAmount * gLightColor * surfaceColor;

    float4 litColor = ambient + diffuse;
    float4 emissiveColor = surfaceColor;

    float emissiveAmount = saturate(gEmissiveStrength);

    float4 finalColor = lerp(
        litColor,
        emissiveColor,
        emissiveAmount);

    finalColor.a = surfaceColor.a;

    return finalColor;
}