#include "MainHeader.h"

cKey		*cKey::m_pInstance	=	NULL;

cKey::cKey(void)
{
	ZeroMemory( m_bKeyDown, KEYMAX );
	ZeroMemory( m_bKeyUp, KEYMAX );
}

cKey::~cKey(void)
{
}

cKey*	cKey::GetInst(VOID)
{
	if( m_pInstance == NULL )
	{
		m_pInstance	=	new cKey;
		RSCMGR->m_bKEY_MGR	=	true;
	}

	return m_pInstance;
}
VOID	cKey::FreeInst(VOID)
{
	if( m_pInstance != NULL )
	{
		delete m_pInstance;
		m_pInstance	=	NULL;
		RSCMGR->m_bKEY_MGR	=	false;
	}
}

//	���� ����
bool	cKey::StayKeyDown( INT nKey )
{
	if( GetAsyncKeyState( nKey ) & 0x8000 )
	{
		return TRUE;
	}

	return FALSE;
}

//	�ѹ��� �ٿ� Ű
bool	cKey::OnceKeyDown( INT nKey )
{
	if( GetAsyncKeyState( nKey ) & 0x8000 )
	{
		//	�ѹ� ��������?
		if( m_bKeyDown[nKey] == false )
		{
			m_bKeyDown[nKey]	=	true;
			return TRUE;
		}
	}

	else
	{
		//	Ŭ����
		m_bKeyDown[nKey]	=	false;
	}	

	return FALSE;
}

//	�ѹ��� �� Ű
bool	cKey::OnceKeyUp( INT nKey )
{
	if( GetAsyncKeyState( nKey ) & 0x8000 )
	{	
		m_bKeyUp[nKey]	=	true;
	}

	else
	{
		//	������ �־���?
		if( m_bKeyUp[nKey] == true )
		{
			//	Ŭ����
			m_bKeyUp[nKey]	=	false;

			return TRUE;
		}
	}	

	return FALSE;
}

//	���Ű
bool	cKey::IsToggleKey( INT nKey )
{
	if( GetKeyState(nKey) & 0x0001 )
	{
		return TRUE;
	}

	return FALSE;
}