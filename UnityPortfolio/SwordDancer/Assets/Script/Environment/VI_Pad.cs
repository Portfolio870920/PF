using UnityEngine;
using System.Collections;

public class VI_Pad : MonoBehaviour 
{
    public      Camera          UICam               = null;
    public      Transform       InnerPad            = null;
                Vector3         dir                 = new Vector3(0,0,0);
                float           distance            = 0.0f;
                float           distanceMax         = 0.0f;
    public      CharactorMove   CharMove            = null;
                bool            IsPress             = false;
                int             touch_ID            = -1;

    void Awake ()
    {
        // InnerPad가 null일 경우 스스로 찾음
        if (!InnerPad)
        {
            InnerPad = GameObject.Find("VI_Pad_Inner").transform;
        }

        // CharMove이 null 일 경우 스스로 찾음
        if (!CharMove)
        {
            CharMove = GameObject.Find("PLAYER").GetComponent<CharactorMove>();
        }

        // 최대 거리
        distanceMax = (transform.position - GameObject.Find("UI Root/UICam/UIPanel/VI_Pad/MaxXpos").transform.position).magnitude;
    }

	// Use this for initialization
	void Start () 
    {
	    
	}
	
	// Update is called once per frame
	void Update () 
    {
        if (IsPress && touch_ID > -1)
        {
            // Mouse 작동 코드
            //Ray ray = UICam.ScreenPointToRay(Input.mousePosition);
            //RaycastHit2D rayhit = Physics2D.GetRayIntersection(ray, Mathf.Infinity);

            //// Raycast를 통해 패드를 클릭하고 있는지 판단
            //if (rayhit.collider == this.GetComponent<Collider2D>())
            //{
            //    Vector3 mouse = Input.mousePosition;
            //    Vector3 padPos = UICam.ScreenToWorldPoint(new Vector3(mouse.x, mouse.y, 0));
            //    InnerPad.position = padPos;
            //}

            // Touch 코드
            Vector3 touch = Input.GetTouch(touch_ID).position;
            Vector3 padPos = UICam.ScreenToWorldPoint(new Vector3(touch.x, touch.y, 0));
            // 최대 Touch 거리를 넘어섰을 경우 최대거리로 제한
            if( (padPos - transform.position).magnitude > distanceMax)
            {
                padPos = transform.position + (padPos - transform.position).normalized * distanceMax;
            }
            InnerPad.position = padPos;
        }

        // 방향벡터 얻기
        Vector3 Temp = (InnerPad.position - transform.position).normalized;
        dir.x = Temp.x;
        dir.y = 0.0f;
        dir.z = Temp.y;

        // 거리 계산
        distance = (InnerPad.position - transform.position).magnitude;
        //Debug.Log(InnerPad.position);

        // 캐릭터 이동 회전 호출
        CharMove.playerMove(dir, distance / distanceMax);
    }

    void OnPress(bool _IsPress)
    {
        // 패드가 눌렸을 경우 터치의 충돌여부 판단과 충돌된 터치번호를 얻음
        IsPress = _IsPress;
        if( IsPress)
        {
            for (int i = 0; i < Input.touchCount; ++i)
            {
                Ray ray = UICam.ScreenPointToRay(Input.GetTouch(i).position);
                RaycastHit2D rayhit = Physics2D.GetRayIntersection(ray, Mathf.Infinity);
                if (rayhit.collider == this.GetComponent<Collider2D>())
                {
                    touch_ID = i;
                }
            }
        }
    }

    void OnRelease()
    {
        IsPress = false;
        touch_ID = -1;
    }
}