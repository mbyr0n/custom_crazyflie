/**
 * @file <crazyflie_position.h>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#ifndef CRAZYFLIE_POSITION_H
#define CRAZYFLIE_POSITION_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_position_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>

using namespace argos;

class CCrazyfliePosition : public CCI_Controller {

public:

   CCrazyfliePosition();

   virtual ~CCrazyfliePosition() {}

   virtual void Init(TConfigurationNode& t_node);

   virtual void ControlStep();

   virtual void Reset() {}

   virtual void Destroy() {}

private:
   CCI_QuadRotorPositionActuator* m_pcPosAct;
   CCI_PositioningSensor* m_pcPosSens;

};

#endif
