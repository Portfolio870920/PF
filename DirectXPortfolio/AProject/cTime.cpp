#include "MainHeader.h"

cTimer*	cTimer::Time_Inst	=	NULL;

cTimer::cTimer(void)
{
	Init();
}
cTimer::~cTimer(void)
{
}

cTimer*	cTimer::GetInst(void)
{
	if ( Time_Inst == NULL )
	{
		Time_Inst = new cTimer;
		RSCMGR->m_bTIME_MGR	=	true;
	}
	return Time_Inst;
}

void	cTimer::FreeInst(void)
{
	if ( Time_Inst != NULL )
	{
		delete	Time_Inst;
		Time_Inst	=	NULL;
		RSCMGR->m_bTIME_MGR	=	false;
	}

	if( m_bIsHardWare	==	false )
	{
		timeEndPeriod(1);
	}
}

HRESULT cTimer::Init(void)
{
	if( QueryPerformanceFrequency( (LARGE_INTEGER*)&m_nPeriodFrequency ) )
	{
		m_bIsHardWare		= true;
		QueryPerformanceCounter( (LARGE_INTEGER*)&m_nPrevTime );

		m_fTimeScale		= 1.0f / m_nPeriodFrequency;
	}

	else
	{
		timeBeginPeriod(1);

		m_bIsHardWare		= false;
		m_nPrevTime			= timeGetTime();
		m_fTimeScale		= 0.001f;
	}

	m_nFrameRate			= 0;
	m_nFPSFrameCount		= 0;
	m_fFPSTimeElapsed		= 0.0f;
	m_fTotalTime			= 0.0f;

	return S_OK;

}

void cTimer::Tick( float fLockFPS )
{
	if( m_bIsHardWare )
	{
		QueryPerformanceCounter( (LARGE_INTEGER*)&m_nNowTime );
	}

	else
	{
		m_nNowTime = timeGetTime();
	}

	m_fTimeElapsed = ( m_nNowTime - m_nPrevTime ) * m_fTimeScale;

	if( fLockFPS > 0.0f )
	{
		while( m_fTimeElapsed < ( 1.0f / fLockFPS ) )
		{
			if( m_bIsHardWare )
			{
				QueryPerformanceCounter( (LARGE_INTEGER*)&m_nNowTime );
			}

			else
			{
				m_nNowTime = timeGetTime();
			}

			m_fTimeElapsed = ( m_nNowTime - m_nPrevTime ) * m_fTimeScale;
		}
	}

	m_nPrevTime = m_nNowTime;

	m_nFPSFrameCount++;

	m_fFPSTimeElapsed += m_fTimeElapsed;

	m_fTotalTime += m_fTimeElapsed;

	if( m_fFPSTimeElapsed > 1.0f )
	{
		m_nFrameRate			= m_nFPSFrameCount;
		m_nFPSFrameCount		= 0;
		m_fFPSTimeElapsed		= 0.0f;
	}
}
