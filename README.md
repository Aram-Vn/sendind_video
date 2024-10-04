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

# Set TX pin as output
pi.set_mode(TX_PIN, pigpio.OUTPUT)

# Prepare the S-BUS frame
sbus_frame = bytearray(25)
sbus_frame[0] = 0x0F  # Start byte
sbus_frame[24] = 0x00  # End byte (no failsafe/lost frame)

# Set channel values (0-2047 range) for 16 channels
channel_values = [1500] * 16  # Example values, 1500 for all channels

def pack_sbus_frame():
    # Clear frame data between start and end byte
    for i in range(1, 24):
        sbus_frame[i] = 0

    # Pack 16 channels (11 bits each) into 22 data bytes
    bit_idx = 0
    for channel in channel_values:
        for i in range(11):
            if channel & (1 << i):
                sbus_frame[1 + (bit_idx // 8)] |= (1 << (bit_idx % 8))
            bit_idx += 1

    # Frame[23] is reserved for flags (we'll leave it at 0x00 for now)
    # Frame[24] is the end byte (0x00 or 0x04 for failsafe)

def send_sbus():
    while True:
        # Prepare S-BUS frame with current channel values
        pack_sbus_frame()

        # Send the frame
        for byte in sbus_frame:
            # Send each byte (inverted, since S-BUS uses inverted signal)
            for i in range(8):
                bit = (byte >> (7 - i)) & 1
                pi.write(TX_PIN, not bit)  # Inverted bit
                time.sleep(0.00001)  # Bit duration, adjust if necessary

        time.sleep(0.02)  # S-BUS frame rate is every 20 ms
        print("S-BUS frame sent with all channels set to 1500.")

try:
    send_sbus()
except KeyboardInterrupt:
    pass
finally:
    pi.stop()

```
```
import time
from pymavlink import mavutil

# Create a connection to the Pixhawk
# Change the serial port and baudrate as needed
master = mavutil.mavlink_connection('/dev/ttyAMA0', baud=57600)

# Wait for a heartbeat from the Pixhawk to ensure connection
master.wait_heartbeat()
print("Heartbeat from Pixhawk received.")

# Function to send RC channel override commands
def send_rc_override(ch1=1500, ch2=1500, ch3=1500, ch4=1500):
    # Define RC channel values (0-8, or None to leave unchanged)
    master.mav.rc_channels_override_send(
        master.target_system,  # target system (Pixhawk)
        master.target_component,  # target component (Pixhawk)
        ch1,  # RC channel 1 (Throttle, 1500 is neutral)
        ch2,  # RC channel 2 (Aileron)
        ch3,  # RC channel 3 (Elevator)
        ch4,  # RC channel 4 (Rudder)
        1500,  # Channel 5 (1500 is neutral)
        1500,  # Channel 6
        1500,  # Channel 7
        1500  # Channel 8
    )

# Example: Sending neutral values for the first 4 RC channels
try:
    while True:
        send_rc_override(ch1=1500, ch2=1500, ch3=1500, ch4=1500)
        print("RC override sent with neutral values.")
        time.sleep(0.1)  # Send RC override every 100 ms
except KeyboardInterrupt:
    pass
finally:
    # Send a command to stop RC overrides (restore manual control)
    send_rc_override(ch1=None, ch2=None, ch3=None, ch4=None)
    print("RC override stopped, manual control restored.")

```
