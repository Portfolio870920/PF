using UnityEngine;
using System.Collections;

public class Gold : MonoBehaviour {
    public          Transform           effect      = null;
    public          int                 gold        = 10;
	// Use this for initialization
	void Start () 
    {
	}
	
	// Update is called once per frame
	void Update () 
    {
	}

    void OnTriggerEnter(Collider _Other)
    {
        if (_Other.tag == "Player")
        {
            // 골드 수취 및 이펙트 효과 내고 파괴
            GameObject.Find("GameMGR").GetComponent<GameMGR>().AmountofGold += gold;
            Instantiate(effect, GameObject.Find("PLAYER").transform.FindChild("Cha_Knight").transform.position, Quaternion.identity);
            Destroy(gameObject);
        }
    }
}
