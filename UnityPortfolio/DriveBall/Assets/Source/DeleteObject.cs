using UnityEngine;
using System.Collections;

public class DeleteObject : MonoBehaviour {

	void OnTriggerEnter2D(Collider2D _Other)
    {
        Destroy(_Other.gameObject);
    }
}
