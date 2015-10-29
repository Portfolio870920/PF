using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class TabSort : MonoBehaviour
{
    // 탭 구분 enum
    private enum ITEM_TYPE
    {
        ALL,
        WEAPON,
        RUNE,
        JEWEL,
        POTION
    };

    private int sorted = 0;

    private GameMGR GameMgr     = null;
    private Inven   Inventory   = null;

    void Start()
    {
        GameMgr     = GameObject.Find("GameMGR").GetComponent<GameMGR>();
        Inventory   = GameObject.Find("UI Root/Camera/Inventory").GetComponent<Inven>();

        // 기본 활성화 탭을 all로 설정
        ChangeSprite(ITEM_TYPE.ALL);
    }

    #region  탭 클릭 이벤트
    public void OnClickAll()
    {
        CheckandExecute(ITEM_TYPE.ALL);
    }

    public void OnClickWeapon()
    {
        CheckandExecute(ITEM_TYPE.WEAPON);
    }

    public void OnClickRuen()
    {
        CheckandExecute(ITEM_TYPE.RUNE);
    }

    public void OnClickJewel()
    {
        CheckandExecute(ITEM_TYPE.JEWEL);
    }

    public void OnClickPosion()
    {
        CheckandExecute(ITEM_TYPE.POTION);
    }
    #endregion

    // 그리드 모두 활성화 함수
    void RollBackList()
    {
        // 인벤토리 리스트 내 모든 오브젝트 활성화
        for (int i = 0; i < GameMgr.Inventory_ObjectList.Count; ++i)
        {
            GameMgr.Inventory_ObjectList[i].SetActive(true);
        }
        #region foreach 버전
        //foreach (GameObject _object in GameMgr.Inventory_ObjectList)
        //{
        //    _object.SetActive(true);
        //}
        #endregion

        // 스크롤뷰 및 그리드 재정렬
        Inventory.Repositioning();
    }

    // 그리드 정렬 함수
    public void ListSort(byte _TYPE)
    {
        // all일 경우 모두 활성
        if (_TYPE == (byte)ITEM_TYPE.ALL)
        {
            RollBackList();
            return;
        }

        // 타입이 다른 자식들 모두 비활성화
        for(int i = 0; i < GameMgr.Inventory_ObjectList.Count; ++i)
        {
            GameMgr.Inventory_ObjectList[i].SetActive(true);
            if (GameMgr.Inventory_ObjectList[i].GetComponent<ItemInfo>().Type != _TYPE)
            {
                GameMgr.Inventory_ObjectList[i].SetActive(false);
            }
        }
        #region foreach 버전
        //foreach (GameObject _object in GameMgr.Inventory_ObjectList)
        //{
        //    _object.SetActive(true);
        //    if (_object.GetComponent<ItemInfo>().Type != _TYPE)
        //    {
        //        _object.SetActive(false);
        //    }
        //}
        #endregion

        // 스크롤뷰 및 그리드 재정렬
        Inventory.Repositioning();
    }

    // enum 값에 따라 정렬 실행
    bool CheckandExecute(ITEM_TYPE _TYPE)
    {
        if( sorted == (int)_TYPE)
        {
            return false;
        }

        ListSort((byte)_TYPE);
        sorted = (int)_TYPE;
        ChangeSprite(_TYPE);

        return true;
    }

    // 탭 스프라이트 변경
    void ChangeSprite(ITEM_TYPE _TYPE)
    {
        for( int i = 0; i < transform.childCount; i++)
        {
            if( i == (int)_TYPE)
            {
                transform.GetChild(i).GetComponent<UIButton>().normalSprite = "TabBG_select";
            }
            else
            {
                transform.GetChild(i).GetComponent<UIButton>().normalSprite = "TabBG";
            }
        }
    }
}
