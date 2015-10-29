using UnityEngine;
using System.Collections;

public class ItemInfo : MonoBehaviour {

    private int     handle      = 0;
    private int     id          = 0;
    private int     attack      = 0;
    private byte    type        = 0;
    private byte    grade       = 0;
    private int     max_count   = 0;
    private string  icon        = null;
    public  float   Popuptime   = 2;
    private bool    Show        = false;

    #region Get_ItemInfo
    public int Handle
    {
        get
        {
            return handle;
        }
    }
    public int Id
    {
        get
        {
            return Id;
        }
    }
    public int Attack
    {
        get
        {
            return attack;
        }
    }
    public byte Type
    {
        get
        {
            return type;
        }
    }
    public byte Grade
    {
        get
        {
            return grade;
        }
    }
    public int Max_Count
    {
        get
        {
            return max_count;
        }
    }
    public string Icon
    {
        get
        {
            return icon;
        }
    }
    #endregion

   public void Set_ItemInfo(int _handle, int _id, int _attack, byte _type, byte _grade, int _max_count, string _icon)
    {
        handle = _handle;
        id = _id;
        attack = _attack;
        type = _type;
        grade = _grade;
        max_count = _max_count;
        icon = _icon;
    }

    public void OnPress()
    {
        StartCoroutine(PressIcon());
        Show = true;
    }

    // 드래그 혹은 포커스 이동 등 다른 행동을 취할 시 Popup취소
    public void OnRelease()
    {
        StopCoroutine(PressIcon());
        Show = false;
    }

    IEnumerator PressIcon()
    {
        // 미리 선언 된 Popuptime 에 이후에 실행
        yield return new WaitForSeconds(Popuptime);

        if ( Show)
        {
            GameObject.Find("UI Root/Camera/PopupWindow/ItemInfo").transform.position = transform.position;
            GameObject.Find("UI Root/Camera/PopupWindow/ItemInfo").SetActive(true);
        }        

        yield return null;
    }
}
