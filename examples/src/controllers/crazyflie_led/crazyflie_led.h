/**
 * @file <crazyflie_led.h>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#ifndef CRAZYFLIE_LED_H
#define CRAZYFLIE_LED_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>

using namespace argos;

class CCrazyflieLed : public CCI_Controller {

public:

   CCrazyflieLed();

   virtual ~CCrazyflieLed() {}

   virtual void Init(TConfigurationNode& t_node);

   virtual void ControlStep();

   virtual void Reset() {}

   virtual void Destroy() {}

private:

   CCI_LEDsActuator* m_pcLedAct;
};

#endif
