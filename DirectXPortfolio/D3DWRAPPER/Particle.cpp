// Particle.cpp: implementation of the CParticle class.
//
//////////////////////////////////////////////////////////////////////

#include "Particle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticle::CParticle()
{
	m_pvPos		= NULL;
	m_pvVel		= NULL;
	m_pwStart	= NULL;
	m_pwLife	= NULL;
	m_pcSColor	= NULL;
	m_pcDColor	= NULL;
	m_pcCColor	= NULL;

	m_nSeedCnt	= 0;
	m_nCurSeed	= 0;
}

CParticle::~CParticle()
{
	Free ();
}

unsigned int	CParticle::Init	( int nSeed, float fGravity )
{
	Free ();
	if ( nSeed < 1 )	return 0xFFFFFFFF;

	m_pvPos		= new D3DXVECTOR3	[nSeed];
	m_pvVel		= new D3DXVECTOR3	[nSeed];
	m_pwStart	= new DWORD			[nSeed];
	m_pwLife	= new DWORD			[nSeed];
	m_pcSColor	= new D3DXCOLOR		[nSeed];
	m_pcDColor	= new D3DXCOLOR		[nSeed];
	m_pcCColor	= new D3DXCOLOR		[nSeed];

	m_nSeedCnt	= nSeed;
	m_nCurSeed	= 0;
	m_fGravity	= fGravity;

	ZeroMemory ( m_pwStart, 4 * nSeed );
	ZeroMemory ( m_pcCColor, sizeof(D3DXCOLOR) * nSeed );
	return 0UL;
}

void			CParticle::Free	( void )
{
	DELETE_ARRAY(m_pvPos);
	DELETE_ARRAY(m_pvVel);
	DELETE_ARRAY(m_pwStart);
	DELETE_ARRAY(m_pwLife);
	DELETE_ARRAY(m_pcSColor);
	DELETE_ARRAY(m_pcDColor);
	DELETE_ARRAY(m_pcCColor);
	m_nCurSeed = 0;
	m_nSeedCnt = 0;
}

int				CParticle::Insert	( LPD3DXVECTOR3 pPos, LPD3DXVECTOR3 pVel, D3DXCOLOR sColor, D3DXCOLOR dColor, DWORD dwLife )
{
	m_pvPos[m_nCurSeed]		= *pPos;
	m_pvVel[m_nCurSeed]		= *pVel;
	m_pwStart[m_nCurSeed]	= timeGetTime();
	m_pwLife[m_nCurSeed]	= dwLife;
	m_pcSColor[m_nCurSeed]	= sColor;
	m_pcDColor[m_nCurSeed]	= dColor;

	m_nCurSeed ++;
	m_nCurSeed	= m_nCurSeed%m_nSeedCnt;
	return m_nCurSeed;
}

unsigned int	CParticle::Update	( void )
{
	for ( int i = 0; i < m_nSeedCnt; i ++ )
	{
		if ( m_pwStart[i] == 0 )							continue;
		if ( (fTimeGetTime()-m_pwStart[i]) > m_pwLife[i] )	{m_pwStart[i] = 0UL;	m_pcCColor[i].a = 0.0f;	continue;}

		float	fAlpha = __min(1.0f, pow((float)(timeGetTime()-m_pwStart[i])/(float)(m_pwLife[i]), 0.5f));
		m_pcCColor[i].a	= m_pcSColor[i].a + (m_pcDColor[i].a - m_pcSColor[i].a) * fAlpha;
		m_pcCColor[i].r	= m_pcSColor[i].r + (m_pcDColor[i].r - m_pcSColor[i].r) * fAlpha;
		m_pcCColor[i].g	= m_pcSColor[i].g + (m_pcDColor[i].g - m_pcSColor[i].g) * fAlpha;
		m_pcCColor[i].b	= m_pcSColor[i].b + (m_pcDColor[i].b - m_pcSColor[i].b) * fAlpha;
		m_pvVel[i].y	+= m_fGravity;
		m_pvPos[i]		+= m_pvVel[i];
	}
	return 0UL;
}

