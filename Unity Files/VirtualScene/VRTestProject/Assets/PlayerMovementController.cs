using UnityEngine;

public class PlayerMovementController : MonoBehaviour
{
    public SerialFloatReader serialReader;  // Link to SerialFloatReader
    public float moveSpeed = 5f;             // Movement speed multiplier
    public float joystickDeadzone = 0.2f;    // Minimum joystick input to start moving

    void Update()
    {
        if (serialReader == null) return;

        HandleMovement();
    }

    void HandleMovement()
    {
        float moveX = serialReader.JoyX;
        float moveZ = serialReader.JoyY;

        // Apply deadzone
        if (Mathf.Abs(moveX) < joystickDeadzone) moveX = 0f;
        if (Mathf.Abs(moveZ) < joystickDeadzone) moveZ = 0f;

        Vector3 move = new Vector3(moveX, 0f, moveZ) * moveSpeed * Time.deltaTime;
        transform.Translate(move, Space.World);
    }
}
