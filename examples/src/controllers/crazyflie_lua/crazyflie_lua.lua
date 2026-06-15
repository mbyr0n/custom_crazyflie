-- crazyflie_lua.lua
-- by Daniel H. Stolfi
-- ADARS project -- PCOG / SnT / University of Luxembourg

counter = 0
function init()
   -- put your code here
   robot.colored_blob_perspective_camera.enable()
end

function step()

  qx = robot.positioning.orientation.x
  qy = robot.positioning.orientation.y
  qz = robot.positioning.orientation.z
  qw = robot.positioning.orientation.w
  
  -- yaw (z-axis rotation)
  siny_cosp = 2 * (qw * qz + qx * qy)
  cosy_cosp = 1 - 2 * (qy * qy + qz * qz)
  yaw = math.atan2(siny_cosp, cosy_cosp) * 180.0 / math.pi  

  log( string.format("%d : %.2f,%.2f,%.2f : %.1f : %.3f - %.1f", counter,
  		 robot.positioning.position.x, robot.positioning.position.y, 
	 	 robot.positioning.position.z, yaw, robot.battery.available_charge,
	 	 robot.battery.time_left) )

  -- camera
  for k, v in pairs(robot.colored_blob_perspective_camera) do
     if (type(v) == "table") then
        log(string.format("..... %d: (%d,%d) (%d,%d,%d)", k, v.x, v.y, v.color.red, v.color.green, v.color.blue) )
     end 
  end
	 	 
  -- LED 
  if counter == 5 then
    robot.leds.set_all_colors(1.0,0.0,0.0)
  end
  if counter == 10 then
    robot.leds.set_all_colors(0.0,1.0,0.0)
  end
  if counter == 15 then
    robot.leds.set_all_colors(0.0,0.0,1.0)
  end
  if counter == 20 then
    robot.leds.set_all_colors(1.0,1.0,0.0)
  end
  if counter == 25 then
    robot.leds.set_all_colors(0.0,1.0,1.0)
  end
  if counter == 30 then
    robot.leds.set_single_color(1, 1.0,0.0,1.0)
  end
  if counter == 35 then
    robot.leds.set_all_colors("white")
  end

  
  --[[ also change the xml file
  -- Position
  if counter == 40 then
    robot.quadrotor.set_absolute_position(2.0,0.0,1.0)
    robot.quadrotor.set_relative_yaw(math.pi/4.0)
  end
  if counter == 230 then
    robot.quadrotor.set_relative_position(-4.0,-1.0,0.5)
    robot.quadrotor.set_absolute_yaw(math.pi)    
  end
  ]]
  -- Speed
  if counter == 40 then
    robot.quadrotor.set_linear_velocity(-1.0,0.0,-0.25)
    robot.quadrotor.set_rotational_speed(-math.pi/2.0)
  end
  if counter == 60 then
    robot.quadrotor.set_linear_velocity(0.0,0.0,0.0)
    robot.quadrotor.set_rotational_speed(0.0)
  end
  if counter == 80 then
    robot.quadrotor.set_linear_velocity(0.5,0.10,0.25)
    robot.quadrotor.set_rotational_speed(math.pi)
  end
  if counter == 160 then
    robot.quadrotor.set_linear_velocity(0.0,0.0,0.0)
    robot.quadrotor.set_rotational_speed(-math.pi/8.0)
  end
  
  -- END 
  if counter == 460 then
    robot.leds.set_single_color(1, "black")
  end

  counter = counter + 1
end

function reset()
   -- put your code here
   counter = 0
end

function destroy()
   -- put your code here
end
