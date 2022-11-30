matrix g_mWVP;			    // ���[���h����ˉe�܂ł̕ϊ��s��

int g_nLightKind;		 // ���C�g�̎��
float3 g_vLightDirction; // ���s���C�g�̕���

matrix g_MatBone[100]; // �{�[���s��

//�\����
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
};

//
//�o�[�e�b�N�X�o�b�t�@�[
//
VS_OUTPUT VS(float4 Pos : POSITION, float4 Weight : WEIGTH, int4 MatOffsetId : MATOFFSETID)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 SkinPos;
	float4 MovePos = (float4)0.f;

	// �X�L�����b�V���A�j���[�V����
	// �E�F�C�g�l�������Ă���΁A�A�j���[�V�����s��Œ��_�𓮂���
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

	// �X�L�����b�V���A�j���[�V�������ړ�������
	Pos += MovePos;
	Pos.w = 1.f;

	// �e�Ƃ��ĕ\���������I�u�W�F�N�g���𑗂�
	output.Pos = mul(Pos, g_mWVP);

	return output;
}

//
//�s�N�Z���V�F�[�_�[
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	return float4(0.f, 0.f, 0.f, 1.f);
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