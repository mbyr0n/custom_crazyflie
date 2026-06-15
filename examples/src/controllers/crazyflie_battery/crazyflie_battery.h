/**
 * @file <crazyflie_battery.h>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#ifndef CRAZYFLIE_BATTERY_H
#define CRAZYFLIE_BATTERY_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_battery_sensor.h>

using namespace argos;

class CCrazyflieBattery : public CCI_Controller {

public:

   CCrazyflieBattery();

   virtual ~CCrazyflieBattery() {}

   virtual void Init(TConfigurationNode& t_node);

   virtual void ControlStep();

   virtual void Reset() {}

   virtual void Destroy() {}

private:

   CCI_BatterySensor* m_pcBattery;
};

#endif
