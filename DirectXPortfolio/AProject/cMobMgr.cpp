#include "MainHeader.h"

cMobMgr*	cMobMgr::MOBINST	=	NULL;

cMobMgr::cMobMgr(void)
{
	//m_viMob	=	m_vMob.begin();	// Ŭ���� ������ ���ÿ� ���ͷ����ʹ� ������ ���ۺκ����� ����
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
			m_vMob.clear();			// ���Ͱ� �Ѹ����� �߰��Ǿ��� �ִٸ� ���͸� �ʱ�ȭ ��Ų��
		delete	MOBINST;
		MOBINST	=	NULL;
		RSCMGR->m_bMOB_MGR	=	false;
	}
}

//void	cMobMgr::Init()
//{
//	m_viMob	=	m_vMob.begin();		// ó������ �ʱ�ȭ ��Ű�� ���� ���ͷ����͸� ������ġ�� ���´�
//	while ( m_viMob	!= m_vMob.end() )	// ���� ���ͷ����Ͱ� ������ ���� �ƴҶ����� �ݺ�
//	{
//		(*m_viMob)->Init();			// ���� ���ͷ����Ͱ� ����Ű�� ����Ŭ������ �ʱ�ȭ ��Ų�� (�ڵ� �ʱ�ȭ ��ġ 100, 0, 100 ������ ����)
//									//(*m_viMob) ��뿡 ����
//		m_viMob++;					// ���� ��ġ�� ����Ű�� ���� ���ͷ����� ����
//	}
//}
void	cMobMgr::Init()
{
	m_nIndex	=	0;		// ó������ �ʱ�ȭ ��Ű�� ���� �ε����� ������ġ�� ���´�
	while ( m_nIndex	!= m_vMob.size() )	// ���� �ε����� ������ ���� �ƴҶ����� �ݺ�
	{
		(m_vMob[m_nIndex])->Init();			// ���� �ε����� ����Ű�� ����Ŭ������ �ʱ�ȭ ��Ų�� (�ڵ� �ʱ�ȭ ��ġ 100, 0, 100 ������ ����)
		//(m_vMob[m_nIndex]) ��뿡 ����
		m_nIndex++;					// ���� ��ġ�� ����Ű�� ���� �ε��� ����
	}
}

void	cMobMgr::AddMob( MOBTYPE Type, D3DXVECTOR3 MobPos, D3DXVECTOR3 MobScl, float MobAngle )	// ���͸� �߰��Ѵ� ���ڷ� ���� Ÿ��, ��ġ, ũ��, �� �� �޴´�
{
	CMonster*	CMOB;
	switch ( Type )	// �޾��� Ÿ�Կ� ���� ���Ϳ� ���� Ŭ������ �ٸ��� �����Ѵ�
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
	m_vMob.push_back(CMOB);	// push_back ��ɾ �̿��Ͽ� ���Ϳ� ����Ŭ������ �־��ش�.
	m_nVectorSize++;		// ���� ������ üũ ����
	//���ͷ�����
	//m_viMob	=	m_vMob.begin();	// ���ͷ����͸� ���� ������ġ�� ���´�
	//while ( m_viMob	!= m_vMob.end() - 1 )	// ���ͷ����Ͱ� ������ �� �ٷ� �� ���ͱ��� ������ ������Ų�� ( ���� �߰��� �׸��� ��ġ )
	//{
	//	m_viMob++;
	//}
	//if ( MobPos != D3DXVECTOR3(0.0f,0.0f,0.0f) || MobScl != D3DXVECTOR3(0.0f,0.0f,0.0f) || MobAngle != 0.0f ) // ���� ���ڰ� ����Ʈ ���ڰ� �ƴ� ���
	//{
	//	(*m_viMob)->Init( MobPos, MobScl, MobAngle ); // �ش� ���ڸ� ���� Ŭ���� init�� ����
	//}
	//else // ���� ���ڰ� ����Ʈ ������ ���
	//{
	//	(*m_viMob)->Init(); // ����Ʈ ���ڷ� ���� ��
	//}

	//�ε���
	m_nIndex	=	0;	// �ε����� ���� ������ġ�� ���´�
	while ( m_nIndex	!= m_vMob.size() - 1 )	// �ε����� ������ �� �ٷ� �� ���ͱ��� ������ ������Ų�� ( ���� �߰��� �׸��� ��ġ )
	{
		m_nIndex++;
	}
	if ( MobPos != D3DXVECTOR3(0.0f,0.0f,0.0f) || MobScl != D3DXVECTOR3(0.0f,0.0f,0.0f) || MobAngle != 0.0f ) // ���� ���ڰ� ����Ʈ ���ڰ� �ƴ� ���
	{
		(m_vMob[m_nIndex])->Init( MobPos, MobScl, MobAngle ); // �ش� ���ڸ� ���� Ŭ���� init�� ����
	}
	else // ���� ���ڰ� ����Ʈ ������ ���
	{
		(m_vMob[m_nIndex])->Init(); // ����Ʈ ���ڷ� ���� ��
	}
}

void	cMobMgr::Update()
{
	//m_viMob	=	m_vMob.begin(); // ���ͷ����͸� ���� ������ġ��
	//while ( m_viMob	!= m_vMob.end() )	// ���ͷ����Ͱ� ������ ���� �ɶ����� �ݺ�
	//{
	//	(*m_viMob)->Update();
	//	m_viMob++; // ���ͷ����� ����
	//}
	m_nIndex	=	0; // �ε����� ���� ������ġ��
	while ( m_nIndex != m_vMob.size() && m_nDieCount != m_nVectorSize )	// �ε����� ������ ���� �ɶ����� �ݺ�
	{
		(m_vMob[m_nIndex])->Update();
		if ( (m_vMob[m_nIndex])->GetPlay_Status().m_Status == STATUS_DIE )
			(m_vMob[m_nIndex])->SetDie_Staus(true);
		m_nIndex++; // �ε��� ����
	}
}
INT		cMobMgr::GetDieCount()
{
	INT		nDieCount	=	0;
	m_nIndex	=	0;
	while ( m_nIndex != m_vMob.size() )	// �ε����� ������ ���� �ɶ����� �ݺ�
	{
		if ( (m_vMob[m_nIndex])->GetPlay_Status().m_Dead	==	true )
		{
			nDieCount++;
		}
		m_nIndex++; // �ε��� ����
	}
	return nDieCount;
}

void	cMobMgr::Render()
{
	//m_viMob	=	m_vMob.begin();	// ���ͷ����͸� ���� ������ġ��
	//while ( m_viMob	!= m_vMob.end() )	// ���ͷ����Ͱ� ������ �P�� �ɴ���� �ݺ�
	//{
	//	(*m_viMob)->Render(); // ���� ���ͷ����Ͱ� ����Ű�� ���� Ŭ������ ����
	//	m_viMob++; // ���ͷ����� ����
	//}
	m_nIndex	=	0;	// �ε����� ���� ������ġ��
	while ( m_nIndex	!= m_vMob.size() )	// �ε����� ������ �P�� �ɴ���� �ݺ�
	{
		(m_vMob[m_nIndex])->Render(); // ���� �ε����� ����Ű�� ���� Ŭ������ ����
		m_nIndex++; // �ε��� ����
	}
}