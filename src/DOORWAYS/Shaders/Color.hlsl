struct VertexIn
{
float3 PosL : POSITION;
float4 Color : COLOR;
};

struct VertexOut
{
float4 PosH : SV_POSITION;
float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
VertexOut vout;

vout.PosH = float4(vin.PosL, 1.0f);
vout.Color = vin.Color;

return vout;
};

float4 PS(VertexOut pin) : SV_TARGET
{
return pin.Color;
};