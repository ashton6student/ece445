import asyncio
import os
import platform
import serial
from bleak import BleakClient, BleakScanner

DEVICE_NAME = "ESP32-Finger-Sensors"
TX_CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  # ESP → PC (Notify)
RX_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  # PC → ESP (Write)
BAUD = 115200

def setup_virtual_serial():
    system = platform.system()
    if system == "Darwin":
        master_fd, slave_fd = os.openpty()
        slave_name = os.ttyname(slave_fd)
        print(f"[macOS] Virtual serial port created: {slave_name}")
        ser = os.fdopen(master_fd, "rb+", buffering=0)
        return ser, slave_name
    elif system == "Windows": # com0com to get virtual COM ports. Fuck Windows.
        COM_PORT = "COM22"  # Adjust if needed
        print(f"[Windows] Using COM port: {COM_PORT}")
        ser = serial.Serial(COM_PORT, BAUD, timeout=0)
        return ser, COM_PORT
    else:
        raise NotImplementedError("Unsupported OS")

async def read_serial_bytes(serial_conn, byte_buffer):
    """Read serial data until newline and return complete line."""
    while True:
        b = await asyncio.to_thread(serial_conn.read, 1)
        if b:
            byte_buffer += b
            if b == b'\n':
                line = bytes(byte_buffer)
                byte_buffer.clear()
                return line
        else:
            await asyncio.sleep(0.001)

async def main():
    ble_dev = await BleakScanner.find_device_by_filter(lambda d, _: d.name == DEVICE_NAME)
    if not ble_dev:
        print("[-] Could not find ESP32 BLE device.")
        return

    async with BleakClient(ble_dev) as ble_client:
        print("[+] Connected to ESP32")

        serial_conn, port_name = setup_virtual_serial()
        print(f"[+] Bridging to virtual serial: {port_name}")

        ble_rx_buffer = bytearray()
        serial_rx_buffer = bytearray()

        # BLE → Serial
        def handle_rx(_, data: bytearray):
            nonlocal ble_rx_buffer
            ble_rx_buffer += data
            while b'\n' in ble_rx_buffer:
                line, _, ble_rx_buffer = ble_rx_buffer.partition(b'\n')
                line += b'\r\n'
                serial_conn.write(line)
                serial_conn.flush()

        await ble_client.start_notify(TX_CHAR_UUID, handle_rx)

        try:
            while True:
                line = await read_serial_bytes(serial_conn, serial_rx_buffer)
                print(line)
                await ble_client.write_gatt_char(RX_CHAR_UUID, line, response=True)
        except KeyboardInterrupt:
            print("\n[*] Interrupted. Cleaning up...")
        finally:
            await ble_client.stop_notify(TX_CHAR_UUID)
            serial_conn.close()
            await ble_client.disconnect()
            print("[*] Bridge closed.")

asyncio.run(main())
