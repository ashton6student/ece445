using UnityEngine;

public class VirtualHandController : MonoBehaviour
{
    [Header("References")]
    public SerialFloatReader serialReader;
   // public Transform realController;   // The real VR controller Transform
    public Animator handAnimator;

    [Header("Lock Hand Settings")]
    public bool lockHand = true;        // Toggle for locking hand in place

    private Vector3 positionOffset;
    private Quaternion rotationOffset;
    private Vector3 frozenPosition;
    private Quaternion frozenRotation;
    private bool wasLockedLastFrame = true;
    private bool lastButton0State = false;

    void Start()
    {
       // if (realController == null)
        //{
         //   Debug.LogError("Real controller reference not set!");
          //  enabled = false;
           // return;
        //}

      //  positionOffset = Quaternion.Inverse(realController.rotation) * (transform.position - realController.position);
      //  rotationOffset = Quaternion.Inverse(realController.rotation) * transform.rotation;
    }

    void Update()
    {
        HandleButtonInput();
       // UpdateHandLock();
        UpdateHandAnimation();
    }

    void HandleButtonInput()
    {
        //if (serialReader == null) return;

        if (serialReader.Button0 && !lastButton0State)
        {
            lockHand = !lockHand;
            Debug.Log($"Hand lock toggled. Now: {lockHand}");
        }

        lastButton0State = serialReader.Button0;
    }
    /*
    void UpdateHandLock()
    {
        if (lockHand)
        {
            if (!wasLockedLastFrame)
            {
                positionOffset = Quaternion.Inverse(realController.rotation) * (frozenPosition - realController.position);
                rotationOffset = Quaternion.Inverse(realController.rotation) * frozenRotation;
            }

            transform.position = realController.position + (realController.rotation * positionOffset);
            transform.rotation = realController.rotation * rotationOffset;
        }
        else
        {
            if (wasLockedLastFrame)
            {
                frozenPosition = transform.position;
                frozenRotation = transform.rotation;
            }

            transform.position = frozenPosition;
            transform.rotation = frozenRotation;
        }

        wasLockedLastFrame = lockHand;
    }
    */
    void UpdateHandAnimation()
    {
        //if (serialReader == null || handAnimator == null) return;

        for (int i = 0; i < serialReader.Values.Length; i++)
        {
            int layerIndex = i + 1;

            if (layerIndex < handAnimator.layerCount)
            {
                handAnimator.SetLayerWeight(layerIndex, serialReader.Values[i]);
             //   Debug.Log($"Layer {layerIndex} weight set to: {serialReader.Values[i]:0.00}");
            }
        }
    }
}
