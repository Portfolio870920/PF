using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class GameMGR : MonoBehaviour 
{
                List<GameObject>            SlimeList           = null;
    public      int                         MaxSlime            = 2;
                GameObject                  playerObj           = null;
                CharactorControl            player              = null;
    public      GameObject                  SlimePrefab         = null;
    public      float                       RebornTime          = 3;
    public      List<Transform>             RegenPoint          = null;
    public      bool                        GameEnd             = false;
    public      int                         AmountofGold        = 0;
                UILabel                     GoldLabel           = null;

    public      GameObject                  HUDText_prefab;
    public      GameObject                  hudroot             = null;
                HUDText                     playerdmgtext       = null;
    public      GameObject                  GameOverLabel       = null;
    public      GameObject                  GameOverPlane       = null;

    void Awake()
    {
        // 몬스터 추가
        SlimeList = new List<GameObject>();

        //for (int i = 0; i < MaxSlime; ++i)
        //{
        //    //SlimeList.Add(new GameObject());
        //    SlimeList.Add(null);
        //}
        
        // 몬스터 생성 코루틴 시작
        StartCoroutine("SlimeGeneration");

        // 플레이어 정보 추출
        playerObj = GameObject.Find("PLAYER/Cha_Knight");
        player = playerObj.GetComponent<CharactorControl>();

        // Find and Set Amount of gold label
        GoldLabel = GameObject.Find("UI Root/UICam/UIPanel/Amount_of_Gold/Amount").GetComponent<UILabel>();
    }

	// Use this for initialization
	void Start () 
    {
        GameObject Temp = NGUITools.AddChild(hudroot, HUDText_prefab);
        Temp.name = "Player_Text";
        playerdmgtext = Temp.GetComponentInChildren<HUDText>();
        Temp.AddComponent<UIFollowTarget>().target = playerObj.transform;
        Temp.GetComponent<UIFollowTarget>().top_level = 1;
	}
	
	// Update is called once per frame
	void Update () 
    {
        // test
        //Vector3 pos = playerObj.transform.position;
        //Debug.Log("플레이어 위치 x : " + pos.x + ", y : " + pos.y + ", z : " + pos.z);
        
        SlimeRegen();
        Update_AmountofGold();
        //Debug.Log("gold : " + AmountofGold);
        if( GameEnd)
        {
            if( !GameOverLabel.active && !GameOverPlane.active)
            {
                GameOverPlane.SetActive(true);
                GameOverLabel.SetActive(true);
            }
            if( GameOverLabel.GetComponent<UILabel>().color.a <= 255f)
            {
                Color Temp = GameOverLabel.GetComponent<UILabel>().color;
                Temp.a += 1f * Time.deltaTime;
                GameOverLabel.GetComponent<UILabel>().color = Temp;
            }

            if( GameOverPlane.GetComponent<MeshRenderer>().material.color.a <= 127f)
            {
                Color Temp = GameOverPlane.GetComponent<MeshRenderer>().material.color;
                Temp.a += 1f * Time.deltaTime;
                GameOverPlane.GetComponent<MeshRenderer>().material.color = Temp;
            }

            
        }
	}

    // 몬스터리스트에서 죽은 몬스터를 부활 대기시간 이후 부활시킴
    void SlimeRegen()
    {
        for (int i = 0; i < SlimeList.Count; ++i)
        {
            if (SlimeList[i] != null && SlimeList[i].GetComponent<Slime>() != null && SlimeList[i].active == false)
            {
                SlimeList[i].GetComponent<Slime>().DeadTimmer += Time.deltaTime;
                if (SlimeList[i].GetComponent<Slime>().DeadTimmer >= RebornTime)
                {
                    SlimeList[i].SetActive(true);
                    SlimeList[i].transform.position = RegenPoint[Random.Range(0, RegenPoint.Count)].position;
                    SlimeList[i].GetComponent<Slime>().ResetState();
                }
            }
        }
    }


    // 몬스터가 플레이어를 공격할 때
    public void SlimeAttack(int _Damage)
    {
        player.CurrentHP -= _Damage;
        player.GetComponent<Animation>().CrossFade("Damage",0.2f);
        playerdmgtext.Add( -_Damage, Color.red, 0f);
    }

    // 플레이어가 몬스터를 공격할 때
    public void PlayerAttack(int _Damage)
    {
        // 사거리 체크
        for (int i = 0; i < SlimeList.Count; ++i)
        {
            if (SlimeList[i].GetComponent<Slime>() != null && SlimeList[i].GetComponent<Slime>().CurrentHP >= 0)
            {
                if (Vector3.Distance(SlimeList[i].transform.position, player.transform.position) <= player.AttackRange &&
                    Vector3.Angle(player.transform.position, SlimeList[i].transform.position) <= 45.0f)
                {
                    //child.AddComponent<UIFollowTarget>().target = SlimeList[i].transform;
                    SlimeList[i].GetComponent<Slime>().DMGText.Add( -_Damage, new Color(1,0.647f,0), 0f);
                    SlimeList[i].GetComponent<Slime>().CurrentHP -= _Damage;
                    SlimeList[i].GetComponent<Rigidbody>().AddForce( (-SlimeList[i].transform.forward + SlimeList[i].transform.up) * 100.0f);
                    Instantiate(Resources.Load("Prefab/Hit"), SlimeList[i].transform.position, Quaternion.identity);
                }
            }
        }
    }

    // 몬스터 활성화 코루틴
    IEnumerator SlimeGeneration()
    {
        for (int i = 0; i < MaxSlime; ++i)
        {
            SlimeList.Add((GameObject)Instantiate(SlimePrefab, RegenPoint[Random.Range(0, RegenPoint.Count)].position, Quaternion.identity));
            SlimeList[i].name += (i + 1);
            SlimeList[i].GetComponent<Slime>().HPBar.name = SlimeList[i].name + "_HP_Bar";
            SlimeList[i].SetActive(true);
            float WaitTime = Random.Range(3f, 6f);

            GameObject Temp = NGUITools.AddChild(hudroot, HUDText_prefab);
            Temp.name = SlimeList[i].name + "_Text";
            SlimeList[i].GetComponent<Slime>().DMGText = Temp.GetComponentInChildren<HUDText>();
            Temp.AddComponent<UIFollowTarget>().target = SlimeList[i].transform;
            Temp.GetComponent<UIFollowTarget>().top_level = 2;

            yield return new WaitForSeconds(WaitTime);
        }
    }

    public void PlayerTextPrint (object _Text, Color _Color)
    {
        playerdmgtext.Add(_Text, _Color, 0.0f);
    }

    public void Update_AmountofGold()
    {
        GoldLabel.text = AmountofGold.ToString();
    }
}
