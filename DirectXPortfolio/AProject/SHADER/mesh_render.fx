#define	SamplerState(a)	sampler_state{Texture = <a>; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Wrap; AddressV = Wrap;}

float4x4	viewproj;
float4x4	world;
float4x4	view;
float4x4	proj;

texture		texD;

float3		lightDir;

sampler g_samTextureD	= SamplerState ( texD );

void		TVShader (	float4 iPos : POSITION, float3 iNor : NORMAL, float2 iTex : TEXCOORD0,
						out float4 oPos	: POSITION,
						out float2 oTex	: TEXCOORD0,
						out float3 oNor	: TEXCOORD1	)
{
    oPos = mul( mul(iPos, world), viewproj );
	oTex.xy = iTex.xy;
	oNor	= mul ( iNor, world);
}

void		TPShader (	float4 oPos	: POSITION,
						float2 oTex : TEXCOORD0,
						float3 oNor : TEXCOORD1,
						out float4 Pixel0 :	COLOR0 )
{
	float	Dot		= max(0, dot ( normalize(oNor), -normalize(lightDir) ));
	Pixel0			= tex2Dbias ( g_samTextureD, float4(oTex.xy, 0.0f, -0.5f) );
	Pixel0.xyz		= Pixel0.xyz * Dot;
}

technique render
{
    pass p0
    {
		alphatestenable	= true;
		alpharef		= 1;
		alphablendenable= true;
		srcblend		= srcalpha;
		destblend		= invsrcalpha;
        VertexShader	= compile	vs_1_1	TVShader	();
        PixelShader		= compile	ps_2_0	TPShader	();
    }
}

