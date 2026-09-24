import serial
import time
import sys

PORT = "/dev/ttyUSB0"
BAUD = 115200

SOH = 0x01
EOT = 0x04
ACK = 0x06
NAK = 0x15
CAN = 0x18

BLOCK_SIZE = 128
PAD = 0x1A


def crc16(data):
    crc = 0

    for byte in data:
        crc ^= byte << 8

        for _ in range(8):
            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF

    return crc


with open("kernel", "rb") as f:
    kernel = f.read()

ser = serial.Serial(PORT, BAUD, timeout=10)

print(f"Kernel size: {len(kernel)} bytes")
print("Waiting for U-Boot...")

# Wait for U-Boot's 'C' (CRC mode) or NAK (checksum mode)
start = time.time()

while True:
    b = ser.read(1)

    if b == b"C":
        crc_mode = True
        break

    if b == bytes([NAK]):
        crc_mode = False
        break

    if b == bytes([CAN]):
        print("U-Boot cancelled transfer")
        sys.exit(1)

    if time.time() - start > 20:
        print("Timeout waiting for U-Boot")
        sys.exit(1)

print("U-Boot ready")
print("CRC mode:", crc_mode)
print("Sending kernel...")

block = 1
offset = 0

while offset < len(kernel):
    chunk = kernel[offset:offset + BLOCK_SIZE]

    if len(chunk) < BLOCK_SIZE:
        chunk += bytes([PAD]) * (BLOCK_SIZE - len(chunk))

    block_number = block & 0xFF

    packet = bytes([
        SOH,
        block_number,
        0xFF - block_number
    ]) + chunk

    if crc_mode:
        crc = crc16(chunk)
        packet += bytes([
            (crc >> 8) & 0xFF,
            crc & 0xFF
        ])
    else:
        checksum = sum(chunk) & 0xFF
        packet += bytes([checksum])

    for attempt in range(10):
        ser.write(packet)

        response = ser.read(1)

        if response == bytes([ACK]):
            break

        if response == bytes([CAN]):
            print("Transfer cancelled")
            sys.exit(1)

    else:
        print(f"Failed at block {block_number}")
        sys.exit(1)

    offset += BLOCK_SIZE
    block += 1

ser.write(bytes([EOT]))

response = ser.read(1)

if response == bytes([ACK]):
    print("Transfer complete")
else:
    print("Unexpected EOT response:", response)

ser.close()
