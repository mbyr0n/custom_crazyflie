/**
 * @file <crazyflie_position.cpp>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#include "crazyflie_position.h"

#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>

/****************************************/
/****************************************/

CCrazyfliePosition::CCrazyfliePosition() :
   m_pcPosAct(NULL),m_pcPosSens(NULL) {}

/****************************************/
/****************************************/

void CCrazyfliePosition::Init(TConfigurationNode& t_node) {
    m_pcPosAct  = GetActuator<CCI_QuadRotorPositionActuator>("quadrotor_position");
    m_pcPosSens = GetSensor  <CCI_PositioningSensor        >("positioning"       );
}

/****************************************/
/****************************************/

void CCrazyfliePosition::ControlStep() {

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
   switch (uTick) {
      case 10:
         m_pcPosAct->SetRelativePosition(CVector3(-2.0,0.0,0.0));
         m_pcPosAct->SetAbsoluteYaw(CRadians::PI);
         break;
      case 250:
         m_pcPosAct->SetAbsolutePosition(CVector3(0.0,2.0,1.5));
         m_pcPosAct->SetRelativeYaw(-CRadians::PI);
         break;
      case 530:
         m_pcPosAct->SetRelativePosition(CVector3(2.0,-4.0,-1.0));
         m_pcPosAct->SetAbsoluteYaw(CRadians::PI_OVER_TWO);
         break;

   }
}

/****************************************/
/****************************************/

REGISTER_CONTROLLER(CCrazyfliePosition, "crazyflie_position_controller")
