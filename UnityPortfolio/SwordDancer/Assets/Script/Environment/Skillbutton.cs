using UnityEngine;
using System.Collections;

public class Skillbutton : MonoBehaviour
{
    bool ispress = false;
    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (ispress && GameObject.Find("PLAYER/Cha_Knight").GetComponent<CharactorControl>().CurrentHP > 0)
        {
            GameObject.Find("PLAYER/Cha_Knight").GetComponent<CharactorControl>().Whirlwind();
        }
    }

    void OnPress(bool isdown)
    {
        ispress = isdown;
        Debug.Log("스킬버튼 활성화 : " + isdown);
        if (!ispress)
        {
            GameObject.Find("PLAYER/Cha_Knight").GetComponent<CharactorControl>().Align();
            Debug.Log("스킬버튼 비활성화 : " + isdown);
        }
    }
}