/**
 * @file <argos3/plugins/robots/crazyflie/simulator/crazyflie_battery_equipped_entity.h>
 *
 * @author Daniel H. Stolfi based on the Adhavan Jayabalan's work.
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */
#ifndef CRAZY_FLIE_BATTERY_EQUIPPED_ENTITY_H
#define CRAZY_FLIE_BATTERY_EQUIPPED_ENTITY_H

namespace argos {
   class CCrazyflieBatteryEquippedEntity;
   class CCrazyflieBatteryDischargeModel;
}

#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/entity/entity.h>
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <map>

#define CF_BATTERY_A -2.347383e-08
#define CF_BATTERY_B  1.582833e-05
#define CF_BATTERY_C -4.506692e-03
#define CF_BATTERY_D  1.009061
#define CF_BATTERY_MAX_T 427.21

namespace argos {

   /****************************************/
   /****************************************/

   /**
    * The battery entity.
    */
   class CCrazyflieBatteryEquippedEntity : public CEntity {

   public:

      ENABLE_VTABLE();

   public:

      CCrazyflieBatteryEquippedEntity(CComposableEntity* pc_parent);

      CCrazyflieBatteryEquippedEntity(CComposableEntity *pc_parent,
                                      const std::string& str_id,
                                      Real f_start_charge = 1.0);

      virtual ~CCrazyflieBatteryEquippedEntity();

      virtual void Init(TConfigurationNode& t_tree);

      virtual std::string GetTypeDescription() const {
         return "crazyflie_battery";
      }

      virtual void Update();

      Real GetAvailableCharge() const {
         return m_fAvailableCharge;
      }

      void SetAvailableCharge(Real f_available_charge) {
         m_fAvailableCharge = std::min(1.0, f_available_charge);
      }

      Real GetTimeLeft() const {
         return m_fTimeLeft;
      }


   protected:

      /** Time Left*/
      Real m_fTimeLeft;
      /** Available charge */
      Real m_fAvailableCharge;
   };

   /****************************************/
   /****************************************/

}

#endif
