#pragma once

typedef	vector<CMonster*>			MOB_VEC;	// 몬스터들을 종류별로 얻기 위해 부모인 몬스터 클래스를 포인터로 가지는 벡터
typedef	vector<CMonster*>::iterator	MOB_ITER;	// 이터레이터

enum	MOBTYPE	// 몬스터 종류를 결정한 enum
{
	KNIGHT,
	TROLL,
	ALDUIN
};

class cMobMgr
{
	cMobMgr(void);
	static	cMobMgr*	MOBINST;	// 인스턴스 변수
			MOB_VEC		m_vMob;		// 몹 벡터
			MOB_ITER	m_viMob;	// 몹 벡터 이터레이터
			MOB_ITER	m_vSaveIter;	// 이터레이터 저장할 변수
			INT			m_nIndex;
			INT			m_nIndexSave;
			INT			m_nDieCount;
			INT			m_nVectorSize;

public:
	~cMobMgr(void);
	static	cMobMgr*	GetInst();
			void		FreeInst();
			void		Init();	// 현재 생성되어있는 모든 몬스터를 초기화 시킴 초기화 좌표 100, 0, 100
			void		AddMob(MOBTYPE Type, D3DXVECTOR3 MobPos = D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3 MobScl = D3DXVECTOR3(0.0f,0.0f,0.0f), float MobAngle = 0.0f);
			// 몬스터를 추가한다 추가와 동시에 초기화 함
			void		Update();
			void		Render();
			INT			GetDieCount();
			//inline	MOB_ITER	IterBegin()	// 이터레이터 처음을 반환
			//{
			//	MOB_ITER	viMob	=	m_vMob.begin();
			//	return viMob;
			//}
			//inline	void		SetIterBengin()	// 이터레이터 처음으로 변경
			//{
			//	m_viMob	=	m_vMob.begin();
			//}
			//inline	MOB_ITER	IterInc()	// 이터레이터 증가
			//{
			//	m_viMob++;
			//	return m_viMob;
			//}
			//inline	MOB_ITER	IterEnd()	// 이터레이터 끝을 반환
			//{
			//	MOB_ITER	viMob	=	m_vMob.end();
			//	return viMob;
			//}
			//inline	MOB_ITER	GetIter()	// 현재 이터레이터를 반환
			//{
			//	return m_viMob;
			//}
			//inline	void		SaveIter(MOB_ITER vIter)	// 현재 이터레이터를 저장
			//{
			//	m_vSaveIter	=	vIter;
			//}
			//inline	void	LoadIter()	// 저장된 이터레이터로 현재 이터레이터 변경
			//{
			//	m_viMob	=	m_vSaveIter;
			//}
			inline	CMonster*	MobVec( INT nIndex ) // 해당 인덱스를 가지는 주소를 반환
			{
				return m_vMob[nIndex];
			}
			inline	INT	IterBegin()	// 인덱스 처음을 반환
			{
				INT	nIndex	=	0;
				return nIndex;
			}
			inline	void	SetIterBengin()	// 인덱스 처음으로 변경
			{
				m_nIndex	=	0;
			}
			inline	void	IterInc()	// 인덱스 증가
			{
				m_nIndex++;
			}
			inline	INT	IterEnd()	// 인덱스 끝을 반환
			{
				INT	nIndex	=	m_vMob.size();
				return nIndex;
			}
			inline	INT	GetIter()	// 현재 인덱스 반환
			{
				return m_nIndex;
			}
			inline	void		SaveIter()	// 현재 인덱스 저장
			{
				m_nIndexSave	=	m_nIndex;
			}
			inline	void	LoadIter()	// 저장된 인덱스 현재 인덱스 변경
			{
				m_nIndex	=	m_nIndexSave;
			}
			inline	INT		GetVectorSize(void)
			{
				return m_nVectorSize;
			}
};
