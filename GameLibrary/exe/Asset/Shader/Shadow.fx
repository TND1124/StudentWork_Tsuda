matrix g_mWVP;			    // ワールドから射影までの変換行列

int g_nLightKind;		 // ライトの種類
float3 g_vLightDirction; // 平行ライトの方向

matrix g_MatBone[100]; // ボーン行列

//構造体
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
};

//
//バーテックスバッファー
//
VS_OUTPUT VS(float4 Pos : POSITION, float4 Weight : WEIGTH, int4 MatOffsetId : MATOFFSETID)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 SkinPos;
	float4 MovePos = (float4)0.f;

	// スキンメッシュアニメーション
	// ウェイト値が入っていれば、アニメーション行列で頂点を動かす
	if (Weight.x != 0.f)
	{
		SkinPos = mul(Pos, g_MatBone[MatOffsetId.x]);
		MovePos += (SkinPos - Pos) * Weight.x;
	}
	if (Weight.y != 0.f)
	{
		SkinPos = mul(Pos, g_MatBone[MatOffsetId.y]);
		MovePos += (SkinPos - Pos) * Weight.y;
	}
	if (Weight.z != 0.f)
	{
		SkinPos = mul(Pos, g_MatBone[MatOffsetId.z]);
		MovePos += (SkinPos - Pos) * Weight.z;
	}
	if (Weight.w != 0.f)
	{
		SkinPos = mul(Pos, g_MatBone[MatOffsetId.w]);
		MovePos += (SkinPos - Pos) * Weight.w;
	}

	// スキンメッシュアニメーション分移動させる
	Pos += MovePos;
	Pos.w = 1.f;

	// 影として表示したいオブジェクト情報を送る
	output.Pos = mul(Pos, g_mWVP);

	return output;
}

//
//ピクセルシェーダー
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	return float4(0.f, 0.f, 0.f, 1.f);
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