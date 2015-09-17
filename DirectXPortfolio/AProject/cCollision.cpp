#include "MainHeader.h"

cCollision::cCollision(void)
{
}

cCollision::~cCollision(void)
{
}

void cCollision::CollisionPos(D3DXVECTOR3* Player_Pos, D3DXVECTOR3* Enemy_Pos)
{
	fLen	=	D3DXVec3Length(&(*Player_Pos - *Enemy_Pos));
	if ( fLen < 50.0f )
	{
		fBackDist	=	50 - fLen;
		D3DXVECTOR3	vEnemyToMyDir;
		D3DXVec3Normalize( &vEnemyToMyDir, &(*Player_Pos - *Enemy_Pos));
		*Player_Pos = *Player_Pos + vEnemyToMyDir * fBackDist;
	}
}

D3DXVECTOR3 cCollision::CollisionCheck(D3DXVECTOR3 Player_Pos, D3DXVECTOR3 Enemy_Pos, float* fTargetAngle, D3DXVECTOR3 Enemy_Look)
{
	D3DXVECTOR3	vMonster_dir, vCross, New_EnemyPos;
	float fLen;
	
	vMonster_dir = ( Player_Pos - Enemy_Pos );			//	적이 플레이어를 바라보는 방향
	D3DXVec3Normalize( &vMonster_dir, &vMonster_dir );	//	그 방향 정규화
	D3DXVec3Normalize( &Enemy_Look, &Enemy_Look);		//	원래 방향 정규화
	fLen	=	D3DXVec3Length( &(Player_Pos - Enemy_Pos) );	//	플레이어와 몬스터의 거리
	New_EnemyPos	=	Enemy_Pos;						//	몬스터 원래 위치
	if( fLen < 50.0f )		//	범위
	{
		D3DXVec3Cross( &vCross, &vMonster_dir, &Enemy_Look);	//	적이 플레이어 바라보는 방향과 원래 방향으로 외적
		//D3DXVec3Normalize(&vCross, &vCross );
		if( vCross.y < 0.99f )		//	시야 각
		{
			*fTargetAngle	-=	vCross.y*0.045f;
			if(fLen > 18.0f)		//	공격 범위
			{
				New_EnemyPos.x += ( vMonster_dir.x * 0.5f );
				New_EnemyPos.z += ( vMonster_dir.z * 0.5f );
			}
			return New_EnemyPos;
		}
	}
	return Enemy_Pos;
}