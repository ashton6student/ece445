using UnityEngine;
using System.IO.Ports;

public class ESP32Controller : MonoBehaviour
{
    // Serial communication
    SerialPort serialPort;
    public string portName = "COM3";  // Change this to your ESP32 COM port
    public int baudRate = 115200;

    // Movement speed
    public float moveSpeed = 0.1f;

    // Joystick Data
    private float joystickX = 0;
    private float joystickY = 0;

    // Reference Point and Tolerance
    private int xCenter = -1;  // Initial reference point for X
    private int yCenter = -1;  // Initial reference point for Y
    public int deltaTolerance = 100;  // Tolerance for joystick drift

    void Start()
    {
        // Open the serial port
        serialPort = new SerialPort(portName, baudRate);
        serialPort.DtrEnable = true;  // Required for some devices
        serialPort.ReadTimeout = 100;
        serialPort.Open();

        Debug.Log("Serial Port Opened: " + portName);
    }

    void Update()
    {
        // Read data from Serial
        if (serialPort.IsOpen)
        {
            try
            {
                string data = serialPort.ReadLine();
                Debug.Log("Raw Data: " + data); // Print the raw data line
                ParseData(data);
                MoveObject();
            }
            catch (System.Exception ex)
            {
                Debug.LogWarning("Serial Read Exception: " + ex.Message);
            }
        }
    }

    // Parse the incoming data
    void ParseData(string data)
    {
        // Example: w: 0 | s: 0 | X: 2048 | Y: 2048 | Joystick Button: Released
        string[] parts = data.Split('|');
        if (parts.Length == 5)
        {
            try
            {
                string yString = parts[2].Split(':')[1].Trim();
                string xString = parts[3].Split(':')[1].Trim();

                // Convert to integer
                int xRaw = int.Parse(xString);
                int yRaw = int.Parse(yString);

                // Set the initial reference point
                if (xCenter == -1 && yCenter == -1)
                {
                    xCenter = xRaw;
                    yCenter = yRaw;
                    Debug.Log($"Reference Point Set - X Center: {xCenter}, Y Center: {yCenter}");
                }

                // Calculate delta from reference point
                int deltaX = xRaw - xCenter;
                int deltaY = yRaw - yCenter;

                // Apply tolerance to avoid drift
                if (Mathf.Abs(deltaX) < deltaTolerance) deltaX = 0;
                if (Mathf.Abs(deltaY) < deltaTolerance) deltaY = 0;

                // Normalize the delta values
                joystickX = (float)deltaX / 2048;
                joystickY = (float)deltaY / 2048;

                // Debugging: Print the normalized values
                Debug.Log($"X Raw: {xRaw}, Y Raw: {yRaw}");
                Debug.Log($"Delta X: {deltaX}, Delta Y: {deltaY}");
                Debug.Log("joystickX (Normalized): " + joystickX);
                Debug.Log("joystickY (Normalized): " + joystickY);
            }
            catch (System.Exception ex)
            {
                Debug.LogError("Data Parsing Exception: " + ex.Message);
            }
        }
        else
        {
            Debug.LogWarning("Unexpected Data Format: " + data);
        }
    }

    // Move the GameObject
    void MoveObject()
    {
        Vector3 movement = new Vector3(joystickX, 0, -joystickY) * moveSpeed;
        transform.Translate(movement);
    }

    private void OnApplicationQuit()
    {
        if (serialPort != null && serialPort.IsOpen)
        {
            serialPort.Close();
            Debug.Log("Serial Port Closed");
        }
    }
}
