#pragma once

typedef	vector<CMonster*>			MOB_VEC;	// ���͵��� �������� ��� ���� �θ��� ���� Ŭ������ �����ͷ� ������ ����
typedef	vector<CMonster*>::iterator	MOB_ITER;	// ���ͷ�����

enum	MOBTYPE	// ���� ������ ������ enum
{
	KNIGHT,
	TROLL,
	ALDUIN
};

class cMobMgr
{
	cMobMgr(void);
	static	cMobMgr*	MOBINST;	// �ν��Ͻ� ����
			MOB_VEC		m_vMob;		// �� ����
			MOB_ITER	m_viMob;	// �� ���� ���ͷ�����
			MOB_ITER	m_vSaveIter;	// ���ͷ����� ������ ����
			INT			m_nIndex;
			INT			m_nIndexSave;
			INT			m_nDieCount;
			INT			m_nVectorSize;

public:
	~cMobMgr(void);
	static	cMobMgr*	GetInst();
			void		FreeInst();
			void		Init();	// ���� �����Ǿ��ִ� ��� ���͸� �ʱ�ȭ ��Ŵ �ʱ�ȭ ��ǥ 100, 0, 100
			void		AddMob(MOBTYPE Type, D3DXVECTOR3 MobPos = D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3 MobScl = D3DXVECTOR3(0.0f,0.0f,0.0f), float MobAngle = 0.0f);
			// ���͸� �߰��Ѵ� �߰��� ���ÿ� �ʱ�ȭ ��
			void		Update();
			void		Render();
			INT			GetDieCount();
			//inline	MOB_ITER	IterBegin()	// ���ͷ����� ó���� ��ȯ
			//{
			//	MOB_ITER	viMob	=	m_vMob.begin();
			//	return viMob;
			//}
			//inline	void		SetIterBengin()	// ���ͷ����� ó������ ����
			//{
			//	m_viMob	=	m_vMob.begin();
			//}
			//inline	MOB_ITER	IterInc()	// ���ͷ����� ����
			//{
			//	m_viMob++;
			//	return m_viMob;
			//}
			//inline	MOB_ITER	IterEnd()	// ���ͷ����� ���� ��ȯ
			//{
			//	MOB_ITER	viMob	=	m_vMob.end();
			//	return viMob;
			//}
			//inline	MOB_ITER	GetIter()	// ���� ���ͷ����͸� ��ȯ
			//{
			//	return m_viMob;
			//}
			//inline	void		SaveIter(MOB_ITER vIter)	// ���� ���ͷ����͸� ����
			//{
			//	m_vSaveIter	=	vIter;
			//}
			//inline	void	LoadIter()	// ����� ���ͷ����ͷ� ���� ���ͷ����� ����
			//{
			//	m_viMob	=	m_vSaveIter;
			//}
			inline	CMonster*	MobVec( INT nIndex ) // �ش� �ε����� ������ �ּҸ� ��ȯ
			{
				return m_vMob[nIndex];
			}
			inline	INT	IterBegin()	// �ε��� ó���� ��ȯ
			{
				INT	nIndex	=	0;
				return nIndex;
			}
			inline	void	SetIterBengin()	// �ε��� ó������ ����
			{
				m_nIndex	=	0;
			}
			inline	void	IterInc()	// �ε��� ����
			{
				m_nIndex++;
			}
			inline	INT	IterEnd()	// �ε��� ���� ��ȯ
			{
				INT	nIndex	=	m_vMob.size();
				return nIndex;
			}
			inline	INT	GetIter()	// ���� �ε��� ��ȯ
			{
				return m_nIndex;
			}
			inline	void		SaveIter()	// ���� �ε��� ����
			{
				m_nIndexSave	=	m_nIndex;
			}
			inline	void	LoadIter()	// ����� �ε��� ���� �ε��� ����
			{
				m_nIndex	=	m_nIndexSave;
			}
			inline	INT		GetVectorSize(void)
			{
				return m_nVectorSize;
			}
};
