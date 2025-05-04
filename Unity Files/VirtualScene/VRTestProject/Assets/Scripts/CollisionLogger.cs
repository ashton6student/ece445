using UnityEngine;
using UnityEngine.InputSystem; // Required for the new Input System

public class CollisionLogger : MonoBehaviour
{
    public float speed = 2.0f; // Movement speed
    private Vector2 moveInput;  // Store joystick input

    private void Update()
    {
        // Move the object based on joystick input
        Vector3 move = new Vector3(moveInput.x, 0, moveInput.y) * speed * Time.deltaTime;
        transform.Translate(move);
    }

    public void OnMove(InputAction.CallbackContext context)
    {
        // Read joystick input
        moveInput = context.ReadValue<Vector2>();
    }

    private void OnCollisionEnter(Collision collision)
    {
        // Log the name of the object that collided with this one
        Debug.Log("Collision detected with: " + collision.gameObject.name);
    }
}
