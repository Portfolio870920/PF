// Particle.h: interface for the CParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLE_H__C8C2DAC8_CB38_423B_8F86_A4FA3F814F5E__INCLUDED_)
#define AFX_PARTICLE_H__C8C2DAC8_CB38_423B_8F86_A4FA3F814F5E__INCLUDED_

#include	"D3DWRAPPER.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CParticle  
{
protected:
	LPD3DXVECTOR3		m_pvPos;
	LPD3DXVECTOR3		m_pvVel;
	LPDWORD				m_pwStart;
	LPDWORD				m_pwLife;
	LPD3DXCOLOR			m_pcSColor;
	LPD3DXCOLOR			m_pcDColor;
	LPD3DXCOLOR			m_pcCColor;

	int					m_nSeedCnt;
	int					m_nCurSeed;

	float				m_fGravity;

public:
	CParticle();
	virtual ~CParticle();

	unsigned int	Init	( int nSeed, float fGravity );
	void			Free	( void );

	int				Insert	( LPD3DXVECTOR3 pPos, LPD3DXVECTOR3 pVel, D3DXCOLOR sColor, D3DXCOLOR dColor, DWORD dwLife );
	unsigned int	Update	( void );
};

typedef	CParticle	*PParticle;

#endif // !defined(AFX_PARTICLE_H__C8C2DAC8_CB38_423B_8F86_A4FA3F814F5E__INCLUDED_)
