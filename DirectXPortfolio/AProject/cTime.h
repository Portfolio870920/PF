#pragma once
class cTimer
{
private:
	bool			m_bIsHardWare;		// 쿼리퍼포먼스 지원 여부 확인

	float			m_fTimeScale;		// 시간 크기
	float			m_fTimeElapsed;		// 시간 경과량
	float			m_fFPSTimeElapsed;	// 초당 FPS경과량
	float			m_fTotalTime;		// 시작후 흘러간 전체 시간

	__int64			m_nNowTime;			// 현제 시간
	__int64			m_nPrevTime;		// 이전 시간
	__int64			m_nPeriodFrequency;	// 시간 주기

	unsigned long	m_nFrameRate;		// FPS
	unsigned long	m_nFPSFrameCount;	// FPS 카운트

	static	cTimer*	Time_Inst;
	cTimer(void);
public:
	~cTimer(void);

	static	cTimer*	GetInst(void);
			void	FreeInst(void);

	HRESULT		Init(void);

	void		Tick( float fLockFPS = 0.0f );

	inline DWORD	GetFrameRate( void )	// 1초당 그리는 프레임 수
	{
		return m_nFrameRate;
	}

	inline float	GetTotalTIme(void)		// 흘러간 현재 시간 확인
	{
		return m_fTotalTime;
	}

};