using UnityEngine;
using System.Collections;

public class HPPotion : MonoBehaviour 
{
    public          Transform           effect      = null;
    public          int                 heal        = 10;
    void Awake()
    {
        // 알파값 수정
        Color alpha = GetComponent<Renderer>().material.color;
        alpha.a = 0.6f;
        GetComponent<Renderer>().material.color = alpha;
    }

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
        if( _Other.transform.tag == "Player")
        {
            // 치유 및 이펙트 효과 내고 파괴
            GameObject.Find("PLAYER").transform.FindChild("Cha_Knight").GetComponent<CharactorControl>().CurrentHP += heal;
            GameObject.Find("GameMGR").GetComponent<GameMGR>().PlayerTextPrint(heal, Color.green);
            Instantiate(effect, GameObject.Find("PLAYER").transform.FindChild("Cha_Knight").transform.position, Quaternion.identity);
            Destroy(gameObject);
        }
    }
}
