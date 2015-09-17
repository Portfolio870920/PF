float4x4	fView;
float4x4	fProj;
float		fTimer= 0;

texture		texPert;
texture		texChannel;
texture		texChannelA;
texture		texChannelR;
texture		texChannelG;
texture		texChannelB;

bool		bTexPert;
bool		bTexChannel;
bool		bTexChannelA;
bool		bTexChannelR;
bool		bTexChannelG;
bool		bTexChannelB;

texture		texNorm;

float4		Splash;
float3		CamPos;

//	몬스터
float3		ActionPos0;
float		ActionLen0;
float3		ActionPos1;
float		ActionLen1;
float3		ActionPos2;
float		ActionLen2;
//	몬스터

float		fDist = 0.0f;

// 그림자
float4x4	matShadow;
texture		shadowTx;
sampler 	g_samTextureS	= sampler_state { Texture = <shadowTx>; };
// 그림자
sampler		samplePer = sampler_state
						{
							Texture = <texPert>;
							MinFilter	= Linear;							
							MagFilter	= Linear;
						};

sampler		sampleNor = sampler_state
						{
							Texture = <texNorm>;
							MinFilter	= Linear;							
							MagFilter	= Linear;
						};

sampler		sampleMask = sampler_state
						{
							Texture = <texChannel>;
							MinFilter	= Linear;							
							MagFilter	= Linear;
						};

sampler		sampleMaskA = sampler_state
						{
							Texture = <texChannelA>;
							MinFilter	= Linear;							
							MagFilter	= Linear;
						};

sampler		sampleMaskR = sampler_state
						{
							Texture = <texChannelR>;
							MinFilter	= Linear;							
							MagFilter	= Linear;
						};

sampler		sampleMaskG = sampler_state
						{
							Texture = <texChannelG>;
							MinFilter	= Linear;							
							MagFilter	= Linear;
						};

sampler		sampleMaskB = sampler_state
						{
							Texture = <texChannelB>;
							MinFilter	= Linear;							
							MagFilter	= Linear;
						};



// 버텍스 쉐이더 꾸미기
float	rad0	= 25.0f;
float3	color[9]=	{
				float3(1.0f, 1.0f, 1.0f),
				float3(1.0f, 1.0f, 0.0f),
				float3(0.0f, 1.0f, 0.0f),
				float3(0.0f, 1.0f, 1.0f),
				float3(0.0f, 0.0f, 1.0f),
				float3(1.0f, 0.0f, 1.0f),
				float3(1.0f, 0.0f, 0.0f),
				float3(1.0f, 1.0f, 1.0f),
				float3(0.8f, 0.8f, 0.8f)
			};

// 노말맵 버텍스쉐이더
void VNShader(	float4 iPos:POSITION,
				float3 iNor:NORMAL,
				half4 iPos1:TEXCOORD0,
				half4 iPos2:TEXCOORD1,
				out float4 oPos:POSITION,  // out은 픽셀 셰이더에 넘겨주는 내용, 좌표.
				out float3 oNor:TEXCOORD0, // 	     
				out float4 oWld:TEXCOORD1)
{
	// 화면좌표
	float	yy[9]	= {	iPos.y,
						iPos1.x, iPos1.y, iPos1.z, iPos1.w,
						iPos2.x, iPos2.y, iPos2.z, iPos2.w};

	int		idx	=	0;
	float	len	=	saturate((length(CamPos-iPos)-30.0f)/1600.0f)*8.0f;
	float	alpha=	smoothstep(0.0f, 1.0f, modf ( len, idx ));

	iPos.y		=	yy[idx] + (yy[min(12,idx+1)]-yy[idx])*alpha;

	oPos		=	mul(iPos, fView);
	oPos		=	mul(oPos, fProj);
	oWld		=	iPos;
	oNor		=	iNor;
}

void VShader(	float4 iPos:POSITION,
				float3 iNor:NORMAL,
				half4 iPos1:TEXCOORD0,
				half4 iPos2:TEXCOORD1,
				out float4 oPos:POSITION,  // out은 픽셀 셰이더에 넘겨주는 내용, 좌표.
				out float3 oNor:TEXCOORD0, // 	     
				out float4 oWld:TEXCOORD1, // 
				out float4 oShd:TEXCOORD2) // 그림자
{
	// 화면좌표
	float	yy[9]	= {	iPos.y,
				iPos1.x, iPos1.y, iPos1.z, iPos1.w,
				iPos2.x, iPos2.y, iPos2.z, iPos2.w };

	int	idx	=	0;
	float	len	=	saturate((length(CamPos-iPos.xyz)-30.0f)/1600.0f)*8.0f;
	float	alpha=	smoothstep(0.0f, 1.0f, modf ( len, idx ));

	iPos.y		=	yy[0];//yy[idx] + (yy[min(12,idx+1)]-yy[idx])*alpha;

	oPos		=	mul(iPos, fView);
	oPos		=	mul(oPos, fProj);
	oWld		=	iPos;
	oNor		=	iNor;
	
	//그림자
	oShd		=	mul( iPos, matShadow );
}

// 픽셀 쉐이더 꾸미기
void PShader(
				float4 iPos:POSITION,
				float3 iNor: TEXCOORD0,
				float4 iWld: TEXCOORD1,
				float4 oShd: TEXCOORD2,
				out float4 oCol:COLOR0 )
{
	float3	nor		= tex2D ( sampleNor, (((iWld.xz-0.0f) / 1024.0f) * float2(1.0f,-1.0f))*0.5+0.5f ).xzy;
	float4	samp	= tex2D ( samplePer, iWld.xz * 0.001f/*-fTimer*0.05f*/ * float2(0,1) );// * 0.5f;
	samp			+=tex2D ( samplePer, iWld.xz * 0.01f /*+fTimer*0.19f*/ * float2(1,1) );// * 0.25f;	
	samp			+=tex2D ( samplePer, iWld.xz * 0.04f /*-fTimer*0.23f*/ * float2(1,0) );// * 0.125f;	
	samp			+=tex2D ( samplePer, iWld.xz * 0.16f /*+fTimer*0.48f*/ * float2(0,1) );// * 0.0625f;	
	samp			+=tex2D ( samplePer, iWld.xz * 0.32f /*-fTimer*0.89f*/ * float2(-1,1) );// * 0.03125f;	
	samp			*=0.2f;

	float	Dot	=	dot ( -normalize(nor-0.5f), normalize(float3(1.0f, -0.5f, -1.0f)) ) *0.7f + 0.3f;

	//그림자
	float4	Shadow	= tex2Dproj( g_samTextureS, oShd );
	
	oCol.xyz		= bTexPert?samp.xyz:1.0f;
	float4	channel	= tex2D ( sampleMask, (((iWld.xz-0.0f) / 1024.0f) * float2(1.0f,-1.0f))*0.5+0.5f );
	oCol.xyz		= channel.w * tex2D ( sampleMaskA, iWld.xz * 0.04f ).xyz + (1.0f-channel.w)*oCol.xyz;
	oCol.xyz		= channel.x * tex2D ( sampleMaskR, iWld.xz * 0.04f ).xyz + (1.0f-channel.x)*oCol.xyz;
	oCol.xyz		= channel.y * tex2D ( sampleMaskG, iWld.xz * 0.04f ).xyz + (1.0f-channel.y)*oCol.xyz;
	oCol.xyz		= channel.z * tex2D ( sampleMaskB, iWld.xz * 0.04f ).xyz + (1.0f-channel.z)*oCol.xyz;
	oCol.xyz		*= ((Dot) * 1.2f) * (Shadow.xyz*0.5f+0.5f); // 에러지점
	//	Fog start
	//	Height Fog
	float	fHeight	= 1.0f-saturate((iWld.y-80.0f)*0.02f);
	oCol.xyz		= oCol * (1.0f-fHeight) + float3(0.7f, 0.7f, 0.7f) * (fHeight);
	
	//	CameraPos Fog
	float fFogZ	=	saturate( length( CamPos.xyz-iWld.xyz )*0.005f);
	oCol.xyz	=	oCol.xyz * (1.0f - fFogZ) + float3(0.0f, 0.0f, 0.0f) * (fFogZ);
	//	Fog end
	
	float	k0 = length( iWld - float3( ActionPos0.x, iWld.y, ActionPos0.z ) );
	oCol.xyz	= (k0>(ActionLen0-0.5f)&&k0<ActionLen0)?float3(1.0f, 0.0f, 0.0f):oCol.xyz;
	
	float	k1 = length( iWld - float3( ActionPos1.x, iWld.y, ActionPos1.z ) );
	oCol.xyz	= (k1>(ActionLen1-0.5f)&&k1<ActionLen1)?float3(1.0f, 0.0f, 0.0f):oCol.xyz;
	
	float	k2 = length( iWld - float3( ActionPos2.x, iWld.y, ActionPos2.z ) );
	oCol.xyz	= (k2>(ActionLen2-0.5f)&&k2<ActionLen2)?float3(1.0f, 0.0f, 0.0f):oCol.xyz;

	oCol.w		= 1;
}

technique	render
{
	pass P0
	{
		//fillmode		=	wireframe;
		CullMode		=	None;
		VertexShader	=	compile vs_3_0		VShader();
		PixelShader		=	compile ps_3_0		PShader();
	}
}

void FillNormal(
				float4 iPos:POSITION,
				float3 iNor: TEXCOORD0,
				float4 iWld: TEXCOORD1,
				out float4 oCol:COLOR0 )
{
	oCol.xyz		= iNor.xzy*0.5f + 0.5f;;
	oCol.w			= 1;
}

technique	createNormal
{
	pass P0
	{
		CullMode		=	none;
		VertexShader	=	compile vs_3_0		VNShader();
		PixelShader		=	compile ps_3_0		FillNormal();
	}
}

void VWaterShader(	float4 iPos:POSITION,
					float3 iNor:NORMAL,
					half4 iPos1:TEXCOORD0,
					half4 iPos2:TEXCOORD1,
					out float4 oPos:POSITION,			// out은 픽셀 셰이더에 넘겨주는 내용, 좌표.
					out float3 oNor:TEXCOORD0,			// 	     
					out float4 oWld:TEXCOORD1)			// 
{
	float	h0	=	iPos.y;
	iPos.y		=	(sin ( fTimer + iPos.x*0.15f ) + cos ( fTimer + iPos.z*0.15f ) )*0.5f;
	iPos.y		+=	(sin ( fTimer + iPos.x*0.5f ) + cos ( fTimer + iPos.z*0.5f ) )	*0.2f;
	iPos.y		+=	5.0f;

	oPos		=	mul(iPos, fView);
	oPos		=	mul(oPos, fProj);
	oWld		=	iPos;
	oNor		=	iNor;
	oWld.y		=	h0;
	oWld.w		=	iPos.y;
}

void PWaterShader(
					float4 iPos:POSITION,
					float3 iNor: TEXCOORD0,
					float4 iWld: TEXCOORD1,
					out float4 oCol:COLOR0 )
{
	float3	nor		= tex2D ( sampleNor, (((iWld.xz-0.0f) / 1024.0f) * float2(1.0f,-1.0f))*0.5+0.5f ).xzy;
	float4	samp	= tex2D ( samplePer, iWld.xz * 0.001f-fTimer*0.05f * float2(0,1) )* 0.5f;
	samp			+=tex2D ( samplePer, iWld.xz * 0.01f+fTimer*0.19f * float2(1,1) ) * 0.25f;	
	samp			+=tex2D ( samplePer, iWld.xz * 0.04f-fTimer*0.23f * float2(1,0) ) * 0.125f;	
	samp			+=tex2D ( samplePer, iWld.xz * 0.16f+fTimer*0.48f * float2(0,1) ) * 0.0625f;	
	samp			+=tex2D ( samplePer, iWld.xz * 0.32f-fTimer*0.89f * float2(-1,1) )* 0.03125f;	
	samp			*=float4(0.2f, 0.3f, 1.0f, 1.0f);

	float	Dot		=dot ( -normalize(nor-0.5f), normalize(float3(1.0f, -0.5f, -1.0f)) );

	oCol.xyz		= samp.xyz * Dot * 3.5f;
	oCol.w			= saturate(abs(iWld.w - iWld.y)*0.25f);
}

technique	render_water
{
	pass P0
	{
		//fillmode	=	wireframe;
		alphablendenable=	true;
		srcblend		=	srcalpha;
		destblend		=	invsrcalpha;
		VertexShader	=	compile vs_3_0		VWaterShader();
		PixelShader		=	compile ps_3_0		PWaterShader();
	}
}