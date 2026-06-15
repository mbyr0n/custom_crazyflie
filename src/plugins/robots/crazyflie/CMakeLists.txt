#
# Crazyflie headers
#
# argos3/plugins/robots/crazyflie/simulator
set(ARGOS3_HEADERS_PLUGINS_ROBOTS_crazyflie_CONTROLINTERFACE
  control_interface/ci_crazyflie_apriltag_sensor.h)

if(ARGOS_BUILD_FOR_SIMULATOR)
  set(ARGOS3_HEADERS_PLUGINS_ROBOTS_crazyflie_SIMULATOR
    simulator/crazyflie_entity.h
    simulator/ground_apriltag_entity.h
    simulator/crazyflie_apriltag_default_sensor.h
    simulator/crazyflie_battery_equipped_entity.h
    simulator/crazyflie_battery_default_sensor.h    
    simulator/crazyflie_camera_equipped_entity.h
    simulator/crazyflie_colored_blob_perspective_camera_default_sensor.h
    simulator/pointmass3d_crazyflie_model.h)
endif(ARGOS_BUILD_FOR_SIMULATOR)

#
# Crazyflie sources
#
set(ARGOS3_SOURCES_PLUGINS_ROBOTS_CRAZYFLIE
  ${ARGOS3_HEADERS_PLUGINS_ROBOTS_crazyflie_CONTROLINTERFACE}
  control_interface/ci_crazyflie_apriltag_sensor.cpp)

if(ARGOS_BUILD_FOR_SIMULATOR)
  set(ARGOS3_SOURCES_PLUGINS_ROBOTS_CRAZYFLIE
    ${ARGOS3_SOURCES_PLUGINS_ROBOTS_CRAZYFLIE}
    ${ARGOS3_HEADERS_PLUGINS_ROBOTS_crazyflie_SIMULATOR}
    simulator/crazyflie_entity.cpp
    simulator/ground_apriltag_entity.cpp
    simulator/crazyflie_apriltag_default_sensor.cpp
    simulator/crazyflie_battery_equipped_entity.cpp
    simulator/crazyflie_battery_default_sensor.cpp
    simulator/crazyflie_camera_equipped_entity.cpp
    simulator/crazyflie_colored_blob_perspective_camera_default_sensor.cpp
    simulator/pointmass3d_crazyflie_model.cpp)
  # Compile the graphical visualization only if the necessary libraries have been found
  if(ARGOS_QTOPENGL_FOUND)
    include_directories(${ARGOS_QTOPENGL_INCLUDE_DIRS})
    set(ARGOS3_HEADERS_PLUGINS_ROBOTS_crazyflie_SIMULATOR
      ${ARGOS3_HEADERS_PLUGINS_ROBOTS_crazyflie_SIMULATOR}
      simulator/qtopengl_crazyflie.h)
    set(ARGOS3_SOURCES_PLUGINS_ROBOTS_CRAZYFLIE
      ${ARGOS3_SOURCES_PLUGINS_ROBOTS_CRAZYFLIE}
      simulator/qtopengl_crazyflie.h
      simulator/qtopengl_crazyflie.cpp)
  endif(ARGOS_QTOPENGL_FOUND)
endif(ARGOS_BUILD_FOR_SIMULATOR)

#
# Create Crazyflie plugin
#
add_library(argos3plugin_${ARGOS_BUILD_FOR}_crazyflie SHARED ${ARGOS3_SOURCES_PLUGINS_ROBOTS_CRAZYFLIE})
target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_crazyflie
  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot
  argos3plugin_${ARGOS_BUILD_FOR}_pointmass3d)
if(ARGOS_QTOPENGL_FOUND)
  target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_crazyflie argos3plugin_${ARGOS_BUILD_FOR}_qtopengl)
endif(ARGOS_QTOPENGL_FOUND)

#
# Add plugin to ARGOS_PLUGIN_PATH
#
set(ARGOS_PLUGIN_PATH "${ARGOS_PLUGIN_PATH}:${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "ARGoS plugin path")

install(FILES ${ARGOS3_HEADERS_PLUGINS_ROBOTS_crazyflie_CONTROLINTERFACE} DESTINATION include/argos3/plugins/robots/crazyflie/control_interface)

if(ARGOS_BUILD_FOR_SIMULATOR)
  install(FILES ${ARGOS3_HEADERS_PLUGINS_ROBOTS_crazyflie_SIMULATOR}      DESTINATION include/argos3/plugins/robots/crazyflie/simulator)
endif(ARGOS_BUILD_FOR_SIMULATOR)

install(TARGETS argos3plugin_${ARGOS_BUILD_FOR}_crazyflie
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/argos3
  ARCHIVE DESTINATION lib/argos3)
