//グローバル
Texture2D g_tex;
matrix g_mWVP;			  // ワールドから射影までの変換行列
float4 g_vColor = float4(1.f, 1.f, 1.f, 1.f);
float g_RectUvTop;		  // 切り取り位置
float g_RectUvLeft;		  // 切り取り位置
float g_RectUvRight;	  // 切り取り位置
float g_RectUvBottom;	  // 切り取り位置

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

//構造体
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

//バーテックスバッファー
VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, g_mWVP);
	output.Tex = Tex;

	// 切り取り設定
	if (Tex.x == 0.f)
	{
		output.Tex.x = g_RectUvLeft;
	}
	else if (Tex.x == 1.f)
	{
		output.Tex.x = g_RectUvRight;
	}
	else if (Tex.y == 0.f)
	{
		output.Tex.y = g_RectUvTop;
	}
	else if (Tex.y == 1.f)
	{
		output.Tex.y = g_RectUvBottom;
	}

	return output;
}

//ピクセルシェーダー
float4 PS(VS_OUTPUT input) : SV_Target
{
	return g_tex.Sample(samLinear, input.Tex) * g_vColor;
}

//
//テクニック
//
technique10 SimpleTexture
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}