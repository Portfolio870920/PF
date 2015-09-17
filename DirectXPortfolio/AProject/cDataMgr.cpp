//#include "DataMgr.h"
#include "MainHeader.h"

CDataMgr* CDataMgr::pInstance = NULL;

CDataMgr::CDataMgr(void)
{
}

CDataMgr::~CDataMgr(void)
{
}

CDataMgr* CDataMgr::GetInst()
{
	if( !pInstance )
	{
		pInstance = new CDataMgr;
		RSCMGR->m_bDATA_MGR	=	true;
	}
	return pInstance;
}
void CDataMgr::FreeInst(void)
{
	if ( pInstance )
	{
		delete pInstance;
		pInstance	=	NULL;
		RSCMGR->m_bDATA_MGR	=	false;
	}
}

void CDataMgr::Save()
{
	if ( RSCMGR->m_bCHAR_MGR )
	{
		FILE* fp = fopen( "data.txt", "w" );

		if ( !fp )
		{
			//MessageBoxA( hwnd, "Data Error", "Warning.", MB_OK );
		}
		fprintf ( fp, "GAME_DATA\nHP		: %f\nPOS		: %f,%f,%f\nSTAGE	: %d"
			,CHARMGR->GetPlay_Status().m_HP, CHARMGR->GetPos().x, CHARMGR->GetPos().y, CHARMGR->GetPos().z, /*임시스테이지값*/1);

		fclose (fp);
	}
}

void CDataMgr::Load()
{
	FILE* fp = fopen( "data.txt", "r" );

	if ( !fp )
	{
		//MessageBoxA( hwnd, "Data Error", "Warning.", MB_OK );
		return;
	}
	ZeroMemory ( pBuffer, 1024 );

	fgets ( pBuffer, 1024, fp );
	ZeroMemory ( pBuffer, 1024 );

	fgets ( pBuffer, 1024, fp );
	//sscanf ( pBuffer, "HP		: %f\0", &pPlayer->m_Status1.m_HP );
	ZeroMemory ( pBuffer, 1024 );
	
	fgets ( pBuffer, 1024, fp );
	//sscanf ( pBuffer, "POS		: %f,%f,%f\0", &pPlayer->m_vPos.x, &pPlayer->m_vPos.y, &pPlayer->m_vPos.z );
	ZeroMemory ( pBuffer, 1024 );

	//fgets ( pBuffer, 1024, fp );
	//sscanf ( pBuffer, "STAGE	: %d\0", &stage);

	fclose (fp);
}