/**
 * @file <crazyflie_camera.h>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#ifndef CRAZYFLIE_CAMERA_H
#define CRAZYFLIE_CAMERA_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_speed_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_colored_blob_perspective_camera_sensor.h>

using namespace argos;

class CCrazyflieCamera : public CCI_Controller {

public:

   CCrazyflieCamera();

   virtual ~CCrazyflieCamera() {}

   virtual void Init(TConfigurationNode& t_node);

   virtual void ControlStep();

   virtual void Reset() {}

   virtual void Destroy() {}

private:
   CCI_QuadRotorSpeedActuator* m_pcSpdAct;
   CCI_PositioningSensor* m_pcPosSens;
   CCI_ColoredBlobPerspectiveCameraSensor* m_pcCamera;
   CCI_LEDsActuator* m_pcLedAct;
};

#endif
