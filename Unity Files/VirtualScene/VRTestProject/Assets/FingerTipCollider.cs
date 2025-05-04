using UnityEngine;

public class FingerTipCollider : MonoBehaviour
{
    [Header("References")]
    public SerialFloatReader serialHandler; // drag your Serial Handler object into this field
    public int fingerId; // 0 = Thumb, 1 = Index, etc.

    private void OnTriggerEnter(Collider other)
    {
        Debug.Log($"{gameObject.name} triggered ENTER with {other.gameObject.name}");
        serialHandler.SendFingerTouch(fingerId, true);
    }

    private void OnTriggerExit(Collider other)
    {
        Debug.Log($"{gameObject.name} triggered EXIT with {other.gameObject.name}");
        serialHandler.SendFingerTouch(fingerId, false);
    }
}
