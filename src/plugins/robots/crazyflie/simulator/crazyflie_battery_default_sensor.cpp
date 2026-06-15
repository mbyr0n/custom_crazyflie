/**
 * @file <argos3/plugins/robots/crazyflie/simulator/crazyflie_battery_default_sensor.cpp>
 *
 * @author Daniel H. Stolfi based on the Adhavan Jayabalan's work.
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/core/simulator/entity/composable_entity.h>

#include "crazyflie_battery_default_sensor.h"
#include "crazyflie_battery_equipped_entity.h"

namespace argos {

   /****************************************/
   /****************************************/

   static CRange<Real> UNIT(0.0f, 1.0f);

   /****************************************/
   /****************************************/

   CCrazyflieBatteryDefaultSensor::CCrazyflieBatteryDefaultSensor() :
      m_pcEmbodiedEntity(nullptr),
      m_pcBatteryEntity(nullptr),
      m_pcRNG(nullptr),
      m_bAddNoise(false) {}

   /****************************************/
   /****************************************/

   void CCrazyflieBatteryDefaultSensor::SetRobot(CComposableEntity& c_entity) {
      try {
          m_pcEmbodiedEntity = &(c_entity.GetComponent<CEmbodiedEntity>("body"));
          m_pcBatteryEntity = &(c_entity.GetComponent<CCrazyflieBatteryEquippedEntity>("crazyflie_battery"));
          m_pcBatteryEntity->Enable();
      }
      catch(CARGoSException&  ex) {
          THROW_ARGOSEXCEPTION_NESTED("Can't set robot for the battery default sensor", ex);
      }
   }

   /****************************************/
   /****************************************/

   void CCrazyflieBatteryDefaultSensor::Init(TConfigurationNode& t_tree) {
      try {
         /* Execute standard logic */
         CCI_BatterySensor::Init(t_tree);
         /* Parse noise range */
         GetNodeAttributeOrDefault(t_tree, "noise_range", m_cNoiseRange, m_cNoiseRange);
         if(m_cNoiseRange.GetSpan() != 0) {
            m_bAddNoise = true;
            m_pcRNG = CRandom::CreateRNG("argos");
         }
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("Initialization error in default battery sensor", ex);
      }
      /* sensor is enabled by default */
      Enable();
   }

   /****************************************/
   /****************************************/

   void CCrazyflieBatteryDefaultSensor::Update() {
      /* sensor is disabled--nothing to do */
      if (IsDisabled()) {
        return;
      }
      /* Save old charge value (used later for time left estimation) */
      m_sReading.AvailableCharge =
         m_pcBatteryEntity->GetAvailableCharge();
      /* Add noise */
      if(m_bAddNoise) {
         m_sReading.AvailableCharge += m_pcRNG->Uniform(m_cNoiseRange);
         /* To trunc battery level between 0 and 1 */
         UNIT.TruncValue(m_sReading.AvailableCharge);
      }
      m_sReading.TimeLeft = m_pcBatteryEntity->GetTimeLeft();
   }

   /****************************************/
   /****************************************/

   void CCrazyflieBatteryDefaultSensor::Reset() {
      /* TODO */
   }

   /****************************************/
   /****************************************/

   REGISTER_SENSOR(CCrazyflieBatteryDefaultSensor,
                   "crazyflie_battery", "default",
                   "Daniel H. Stolfi based on the Adhavan Jayabalan's work",
                   "1.0",
                   "A battery level sensor for Crazyflie drones.",

                   "This sensor returns the current battery level of a Crazyflie drone. In\n"
                   "controllers, you must include the ci_battery_sensor.h header.\n\n"

                   "This sensor is enabled by default.\n\n"

                   "REQUIRED XML CONFIGURATION\n\n"
                   "  <controllers>\n"
                   "    ...\n"
                   "    <my_controller ...>\n"
                   "      ...\n"
                   "      <sensors>\n"
                   "        ...\n"
                   "        <crazyflie_battery implementation=\"default\" />\n"
                   "        ...\n"
                   "      </sensors>\n"
                   "      ...\n"
                   "    </my_controller>\n"
                   "    ...\n"
                   "  </controllers>\n\n"

                   "OPTIONAL XML CONFIGURATION\n\n"

                   "It is possible to add uniform noise to the sensor, thus matching the\n"
                   "characteristics of a real robot better. You can add noise through the\n"
                   "attribute 'noise_range' as follows:\n\n"
                   "  <controllers>\n"
                   "    ...\n"
                   "    <my_controller ...>\n"
                   "      ...\n"
                   "      <sensors>\n"
                   "        ...\n"
                   "        <crazyflie_battery implementation=\"default\"\n"
                   "                           noise_range=\"-0.3:0.4\" />\n"
                   "        ...\n"
                   "      </sensors>\n"
                   "      ...\n"
                   "    </my_controller>\n"
                   "    ...\n"
                   "  </controllers>\n\n",

                   "Usable"
		  );

}
