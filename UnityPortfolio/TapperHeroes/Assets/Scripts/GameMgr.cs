using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Text;


public class GameMgr : MonoBehaviour
{
    public GameObject Player = null;
    public Transform Monster = null;
    public List<GameObject> TextList = null;
    public bool OnDebuging = false;
    public bool IsGamePlaying = true;    
    public uint WeaponLevel = 1;
    public uint Damage = 1;
    public float AttackTimer = 30.0f;
    public float CriticalPercent = 0;

    uint StageNumber = 1;
    uint lastStageNumber = 0;
    uint Gold = 0;
    uint ActivatedHeroCount = 0;
    float DamageRand = 0;
    float CurrentTimer = 0;
    

    void Awake()
    {
        DamageRand = Damage * 0.05f;
        ResetTimer();
        if (Player == null)
        {
            Player = GameObject.Find("FreeCharacter_model");
        }
        if (TextList.Count == 0)
        {
            GameObject Temp = GameObject.Find("UI Root/UICam/Panel/DMGText");
            int ChildCount = Temp.transform.childCount;
            for( int i = 0; i < ChildCount; i++)
            {
                TextList.Add(Temp.transform.GetChild(i).gameObject);
            }
        }
        if( Monster == null)
        {
            Monster = GameObject.Find("Monster").transform;
        }
    }

    public void PlayEffect(Vector3 _Position, string _EffectName)
    {
        string Path = string.Concat("PreFab/Effect/", _EffectName);

        GameObject Temp = NGUITools.AddChild(gameObject, (GameObject)Resources.Load(Path));
        Temp.transform.position = UICamera.currentCamera.ScreenToWorldPoint(_Position);
    }

    void LoadData()
    {

    }

    void SaveData()
    {

    }

    void OnApplicationQuit()
    {

    }

    void OnApplicationPause()
    {
        IsGamePlaying = true;
    }

    public void Attack()
    {
        uint CurrntDamage = Damage + (uint)DamageRand;
        if( Random.Range(0.0f, 100.0f) <= CriticalPercent)
        {
            CurrntDamage = (uint)(CurrentTimer * 1.5f);
        }

        DamagePrint(CurrntDamage);
    }

    void DamagePrint(uint _Damage)
    {
        foreach(GameObject _object in TextList)
        {
            if(!_object.active)
            {
                _object.SetActive(true);
                _object.GetComponent<DamageText>().Init(_Damage, Monster.position);
                return;
            }
        }
    }

    void ChangeEnvironment()
    {

    }

    void ResetTimer()
    {
        CurrentTimer = AttackTimer;
    }
}
