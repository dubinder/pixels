//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//  return pos;
//}

struct VIN
{
  float3 position : POSITION;
  float4 color: COLOR;
};

struct VOut
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

VOut main(float4 position :  SV_POSITION, float4 color : COLOR)
{
  VOut output;

  output.position = position;
  output.color = color;

  return output;
}
