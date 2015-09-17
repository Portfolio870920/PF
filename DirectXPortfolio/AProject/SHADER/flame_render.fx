#define	SamplerState1(a)	sampler_state{Texture = <a>; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Clamp; AddressV = Clamp;}
//#define	SamplerState1(a)	sampler_state{Texture = <a>; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Mirror; AddressV = Mirror;}
#define	SamplerState2(a)	sampler_state{Texture = <a>; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Mirror; AddressV = Mirror;}

float4x4	viewproj;
float4x4	world;
float4x4	view;
float4x4	proj;

texture		texD;
texture		texN;

sampler g_samTextureD	= SamplerState1 ( texD );
sampler g_samTextureN	= SamplerState2 ( texN );

float		fTimer = 0.0f;
void		TVShader (	float4	iPos	: POSITION,
						float2	iTex	: TEXCOORD0,
						out float4 oPos	: POSITION,
						out float2 oTex	: TEXCOORD0	)
{
	oPos	= mul( mul(iPos,world), viewproj );
	oTex.xy = float2(iTex.x, iTex.y);
}

void		TPShader (	float4 oPos	: POSITION,
						float2 oTex : TEXCOORD0,
						out float4 Pixel0 :	COLOR0 )
{
	float3	nor0	= (tex2D ( g_samTextureN, oTex + float2(0.0f, fTimer) ).xyz + tex2D ( g_samTextureN, oTex*0.5f + float2(0.0f, fTimer)*0.25f ).xyz)*0.5f-0.5f;
	Pixel0 = tex2D ( g_samTextureD, oTex + nor0.xz * (1.0f-pow(saturate(oTex.y), 0.7f)) );
}

technique render
{
    pass p0
    {
		alphablendenable	= true;
		srcblend			= srcalpha;
		destblend			= invsrcalpha;
		alphatestenable		= true;
		alpharef			= 1;
		cullmode			= none;
        VertexShader	= compile	vs_2_0	TVShader	();
        PixelShader		= compile	ps_2_0	TPShader	();
    }
}

