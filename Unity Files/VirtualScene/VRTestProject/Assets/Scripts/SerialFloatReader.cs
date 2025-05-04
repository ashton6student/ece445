using System;
using System.IO.Ports;
using UnityEngine;

public class SerialFloatReader : MonoBehaviour
{
    [Header("Serial Settings")]
    public string portName = "COM25";
    public int baudRate = 115200;

    private SerialPort serialPort;

    [Header("Parsed Data")]
    public float[] Values { get; private set; } = new float[5]; // Thumb, Index, Middle, Ring, Pinky
    public float JoyX { get; private set; }
    public float JoyY { get; private set; }
    public bool Button0 { get; private set; }
    //public bool Button1 { get; private set; }

    [Tooltip("Enable for debug output of received values")]
    public bool debugValues = false;

    [Header("Sending Data (Manual)")]
    public bool sendTestMessage = false;
    public string messageToSend = "LED_ON";

    void Start()
    {
        try
        {
            serialPort = new SerialPort(portName, baudRate);
            serialPort.ReadTimeout = 100;
            serialPort.WriteTimeout = 100;
            serialPort.Open();
            Debug.Log("Serial port opened on " + portName);
        }
        catch (Exception e)
        {
            Debug.LogError("Failed to open serial port: " + e.Message);
        }
    }

    void Update()
    {
        ReadSerialData();

        if (sendTestMessage)
        {
            SendSerialData(messageToSend);
            sendTestMessage = false;
        }
    }

    void ReadSerialData()
    {
        if (serialPort != null && serialPort.IsOpen)
        {
            try
            {
                string line = serialPort.ReadLine().Trim();
                string[] sections = line.Split(',');

                if (sections.Length == 8)
                {
                    for (int i = 0; i < Values.Length; i++)
                        Values[i] = Mathf.Clamp01(float.Parse(sections[i].Trim(), System.Globalization.CultureInfo.InvariantCulture));

                    JoyX = float.Parse(sections[5].Trim(), System.Globalization.CultureInfo.InvariantCulture);
                    JoyY = float.Parse(sections[6].Trim(), System.Globalization.CultureInfo.InvariantCulture);
                    Button0 = sections[7].Trim() == "1";
                    //Button1 = sections[8].Trim() == "1";

                    if (debugValues)
                        Debug.Log($"Parsed: [{string.Join(", ", Values)}], JoyX: {JoyX}, JoyY: {JoyY}, Button0: {Button0}");
                }
                else
                {
                    if (debugValues)
                        Debug.LogWarning($"Serial data format mismatch. Got {sections.Length} values\nLine: {line}");
                }
            }
            catch (TimeoutException et)
            {
                Debug.LogWarning("Serial timeout error: " + et.Message);
            }
            catch (Exception e)
            {
                Debug.LogWarning("Serial read error: " + e.Message);
            }
        }
    }

    // --- NEW: Send string data manually ---
    public void SendSerialData(string message)
    {
        if (serialPort != null && serialPort.IsOpen)
        {
            try
            {
                serialPort.WriteLine(message);
               // if (debugValues)
                    Debug.Log("Sent to Arduino: " + message);
            }
            catch (Exception e)
            {
                Debug.LogWarning("Serial write error: " + e.Message);
            }
        }
    }

    // --- NEW: Send finger touch/release events ---
    public void SendFingerTouch(int fingerId, bool isTouching)
    {
        if (serialPort != null && serialPort.IsOpen)
        {
            try
            {
                string message = $"{fingerId}:{(isTouching ? 1 : 0)}";
                serialPort.WriteLine(message);
               // if (debugValues)
                    Debug.Log($"Sent to Arduino [Finger]: {message}");
            }
            catch (Exception e)
            {
                Debug.LogWarning("Serial write error (FingerTouch): " + e.Message);
            }
        }
    }

    void OnApplicationQuit()
    {
        if (serialPort != null && serialPort.IsOpen)
        {
            serialPort.Close();
            Debug.Log("Serial port closed.");
        }
    }
}
