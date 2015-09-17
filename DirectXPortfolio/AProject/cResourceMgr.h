#pragma once

class cResourceMgr
{
	enum	Mgr_State
	{
		KEY_MGR,	//키 매니저
		SCENE_MGR,	//씬 매니저
		TIME_MGR,	//시간 매니저
		CAM_MGR,	//카메라 매니저
		MOB_MGR,	//몹 매니저
		CHAR_MGR,	//캐릭터 매니저
		MAP_MGR,	//맵 매니저
		TEXT_MGR,	//텍스트 매니저
		DATA_MGR,	//데이터 매니저
		OPTION,		//옵션
		SOUND_MGR	//사운드매니저
	};
	cResourceMgr(void);
	static	cResourceMgr*	m_pResourceMgrInst;

public:
	virtual ~cResourceMgr(void);
	static	cResourceMgr*	GetInst(void);
			void			FreeInst(void);
			void			Init(void);
			void			AllFree(void);
			void			Free(Mgr_State	Name);
			bool			IsAllive(Mgr_State Name);

			bool			m_bKEY_MGR;
			bool			m_bSCENE_MGR;
			bool			m_bTIME_MGR;
			bool			m_bCAM_MGR;
			bool			m_bMOB_MGR;
			bool			m_bCHAR_MGR;
			bool			m_bMAP_MGR;
			bool			m_bTEXT_MGR;
			bool			m_bDATA_MGR;
			bool			m_bOPTION;
			bool			m_bSOUND_MGR;
};