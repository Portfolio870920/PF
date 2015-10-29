using UnityEngine;
using System.Collections;

public class HudRoot : MonoBehaviour {

    static public GameObject go;
    void Awake()
    {
        go = gameObject;
    }
}
