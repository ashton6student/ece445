import serial
import time

PORT = "COM25"
BAUD = 115200

def main():
    print(f"[*] Opening serial port: {PORT}")
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except Exception as e:
        print(f"[-] Failed to open port: {e}")
        return

    try:

        while True:
            TEST_STRING = input("[*] Provide input string: ")
            TEST_STRING_LINE = (TEST_STRING + "\n").encode()
            print(f"[*] Sending: {TEST_STRING_LINE.decode().strip()}")
            ser.write(TEST_STRING_LINE)

            time.sleep(0.2)
            received = ser.readline()

            try:
                decoded = received.decode(errors='ignore').strip()
            except UnicodeDecodeError:
                decoded = "<invalid UTF-8>"

            print(f"[*] ESP-BLE said: {decoded}")

            print("")

    except KeyboardInterrupt:
        print("\n[*] Caught exit, closing serial and exiting.")
        ser.close()

if __name__ == "__main__":
    main()
