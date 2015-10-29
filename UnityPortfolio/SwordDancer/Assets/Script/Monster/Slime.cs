using UnityEngine;
using System.Collections;

public class Slime : MonoBehaviour 
{
    public      float                   SlimeHP                 = 50;
    public      float                   CurrentHP               = 50;
    public      int                     HPRandValue             = 10;
    public      int                     SlimeDamage             = 5;
    public      int                     DamageRandValue         = 3;
    public      float                   AttackRange             = 2f;
    public      float                   AttackSpeed             = 1.5f;
    public      float                   ChaseCancleTime         = 5.0f;
    public      float                   ChaseTime               = 0;
    public      float                   MoveSpeed               = 2.5f;
    public      float                   RotSpeed                = 100;
    public      Transform               myTarget;
    private     StateMachine<Slime>     state                   = null;
    public      Animation               Ani                     = null;
    public      float                   DeadTimmer              = 0;
    public      bool                    IsDead                  = false;
    public      GameMGR                 MGR;
    public      GameObject              HPBar;
    public      HUDText                 DMGText                 = null;
    public      NavMeshAgent            navi                    = null;



    void Awake()
    {
        // 게임 매니져 설정
        MGR = GameObject.Find("GameMGR").GetComponent<GameMGR>();
        // 초기 설정
        ResetState();
        // 애니메이션 컴포넌트 가져오기
        Ani = GetComponent<Animation>();
        //체력바 생성
        HPBar = (GameObject)Instantiate(Resources.Load("Prefab/MOBHPbar", typeof(GameObject)), new Vector3(0, 0, 0), Quaternion.identity);
        // 네비메쉬 설정
        navi = GetComponent<NavMeshAgent>();
        navi.acceleration = 1f;
        navi.speed = MoveSpeed;
        navi.angularSpeed = RotSpeed;
        navi.stoppingDistance = AttackRange;
    }

	// Use this for initialization
	void Start () 
    {
        
	}
	
	// Update is called once per frame
	void Update () 
    {
        state.Update();
        HPBarUpdate();        
	}

    // 체력바 업데이트
    void HPBarUpdate()
    {
        // 위치 재조정
        Vector3 Pos = Camera.main.WorldToViewportPoint(transform.position);
        Vector3 hpbarpos = GameObject.Find("UICam").GetComponent<Camera>().ViewportToWorldPoint(Pos);
        hpbarpos.y += 0.2f;
        HPBar.transform.position = new Vector3(hpbarpos.x, hpbarpos.y, 0);

        HPBar.transform.GetComponent<UISlider>().value = CurrentHP / SlimeHP;
    }
    
    // 상태변경
    public void ChangeState(FSM_State<Slime> _State)
    {
        state.ChangeState(_State);
    }

    void OnTriggerEnter (Collider _Other)
    {
        // 플레이어 접근시 move_state로 변경
        if( _Other.transform.tag == "Player")
        {
#if DEBUG
            Debug.Log("플레이어가 범위 내에 접근하였습니다.");
#endif
            myTarget = _Other.transform.FindChild("Cha_Knight");

            if( CheckRange())
            {
                ChangeState(State_Attack.Instance);
            }
            else
            {
                ChangeState(State_Move.Instance);
            }
        }
        else
        {
            return;
        }

    }

    public bool CheckRange()
    {
        if( Vector3.Distance(myTarget.transform.position, transform.position) <= AttackRange)
        {
            return true;
        }
        return false;
    }

    public bool CheckAngle()
    {
        if (Vector3.Dot(myTarget.transform.position, transform.position) >= 0.5f)
        {
            return true;
        }
        return false;
    }

    //public void AlphaChange(float _Value, float _Max)
    //{
    //    Color color = transform.GetComponent<Renderer>().material.color;
    //    color.a *= _Value;
    //    if (color.a > _Max)
    //        color.a = _Max;
    //    GetComponent<Renderer>().material.color = color;
    //}

    public void ResetState()
    {
        // 능력치 설정
        CurrentHP = Random.Range(SlimeHP, SlimeHP + HPRandValue);
        state = new StateMachine<Slime>();
        // 초기 상태 설정
        state.Initial_Setting(this, State_Move.Instance);
        // 타겟 null 설정
        myTarget = null;
    }

    public void CreateItem( int _MaxPotion, int _MaxGold)
    {
        for(int i = 0 ; i < _MaxPotion ; i++)
        {
            Instantiate(Resources.Load("Prefab/HPPotion"),
                new Vector3(transform.position.x + Random.Range(-2f, 2f), 0.5f, transform.position.z + Random.Range(-2f, 2f)),
                Quaternion.identity);
        }

        for (int i = 0; i < _MaxGold; i++)
        {
            Instantiate(Resources.Load("Prefab/Gold"), 
                new Vector3(transform.position.x + Random.Range(-2f, 2f), 0.5f, transform.position.z + Random.Range(-2f, 2f)),
                Quaternion.identity);
        }
    }


}
