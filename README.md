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

