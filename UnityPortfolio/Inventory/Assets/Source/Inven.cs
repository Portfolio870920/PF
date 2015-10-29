using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
using System.Text.RegularExpressions;

public class Inven : MonoBehaviour {

    #region PRIVATE

    private GameMGR GameMgr = null;
    private UIGrid Inventory_Grid = null;

    #endregion

    #region PUBLIC

    public GameObject Item_prefab = null;
    public int signe = 1;

    #endregion

    void Awake()
    {
        GameMgr = GameObject.Find("GameMGR").GetComponent<GameMGR>();
        Inventory_Grid = transform.FindChild("Grid").GetComponent<UIGrid>();
    }

    void Start()
    {
        // 인벤토리창이 활성화 되었을 때 실행하도록 Start에 배치
        Load_InventoryData();
    }

    void Load_InventoryData()
    {
        // CVS 데이터 로드
        List<Dictionary<string, object>> CVSData = ReadCVS("TestItem");

        // 인벤토리 리스트 작성
        for (int i = 0; i < CVSData.Count; ++i)
        {
            GameObject tempObject = NGUITools.AddChild(Inventory_Grid.gameObject, Item_prefab);

            tempObject.transform.localPosition = Vector3.zero;

            // 오브젝트 이름 설정 [Handle_Grade_Type]
            tempObject.name = CVSData[i]["Handle"] + "_" +
                CVSData[i]["grade"] + "_" +
                CVSData[i]["type"];

            // 스프라이트 설정
            tempObject.GetComponent<UISprite>().spriteName = CVSData[i]["icon"].ToString();

            // 등급 설정
            tempObject.transform.FindChild("Grade").GetComponent<UILabel>().text = "LV." + (byte)((int)(CVSData[i]["grade"]));

            // 아이템 정보 설정
            tempObject.GetComponent<ItemInfo>().Set_ItemInfo((int)CVSData[i]["Handle"],
                (int)CVSData[i]["id"],
                (int)CVSData[i]["attack"],
                (byte)((int)(CVSData[i]["type"])),
                (byte)((int)(CVSData[i]["grade"])),
                (int)CVSData[i]["max_count"],
                CVSData[i]["icon"].ToString());

            // 게임 매니저의 리스트에 추가
            GameMgr.Inventory_ObjectList.Add(tempObject);
        }
        #region foreach 버전
        //foreach (Dictionary<string, object> _Dictionary in CVSData)
        //{
        //    GameObject tempObject = NGUITools.AddChild(Inventory_grid, Item_prefab);

        //    tempObject.transform.localPosition = Vector3.zero;

        //    // Set Object name [Handle_Grade_Type]
        //    tempObject.name = _Dictionary["Handle"] + "_" +
        //        _Dictionary["grade"] + "_" +
        //        _Dictionary["type"];

        //    tempObject.GetComponent<UISprite>().spriteName = _Dictionary["icon"].ToString();

        //    tempObject.transform.FindChild("Grade").GetComponent<UILabel>().text = "LV." + (byte)((int)(_Dictionary["grade"]));

        //    // Set ItemInfomation
        //    tempObject.GetComponent<ItemInfo>().Set_ItemInfo((int)_Dictionary["Handle"],
        //        (int)_Dictionary["id"],
        //        (int)_Dictionary["attack"],
        //        (byte)((int)(_Dictionary["type"])),
        //        (byte)((int)(_Dictionary["grade"])),
        //        (int)_Dictionary["max_count"],
        //        _Dictionary["icon"].ToString());

        //    // Adding to list
        //    GameMgr.Inventory_ObjectList.Add(tempObject);
        //}
        #endregion

        // 스크롤뷰와 그리드 위치 초기화
        Repositioning();
    }

    // 그리드 정렬
    public void GridSort(int _signe)
    {
        // 부호변경(오름차순 내림차순을 위하여)
        signe = _signe;

        // 그리드 자식들 해제
        Inventory_Grid.transform.DetachChildren();

        // 정렬
        GameMgr.Inventory_ObjectList.Sort(
            delegate (GameObject c1, GameObject c2)
            {
                return c1.GetComponent<ItemInfo>().Grade.CompareTo(c2.GetComponent<ItemInfo>().Grade) * _signe;
            }
        );

        // 그리드 자식들 재 추가
        for( int i = 0; i < GameMgr.Inventory_ObjectList.Count; ++i)
        {
            Inventory_Grid.AddChild(GameMgr.Inventory_ObjectList[i].transform);
        }
        #region foreach 버전
        //foreach (GameObject _object in GameMgr.Inventory_ObjectList)
        //{
        //    Inventory_grid.AddChild(_object.transform);
        //}
        #endregion

        // 그리드 및 스크롤뷰 위치 초기화
        Repositioning();
    }
    
    // 그리드 및 스크롤뷰 위치 초기화
    public void Repositioning()
    {
        Inventory_Grid.Reposition();
        gameObject.GetComponent<UIScrollView>().ResetPosition();
    }

    // CVS 리더
    public List<Dictionary<string, object>> ReadCVS(string file)
    {
        // 요소 구분 문자
        string SPLIT_RE = @",(?=(?:[^""]*""[^""]*"")*(?![^""]*""))";
        // 줄 구분 문자
        string LINE_SPLIT_RE = @"\r\n|\n\r|\n|\r";
        // 트림 문자 배열
        char[] TRIM_CHARS = { '\"' };
        
        // 리스트 생성
        var list = new List<Dictionary<string, object>>();

        // cvs 파일 로드
        TextAsset   data    = Resources.Load(file) as TextAsset;

        // 줄 구분 역할 문자로 끊어 배열 생성
        var         lines   = Regex.Split(data.text, LINE_SPLIT_RE);

        // 파일에 읽을 데이터가 있는지 판정
        if (lines.Length <= 1)
        {
            return list;
        }

        // 첫번째 줄 헤더 추출
        var         header  = Regex.Split(lines[0], SPLIT_RE);

        // 3번째 줄 부터 데이터가 들어가 있으므로 2 부터 시작했습니다.
        for (var i = 2; i < lines.Length; i++)
        {
            // 요소 추출
            var values = Regex.Split(lines[i], SPLIT_RE);

            // 요소가 유무 체크
            if (values.Length == 0 || values[0] == "")
            {
                continue;
            }

            var entry = new Dictionary<string, object>();

            for (var j = 0; j < header.Length && j < values.Length; j++)
            {
                string value = values[j];

                // 요소 외 문자 제거
                value = value.TrimStart(TRIM_CHARS).TrimEnd(TRIM_CHARS).Replace("\\", "");
                object finalvalue = value;

                // 32비트 Signed 정수,실수로 변경 후 추가 둘 다 아니라면 그대로 엔트리에 추가
                int n;
                float f;
                if (int.TryParse(value, out n))
                {
                    finalvalue = n;
                }
                else if (float.TryParse(value, out f))
                {
                    finalvalue = f;
                }
                entry[header[j]] = finalvalue;
            }
            // 완성 된 엔트리 리스트에 추가
            list.Add(entry);
        }
        // 완성 된 리스트 반환
        return list;
    }
}