/**
 * @file <crazyflie_speed.cpp>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#include "crazyflie_speed.h"

#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>

/****************************************/
/****************************************/

CCrazyflieSpeed::CCrazyflieSpeed() :
   m_pcSpdAct(NULL),m_pcPosSens(NULL) {}

/****************************************/
/****************************************/

void CCrazyflieSpeed::Init(TConfigurationNode& t_node) {
    m_pcSpdAct  = GetActuator<CCI_QuadRotorSpeedActuator>("quadrotor_speed");
    m_pcPosSens = GetSensor  <CCI_PositioningSensor     >("positioning"    );
}

/****************************************/
/****************************************/

void CCrazyflieSpeed::ControlStep() {

   unsigned uTick = CSimulator::GetInstance().GetSpace().GetSimulationClock();
   std::string sId = CCI_Controller::GetId();

   /* Get readings from position sensor */
   CVector3 cPos = m_pcPosSens->GetReading().Position;
   CQuaternion cOri = m_pcPosSens->GetReading().Orientation;
   CRadians cZAngle, cYAngle, cXAngle;
   cOri.ToEulerAngles(cZAngle, cYAngle, cXAngle);
   LOG << "Position: " << std::fixed << std::setprecision(2) << cPos.GetX() << "," << cPos.GetY() << "," << cPos.GetZ();
   LOG << " : Orientation: " << ToDegrees(cXAngle.UnsignedNormalize()).GetValue() <<
                           "," << ToDegrees(cYAngle.UnsignedNormalize()).GetValue() <<
                           "," << ToDegrees(cZAngle.UnsignedNormalize()).GetValue() << std::endl;

   /* Movement */
   if (uTick > 150) {
      m_pcSpdAct->SetLinearVelocity(CVector3::ZERO);
      m_pcSpdAct->SetRotationalSpeed(CRadians::ZERO);
   } else if (uTick > 100) {
      m_pcSpdAct->SetLinearVelocity(CVector3(0.33,-0.5,-0.33));
      m_pcSpdAct->SetRotationalSpeed(CRadians::PI_OVER_TWO);
   } else if (uTick > 80) {
      m_pcSpdAct->SetLinearVelocity(CVector3::ZERO);
      m_pcSpdAct->SetRotationalSpeed(CRadians::ZERO);
   } else if (uTick > 60) {
      m_pcSpdAct->SetLinearVelocity(CVector3(1.0,0.5,0.5));
      m_pcSpdAct->SetRotationalSpeed(-CRadians::PI_OVER_SIX);
   } else if (uTick > 40) {
      m_pcSpdAct->SetLinearVelocity(CVector3::ZERO);
      m_pcSpdAct->SetRotationalSpeed(CRadians::ZERO);
   } else if (uTick > 20) {
      m_pcSpdAct->SetLinearVelocity(CVector3(-1.0,0.0,0.0));
      m_pcSpdAct->SetRotationalSpeed(CRadians::PI_OVER_SIX);
   }
}

/****************************************/
/****************************************/

REGISTER_CONTROLLER(CCrazyflieSpeed, "crazyflie_speed_controller")
