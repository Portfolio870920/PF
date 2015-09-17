#define	SamplerState(a)	sampler_state{Texture = <a>; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Clamp; AddressV = Clamp;}

texture		texD	: TEXIMAGE0;
float2		pos		: Position;
float2		scl		: Scale;
float4		rct		: TexRect;

sampler g_samTextureD	= SamplerState ( texD );

void		TVShader (	float4	iPos	: POSITION,
						float2	iTex	: TEXCOORD0,
						out float4 oPos	: POSITION,
						out float2 oTex	: TEXCOORD0	)
{
	oPos.xy		= iPos.xy * scl + (pos-0.5f)*2.0f;
	oPos.zw		= float2(0.1f, 1.0f);
	oTex		= iTex * rct.zw + rct.xy;
}

void		TPShader (	float4 oPos		: POSITION,
						float2 oTex		: TEXCOORD0,
						out float4 Pixel0	: COLOR0 )
{
	Pixel0		= tex2D ( g_samTextureD, oTex );
	Pixel0.w	= 1.0f;
}

technique Render
{
    pass p0
    {
		alphablendenable	= true;
		srcblend			= srcalpha;
		destblend			= invsrcalpha;
		alphatestenable	= true;
		alpharef			= 1;
		cullmode			= none;
       VertexShader		= compile	vs_2_0	TVShader	();
		PixelShader		= compile	ps_2_0	TPShader	();
    }
}

