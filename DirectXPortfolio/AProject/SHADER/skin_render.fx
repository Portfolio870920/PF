#define	SamplerState(a)	sampler_state{Texture = <a>; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Wrap; AddressV = Wrap;}

float4x4	viewproj;
float4x4	world;
float4x4	view;
float4x4	proj;

texture		texD;
texture		texN;
float3		lightDir;
float4x3	Transforms[78];
float		buffcheck;
float3		SubColor;

float3		AtteckCheck = float3( 0.0f, 0.0f, 0.0F );
float3		vCameraPos;
float		fSpecular	= 1.0f;
float		fStrength	= 10.0f;
bool		bNor		= false;
sampler g_samTextureD	= SamplerState ( texD );
sampler g_samTextureN	= SamplerState ( texN );

//셀프그림자
float4x4	matShadow;
texture		shadowTx;
sampler g_samTextureS	= sampler_state { Texture = <shadowTx>; };
//셀프그림자

void		TVShader (	float4	iPos		: POSITION,
						float4	weights		: BLENDWEIGHT0,
						int4	boneIndices	: BLENDINDICES0,
						float3	iNor		: NORMAL,
						float2	iTex		: TEXCOORD0,
						float3	iTan		: TANGENT0,
						out float4 oPos	: POSITION,
						out float2 oTex	: TEXCOORD0,
						out float3 oNor	: TEXCOORD1,
						out float3 oTan	: TEXCOORD2,
						out float4 oWld	: TEXCOORD3,
						out float4 oShd : TEXCOORD4	)
{
	float3	p = float3(0.0f, 0.0f, 0.0f);
	float3	n = 0.0f;
	float3	t = 0.0f;
	for(int i = 0; i < 4; ++i)
	{
		p += weights[i] * mul(iPos, Transforms[boneIndices[i]]);
		n += weights[i] * mul(iNor, Transforms[boneIndices[i]]);
		t += weights[i] * mul(iTan, Transforms[boneIndices[i]]);
	}

	oWld	= mul(float4(p.xyz, 1.0f), world);
	oTex.xy = float2(iTex.x, 1.0f-iTex.y);
	oNor	= mul ( n, world);
	oTan	= mul ( t, world);
	oPos	= mul( oWld, viewproj );
	
	//셀프그림자
	oShd		=	mul( oWld, matShadow );
}

void		TPShader (	float4 oPos	: POSITION,
						float2 oTex : TEXCOORD0,
						float3 oNor : TEXCOORD1,
						float3 oTan : TEXCOORD2,
						float4 oWld : TEXCOORD3,
						float4 oShd : TEXCOORD4,
						out float4 Pixel0 :	COLOR0 )
{
	//그림자
	float4	Shadow	= tex2Dproj( g_samTextureS, oShd );
	
	Pixel0			= tex2Dbias ( g_samTextureD, float4(oTex.xy, 0.0f, -0.5f) );
	float3	nTex	= normalize(tex2Dbias ( g_samTextureN, float4(oTex.xy, 0.0f, -1.0f) ).xyz - 0.5f);
	float3	oBin	= cross ( oNor, oTan );
	float3	EyeDir	=-normalize(oWld.xyz - vCameraPos);
	float3	LitDir	=-normalize(lightDir);
	float3	fHalf	= normalize(EyeDir+LitDir);

	float3	NorDir	= normalize((bNor)?mul ( nTex, (float3x3(oTan, oBin, oNor)) ):oNor);

	float	Spec	= max(0.0f, pow(saturate(dot ( fHalf, NorDir )), fStrength)*fSpecular);
	float	Dot		= max(0, dot ( NorDir, LitDir)) + 0.4f;
	Pixel0.xyz		= Pixel0.xyz * Dot * (Shadow.xyz*0.5f+0.5f) + Spec;
	
	//	Fog start
	float	fHeight	= 1.0f-saturate((oWld.y-80.0f)*0.02f);
	Pixel0.xyz		= Pixel0 * (1.0f-fHeight) + float3(0.7f, 0.7f, 0.7f) * (fHeight);

	
	//	CameraPos Fog
	float fFogZ	=	saturate(length(vCameraPos.xyz-oWld.xyz)*0.004f);
	Pixel0.xyz	=	Pixel0.xyz * (1.0f - fFogZ) + float3(0.0f, 0.0f, 0.0f) * (fFogZ);
	
	//	Fog end
	
	//	맞앗을 경우
	Pixel0.xyz	+=	max( 0.0f, AtteckCheck );

//	Pixel0.xyz		= NorDir;
	Pixel0.w		= saturate(pow(Pixel0.w+0.7f, 8));
	
	float EyeDot	= dot( EyeDir, NorDir.xyz );
	EyeDot	=	pow( EyeDot, buffcheck );
	//float3	subcolor = float3(1.0f, 0.0f, 0.0f);
	Pixel0.xyz	+=	(1.0f-EyeDot) * SubColor;
//	Pixel0.xyz	=	EyeDot>0.5f ? 1.0f : 0.0f;
	
}

void		TSShader (	float4 oPos	: POSITION,
						float2 oTex : TEXCOORD0,
						float3 oNor : TEXCOORD1,
						float3 oTan : TEXCOORD2,
						float4 oWld : TEXCOORD3,
						out float4 Pixel0 :	COLOR0 )
{	
	Pixel0		= 0.0f;
	Pixel0.w	= 1.0f;
}

technique render
{
    pass p0
    {
		alphablendenable	= false;
		srcblend			= srcalpha;
		destblend			= invsrcalpha;
		alphatestenable		= true;
		alpharef			= 1;
        VertexShader	= compile	vs_3_0	TVShader	();
        PixelShader		= compile	ps_3_0	TPShader	();
    }
}

technique createshadow
{
    pass p0
    {
		alphablendenable	= false;
		srcblend			= srcalpha;
		destblend			= invsrcalpha;
		alphatestenable		= true;
		alpharef			= 0;
        VertexShader	= compile	vs_3_0	TVShader	();
        PixelShader		= compile	ps_3_0	TSShader	();
    }
}