using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class GameMGR : MonoBehaviour {

    private List<GameObject> Inventory_objectList  = new List<GameObject>();

    public  List<GameObject> Inventory_ObjectList
    {
        get
        {
            return Inventory_objectList;
        }
        set
        {
            Inventory_objectList = value;
        }
    }
}
