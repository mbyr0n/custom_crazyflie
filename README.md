# ARGoS Crazyflie Plugin

ARGoS3 plugin for the [Crazyflie](https://www.bitcraze.io/products/crazyflie-2-1/) drone. Adds a `crazyflie` entity with simulated sensors (AprilTag detector, colored blob camera, battery) and supports standard ARGoS actuators (quadrotor control, LEDs, range-and-bearing).

## Compiling the code

Make sure you have ARGoS >= 3.0.0-beta52 installed!

Commands:

```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../src
make
sudo make install
```

If ARGoS does not find the new robot, try:

```shell
cmake -DCMAKE_BUILD_TYPE=Release ../src -DCMAKE_INSTALL_PREFIX=/usr
```

For Debug builds:

```shell
cmake -DCMAKE_BUILD_TYPE=Debug
```

## Based On

This work is based on the following resources:

**ARGoS-Crazyflie Model**
- Authors: Daniel H. Stolfi and Grégoire Danoy
- Paper: "An ARGoS plug-in for the Crazyflie drone" (2024)
- Repository: <https://gitlab.uni.lu/adars/crazyflie>

**ARGoS Simulator**
- Author: Carlo Pinciroli
- Paper: "ARGoS: a Modular, Parallel, Multi-Engine Simulator for Multi-Robot Systems" (2012)
- Repository: <https://github.com/ilpincy/argos3>

## Sensors

### `crazyflie_apriltag` (downward-facing)

Detects AprilTag entities below the drone. Tags above the camera plane are filtered out. Supports configurable field of view, range, and camera offset. Returns relative pose data for each detected tag.

**XML configuration:**

```xml
<lua_controller id="my_controller">
  <sensors>
    <crazyflie_apriltag implementation="default"
                        tag_medium="tags"
                        range="3.0"
                        fov="110"
                        show_rays="false"
                        show_fov="false"
                        camera_offset="0,0,-0.01" />
  </sensors>
</lua_controller>
```

| Parameter | Required | Default | Description |
|-----------|----------|---------|-------------|
| `tag_medium` | Yes | — | Tag medium ID (see below) |
| `range` | No | `3.0` | Max detection range in meters |
| `fov` | No | `110` | Field of view in degrees (0–180) |
| `show_rays` | No | `false` | Visualize detection rays in Qt/OpenGL |
| `show_fov` | No | `false` | Visualize FOV cone in Qt/OpenGL |
| `camera_offset` | No | `0,0,-0.01` | Camera position offset from body origin (meters) |

The tag medium must be declared in the `<media>` section:

```xml
<media>
  <tag id="tags" index="grid" grid_size="20,20,20" />
</media>
```

**Lua usage:**

The sensor is enabled by default. Detected tags are available in `robot.apriltag`:

```lua
for i, tag in ipairs(robot.apriltag) do
  log(string.format("tag[%d] id=%d x=%.3f y=%.3f yaw=%.3f distance=%.3f angle=%.3f",
                    i, tag.id, tag.x, tag.y, tag.yaw, tag.distance, tag.angle))
end
```

Each tag entry contains:

| Field | Type | Description |
|-------|------|-------------|
| `id` | integer | Tag ID (parsed from payload) |
| `x` | number | Relative X position in body frame (m) |
| `y` | number | Relative Y position in body frame (m) |
| `yaw` | number | Relative yaw in body frame (rad) |
| `distance` | number | Euclidean distance (m) |
| `angle` | number | Bearing angle (atan2 of y/x) |

**Example:** `examples/src/experiments/crazyflie_apriltag.argos`

### `crazyflie_colored_blob_perspective_camera` (forward-facing)

Detects colored LED blobs from other robots or light entities. Returns pixel coordinates and RGB color of each visible blob.

**XML configuration:**

```xml
<lua_controller id="my_controller">
  <sensors>
    <crazyflie_colored_blob_perspective_camera implementation="default"
                                                medium="leds"
                                                show_rays="true"
                                                noise_std_dev="0.0" />
  </sensors>
</lua_controller>
```

| Parameter | Required | Default | Description |
|-----------|----------|---------|-------------|
| `medium` | Yes | — | LED medium ID |
| `show_rays` | No | `false` | Visualize detection rays in Qt/OpenGL |
| `noise_std_dev` | No | `0.0` | Gaussian noise std. dev. added to blob pixel positions |

The LED medium must be declared:

```xml
<media>
  <led id="leds" />
</media>
```

**Lua usage:**

The sensor is disabled by default — call `.enable()` in `init()`:

```lua
function init()
  robot.colored_blob_perspective_camera.enable()
end

function step()
  for k, v in pairs(robot.colored_blob_perspective_camera) do
    if type(v) == "table" then
      log(string.format("blob %d: pixel=(%d,%d) color=(%d,%d,%d)",
                        k, v.x, v.y, v.color.red, v.color.green, v.color.blue))
    end
  end
end
```

**Example:** `examples/src/experiments/crazyflie_camera.argos`

### `crazyflie_battery`

Reports the battery available charge (0.0–1.0) and estimated time left (seconds). Charge depletes over time via a cubic discharge model. Enabled by default.

```xml
<sensors>
  <crazyflie_battery implementation="default" noise_range="-0.3:0.4" />
</sensors>
```

**Lua:** `robot.battery.available_charge`, `robot.battery.time_left`

### `positioning` (generic)

Absolute position (`CVector3`) and orientation (`CQuaternion`) in the arena. Enabled by default.

```xml
<sensors>
  <positioning implementation="default" />
</sensors>
```

**Lua:** `robot.positioning.position.{x,y,z}`, `robot.positioning.orientation.{w,x,y,z}`

### `range_and_bearing` (generic, sensor side)

Receives data packets from nearby robots with range and bearing information.

```xml
<sensors>
  <range_and_bearing implementation="medium" medium="rab" />
</sensors>
```

**Lua:** `robot.range_and_bearing[index].{range, horizontal_bearing, vertical_bearing, data}`

## Actuators

### `quadrotor_position`

Sets the target position and yaw. The point-mass physics engine uses PD control to reach the target.

```xml
<actuators>
  <quadrotor_position implementation="default" />
</actuators>
```

**Lua:**
| Function | Description |
|----------|-------------|
| `robot.quadrotor.set_absolute_position(x, y, z)` | Fly to absolute position |
| `robot.quadrotor.set_absolute_yaw(rad)` | Set absolute yaw |
| `robot.quadrotor.set_relative_position(x, y, z)` | Fly relative to current position |
| `robot.quadrotor.set_relative_yaw(rad)` | Rotate relative to current yaw |

### `quadrotor_speed`

Sets linear velocity (m/s) and rotational speed (rad/s) for velocity-mode control.

```xml
<actuators>
  <quadrotor_speed implementation="default" />
</actuators>
```

**Lua:** `robot.quadrotor.set_linear_velocity(x, y, z)`, `robot.quadrotor.set_rotational_speed(rad)`

### `leds`

Controls the bottom LED on the Crazyflie.

```xml
<actuators>
  <leds implementation="default" medium="leds" />
</actuators>
```

**Lua:** `robot.leds.set_all_colors(r, g, b)`, `robot.leds.set_single_color(index, r, g, b)`

### `range_and_bearing` (generic, actuator side)

Sends data packets via the RAB medium.

```xml
<actuators>
  <range_and_bearing implementation="default" />
</actuators>
```

**Lua:** `robot.range_and_bearing.set_data(...)`, `robot.range_and_bearing.clear_data()`

## Entity Configuration

Attributes on the `<crazyflie>` entity tag:

| Attribute | Default | Description |
|-----------|---------|-------------|
| `rab_range` | `3.0` | Max RAB communication range (m) |
| `rab_data_size` | `10` | RAB packet size (bytes) |

Battery initial charge (omit for infinite battery):

```xml
<crazyflie id="drone1" rab_range="5.0" rab_data_size="16">
  <body position="0,0,0" orientation="0,0,0"/>
  <controller config="my_controller"/>
  <crazyflie_battery start_charge="0.85"/>
</crazyflie>
```

## Reference

If you use this plug-in in your research, please cite:

D. H. Stolfi and G. Danoy, **An ARGoS plug-in for the Crazyflie drone.** 2024, <https://arxiv.org/abs/2401.16948>.
