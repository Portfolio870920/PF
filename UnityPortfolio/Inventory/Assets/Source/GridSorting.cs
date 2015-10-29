using UnityEngine;
using System.Collections;

public class GridSorting : MonoBehaviour
{
    private GameObject  GameMGR      = null;
    private GameObject  Inventory    = null;
    private int         signe        = 0;

    void Start()
    {
        GameMGR   = GameObject.Find("GameMGR");
        Inventory = GameObject.Find("UI Root/Camera/Inventory");
    }

    void OnClick()
    {
        // 부호 변수를 가져옴
        signe = Inventory.GetComponent<Inven>().signe;
        // 부호 변수를 반대로 적용
        signe *= -1;
        // 정렬호출
        Inventory.GetComponent<Inven>().GridSort(signe);
    }
}
