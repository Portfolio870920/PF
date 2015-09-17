#pragma once
class cTimer
{
private:
	bool			m_bIsHardWare;		// ���������ս� ���� ���� Ȯ��

	float			m_fTimeScale;		// �ð� ũ��
	float			m_fTimeElapsed;		// �ð� �����
	float			m_fFPSTimeElapsed;	// �ʴ� FPS�����
	float			m_fTotalTime;		// ������ �귯�� ��ü �ð�

	__int64			m_nNowTime;			// ���� �ð�
	__int64			m_nPrevTime;		// ���� �ð�
	__int64			m_nPeriodFrequency;	// �ð� �ֱ�

	unsigned long	m_nFrameRate;		// FPS
	unsigned long	m_nFPSFrameCount;	// FPS ī��Ʈ

	static	cTimer*	Time_Inst;
	cTimer(void);
public:
	~cTimer(void);

	static	cTimer*	GetInst(void);
			void	FreeInst(void);

	HRESULT		Init(void);

	void		Tick( float fLockFPS = 0.0f );

	inline DWORD	GetFrameRate( void )	// 1�ʴ� �׸��� ������ ��
	{
		return m_nFrameRate;
	}

	inline float	GetTotalTIme(void)		// �귯�� ���� �ð� Ȯ��
	{
		return m_fTotalTime;
	}

};