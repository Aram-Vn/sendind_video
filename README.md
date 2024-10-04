# sendind_video
```bash
 g++ -o joystick_reader joystick_reader.cpp -I/usr/include/libevdev-1.0 -levdev
```

```
e.g on PI 1

auto eth0
iface eth0 inet static
address 192.168.1.5
netmask 255.255.255.0
gateway 192.168.1.6
on PI 2

auto eth0
iface eth0 inet static
address 192.168.1.6
netmask 255.255.255.0
gateway 192.168.1.5
```


```
interface eth0
static ip_address=192.168.1.11/24
static routers=192.168.1.1
static domain_name_servers=8.8.8.8  # Google DNS, or your router's IP
```

```
auto eth0
iface eth0 inet static
address 192.168.1.5
netmask 255.255.255.0
```

```
auto eth0
iface eth0 inet static
address 192.168.1.6
netmask 255.255.255.0
```

```
from pymavlink import mavutil

# Establish a connection to the Pixhawk (serial port or UDP)
master = mavutil.mavlink_connection('/dev/ttyAMA0', baud=57600)

# Wait for a heartbeat to ensure communication
master.wait_heartbeat()

# Example to control the gimbal (set pitch, roll, yaw)
pitch = 30.0  # Pitch angle in degrees
roll = 0.0    # Roll angle in degrees
yaw = 45.0    # Yaw angle in degrees

# Send MAV_CMD_DO_MOUNT_CONTROL message
master.mav.command_long_send(
    master.target_system,      # target system
    master.target_component,   # target component
    mavutil.mavlink.MAV_CMD_DO_MOUNT_CONTROL,  # Command ID
    0,  # Confirmation (0 = first time, 1+ = resend)
    pitch,  # Pitch (in degrees)
    roll,   # Roll (in degrees)
    yaw,    # Yaw (in degrees)
    0, 0, 0,  # Unused parameters
    mavutil.mavlink.MAV_MOUNT_MODE_MAVLINK_TARGETING  # Gimbal mount mode
)

print("Gimbal command sent")
```
```
import pigpio
import time

# Set up pigpio
pi = pigpio.pi()

if not pi.connected:
    print("Failed to connect to pigpio daemon.")
    exit()

# Define GPIO pins
TX_PIN = 23  # Example TX pin

# Start the serial connection
pi.set_mode(TX_PIN, pigpio.OUTPUT)

# Prepare the S-BUS frame
sbus_frame = bytearray(25)
sbus_frame[0] = 0x0F  # Start byte
sbus_frame[24] = 0x00  # End byte

# Set channel values (0-2047) for 16 channels
for i in range(16):
    value = 1500  # Channel value set to 1500
    if i < 8:
        sbus_frame[i * 2 + 1] = value & 0xFF       # Low byte for channels 1-8
        sbus_frame[i * 2 + 2] = (value >> 8) & 0x07  # High byte for channels 1-8
    else:
        sbus_frame[(i - 8) * 2 + 9] = value & 0xFF      # Low byte for channels 9-16
        sbus_frame[(i - 8) * 2 + 10] = (value >> 8) & 0x07  # High byte for channels 9-16

# Function to continuously send S-BUS frames
def send_sbus():
    while True:
        pi.write(TX_PIN, 1)  # Set TX high (idle)
        time.sleep(0.00001)  # Small delay
        for byte in sbus_frame:
            for i in range(8):
                pi.write(TX_PIN, (byte >> (7 - i)) & 1)  # Send each bit
                time.sleep(0.00001)  # Adjust timing as needed
        time.sleep(0.02)  # Wait 20 ms before sending the next frame
        print("S-BUS frame sent with all channels set to 1500.")

try:
    send_sbus()
except KeyboardInterrupt:
    pass
finally:
    pi.stop()
```

