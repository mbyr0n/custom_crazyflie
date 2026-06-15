add_library(crazyflie_led MODULE crazyflie_led.h crazyflie_led.cpp)
target_link_libraries(crazyflie_led
  argos3core_simulator
  argos3plugin_simulator_crazyflie
  argos3plugin_simulator_genericrobot)

