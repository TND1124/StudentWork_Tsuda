//�O���[�o��
Texture2D g_tex;			// �e�N�X�`���[�f�[�^
matrix g_mW;			    // ���[���h�s��
matrix g_mWVP;			    // ���[���h����ˉe�܂ł̕ϊ��s��

int g_nLightKind[4];		// ���C�g�̎��
float3 g_vLightDirction[4]; // ���s���C�g�̕���
float4 g_vLightPosition[4];	// �_���C�g�̈ʒu
float g_fLightIntensity[4];	// �_���C�g�̋���

float4 g_Ambient = float4(0.4f, 0.4f, 0.4f, 0.f);  //�A���r�G���g��
float4 g_Diffuse = float4(0.4f, 0.4f, 0.4f, 0.f);  //�g�U����
float4 g_SpecularColor = float4(0.2f, 0.2f, 0.2f, 0.f); //���ʔ���
float g_fSpecularPower = 0.f;					   // ���ʔ��˂̋���
float4 g_Emissive = float4(0.f, 0.f, 0.f, 0.f);

float3 g_vEye; // �J�����̎����x�N�g��

matrix g_MatBone[100]; // �{�[���s��

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
	float4 Posw : POSITION;
    float2 Tex : TEXCOORD;
	float3 Nor : NORMAL;
};

//
//�o�[�e�b�N�X�o�b�t�@�[
//
VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEXCOORD, float3 Nor : NORMAL,
			float4 Weight : WEIGTH, int4 MatOffsetId : MATOFFSETID)
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
//�s�N�Z���V�F�[�_�[
//
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float4 color = (float4)0.f;

	for (int i = 0; i < 4; i++)
	{
		float4 LightColor = (float4)0.f;

		// ���C�g���g�p���Ȃ�
		if (g_nLightKind[i] == 0) 
		{
			// �������Ȃ�
		}				 
		// �_����
		else if (g_nLightKind[i] == 1)
		{
			// ���C�g�̈ʒu����I�u�W�F�N�g�̈ʒu�܂ł̋���
			float distance = 
				(input.Posw.x - g_vLightPosition[i].x) * (input.Posw.x - g_vLightPosition[i].x) +
				(input.Posw.y - g_vLightPosition[i].y) * (input.Posw.y - g_vLightPosition[i].y) +
				(input.Posw.z - g_vLightPosition[i].z) * (input.Posw.z - g_vLightPosition[i].z);

			// �����ɂ���ă��C�g�̋�����ς���
			float LightPower = (g_fLightIntensity[i] * 5.f) / distance;

			// ���C�g�̈ʒu����I�u�W�F�N�g�̈ʒu�ւ̃x�N�g��
			float3 vLightDir = normalize((float3)input.Posw - (float3)g_vLightPosition[i]);

			// �f�B�t���[�Y
			LightColor += LightPower * g_Diffuse * 10.f * dot(input.Nor, -vLightDir);

			// �n�[�t�x�N�g���쐬
			float3 vHalf = normalize(vLightDir + g_vEye);

			// �X�y�L����
			float spe = g_SpecularColor * pow(dot(input.Nor, vHalf), g_fSpecularPower);
			if (spe > 0.f)
			{
				LightColor += spe;
			}

			// �A���r�G���g
			LightColor += g_Ambient;
		}
		// ���s���C�g
		else if (g_nLightKind[i] == 2)
		{
			// �@���̌����ƃ��C�g�̌����Ŗ��邳�����߂�
			LightColor += 2.f * g_Diffuse * dot(input.Nor, normalize(-g_vLightDirction[i]));
			
			// �A���r�G���g
			LightColor += g_Ambient;
		}

		// ���C�g�J���[����Ԗ��邢�l���g��
		color = max(color, LightColor);
	}

	// �G�~�b�V�u
	color += g_Emissive;

	// �e�N�X�`���\��t��
	float4 texColor = g_tex.Sample(samLinear, input.Tex);
	if (texColor.w != 0.f)
	{
		color = texColor * color;
		color.w = texColor.w;
	}

	// 0.f�`1.f�̊ԂɎ��߂�
	color = clamp(color, 0.f, 1.f);

	return color;
}

//
//�e�N�j�b�N
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

