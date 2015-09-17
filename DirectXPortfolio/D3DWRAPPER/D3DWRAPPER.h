// D3DWRAPPER.h: interface for the D3DWRAPPER class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_D3DWRAPPER_H__FF783B93_7630_4B9F_9670_938C1473984D__INCLUDED_)
#define AFX_D3DWRAPPER_H__FF783B93_7630_4B9F_9670_938C1473984D__INCLUDED_

#include "d3dx9.h"
#include <algorithm>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	DELETE_ARRAY(a)		{if ( a ) {delete [] a; a = NULL;}}
#define	RELEASE_NODEL(a)	{if ( a ) {a->Release(); a = NULL;}}
#define	IdentityMatrix		(D3DXMATRIX(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f ))

double	fTimeGetTime	( void );

class D3DWRAPPER  
{
protected:
	LPDIRECT3D9				m_pDirect3D;
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	D3DXVECTOR3				m_vLightDir;
	D3DXVECTOR3				m_vCameraPos;
	bool					m_bExternal;


	__int64					m_dwTime;
	__int64					m_dwInterval;

	LPDIRECT3DVERTEXBUFFER9			m_pQuadVertex;
	LPDIRECT3DVERTEXDECLARATION9	m_pQuadDecl;
	LPD3DXEFFECT					m_pEffect;
	D3DXHANDLE						m_hPos;
	D3DXHANDLE						m_hScl;
	D3DXHANDLE						m_hTRect;
	D3DXHANDLE						m_hTex;
	D3DXHANDLE						m_hTech;

public:
	static	D3DWRAPPER*		m_pInstance;
	static	D3DWRAPPER*		GetInstance(void);
	// 그림자 추가 변수
	LPDIRECT3DTEXTURE9		m_pShadowTex;
	LPDIRECT3DTEXTURE9		m_pShadowDep;
	bool					m_bShadow;
	D3DXMATRIX				m_mLView;
	D3DXMATRIX				m_mLProj;
	D3DXMATRIX				m_mLBias;
	LPDIRECT3DSURFACE9		m_pPrevRT; // 이전 것을 보관할 변수
	LPDIRECT3DSURFACE9		m_pPrevDp; // 이전 것을 보관할 변수
	D3DXMATRIX				mTemp1, mTemp2;
	// 그림자 추가 변수 끝
	D3DXMATRIX				m_mView, m_mProj;
public:
	D3DWRAPPER();
	virtual ~D3DWRAPPER();

	bool							Init	( HWND hwnd, int width, int height, D3DFORMAT fmt, bool bWindowed );
	bool							Init	( LPDIRECT3DDEVICE9 pDevice );
	void							Free	(void);

	D3DXMATRIX						SetCamera		( D3DXVECTOR3& vPos, D3DXVECTOR3& vTgt, D3DXVECTOR3& vUp );
	D3DXMATRIX						SetPerspective	( float fFov, float fAspect, float fNear, float fFar );
	D3DXMATRIX						SetOrthographic ();
	inline void						SetViewMatrix	( D3DXMATRIX* view )
	{
		m_mView = *view;
		m_pD3DDevice->SetTransform( D3DTS_VIEW, &m_mView );
	}
	void							SetLightDir		( D3DXVECTOR3& vDir );
	D3DXVECTOR3						GetLightDir		( void );
	D3DXVECTOR3						GetCameraPos	( void );
	void							CheckFrame		( void );
	void							DrawQuad		( float	x, float y, float w, float h, LPD3DXVECTOR4 pRect = NULL, LPDIRECT3DTEXTURE9 pTex = NULL );

	void							BeginShadow		( D3DXVECTOR3 vLightPos, D3DXVECTOR3 vLightDir ); // 그림자 그리기 시작
	void							EndShadow		( void ); // 그림자 그리기 종료

	float							GetTimeDelta	( DWORD dwDefault = 33 );
	__inline	LPDIRECT3DDEVICE9	GetDevice(void){return m_pD3DDevice;}
	__inline	D3DXQUATERNION		QuaternionFromRotate	( D3DXVECTOR4 Rot )
	{
		D3DXQUATERNION	r;
		r.x = (float)sin(Rot.w/2.0f) * Rot.x;
		r.y = (float)sin(Rot.w/2.0f) * Rot.y;
		r.z = (float)sin(Rot.w/2.0f) * Rot.z;
		r.w = (float)cos(Rot.w/2.0f);
		return r;
	}
	__inline	D3DXMATRIX	MatrixFromVector( D3DXVECTOR3 Rot )
	{
		D3DXMATRIX	ret;
		float	x, y, z, w;

		FLOAT fSinYaw   = (float)sin( Rot.z/2.0f );
		FLOAT fCosYaw   = (float)cos( Rot.z/2.0f );
		FLOAT fCosPitch = (float)cos( Rot.y/2.0f );
		FLOAT fSinPitch = (float)sin( Rot.y/2.0f );
		FLOAT fSinRoll  = (float)sin( Rot.x/2.0f );
		FLOAT fCosRoll  = (float)cos( Rot.x/2.0f );

		x = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
		y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;

		D3DXMatrixRotationQuaternion( &ret, &D3DXQUATERNION(x, y, z, w));
		return ret;
	}
};

#define		D3DFRAME		(D3DWRAPPER::GetInstance())
#endif // !defined(AFX_D3DWRAPPER_H__FF783B93_7630_4B9F_9670_938C1473984D__INCLUDED_)
