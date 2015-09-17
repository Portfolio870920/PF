#pragma once

enum	ESOUND
{
	MAIN_SOUND,
	CHAR_BS1,
	CHAR_BS2,
	CHAR_BS3,
	CHAR_BS4,
	CHAR_M00,
	CHAR_M01,
	CHAR_M02,
	CHAR_DASH,
	BGM_B2,
	BGM_B3,
	CHAR_SKILL1,
	CHAR_BUFF1,
	CHAR_BUFF2,
	CHAR_BUFF3,
	CHAR_BUFF4,
	MOB_KNIGHT_ATTACK,
	MOB_KNIGHT_BEHIT1,
	MOB_KNIGHT_BEHIT2,
	MOB_KNIGHT_BEHIT3,
	MOB_KNIGHT_BEHIT4,
	SOUND_END
};
enum	ESOUNDTYPE
{
	SOUND_BGM,
	SOUND_EFFECT
};
struct	SoundInfo
{
	Sound*		pSound;
	Channel*	pChannel;
	int			nSoundType;
};

typedef	std::vector< SoundInfo >	SOUNDVEC;

class cSound
{
private:
	System*		m_pSystem;
	SOUNDVEC	m_SoundVec;
	float		m_fBGM_Volume;
	float		m_fEffect_Volume;
	cSound(void);
	static	cSound*	SOUNDINST;

public:
	static	cSound*	GetInst(void);
	void	FreeInst(void);
	~cSound(void);
	void	Init(void);
	void	Free(void);
	bool	PlaySound( INT nSoundName );
	bool	StopSound( INT nSoundName );

	inline	bool	IsPlaying( INT nSoundName )
	{
		bool bBool	=	false;
		m_SoundVec[nSoundName].pChannel->isPlaying(&bBool);
		return bBool;
	}
	inline	void	SetBGM_Volune( float fVolume )
	{
		m_fBGM_Volume = fVolume;
	}
	inline	void	SetEffect_Volune( float fVolume )
	{
		m_fEffect_Volume = fVolume;
	}
};