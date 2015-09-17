#pragma once

class CDataMgr
{
public:
	CDataMgr(void);
	~CDataMgr(void);

	static CDataMgr* GetInst();
	void FreeInst(void);

	void Save();
	void Load();

	//load
	char	pBuffer[1024];
	//float	hp, posx, posy, posz;
	//int		stage;	

	//save
	Cplayer* pPlayer;

	static CDataMgr* pInstance;
};

//#define DATAMGR (CDataMgr::GetInstance())