using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class CharactorControl : MonoBehaviour {

    public  int             playerHP                = 100;
    public  int             CurrentHP               = 0;
    public  int             playerMP                = 100;
    public  int             CurrentMP               = 0;
    public  int             playerDamage            = 3;
    public  int             DamageRandValue         = 2;
    public  int             CriDamage               = 2;
    public  float           CriPercent              = 20f;
    public  float           moveSpeed               = 1.0f;
    public  float           rotationSpeed           = 1.0f;
    public  float           AttackRange             = 2.0f;
    public  GameMGR         MGR                     = null;
    public  UISlider        HPbar                   = null;
    public  UISlider        MPbar                   = null;
    public  bool            IsDead                  = false;
            Transform       Windparticle            = null;
            ParticleSystem  WindEffect              = null;
            float           CoolTime                = 1f;
            float           CurrentTime             = 1f;

    
    //public  GameObject  Target                  = null;

    void Awake()
    {
        // 게임 매니저 등록
        if( MGR == null)
        {
            MGR = GameObject.Find("GameMGR").GetComponent<GameMGR>();
        }
        
        // HP, MP설정
        CurrentHP = playerHP;
        CurrentMP = playerMP;
    }

	// Use this for initialization
	void Start () 
    {

	}
	
	// Update is called once per frame
	void Update () 
    {
        // hp판단
        if (CurrentHP > 0)
        {
            if (Input.GetKey(KeyCode.Space))
            {
                Attack();
            }

            if (Input.GetKey(KeyCode.R))
            {
                Whirlwind();
                //StartCoroutine(Wirldwind());
            }
            
            if (Input.GetKeyUp(KeyCode.R))
            {
                Align();
            }

            if( CurrentMP < playerMP)
            {
                CurrentMP++;
            }
        }
        else
        {
            if (!IsDead)
            {
                GetComponent<Animation>().CrossFade("Dead");
                IsDead = true;
                MGR.GameEnd = true;
            }            
        }

        // HPMP바 조정부분
        float fHp = (float)CurrentHP / (float)playerHP;
        float fMP = (float)CurrentMP / (float)playerMP;
        HPbar.value = fHp;
        MPbar.value = fMP;        
	}

    public void Whirlwind()
    {
        // 소용돌이 이펙트 쿨
        CurrentTime += Time.deltaTime;
        Instantiate(Resources.Load("Prefab/CFXM_ElectricityBolt"), transform.position, Quaternion.AngleAxis(-90, Vector3.right) * transform.rotation);
        // 소용돌이가 연속으로 나올경우 눈이 아프기에 일정 쿨을 둠
        if (CoolTime <= CurrentTime)
        {
            Instantiate(Resources.Load("Prefab/CFXM_Tornado"), transform.position, Quaternion.AngleAxis(-90, Vector3.right));
            CurrentTime = 0;
        }
        CurrentMP -= 2;
        transform.rotation *= Quaternion.AngleAxis(40f * rotationSpeed, transform.up);
        Attack();
    }

    public void Align()
    {
        // 캐릭터 방향 재정렬
        CurrentTime = CoolTime;
        transform.LookAt(transform.parent.FindChild("Forward").transform);
    }

    void Attack()
    {
        CriDamage = (Random.Range(0, 100) <= CriPercent ? 2 : 1);
        MGR.PlayerAttack(Random.Range(playerDamage * CriDamage, (playerDamage + Random.Range(0, DamageRandValue)) * CriDamage));
        GetComponent<Animation>().CrossFade("Attack", 0.2f);
    }

    public void onAniEnd()
    {
        GetComponent<Animation>().CrossFade("Wait", 0.2f);
    }
}
