#define	SamplerState1(a)	sampler_state{Texture = <a>; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Clamp; AddressV = Clamp;}
#define	SamplerState2(a)	sampler_state{Texture = <a>; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Mirror; AddressV = Mirror;}

float4x4	viewproj;
float4x4	world;
float4x4	view;
float4x4	proj;

float3		positions	[40];
float2		scales		[40];
float4		colors		[40];
texture		texD;

sampler g_samTextureD	= SamplerState1 ( texD );

float		fTimer = 0.0f;
void		TVShader (	float4	iPos	: POSITION,
						float2	iTex	: TEXCOORD0,
						float	iIdx	: TEXCOORD1,
						out float4 oPos	: POSITION,
						out float2 oTex	: TEXCOORD0,
						out float4 oCol	: TEXCOORD1	)
{
	int		idx = min(99, (int)(iIdx));
	oPos	= mul( mul(iPos,world)*float4(scales[idx].xyx,1.0f)+float4(positions[idx].xyz, 0.0f), viewproj );
	oTex.xy = float2(iTex.x, iTex.y);
	oCol	= colors[idx];
}

void		TPShader (	float4 oPos	: POSITION,
						float2 oTex : TEXCOORD0,
						float4 oCol : TEXCOORD1,
						out float4 Pixel0 :	COLOR0 )
{
	Pixel0 = tex2D ( g_samTextureD, oTex ) * oCol;
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

