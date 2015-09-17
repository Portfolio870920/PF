//#include "pSystem.h"

#include <cstdlib>

#include "MainHeader.h"

BoundingBox::BoundingBox()
{
	// infinite small 
	_min.x = FLT_MAX;
	_min.y = FLT_MAX;
	_min.z = FLT_MAX;

	_max.x = -FLT_MAX;
	_max.y = -FLT_MAX;
	_max.z = -FLT_MAX;
}

bool BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if( p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z )
	{
		return true;
	}
	else
	{
		return false;
	}
}

BoundingSphere::BoundingSphere()
{
	_radius = 0.0f;
}

//파티클에는 랜덤이 필요하므로 함수2개 추가
//랜덤함수 srand()를 이용해 난수발생기의 씨를 지정하는 과정을 잊지말도록
//[lowBound,highBound]범위에 있는 임의의 float을 리턴
float GetRandomFloat(float lowBound, float highBound)
{
	if( lowBound >= highBound ) //잘못된 입력
		return lowBound;

	//[0,1]범위의 임의 float을 얻는다.
	float f = (rand() % 10000) * 0.0001f; 

	//[lowBound,highBound]범위의 float을 리턴한다. 
	return (f * (highBound - lowBound)) + lowBound; 
}
//최소점 min과 최대점 max로 정의된 상자 내의 임의 벡터를 출력한다.
void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

//float을 DWORD로 형변환, SetRenderState가 float이 아닌 DWORD를 인수로 받아서 필요
DWORD FtoDw(float f)
{
	return *((DWORD*)&f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//디폴트 값을 초기화하는 생성자
PSystem::PSystem()
{
	_device = 0;
	_vb     = 0;
	_tex    = 0;
}
//버텍스버퍼 텍스처를 해제하는 파괴자
PSystem::~PSystem()
{
	_vb->Release();
	_tex->Release();
}
//포인트 스프라이트를 저장하기 위한 버텍스 버퍼를 만들고 텍스처를 만드는 등의 초기화 작업을 처리
bool PSystem::init(IDirect3DDevice9* device, char* texFileName)
{
	_device = device; // save a ptr to the device

	HRESULT hr = 0;
	
	//동적 버텍스 버퍼
	//매 프레임마다 파티클을 갱신해야 하며 이는 곧 버텍스 버퍼의 메모리에 접근해야 함을 의미 	
	// D3DPOOL_MANAGED은 D3DUSAGE_DYNAMIC와 함께 사용할 수없다. 
	//버텍스버퍼가 포인트 스프라이트를 보관할 것임을 지정하는 D3DUSAGE_POINTS를 이용했음을 확인
	//버텍스버퍼크기가 _vbSize에 의해 미리 정의되며 시스템 내의 파티클 수와는 관련이 없음을 주의
	//이는 파티클 시스템의 렌더링이 한번에 수행되지 않고 단계적으로 이루어지기 때문
	hr = device->CreateVertexBuffer(_vbSize * sizeof(Particle), 
									D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, 
									CUSTOM_PARTICLE, 
									D3DPOOL_DEFAULT, 
									&_vb, 
									0);
	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexBuffer() - FAILED", "PSystem", 0);
		return false;
	}

	//텍스처
	hr = D3DXCreateTextureFromFile(device, texFileName, &_tex);

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateTextureFromFile() - FAILED", "PSystem", 0);
		return false;
	}

	return true;
}

//시스템 내의 모든 파티클 속성을 리셋
void PSystem::reset()
{
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		//파티클의 속성이 리셋되는 방식은 파티클 시스템에 따라 달라지므로 
		//자식클래스에서 함수를 구현하도록 순수가상함수로 선언
		resetParticle( &(*i) );
	}
}

//시스템에 파티클을 추가. 리스트에 추가하기 전에 파티클을 초기화 하는데 resetParticle()을 이용
void PSystem::addParticle()
{
	Attribute attribute;

	resetParticle(&attribute);

	_particles.push_back(attribute);
}

//포인트 스프라이트의 동작들은 거의 대부분 렌더상태에 의해 제어된다.
//초기 렌더상태를 지정. 시스템에 따라 달라질 수 있으므로 가상함수로 선언.
void PSystem::preRender()
{
	_device->SetRenderState(D3DRS_LIGHTING, false);
	
	//true	현재지정된 전체 텍스처를 포인트 스프라이트의 텍스처 매핑에 이용할 것임을 의미
	//false	포인트 스프라이트의 텍스처 좌표로 지정한 텍셀을 이용해 포인트 스프라이트의 텍스처를 입혀야 하도록 지정
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	
	//true	포인트 스프라이트 크기는 카메라와의 거리에 따라 적절하게 조정됨
	//false	포인트 크기를 화면상의 픽셀 단위로 해석하도록 지정
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, true); 
	
	//포인트 스프라이트 크기를 지정
	_device->SetRenderState(D3DRS_POINTSIZE, FtoDw(_size));
	
	//포인트 스프라이트 최소 크기 지정
	_device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

	//포인트 스프라이트 최대 크기 지정
	//_device->SetRenderState(D3DRS_POINTSIZE_MAX, 최대크기);

	//이 세 개의 상수는 (카메라와 포인트 스프라이트)거리에 따라 포인트 스프라이트의 크기가 변하는 방법을 제어
	//다음 코드는 포인트 스프라이트가 작아지도록 포인트 스프라이트 거리 상수 값을 지정함
	_device->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));
		
	//텍스처의 알파를 이용.
	_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	//알파블렌딩을 활성화하여 텍스처의 알파 채널이 텍스처의 픽셀 투명도를 결정하도록 했음.
	//예를 들어 흰색의 원형과 검은색의 알파채널을 갖는 흰색 텍스처를 이용하면 흰색 원 모양의 파티클을 만들 수 있음.
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
//	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	//추가
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

//렌더 상태를 복구하는데 이용. 시스템에 따라 달라질 수 있으므로 가상함수로 선언.
void PSystem::postRender()
{
	_device->SetRenderState(D3DRS_LIGHTING,          true);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE,  false);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,  false);
	//추가
	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void PSystem::render()
{
	if( !_particles.empty() )
	{
		//렌더상태를 지정한다.
		preRender();
		
		_device->SetTexture(0, _tex);
		_device->SetFVF(CUSTOM_PARTICLE);
		_device->SetStreamSource(0, _vb, 0, sizeof(Particle));

		//버텍스버퍼를 벗어날 경우 처음부터 시작한다.
		if(_vbOffset >= _vbSize) _vbOffset = 0;

		Particle* v = 0;

		_vb->Lock(	_vbOffset * sizeof( Particle ), 
					_vbBatchSize * sizeof( Particle ), 
					(void**)&v, 
					_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		//모든 파티클이 렌더링될때까지
		std::list<Attribute>::iterator i;
		for(i = _particles.begin(); i != _particles.end(); i++)
		{
			if( i->_isAlive )
			{
				//한단계의 생존한 파티클을 다음 버텍스버퍼 세그먼트로 복사한다.
				v->_position = i->_position;
				v->_color    = (D3DCOLOR)i->_color;
				v++; // next element;

				numParticlesInBatch++; //단계카운터를 증가시킨다.

				//현재단계가 모두 채워져 있는가?
				if(numParticlesInBatch == _vbBatchSize) 
				{
					//버텍스버퍼로 복사된 마지막단계의 파티클들을 그린다.
					_vb->Unlock();

					_device->DrawPrimitive(D3DPT_POINTLIST, _vbOffset, _vbBatchSize);

					//단계가 그려지는 동안 다음단계를 파티클로 채운다.

					//다음단계의 처음 오프셋으로 이동한다.
					_vbOffset += _vbBatchSize; 

					//버텍스버퍼의 경계를 넘는 메모리로 오프셋을 설정하지 않는다.
					//경계를 넘을 경우 처음부터 시작한다.
					if(_vbOffset >= _vbSize) _vbOffset = 0;       

					_vb->Lock(	_vbOffset * sizeof( Particle ), 
								_vbBatchSize * sizeof( Particle ), 
								(void**)&v, 
								_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; //다음단계를 위한 리셋
				}	
			}
		}

		_vb->Unlock();

		//조건이 만족되지 못하여 마지막단계가 렌더링되지않는 경우가 발생할 수 있다.
		//일부만 채워진 단계는 바로 이곳에서 렌더링된다.
		if( numParticlesInBatch )
		{
			_device->DrawPrimitive(D3DPT_POINTLIST, _vbOffset, numParticlesInBatch);
		}

		//다음 블록
		_vbOffset += _vbBatchSize; 

		//렌더상태를 리셋한다.
		postRender();
	}
}

//현재 시스템에 파티클이 없는 경우 True를 리턴하며 그렇지 않으면 False를 리턴.
bool PSystem::isEmpty()
{
	return _particles.empty();
}

//시스템 내의 파티클이 모두 죽은 경우 True를 리턴하며 그렇지 않으면 False를 리턴.
//빈 상태		시스템 내에 파티클이 없는 상태 
//죽은 상태		파티클이 존재하지만 죽은 것으로 표시된 상태
bool PSystem::isDead()
{
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		//최소 하나의 파티클이라도 남아있다면 죽지 않았음을 리턴.
		if( i->_isAlive )
			return false;
	}
	//하나의 파티클도 없다면 죽었음을 리턴.
	return true;
}

//속성리스트 _particle을 검색하여 죽은 파티클을 리스트에서 제거
void PSystem::removeDeadParticles()
{
	std::list<Attribute>::iterator i;

	i = _particles.begin();

	while( i != _particles.end() )
	{
		if( i->_isAlive == false )
		{
			// erase는 다음 반복자를 리턴하므로 우리가 반복자를 증가시킬 필요가 없다.
			i = _particles.erase(i); 
		}
		else
		{
			i++; //살았을 경우 다음 파티클
		}
	}
}

//removeDeadParticles()
//일부 파티클 시스템에서는 죽은 파티클을 제거하기보다는 재활용하는 것이 효율적일 수 있다.
//즉, 파티클이 만들어지고 제거될때 할당과 해제를 반복하는 것이 아니라 간단하게 죽은 파티클을 리셋하여 새로운 파티클로 이용

//추가함수
//충돌판정함수
bool PSystem::isCollision(D3DXVECTOR3 vPos)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++) {
		//생존한 파티클만 충돌판정
		if( i->_isAlive) { 
			if ((vPos.x >= i->_position.x - 10.0f && vPos.x <= i->_position.x + 10.0f) && 
				(vPos.z >= i->_position.z - 10.0f && vPos.z <= i->_position.z + 10.0f)) return true;
		}
	}
}

//추가함수
//반복렌더가 아닌경우 키에 추가해줄 함수
//여러개 파티클을 제어 가능하게 하려면 이런식으로 애드파티클을 반복문으로 돌린다.
void PSystem::addParticles(int numParticles)
{
	for(int i = 0; i < numParticles; i++) addParticle();
}

//추가함수
//파티클이 끝난다음 연쇄적으로 다른 파티클을 그리는 함수
void PSystem::SerialParticles(PSystem** pParticle1, PSystem** pParticle2)
{
	std::list<Attribute>::iterator i;

	for ( i = _particles.begin(); i != _particles.end(); i++ ) {
		if( i->_isAlive) { 

			//(*pParticle1)->_origin.y = Terrain->GetHeight(i->_position.x, i->_position.y);
			
			if ( i->_position.y <= MAPMGR->GetHeight(i->_position.x, i->_position.z) + 0.3f
				&& i->_position.y >= MAPMGR->GetHeight( i->_position.x, i->_position.z ) - 0.3f ) {
				(*pParticle1)->_origin	= i->_position;
				(*pParticle1)->_origin.y = i->_position.y;	
				(*pParticle1)->addParticles(20);

				//(*pParticle2)->_origin = i->_position;
				//(*pParticle2)->_origin.y = 35.0f;
				//(*pParticle2)->addParticles(70);
				break;
			}
		}
	}

	//포인터로 받은 파티클의 포지션에 현재파티클 포지션을 대입시킨후 그 위치의 y좌표가 0이 되면 연쇄파티클을 터트린다.
}

//불꽃//주석완료////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//시스템 원천으로의 포인터와 시스템이 가진 파티클의 수를 받음
//시스템의 원천이란 불꽃이 폭발할 장소를 의미
Fire::Fire(D3DXVECTOR3* origin, int numParticles)
{
	_porigin        = origin;
	_size          = 5.0f;
	_vbSize        = 2048;
	_vbOffset      = 0;   
	_vbBatchSize   = 512; 

	//시작하자마자 그려진다. 주석처리하면 제어가능.
	//for(int i = 0; i < numParticles; i++) addParticle();
}

//순수가상함수 정의
//시스템 원천의 파티클을 초기화하고 구체 내에서 임의의 속도를 만들며,
//시스템 내의 파티클들은 임의의 컬러를 부여받는다.
//마지막으로 각 파티클들이 2초 동안 유지하도록 수명을 지정한다.
void Fire::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;
	attribute->_position = *_porigin;

	//y값 조절
	//attribute->_position.y += 5.0f;  

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f,  1.0f,  1.0f);

	GetRandomVector(&attribute->_velocity, &min, &max);
	
	//구체를 만들기 위한 정규화
	D3DXVec3Normalize(&attribute->_velocity, &attribute->_velocity);

	attribute->_velocity *= 5.0f;

	//attribute->_color = D3DXCOLOR(GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), 1.0f);
	attribute->_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	attribute->_age      = 0.0f;
	attribute->_lifeTime = 5.0f;
}

//순수가상함수 정의
//각 파티클의 위치를 갱신하고 수명을 초과한 파티클의 죽음을 처리한다.
//이 시스템은 죽은 파티클을 제거하지는 않는다는 데 주의하자.
//이것은 새로운 불꽃을 만들 때 기존의 죽은 Fire시스템을 재활용할 수 있기 때문.
void Fire::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++) {
		//생존한 파티클만 갱신
		if( i->_isAlive ) {
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;
			
			//죽인다.
			if(i->_age > i->_lifeTime) i->_isAlive = false;
		}
	}
	//추가하면 반복하지 않는다구
	removeDeadParticles();
}

void Fire::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime)*0.001f;

	update(timeDelta);

	if ( isDead() ) reset();

	lastTime = currTime;
}


//가상함수 재정의
//다른 블렌드 인수가 이용된다. 또한 깊이 버퍼로의 쓰기도 허용되지 않는다.
//디폴트 블렌드인수와 쓰기 여부를 변경하고자 한다면 프리렌더 포스트렌더를 오버라이드하면 된다.
//두 메서드 모두 부모메서드를 호출하고 있음을 확인하자.
void Fire::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//z버퍼 읽기는 가능하지만 쓰기는 허용하지 않는다.
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

//가상함수 재정의
void Fire::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

//입자총//주석완료///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//카메라로의 포인터를 받는다. 
//새로운 파티클을 만들때마다 카메라의 위치와 방향에 대한 정보가 필요하기 때문
//me캐릭터의 위치와 방향을 받도록 수정
Gun::Gun(D3DXVECTOR3* vPos, D3DXVECTOR3* vDir)
{
	_vPos			= vPos;
	_vDir			= vDir;

	_size           = 20.0f;
	_vbSize         = 2048;
	_vbOffset       = 0;  
	_vbBatchSize    = 512; 
}

//순수가상함수 정의
//파티클의 위치를 카메라의 현재 위치로 지정하고 파티클의 속도를
//카메라가 바라보고 있는 방향의 100배로 지정한다.
void Gun::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;

	//파티클 위치에 카메라 위치를 이용한다.
	attribute->_position	=	*_vPos;
	//me캐릭터에서 좀 떨어져서 발사되도록  
	attribute->_position	+=	*_vDir * 10.0f;//* 280.0f;
	//카메라보다 약간 아래쪽으로 이동해 총을 들고 있는 것 같은 효과를 얻는다.
	//attribute->_position.y -= 1.0f; 
	//attribute->_position.y +=	500.0f;

	
	//좌우상하로 뿌리게
	_vDir->x += GetRandomFloat(-0.05f, 0.05f);
	_vDir->y += GetRandomFloat(-0.05f, 0.05f);
	
	//카메라가 바라보는 방향으로 발사한다.
	//me한발에서 여러개가 줄줄이 나가도록 수정
	attribute->_velocity = *_vDir /* * 200.0f;*/ * GetRandomFloat(0.0f, 1.0f) * 300.0f;

	attribute->_color =   D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	attribute->_age      = 0.0f; 
	attribute->_lifeTime = 1.5f; //수명은 1초
}

//순수가상함수 정의
//파티클의 위치를 갱신하고 수명이 다한 경우 죽은 것으로 처리
//뒤에는 파티클 리스트를 검색하여 죽은 파티클을 제거한다.
void Gun::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		i->_position += i->_velocity * timeDelta;

		i->_age += timeDelta;

		if(i->_age > i->_lifeTime)	//수명이 다하면 죽인다.
			i->_isAlive = false;
	}
	removeDeadParticles();			//죽은 파티클을 비운다.
}

void Gun::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime)*0.001f;

	update(timeDelta);

	lastTime = currTime;
}


//함수추가
void Gun::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//z버퍼 읽기는 가능하지만 쓰기는 허용하지 않는다.
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

//함수추가
void Gun::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

//블레스트//반복불능////////////////////////////////////////////////////////////////////////////////////////////////////////////
blast::blast(D3DXVECTOR3* origin, int numParticles)
{
	_origin        = *origin;
	_size          = 50.0f;
	_vbSize        = 2048;
	_vbOffset      = 0;   
	_vbBatchSize   = 512; 

	//시작하자마자 그려진다. 주석처리하면 제어가능.
	//for(int i = 0; i < numParticles; i++) addParticle();
}

void blast::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;
	attribute->_position = _origin;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f,  0.0f,  1.0f);

	GetRandomVector(&attribute->_velocity, &min, &max);

	//구체를 만들기 위한 정규화
	D3DXVec3Normalize(&attribute->_velocity, &attribute->_velocity);

	attribute->_velocity *= 70.0f;

	//attribute->_color = D3DXCOLOR(GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), 1.0f);
	attribute->_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	attribute->_age      = 0.0f;
	attribute->_lifeTime = 1.0f;
}

void blast::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++) {
		//생존한 파티클만 갱신
		if( i->_isAlive ) {
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;

			//죽인다.
			if(i->_age > i->_lifeTime) i->_isAlive = false;
		}
	}
	//추가하면 반복하지 않는다구
	removeDeadParticles();
}

void blast::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime)*0.001f;

	update(timeDelta);

	if ( isDead() ) reset();

	lastTime = currTime;
}


void blast::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//z버퍼 읽기는 가능하지만 쓰기는 허용하지 않는다.
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void blast::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

//버프//_porigin////////////////////////////////////////////////////////////////////////////////////////////////////////////
//이상하게 _origin이 포인터가 되면 &D3DXVECTOR3()을 인수로 넣을시 반복이 않된다... 뭐지? 
buff::buff(D3DXVECTOR3* origin, int numParticles)
{
	_porigin        = origin;
	_size          = 5.0f;
	_vbSize        = 2048;
	_vbOffset      = 0;   
	_vbBatchSize   = 512; 

	for(int i = 0; i < numParticles; i++) addParticle();
}

void buff::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;
	attribute->_position = *_porigin;

	attribute->_position.y += 5.0f;  

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f, 0.0f, 1.0f);

	GetRandomVector(&attribute->_velocity, &min, &max);

	//구체를 만들기 위한 정규화
	D3DXVec3Normalize(&attribute->_velocity, &attribute->_velocity);

	attribute->_velocity *= 30.0f;

	//attribute->_color = D3DXCOLOR(GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), 1.0f);
	attribute->_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	attribute->_age      = 0.0f;
	attribute->_lifeTime = 0.7f;
}

void buff::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++) {
		//생존한 파티클만 갱신
		if( i->_isAlive ) {
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;

			//죽인다.
			if(i->_age > i->_lifeTime) i->_isAlive = false;
		}
	}
	//추가하면 반복하지 않는다구
	//removeDeadParticles();
}

void buff::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime) * 0.001f;

	update(timeDelta);

	if ( isDead() ) reset();
	
	lastTime = currTime;
}

void buff::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//z버퍼 읽기는 가능하지만 쓰기는 허용하지 않는다.
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void buff::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

//눈//주석완료////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//경계상자 구조체를 가리키는 포인터와 시스템 내 파티클의 수를 받음
//경계상자는 눈송이가 떨어질 부피를 정의하며 만약 눈송이가 이범위 밖으로
//벗어나면 즉시 소멸하고 다시 만들어진다.
Snow::Snow(BoundingBox* boundingBox, int numParticles)
{
	_boundingBox   = *boundingBox;
	_size          = 7.0f;
	_vbSize        = 2048;
	_vbOffset      = 0; 
	_vbBatchSize   = 512; 
	
	for(int i = 0; i < numParticles; i++) addParticle();
}

//순수가상함수 정의
//경계상자 내 임의의 x z좌표 위치에서 눈송이를 만들고 y좌표를 경계상자의 최상단과 같도록 지정.
//이어 눈송이에 속도를 부여하여 아래 방향으로 그리고 약간 왼쪽으로 떨어지도록 한다.
void Snow::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;

	//눈송이의 위치 지정을 위해 임의의 x, z 좌표를 얻는다.
	GetRandomVector(&attribute->_position, &_boundingBox._min, &_boundingBox._max);

	//y좌표는 랜덤으로 받지 않고 항상 경계상자의 최상단이 된다.
	attribute->_position.y = _boundingBox._max.y; 

	//눈송이는 아래쪽으로 떨어지며 약간 왼쪽을 향한다.
	attribute->_velocity.x = GetRandomFloat(0.0f, 1.0f) * -10.0f/*0.0f*/;
	attribute->_velocity.y = GetRandomFloat(0.0f, 1.0f) * -50.0f/*GetRandomFloat(0.8f, 1.0f) * -100.0f*/;
	attribute->_velocity.z = 0.0f;

	//흰색의 눈송이
	attribute->_color = WHITE;
}

//순수가상함수 정의
//파티클의 위치를 갱신하며 시스템의 경계상자를 벗어났는지 확인한다. 
//만약 경계상자를 벗어났다면 해당 파티클을 초기화한다.
void Snow::update(float timeDelta)
{
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		i->_position += i->_velocity * timeDelta;

		//포인트가 경계를 벗어났는가?
		if( _boundingBox.isPointInside( i->_position ) == false ) 
		{
			//경계를 벗어난 파티클을 재활용한다.
			resetParticle( &(*i) );
		}
	}
}

void  Snow::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime)*0.001f;

	update(timeDelta);

	lastTime = currTime;
}
