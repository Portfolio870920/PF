using UnityEngine;
using System.Collections;

public class Tapping : MonoBehaviour {
    public GameMgr Manager = null;
    public bool CanIHit = true;
    //int a = 0;
    float time = 0;

    void Awake()
    {
        if( Manager == null)
        {
            Manager = GameObject.Find("GameManager").GetComponent<GameMgr>();
            if( Manager == null)
            {
                Debug.Log("Can not find Manager in Tapping.cs");
            }
        }
    }
    void Update()
    {
        time += Time.deltaTime;
    }

	void OnPress(bool _IsPress)
    {
        if (CanIHit  && _IsPress)
        {
            CanIHit = false;
            Vector3 position = new Vector3(UICamera.lastTouchPosition.x, UICamera.lastTouchPosition.y, 0);
            Manager.PlayEffect(position, "Hit");
            //a++;            
            //Debug.Log("hit : " + a + "   time : " + time.ToString("#.#0"));
            Manager.Attack();
        }

        else if(!_IsPress)
        {
            CanIHit = true;
        }
    }
}
