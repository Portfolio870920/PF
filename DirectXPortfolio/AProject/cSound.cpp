#include "MainHeader.h"

cSound* cSound::SOUNDINST	=	NULL;

cSound::cSound(void)
{

}

cSound::~cSound(void)
{

}

cSound*	cSound::GetInst(void)
{
	if ( !SOUNDINST )
	{
		SOUNDINST	=	new cSound;
		RSCMGR->m_bSOUND_MGR	=	true;
	}
	return SOUNDINST;
}

void	cSound::FreeInst(void)
{
	if ( SOUNDINST )
	{
		Free();
		delete	SOUNDINST;
		SOUNDINST	=	NULL;
		RSCMGR->m_bSOUND_MGR	=	false;
	}
}

void	cSound::Init(void)
{
	System_Create(&m_pSystem);
	m_pSystem->init(24, FMOD_INIT_NORMAL, NULL );

	m_fBGM_Volume	=	1.0f;
	m_fEffect_Volume=	1.0f;

	SoundInfo	Data[21];
	ZeroMemory( Data, sizeof(Data) );

	m_pSystem->createSound( (TCHAR*)"SOUND/MAIN/puk2_OP.wav\0", FMOD_HARDWARE, 0, &Data[0].pSound );
	Data[0].nSoundType	=	SOUND_BGM;
	m_SoundVec.push_back( Data[0] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/cgmon_bs1.wav\0", FMOD_HARDWARE, 0, &Data[1].pSound );
	Data[1].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[1] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/cgmon_bs2.wav\0", FMOD_HARDWARE, 0, &Data[2].pSound );
	Data[2].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[2] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/cgmon_bs3.wav\0", FMOD_HARDWARE, 0, &Data[3].pSound );
	Data[3].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[3] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/cgmon_bs4.wav\0", FMOD_HARDWARE, 0, &Data[4].pSound );
	Data[4].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[4] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/cgmon_m00.wav\0", FMOD_HARDWARE, 0, &Data[5].pSound );
	Data[5].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[5] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/cgmon_m01.wav\0", FMOD_HARDWARE, 0, &Data[6].pSound );
	Data[6].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[6] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/cgmon_m02.wav\0", FMOD_HARDWARE, 0, &Data[7].pSound );
	Data[7].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[7] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/Dash.wav\0", FMOD_HARDWARE, 0, &Data[8].pSound );
	Data[8].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[8] );
	m_pSystem->createSound( (TCHAR*)"SOUND/BATTLE/cgbgm_b2.wav\0", FMOD_HARDWARE, 0, &Data[9].pSound );
	Data[9].nSoundType	=	SOUND_BGM;
	m_SoundVec.push_back( Data[9] );
	m_pSystem->createSound( (TCHAR*)"SOUND/BATTLE/cgbgm_b3.wav\0", FMOD_HARDWARE, 0, &Data[10].pSound );
	Data[10].nSoundType	=	SOUND_BGM;
	m_SoundVec.push_back( Data[10] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/skill1.wav\0", FMOD_HARDWARE, 0, &Data[11].pSound );
	Data[11].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[11] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/buff1.mp3\0", FMOD_HARDWARE, 0, &Data[12].pSound );
	Data[12].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[12] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/buff2.wav\0", FMOD_HARDWARE, 0, &Data[13].pSound );
	Data[13].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[13] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/buff3.wav\0", FMOD_HARDWARE, 0, &Data[14].pSound );
	Data[14].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[14] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/buff4.wav\0", FMOD_HARDWARE, 0, &Data[15].pSound );
	Data[15].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[15] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/knight_attack01.mp3\0", FMOD_HARDWARE, 0, &Data[16].pSound );
	Data[16].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[16] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/knight_behit01.mp3\0", FMOD_HARDWARE, 0, &Data[17].pSound );
	Data[17].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[17] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/knight_behit02.mp3\0", FMOD_HARDWARE, 0, &Data[18].pSound );
	Data[18].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[18] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/knight_behit03.mp3\0", FMOD_HARDWARE, 0, &Data[19].pSound );
	Data[19].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[19] );
	m_pSystem->createSound( (TCHAR*)"SOUND/CHARACTER/knight_behit04.mp3\0", FMOD_HARDWARE, 0, &Data[20].pSound );
	Data[20].nSoundType	=	SOUND_EFFECT;
	m_SoundVec.push_back( Data[20] );
}

void	cSound::Free(void)
{
	for ( unsigned int i = 0 ; i < m_SoundVec.size() ; i++ )
	{
		m_SoundVec[i].pSound->release();
	}
	m_SoundVec.clear();

	m_pSystem->release();
	m_pSystem->close();
}

bool	cSound::PlaySound( INT nSoundName )
{
	if ( nSoundName >= SOUND_END || nSoundName < MAIN_SOUND )
		return false;

	m_pSystem->playSound( FMOD_CHANNEL_FREE, m_SoundVec[nSoundName].pSound, 0, &m_SoundVec[nSoundName].pChannel );

	if ( m_SoundVec[nSoundName].nSoundType	== SOUND_BGM )
		m_SoundVec[nSoundName].pChannel->setVolume( m_fBGM_Volume );

	else
		m_SoundVec[nSoundName].pChannel->setVolume( m_fEffect_Volume );
	return true;
}

bool	cSound::StopSound( INT nSoundName )
{
	if ( nSoundName >= SOUND_END || nSoundName < MAIN_SOUND )
		return false;
	m_SoundVec[nSoundName].pChannel->stop();
	return true;
}