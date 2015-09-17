// ParticleSprite.h: interface for the CParticleSprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLESPRITE_H__472ADCEF_9AB2_4B4D_A224_A2B391B4EF3A__INCLUDED_)
#define AFX_PARTICLESPRITE_H__472ADCEF_9AB2_4B4D_A224_A2B391B4EF3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Particle.h"

class CParticleSprite : public CParticle  
{
protected:
	LPD3DXVECTOR2							m_pvSize;
	LPD3DXVECTOR2							m_pvSizeS;
	LPD3DXVECTOR2							m_pvSizeD;

	LPDIRECT3DVERTEXBUFFER9					m_pInstance;

	static	LPD3DXEFFECT					s_pEffect;
	static	LPDIRECT3DVERTEXDECLARATION9	s_pVDecl;
	static	LPDIRECT3DVERTEXBUFFER9			s_pImage;
	static	LPDIRECT3DINDEXBUFFER9			s_pIndex;
public:
	CParticleSprite();
	virtual ~CParticleSprite();
	unsigned int	Init	( int nSeed, float fGravity );
	static void		CreateEffect	( char* szFileName );

	int				Insert	( LPD3DXVECTOR3 pPos, LPD3DXVECTOR2 pSclS, LPD3DXVECTOR2 pSclD, LPD3DXVECTOR3 pVel, D3DXCOLOR sColor, D3DXCOLOR dColor, DWORD dwLife );

	unsigned int	Update	( void );
	void			Render	( LPDIRECT3DTEXTURE9 pTex );
};
typedef	CParticleSprite	*PParticleSprite;

#endif // !defined(AFX_PARTICLESPRITE_H__472ADCEF_9AB2_4B4D_A224_A2B391B4EF3A__INCLUDED_)
