using System.IO.Ports; // For serial communication
using UnityEngine;

public class ArduinoControlledMovement : MonoBehaviour
{
    public float moveSpeed = 5f;           // Movement speed of the GameObject
    public string portName = "COM3";      // Update this to match your Arduino COM port
    public int baudRate = 115200;         // Match the baud rate of the Arduino
    private SerialPort serialPort;        // Serial port object

    private int arduinoW = 0;             // Arduino "w" state (move forward)
    private int arduinoS = 0;             // Arduino "s" state (move backward)

    void Start()
    {
        // Initialize the serial port
        serialPort = new SerialPort(portName, baudRate);
        try
        {
            serialPort.Open();
            serialPort.ReadTimeout = 100; // Set timeout for reading data
            Debug.Log("Serial port opened successfully!");
        }
        catch (System.Exception e)
        {
            Debug.LogError("Error opening serial port: " + e.Message);
        }
    }

    void Update()
    {
        // Read Arduino data
        if (serialPort != null && serialPort.IsOpen)
        {
            try
            {
                while (serialPort.BytesToRead > 0)
                {
                    string data = serialPort.ReadLine(); // Read data from Arduino
                    Debug.Log("Raw data from COM port: " + data);
                    ParseArduinoData(data); // Parse the Arduino data
                }
            }
            catch (System.TimeoutException)
            {
                // Ignore timeout exceptions
            }
            catch (System.Exception e)
            {
                Debug.LogError("Error reading serial data: " + e.Message);
            }
        }

        // Handle movement
        float moveX = 0f; // Movement along the X-axis (left-right)
        float moveZ = 0f; // Movement along the Z-axis (up-down)

        // Keyboard input
        if (Input.GetKey(KeyCode.D)) moveX += 1f; // Move right
        if (Input.GetKey(KeyCode.A)) moveX -= 1f; // Move left
        if (Input.GetKey(KeyCode.W) || arduinoW == 1) moveZ += 1f; // Move up (forward)
        if (Input.GetKey(KeyCode.S) || arduinoS == 1) moveZ -= 1f; // Move down (backward)

        // Apply movement to the GameObject
        Vector3 move = new Vector3(moveX, 0f, moveZ) * moveSpeed * Time.deltaTime;
        transform.Translate(move, Space.World);
    }

    void ParseArduinoData(string data)
    {
        // Example input: "w:1 s:0"
        string[] parts = data.Split(' '); // Split the string by spaces
        foreach (string part in parts)
        {
            if (part.StartsWith("w:"))
            {
                if (int.TryParse(part.Substring(2), out int value))
                {
                    arduinoW = value;
                }
            }
            else if (part.StartsWith("s:"))
            {
                if (int.TryParse(part.Substring(2), out int value))
                {
                    arduinoS = value;
                }
            }
        }
        Debug.Log($"Parsed values - Arduino W: {arduinoW}, Arduino S: {arduinoS}");
    }

    void OnApplicationQuit()
    {
        if (serialPort != null && serialPort.IsOpen)
        {
            serialPort.Close(); // Close the serial port when the application quits
            Debug.Log("Serial port closed.");
        }
    }
}
