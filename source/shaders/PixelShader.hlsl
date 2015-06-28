//float4 main() : SV_TARGET
//{
//  return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

float4 main(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
  return color;
}