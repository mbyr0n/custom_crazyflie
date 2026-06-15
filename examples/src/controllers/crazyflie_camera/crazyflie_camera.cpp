/**
 * @file <crazyflie_camera.cpp>
 *
 * @author Daniel H. Stolfi
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#include "crazyflie_camera.h"

#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>

/****************************************/
/****************************************/

CCrazyflieCamera::CCrazyflieCamera() :
   m_pcSpdAct(NULL),m_pcPosSens(NULL),m_pcCamera(NULL),m_pcLedAct(NULL) {}

/****************************************/
/****************************************/

void CCrazyflieCamera::Init(TConfigurationNode& t_node) {
   m_pcSpdAct  = GetActuator<CCI_QuadRotorSpeedActuator            >("quadrotor_speed"                          );
   m_pcPosSens = GetSensor  <CCI_PositioningSensor                 >("positioning"                              );
   m_pcCamera  = GetSensor  <CCI_ColoredBlobPerspectiveCameraSensor>("crazyflie_colored_blob_perspective_camera");
   m_pcLedAct  = GetActuator<CCI_LEDsActuator                      >("leds"                                     );
   m_pcCamera->Enable();
}

/****************************************/
/****************************************/

void CCrazyflieCamera::ControlStep() {

   unsigned uTick = CSimulator::GetInstance().GetSpace().GetSimulationClock();
   std::string sId = CCI_Controller::GetId();

   /* Perspective Camera */
   const CCI_ColoredBlobPerspectiveCameraSensor::SReadings& sReadings = m_pcCamera->GetReadings();
   LOG << sId << "> Camera: " << std::endl;
   for (size_t i = 0; i < sReadings.BlobList.size(); i++) {
         CCI_ColoredBlobPerspectiveCameraSensor::SBlob* sBlob = sReadings.BlobList[i];
      LOG << ".....(Color = " << sBlob->Color << ", X = " << sBlob->X << ",Y = " << sBlob->Y << ")" << std::endl;
   }

   if (sId == "1") {
      m_pcLedAct->SetAllColors(CColor::RED);
   }
   if (sId == "2") {
      m_pcLedAct->SetAllColors(CColor::BLUE);
      m_pcSpdAct->SetRotationalSpeed(CRadians::PI_OVER_SIX);
   }

}

/****************************************/
/****************************************/

REGISTER_CONTROLLER(CCrazyflieCamera, "crazyflie_camera_controller")
