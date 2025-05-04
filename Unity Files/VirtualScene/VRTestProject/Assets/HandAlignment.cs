using UnityEngine;

public class HandAlignment6DOF : MonoBehaviour
{
    public Transform realController;
    public Transform virtualHand;
    public bool lockHand = true;

    private Matrix4x4 handRelativeToController; // The full 6DOF relationship
    private Vector3 frozenPosition;
    private Quaternion frozenRotation;
    private bool wasLockedLastFrame = true;

    void Start()
    {
        CaptureRelativeMatrix();
        wasLockedLastFrame = lockHand;
    }

    void LateUpdate()
    {
        if (lockHand)
        {
            if (!wasLockedLastFrame)
            {
                // Just transitioned from unlocked -> locked
                CaptureRelativeMatrix();
            }

            // While locked: apply matrix multiplication
            Matrix4x4 controllerWorld = Matrix4x4.TRS(realController.position, realController.rotation, Vector3.one);
            Matrix4x4 handWorld = controllerWorld * handRelativeToController;

            virtualHand.position = handWorld.GetColumn(3); // Extract translation
            virtualHand.rotation = handWorld.rotation;     // Extract rotation
        }
        else
        {
            if (wasLockedLastFrame)
            {
                frozenPosition = virtualHand.position;
                frozenRotation = virtualHand.rotation;
            }

            // While unlocked: freeze pose
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
