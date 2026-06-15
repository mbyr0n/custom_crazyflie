/**
 * @file <crazyflie_battery.cpp>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#include "crazyflie_battery.h"

#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>

/****************************************/
/****************************************/

CCrazyflieBattery::CCrazyflieBattery() :
   m_pcBattery(NULL) {}

/****************************************/
/****************************************/

void CCrazyflieBattery::Init(TConfigurationNode& t_node) {
   m_pcBattery = GetSensor  <CCI_BatterySensor>("crazyflie_battery");
}

/****************************************/
/****************************************/

void CCrazyflieBattery::ControlStep() {

   unsigned uTick = CSimulator::GetInstance().GetSpace().GetSimulationClock();

   /* Get readings from Battery sensor */
   CCI_BatterySensor::SReading tBatReading = m_pcBattery->GetReading();
   LOG << uTick << ": Available Charge: " << std::fixed << std::setprecision(3) << tBatReading.AvailableCharge << "  Time Left: " << tBatReading.TimeLeft << std::endl;

   /* Check available charge */
   if (tBatReading.AvailableCharge <= 0.0) {
      CSimulator::GetInstance().Terminate();
   }
}

/****************************************/
/****************************************/

REGISTER_CONTROLLER(CCrazyflieBattery, "crazyflie_battery_controller")
