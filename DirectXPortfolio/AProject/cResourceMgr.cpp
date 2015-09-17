#include "MainHeader.h"

cResourceMgr*	cResourceMgr::m_pResourceMgrInst	=	NULL;

cResourceMgr::cResourceMgr(void)
{

}

cResourceMgr::~cResourceMgr(void)
{
}

cResourceMgr*	cResourceMgr::GetInst(void)
{
	if( m_pResourceMgrInst == NULL )
	{
		m_pResourceMgrInst	=	new cResourceMgr;
	}
	return m_pResourceMgrInst;
}

void	cResourceMgr::FreeInst(void)
{
	if ( m_pResourceMgrInst != NULL )
	{
		delete m_pResourceMgrInst;
		m_pResourceMgrInst	=	NULL;
	}
}

void	cResourceMgr::Init(void)
{
	m_bKEY_MGR		=	false;
	m_bSCENE_MGR	=	false;
	m_bTIME_MGR		=	false;
	m_bCAM_MGR		=	false;
	m_bMOB_MGR		=	false;
	m_bCHAR_MGR		=	false;
	m_bMAP_MGR		=	false;
	m_bTEXT_MGR		=	false;
	m_bDATA_MGR		=	false;
	m_bOPTION		=	false;
	m_bSOUND_MGR	=	false;
}

void	cResourceMgr::AllFree(void)
{
	Free(KEY_MGR);
	Free(SCENE_MGR);
	Free(TIME_MGR);
	Free(CAM_MGR);
	Free(CHAR_MGR);
	Free(MAP_MGR);
	Free(MOB_MGR);
	Free(TEXT_MGR);
	Free(DATA_MGR);
	Free(OPTION);
	Free(SOUND_MGR);
	D3DFRAME->Free ();		//디바이스
	FreeInst();
}

void	cResourceMgr::Free(Mgr_State	Name)
{
	switch (Name)
	{
	case KEY_MGR:	//키 매니저
		{
			if ( m_bKEY_MGR )
				KEYMGR->FreeInst();
			break;
		}
	case SCENE_MGR:	//씬 매니저
		{
			if ( m_bSCENE_MGR )
				SCENEMGR->FreeInst();
			break;
		}
	case TIME_MGR:	//시간 매니저
		{
			if ( m_bTIME_MGR )
				TIMEMGR->FreeInst();
			break;
		}
	case CAM_MGR:	//카메라 매니저
		{
			if ( m_bCAM_MGR )
				CAMMGR->FreeInst();
			break;
		}
	case MOB_MGR:	//몹 매니저
		{
			if ( m_bMOB_MGR )
				MOBMGR->FreeInst();
			break;
		}
	case CHAR_MGR:	//캐릭터 매니저
		{
			if ( m_bCHAR_MGR )
				CHARMGR->FreeInst();
			break;
		}
	case MAP_MGR:	//맵 매니저
		{
			if ( m_bMAP_MGR )
				MAPMGR->FreeInst();
			break;
		}
	case TEXT_MGR:
		{
			if ( m_bTEXT_MGR )
				TEXTMGR->FreeInst();
			break;
		}
	case DATA_MGR:
		{
			if ( m_bDATA_MGR )
				DATAMGR->FreeInst();
			break;
		}
	case OPTION:
		{
			if ( m_bOPTION )
				COPTION->FreeInst();
			break;
		}
	case SOUND_MGR:
		{
			if ( m_bSOUND_MGR )
				SOUNDMGR->FreeInst();
			break;
		}
	}
}

bool	cResourceMgr::IsAllive(Mgr_State Name)
{
	bool	bCheck;
	switch (Name)
	{
	case KEY_MGR:	//키 매니저
		{
			bCheck	=	m_bKEY_MGR;
			break;
		}
	case SCENE_MGR:	//씬 매니저
		{
			bCheck	=	m_bSCENE_MGR;
			break;
		}
	case TIME_MGR:	//시간 매니저
		{
			bCheck	=	m_bTIME_MGR;
			break;
		}
	case CAM_MGR:	//카메라 매니저
		{
			bCheck	=	m_bCAM_MGR;
			break;
		}
	case MOB_MGR:	//몹 매니저
		{
			bCheck	=	m_bMOB_MGR;
			break;
		}
	case CHAR_MGR:	//캐릭터 매니저
		{
			bCheck	=	m_bCHAR_MGR;
			break;
		}
	case MAP_MGR:	//맵 매니저
		{
			bCheck	=	m_bMAP_MGR;
			break;
		}
	case TEXT_MGR:
		{
			bCheck	=	m_bTEXT_MGR;
			break;
		}
	case DATA_MGR:
		{
			bCheck	=	m_bDATA_MGR;
			break;
		}
	case OPTION:
		{
			bCheck	=	m_bOPTION;
			break;
		}
	case SOUND_MGR:
		{
			bCheck	=	m_bSOUND_MGR;
			break;
		}
	}
	return bCheck;
}
