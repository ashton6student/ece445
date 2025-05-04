using UnityEngine;

public class HandAnimatorController : MonoBehaviour
{
    [Header("References")]
    public SerialFloatReader serialReader;
    public Animator handAnimator;

    void Update()
    {
        UpdateHandAnimation();
    }

    void UpdateHandAnimation()
    {
        if (serialReader == null || handAnimator == null) return;

        for (int i = 0; i < serialReader.Values.Length; i++)
        {
            int layerIndex = i + 1; // Base layer = 0, fingers start at 1

            if (layerIndex < handAnimator.layerCount)
            {
                handAnimator.SetLayerWeight(layerIndex, serialReader.Values[i]);
            }
        }
    }
}
