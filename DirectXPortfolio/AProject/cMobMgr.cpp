#include "MainHeader.h"

cMobMgr*	cMobMgr::MOBINST	=	NULL;

cMobMgr::cMobMgr(void)
{
	//m_viMob	=	m_vMob.begin();	// 클래스 생성과 동시에 이터레이터는 벡터의 시작부분으로 놓음
	//m_vSaveIter	=	m_viMob;
	m_nIndex		=	0;
	m_nIndexSave	=	0;
	m_nDieCount		=	0;
	m_nVectorSize	=	0;
}

cMobMgr::~cMobMgr(void)
{
}

cMobMgr* cMobMgr::GetInst()
{
	if ( !MOBINST )
	{
		MOBINST	=	new cMobMgr;
		RSCMGR->m_bMOB_MGR	=	true;
	}
	return MOBINST;
}

void	cMobMgr::FreeInst()
{
	if ( MOBINST )
	{
		if(m_vMob.size() > 0)		//
			m_vMob.clear();			// 몬스터가 한마리라도 추가되어져 있다면 벡터를 초기화 시킨다
		delete	MOBINST;
		MOBINST	=	NULL;
		RSCMGR->m_bMOB_MGR	=	false;
	}
}

//void	cMobMgr::Init()
//{
//	m_viMob	=	m_vMob.begin();		// 처음부터 초기화 시키기 위해 이터레이터를 시작위치로 놓는다
//	while ( m_viMob	!= m_vMob.end() )	// 현재 이터레이터가 벡터의 끝이 아닐때까지 반복
//	{
//		(*m_viMob)->Init();			// 현재 이터레이터가 가르키는 몬스터클래스를 초기화 시킨다 (자동 초기화 위치 100, 0, 100 겹쳐짐 주의)
//									//(*m_viMob) 사용에 주의
//		m_viMob++;					// 다음 위치를 가르키기 위해 이터레이터 증가
//	}
//}
void	cMobMgr::Init()
{
	m_nIndex	=	0;		// 처음부터 초기화 시키기 위해 인덱스를 시작위치로 놓는다
	while ( m_nIndex	!= m_vMob.size() )	// 현재 인덱스가 벡터의 끝이 아닐때까지 반복
	{
		(m_vMob[m_nIndex])->Init();			// 현재 인덱스가 가르키는 몬스터클래스를 초기화 시킨다 (자동 초기화 위치 100, 0, 100 겹쳐짐 주의)
		//(m_vMob[m_nIndex]) 사용에 주의
		m_nIndex++;					// 다음 위치를 가르키기 위해 인덱스 증가
	}
}

void	cMobMgr::AddMob( MOBTYPE Type, D3DXVECTOR3 MobPos, D3DXVECTOR3 MobScl, float MobAngle )	// 몬스터를 추가한다 인자로 몬스터 타입, 위치, 크기, 각 을 받는다
{
	CMonster*	CMOB;
	switch ( Type )	// 받아진 타입에 따라 벡터에 넣을 클래스를 다르게 설정한다
	{
	case TROLL:
		break;
	case ALDUIN:
		CMOB	=	new CMobAlduin;
		break;
	case KNIGHT:
	default:
		CMOB	=	new cMobKnight;
	}
	m_vMob.push_back(CMOB);	// push_back 명령어를 이용하여 벡터에 몬스터클래스를 넣어준다.
	m_nVectorSize++;		// 벡터 사이즈 체크 증가
	//이터레이터
	//m_viMob	=	m_vMob.begin();	// 이터레이터를 벡터 시작위치로 놓는다
	//while ( m_viMob	!= m_vMob.end() - 1 )	// 이터레이터가 벡터의 끝 바로 전 벡터까지 가도록 증가시킨다 ( 새로 추가된 항목의 위치 )
	//{
	//	m_viMob++;
	//}
	//if ( MobPos != D3DXVECTOR3(0.0f,0.0f,0.0f) || MobScl != D3DXVECTOR3(0.0f,0.0f,0.0f) || MobAngle != 0.0f ) // 받은 인자가 디폴트 인자가 아닐 경우
	//{
	//	(*m_viMob)->Init( MobPos, MobScl, MobAngle ); // 해당 인자를 몬스터 클래스 init에 적용
	//}
	//else // 받은 인자가 디폴트 인자일 경우
	//{
	//	(*m_viMob)->Init(); // 디폴트 인자로 들어가게 함
	//}

	//인덱스
	m_nIndex	=	0;	// 인덱스를 벡터 시작위치로 놓는다
	while ( m_nIndex	!= m_vMob.size() - 1 )	// 인덱스가 벡터의 끝 바로 전 벡터까지 가도록 증가시킨다 ( 새로 추가된 항목의 위치 )
	{
		m_nIndex++;
	}
	if ( MobPos != D3DXVECTOR3(0.0f,0.0f,0.0f) || MobScl != D3DXVECTOR3(0.0f,0.0f,0.0f) || MobAngle != 0.0f ) // 받은 인자가 디폴트 인자가 아닐 경우
	{
		(m_vMob[m_nIndex])->Init( MobPos, MobScl, MobAngle ); // 해당 인자를 몬스터 클래스 init에 적용
	}
	else // 받은 인자가 디폴트 인자일 경우
	{
		(m_vMob[m_nIndex])->Init(); // 디폴트 인자로 들어가게 함
	}
}

void	cMobMgr::Update()
{
	//m_viMob	=	m_vMob.begin(); // 이터레이터를 벡터 시작위치로
	//while ( m_viMob	!= m_vMob.end() )	// 이터레이터가 벡터의 끝이 될때까지 반복
	//{
	//	(*m_viMob)->Update();
	//	m_viMob++; // 이터레이터 증가
	//}
	m_nIndex	=	0; // 인덱스를 벡터 시작위치로
	while ( m_nIndex != m_vMob.size() && m_nDieCount != m_nVectorSize )	// 인덱스가 벡터의 끝이 될때까지 반복
	{
		(m_vMob[m_nIndex])->Update();
		if ( (m_vMob[m_nIndex])->GetPlay_Status().m_Status == STATUS_DIE )
			(m_vMob[m_nIndex])->SetDie_Staus(true);
		m_nIndex++; // 인덱스 증가
	}
}
INT		cMobMgr::GetDieCount()
{
	INT		nDieCount	=	0;
	m_nIndex	=	0;
	while ( m_nIndex != m_vMob.size() )	// 인덱스가 벡터의 끝이 될때까지 반복
	{
		if ( (m_vMob[m_nIndex])->GetPlay_Status().m_Dead	==	true )
		{
			nDieCount++;
		}
		m_nIndex++; // 인덱스 증가
	}
	return nDieCount;
}

void	cMobMgr::Render()
{
	//m_viMob	=	m_vMob.begin();	// 이터레이터를 벡터 시작위치로
	//while ( m_viMob	!= m_vMob.end() )	// 이터레이터가 벡터의 긑이 될대까지 반복
	//{
	//	(*m_viMob)->Render(); // 현재 이터레이터가 가르키는 몬스터 클래스를 렌더
	//	m_viMob++; // 이터레이터 증가
	//}
	m_nIndex	=	0;	// 인덱스를 벡터 시작위치로
	while ( m_nIndex	!= m_vMob.size() )	// 인덱스가 벡터의 긑이 될대까지 반복
	{
		(m_vMob[m_nIndex])->Render(); // 현재 인덱스가 가르키는 몬스터 클래스를 렌더
		m_nIndex++; // 인덱스 증가
	}
}