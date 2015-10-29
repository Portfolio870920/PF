using UnityEngine;
using System.Collections;

public class Status : MonoBehaviour {
    UILabel             levelLabel         = null;
    UILabel             AttackLabel        = null;
    UILabel             CriticalLabel      = null;
    UILabel             HealthLabel        = null;
    UILabel             ManaLabel          = null;
    CharactorControl    PlayerInfo         = null;

    void Awake()
    {
        // Find n set SubLabel
        levelLabel      = GameObject.Find("UI Root/UICam/UIPanel/Status/Level/Sub").GetComponent<UILabel>();
        AttackLabel     = GameObject.Find("UI Root/UICam/UIPanel/Status/Attack/Sub").GetComponent<UILabel>();
        CriticalLabel   = GameObject.Find("UI Root/UICam/UIPanel/Status/Critical/Sub").GetComponent<UILabel>();
        HealthLabel     = GameObject.Find("UI Root/UICam/UIPanel/Status/Health/Sub").GetComponent<UILabel>();
        ManaLabel       = GameObject.Find("UI Root/UICam/UIPanel/Status/Mana/Sub").GetComponent<UILabel>();

        // Get PlayerInfo
        PlayerInfo  = GameObject.Find("PLAYER/Cha_Knight").GetComponent<CharactorControl>();
    }

	// Use this for initialization
	void Start () 
    {
	    
	}
	
	// Update is called once per frame
	void Update () 
    {
        GetComponent<BoxCollider2D>().size = GetComponent<UISprite>().localSize;
        levelLabel.text     = "1";
        AttackLabel.text    = PlayerInfo.playerDamage.ToString();
        CriticalLabel.text  = PlayerInfo.CriPercent.ToString() +"%";
        HealthLabel.text    = PlayerInfo.playerHP.ToString();
        ManaLabel.text      = PlayerInfo.playerMP.ToString();
	}

    void OnClick()
    {
        gameObject.SetActive(false);
    }

    void OnRelease()
    {
        gameObject.SetActive(false);
    }
}
