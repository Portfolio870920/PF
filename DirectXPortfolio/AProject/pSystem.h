#ifndef __pSystemH__
#define __pSystemH__

#include "D3Dx9.h"

#include <list>
#include <string>
#include <limits>

const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

struct BoundingBox
{
	BoundingBox();

	bool isPointInside(D3DXVECTOR3& p);

	D3DXVECTOR3 _min;
	D3DXVECTOR3 _max;
};

struct BoundingSphere
{
	BoundingSphere();

	D3DXVECTOR3 _center;
	float       _radius;
};

//파티클에는 랜덤이 필요하므로 함수2개 추가
float GetRandomFloat(float lowBound, float highBound);
void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max);
//float을 DWORD로 형변환, SetRenderState가 float이 아닌 DWORD를 인수로 받아서 필요
DWORD FtoDw(float f);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//파티클의 위치와 컬러를 표현하는데는 다음과 같은 버텍스 구조체가 이용됨
//파티클 단위의 크기제어가 가능하려면 그래픽카드가 D3DFVFCAPS_PSIZE가 지원되거나 버텍스 셰이더를 이용해야한다.
struct Particle
{
	D3DXVECTOR3 _position;
	D3DCOLOR    _color;
};
#define CUSTOM_PARTICLE		D3DFVF_XYZ | D3DFVF_DIFFUSE

//파티클 속성
//부가적인 속성들은 파티클렌더링하는데 필수적인 것은 아니므로
//필요한 데이터와 파티클 속성을 별도의 구조체 내에 보관한다.
//파티클을 만들고 파괴하고 갱신할때는 이 속성을 이용해 작업하며 
//렌더링할 준비가 완료되면 Particle 구조체로 위치와 속성을 복사해 이용
struct Attribute
{
	Attribute()
	{
		_lifeTime = 0.0f;
		_age      = 0.0f;
		_isAlive  = true;
	}

	//파티클 속성은 모델링하려는 파티클 시스템 종류에 따라 달라지지만
	//약간의 일반화를 거치면 몇 가지 공통적인 속성을 가려낼 수 있다.
	//대부분의 시스템에서는 이들 속성을 모두 이용하지 않으며 일부 시스템
	//에서는 여기에 포함되지 않은 부가적인 속성을 필요로 한다.

	D3DXVECTOR3 _position;		// 월드 스페이스 내의 파티클 위치
	D3DXVECTOR3 _velocity;		// 파티클 속도, 초당 이동단위로 기록함
	D3DXVECTOR3 _acceleration;	// 파티클 가속, 초당 이동단위로 기록함
	float       _lifeTime;		// 파티클이 소멸할때까지 유지되는 시간  
	float       _age;			// 파티클 현재 나이  
	D3DXCOLOR   _color;			// 파티클 컬러   
	D3DXCOLOR   _colorFade;		// 파티클 컬러가 시간의 흐름에 따라 퇴색하는 방법
	bool        _isAlive;		// 파티클이 생존한 경우 True 소멸할 경우 False
};

//파티클 시스템
//파티클들의 모임이며 파티클을 보여주고 관리하는 역할을 담당
//파티클 갱신과 디스플레이, 소멸, 생성 등을 관장하는 역할
class PSystem
{
public:

	PSystem();
	virtual ~PSystem();

	virtual bool init(IDirect3DDevice9* device, char* texFileName);
	virtual void reset();
	
	//파티클의 속성이 리셋되는 방식은 파티클 시스템에 따라 달라지므로 자식클래스에서 함수를 구현하도록 순수가상함수로 선언
	virtual void resetParticle(Attribute* attribute) = 0;
	virtual void addParticle();

	//시스템 내의 모든 파티클들을 갱신. 특정 파티클 시스템에 따라 달라지므로 자식클래스에서 구현하도록 순수가상함수로 선언
	virtual void update(float timeDelta) = 0;
	//추가함수
	virtual void update() = 0;

	virtual void preRender();
	virtual void postRender();

	virtual void render();

	bool isEmpty();
	bool isDead();

	//추가함수
	bool isCollision(D3DXVECTOR3 vPos);
	void addParticles(int numParticles);
	void SerialParticles(PSystem** pParticle1, PSystem** pParticle2);

protected:
	virtual void removeDeadParticles();

protected:
	IDirect3DDevice9*       _device;
	D3DXVECTOR3	            _origin;		//시스템 내에서 파티클이 시작되는 곳
	BoundingBox				_boundingBox;	//경계상자는 파티클이 이동할 수 있는 부피를 제한하는데 이용됨 
											//경계상자로 정의하면 이 영역을 벗어난 파티클들은 곧바로 소멸.
	float                   _emitRate;		//시스템에 새로운 파티클이 추가되는 비율. 초당 파티클 수로 기록.
	float                   _size;			//시스템 내 모든 파티클의 크기
	IDirect3DTexture9*      _tex;			//텍스처
	IDirect3DVertexBuffer9* _vb;			//버텍스버퍼
	std::list<Attribute>    _particles;		//파티클을 만들고 제거하고 갱신하는데 이 리스트를 이용함.
	int                     _maxParticles;	//주어진 시간 동안 시스템이 가질 수 있는 최대 파티클의 수.

	//
	//다음 3가지 속성은 파티클시스템 효율적으로 렌더링하기 위함
	//

	DWORD _vbSize;      //버텍스 버퍼가 한번에 보관할 수 있는 파티클의 수. 
						//이 값은 실제 파티클 시스템 내의 파티클 수와는 독립적.
	DWORD _vbOffset;    //버텍스버퍼에서 복사를 시작할 파티클 내 다음 단계로의 오프셋(바이트가 아닌 파티클단위) 
	DWORD _vbBatchSize; //하나의 단계에 정의된 파티클의 수
};

///////////////////////////////////////////////////////////////////////////////////////////////////

//불꽃
class Fire : public PSystem
{
public:
	Fire(D3DXVECTOR3* origin, int numParticles);
	//순수가상함수 정의
	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();
	//가상함수 재정의
	void preRender();
	void postRender();
protected:
	//me요상하게 벡터로 넣으면 반복이 않되서 포인터를 따로 추가
	D3DXVECTOR3* _porigin;		
};

//입자총
class Gun : public PSystem
{
public:
	Gun(D3DXVECTOR3* vPos, D3DXVECTOR3* vDir);
	//순수가상함수 정의
	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();
	//추가함수
	void Gun::preRender();
	void Gun::postRender();

private:
	D3DXVECTOR3* _vPos;
	D3DXVECTOR3* _vDir;
};

//블레스트
class blast : public PSystem
{
public:
	blast(D3DXVECTOR3* origin, int numParticles);
	//순수가상함수 정의
	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();
	//가상함수 재정의
	void preRender();
	void postRender();
};

//버프
class buff : public PSystem
{
public:
	buff(D3DXVECTOR3* origin, int numParticles);

	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();

	void preRender();
	void postRender();
protected:
	//me요상하게 벡터로 넣으면 반복이 않되서 포인터를 따로 추가
	D3DXVECTOR3* _porigin;		
};

//눈
class Snow : public PSystem
{
public:
	Snow(BoundingBox* boundingBox, int numParticles);
	//순수가상함수 정의
	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();
};


#endif // __pSystemH__