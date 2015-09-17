#pragma once

class cResourceMgr
{
	enum	Mgr_State
	{
		KEY_MGR,	//Ű �Ŵ���
		SCENE_MGR,	//�� �Ŵ���
		TIME_MGR,	//�ð� �Ŵ���
		CAM_MGR,	//ī�޶� �Ŵ���
		MOB_MGR,	//�� �Ŵ���
		CHAR_MGR,	//ĳ���� �Ŵ���
		MAP_MGR,	//�� �Ŵ���
		TEXT_MGR,	//�ؽ�Ʈ �Ŵ���
		DATA_MGR,	//������ �Ŵ���
		OPTION,		//�ɼ�
		SOUND_MGR	//����Ŵ���
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