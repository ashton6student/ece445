using UnityEngine;

public class HandCalibrationController : MonoBehaviour
{
    [Header("References")]
    public SerialFloatReader serialReader;
    public Transform realController;   // Real VR controller (e.g., Left/Right Hand)
    public Transform virtualHand;       // Your VR hand (target object)

    [Header("Lock Hand Settings")]
    public bool lockHand = true;

    private Matrix4x4 handRelativeToController;
    private Vector3 frozenPosition;
    private Quaternion frozenRotation;
    private bool wasLockedLastFrame = true;
    private bool lastButton0State = false;

    void Start()
    {
        if (realController == null || virtualHand == null)
        {
            Debug.LogError("Real controller or virtual hand reference not set!");
            enabled = false;
            return;
        }

        CaptureRelativeMatrix();
        wasLockedLastFrame = lockHand;
    }

    void Update()
    {
        // HandleButtonInput();
        UpdateHandLock();
    }

  /*  void LateUpdate()
    {
        UpdateHandLock();
    }*/

   /* void HandleButtonInput()
    {
        if (serialReader == null) return;

        if (serialReader.Button0 && !lastButton0State)
        {
            lockHand = !lockHand;
            Debug.Log($"Hand lock toggled. Now: {lockHand}");
        }

        lastButton0State = serialReader.Button0;
    }*/

    void UpdateHandLock()
    {
        if (lockHand)
        {
            if (!wasLockedLastFrame)
            {
                // Just transitioned from unlocked -> locked
                CaptureRelativeMatrix();
                Debug.Log("setting LED");
                serialReader.SendSerialData("1");
            }

            // While locked: apply controller * relative matrix
            Matrix4x4 controllerWorld = Matrix4x4.TRS(realController.position, realController.rotation, Vector3.one);
            Matrix4x4 handWorld = controllerWorld * handRelativeToController;

            virtualHand.position = handWorld.GetColumn(3); // translation
            virtualHand.rotation = handWorld.rotation;     // rotation
        }
        else
        {
            if (wasLockedLastFrame)
            {
                // Just transitioned to unlocked: freeze pose
                frozenPosition = virtualHand.position;
                frozenRotation = virtualHand.rotation;
                Debug.Log("unsetting LED");
                serialReader.SendSerialData("0");
            }

            // While unlocked: maintain frozen pose
            virtualHand.position = frozenPosition;
            virtualHand.rotation = frozenRotation;
        }

        wasLockedLastFrame = lockHand;
    }

    void CaptureRelativeMatrix()
    {
        // Controller-to-world matrix
        Matrix4x4 controllerWorld = Matrix4x4.TRS(realController.position, realController.rotation, Vector3.one);
        Matrix4x4 controllerWorldInverse = controllerWorld.inverse;

        // Hand-to-world matrix
        Matrix4x4 handWorld = Matrix4x4.TRS(virtualHand.position, virtualHand.rotation, Vector3.one);

        // Calculate hand relative to controller
        handRelativeToController = controllerWorldInverse * handWorld;
    }
}
