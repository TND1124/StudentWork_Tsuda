//グローバル
Texture2D g_tex;			// テクスチャーデータ
matrix g_mW;			    // ワールド行列
matrix g_mWVP;			    // ワールドから射影までの変換行列

int g_nLightKind[4];		// ライトの種類
float3 g_vLightDirction[4]; // 平行ライトの方向
float4 g_vLightPosition[4];	// 点ライトの位置
float g_fLightIntensity[4];	// 点ライトの強さ

float4 g_Ambient = float4(0.4f, 0.4f, 0.4f, 0.f);  //アンビエント光
float4 g_Diffuse = float4(0.4f, 0.4f, 0.4f, 0.f);  //拡散反射
float4 g_SpecularColor = float4(0.2f, 0.2f, 0.2f, 0.f); //鏡面反射
float g_fSpecularPower = 0.f;					   // 鏡面反射の強さ
float4 g_Emissive = float4(0.f, 0.f, 0.f, 0.f);

float3 g_vEye; // カメラの視線ベクトル

matrix g_MatBone[100]; // ボーン行列

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
	float4 Posw : POSITION;
    float2 Tex : TEXCOORD;
	float3 Nor : NORMAL;
};

//
//バーテックスバッファー
//
VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEXCOORD, float3 Nor : NORMAL,
			float4 Weight : WEIGTH, int4 MatOffsetId : MATOFFSETID)
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

	Pos += MovePos;
	Pos.w = 1.f;

    output.Pos = mul(Pos, g_mWVP);
	output.Posw = mul(Pos, g_mW);
    output.Tex = Tex;
	Nor = mul(Nor, g_mW);
	output.Nor = normalize(Nor);

    return output;
}


//
//ピクセルシェーダー
//
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float4 color = (float4)0.f;

	for (int i = 0; i < 4; i++)
	{
		float4 LightColor = (float4)0.f;

		// ライトを使用しない
		if (g_nLightKind[i] == 0) 
		{
			// 何もしない
		}				 
		// 点光源
		else if (g_nLightKind[i] == 1)
		{
			// ライトの位置からオブジェクトの位置までの距離
			float distance = 
				(input.Posw.x - g_vLightPosition[i].x) * (input.Posw.x - g_vLightPosition[i].x) +
				(input.Posw.y - g_vLightPosition[i].y) * (input.Posw.y - g_vLightPosition[i].y) +
				(input.Posw.z - g_vLightPosition[i].z) * (input.Posw.z - g_vLightPosition[i].z);

			// 距離によってライトの強さを変える
			float LightPower = (g_fLightIntensity[i] * 5.f) / distance;

			// ライトの位置からオブジェクトの位置へのベクトル
			float3 vLightDir = normalize((float3)input.Posw - (float3)g_vLightPosition[i]);

			// ディフューズ
			LightColor += LightPower * g_Diffuse * 10.f * dot(input.Nor, -vLightDir);

			// ハーフベクトル作成
			float3 vHalf = normalize(vLightDir + g_vEye);

			// スペキュラ
			float spe = g_SpecularColor * pow(dot(input.Nor, vHalf), g_fSpecularPower);
			if (spe > 0.f)
			{
				LightColor += spe;
			}

			// アンビエント
			LightColor += g_Ambient;
		}
		// 平行ライト
		else if (g_nLightKind[i] == 2)
		{
			// 法線の向きとライトの向きで明るさを決める
			LightColor += 2.f * g_Diffuse * dot(input.Nor, normalize(-g_vLightDirction[i]));
			
			// アンビエント
			LightColor += g_Ambient;
		}

		// ライトカラーが一番明るい値を使う
		color = max(color, LightColor);
	}

	// エミッシブ
	color += g_Emissive;

	// テクスチャ貼り付け
	float4 texColor = g_tex.Sample(samLinear, input.Tex);
	if (texColor.w != 0.f)
	{
		color = texColor * color;
		color.w = texColor.w;
	}

	// 0.f～1.fの間に収める
	color = clamp(color, 0.f, 1.f);

	return color;
}

//
//テクニック
//
technique10 SimpleTexture
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

