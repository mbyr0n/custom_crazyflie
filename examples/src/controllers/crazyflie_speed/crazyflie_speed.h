/**
 * @file <crazyflie_speed.h>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#ifndef CRAZYFLIE_SPEED_H
#define CRAZYFLIE_SPEED_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_speed_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>

using namespace argos;

class CCrazyflieSpeed : public CCI_Controller {

public:

   CCrazyflieSpeed();

   virtual ~CCrazyflieSpeed() {}

   virtual void Init(TConfigurationNode& t_node);

   virtual void ControlStep();

   virtual void Reset() {}

   virtual void Destroy() {}

private:
   CCI_QuadRotorSpeedActuator* m_pcSpdAct;
   CCI_PositioningSensor* m_pcPosSens;

};

#endif
