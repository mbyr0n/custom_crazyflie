/**
 * @file <crazyflie_led.cpp>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#include "crazyflie_led.h"

#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>

/****************************************/
/****************************************/

CCrazyflieLed::CCrazyflieLed() :
   m_pcLedAct(NULL) {}

/****************************************/
/****************************************/

void CCrazyflieLed::Init(TConfigurationNode& t_node) {
   m_pcLedAct = GetActuator<CCI_LEDsActuator>("leds");
}

/****************************************/
/****************************************/

void CCrazyflieLed::ControlStep() {

   unsigned uTick = CSimulator::GetInstance().GetSpace().GetSimulationClock();

   /* LED colour */
   switch (uTick) {
      case 10:
         m_pcLedAct->SetAllColors(CColor::RED);
         break;
      case 20:
         m_pcLedAct->SetAllColors(CColor::GREEN);
         break;
      case 30:
         m_pcLedAct->SetAllColors(CColor::BLUE);
         break;
      case 40:
         m_pcLedAct->SetAllColors(CColor::CYAN);
         break;
      case 50:
         m_pcLedAct->SetAllColors(CColor::MAGENTA);
         break;
      case 60:
         m_pcLedAct->SetSingleColor(0, CColor::YELLOW);
         break;
      case 70:
         m_pcLedAct->SetSingleColor(0, CColor(255,255,255));
         break;
      case 80:
         m_pcLedAct->SetAllColors(CColor::BLACK);
         break;
   }



}

/****************************************/
/****************************************/

REGISTER_CONTROLLER(CCrazyflieLed, "crazyflie_led_controller")
