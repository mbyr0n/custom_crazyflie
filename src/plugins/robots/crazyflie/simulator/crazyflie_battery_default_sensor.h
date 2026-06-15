/**
 * @file <argos3/plugins/robots/crazyflie/simulator/crazyflie_battery_default_sensor.h>
 *
 * @author Daniel H. Stolfi based on the Adhavan Jayabalan's work.
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#ifndef CRAZYFLIE_BATTERY_DEFAULT_SENSOR_H
#define CRAZYFLIE_BATTERY_DEFAULT_SENSOR_H

#include <string>
#include <map>

namespace argos {
   class CCrazyflieBatteryDefaultSensor;
   class CCrazyflieBatteryEquippedEntity;
   class CPhysicsEngine;
}

#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_battery_sensor.h>

namespace argos {

   class CCrazyflieBatteryDefaultSensor : public CSimulatedSensor,
                                          public CCI_BatterySensor {

   public:

      CCrazyflieBatteryDefaultSensor();

      virtual ~CCrazyflieBatteryDefaultSensor() {}

      virtual void SetRobot(CComposableEntity& c_entity);

      virtual void Init(TConfigurationNode& t_tree);

      virtual void Update();

      virtual void Reset();

   protected:

      /** Reference to embodied entity associated to this sensor */
      CEmbodiedEntity* m_pcEmbodiedEntity;

      /** Reference to battery sensor equipped entity associated to this sensor */
      CCrazyflieBatteryEquippedEntity* m_pcBatteryEntity;

      /** Random number generator */
      CRandom::CRNG* m_pcRNG;

      /** Whether to add noise or not */
      bool m_bAddNoise;

      /** Noise range on battery level */
      CRange<Real> m_cNoiseRange;
   };

}

#endif
