//�O���[�o��
Texture2D g_tex;
matrix g_mWVP;			  // ���[���h����ˉe�܂ł̕ϊ��s��
float4 g_vColor = float4(1.f, 1.f, 1.f, 1.f);
float g_RectUvTop;		  // �؂���ʒu
float g_RectUvLeft;		  // �؂���ʒu
float g_RectUvRight;	  // �؂���ʒu
float g_RectUvBottom;	  // �؂���ʒu

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

//�\����
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

//�o�[�e�b�N�X�o�b�t�@�[
VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, g_mWVP);
	output.Tex = Tex;

	// �؂���ݒ�
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

//�s�N�Z���V�F�[�_�[
float4 PS(VS_OUTPUT input) : SV_Target
{
	return g_tex.Sample(samLinear, input.Tex) * g_vColor;
}

//
//�e�N�j�b�N
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