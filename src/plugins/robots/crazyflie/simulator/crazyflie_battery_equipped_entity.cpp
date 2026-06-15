/**
 * @file <argos3/plugins/robots/crazyflie/simulator/crazyflie_battery_equipped_entity.cpp>
 *
 * @author Daniel H. Stolfi based on the Adhavan Jayabalan's work.
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */
#include "crazyflie_battery_equipped_entity.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/entity/composable_entity.h>

namespace argos {

   /****************************************/
   /****************************************/

   CCrazyflieBatteryEquippedEntity::CCrazyflieBatteryEquippedEntity(CComposableEntity *pc_parent) :
      CEntity(pc_parent){
          Disable();
   }

   /****************************************/
   /****************************************/

   CCrazyflieBatteryEquippedEntity::CCrazyflieBatteryEquippedEntity(CComposableEntity *pc_parent,
                                                  const std::string &str_id,
                                                  Real f_start_charge) :
      CEntity(pc_parent, str_id),
      m_fTimeLeft(CF_BATTERY_MAX_T),
      m_fAvailableCharge(std::min(1.0,f_start_charge)) {
          Disable();
   }

   /****************************************/
   /****************************************/

   CCrazyflieBatteryEquippedEntity::~CCrazyflieBatteryEquippedEntity() {
   }

   /****************************************/
   /****************************************/

   void CCrazyflieBatteryEquippedEntity::Init(TConfigurationNode& t_tree) {
      try {
         CEntity::Init(t_tree);
         /* Get initial battery level */
         /* Get initial battery charge */
         GetNodeAttributeOrDefault(t_tree, "start_charge",  m_fAvailableCharge,  m_fAvailableCharge);
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("Error initializing the battery sensor equipped entity \"" << GetId() << "\"", ex);
      }
   }

   /****************************************/
   /****************************************/

   void CCrazyflieBatteryEquippedEntity::Update() {
      if(CSimulator::GetInstance().GetSpace().GetSimulationClock() > 0) {
         if (m_fAvailableCharge > 0.0) {
            Real d = CF_BATTERY_D - m_fAvailableCharge;
            Real d0 = CF_BATTERY_B*CF_BATTERY_B - 3*CF_BATTERY_A*CF_BATTERY_C;
            Real d1 = 2*CF_BATTERY_B*CF_BATTERY_B*CF_BATTERY_B -
                        9*CF_BATTERY_A*CF_BATTERY_B*CF_BATTERY_C +
                        27*CF_BATTERY_A*CF_BATTERY_A*d;
            Real C = std::cbrt((d1 + sqrt(d1*d1 - 4*d0*d0*d0)) / 2);
            Real t = (-1/(3*CF_BATTERY_A) * (CF_BATTERY_B + C + d0/C)) + CSimulator::GetInstance().GetPhysicsEngines()[0]->GetSimulationClockTick();

            m_fTimeLeft = CF_BATTERY_MAX_T - t;
            if (t > CF_BATTERY_MAX_T) {
               m_fAvailableCharge = 0.0;
               m_fTimeLeft = 0.0;
            } else {
               m_fAvailableCharge =
                  Max<Real>(0.0,
                         CF_BATTERY_D +
                         CF_BATTERY_C * t +
                         CF_BATTERY_B * t * t +
                         CF_BATTERY_A * t * t * t);
            }
         } else {
            m_fTimeLeft = 0.0;
         }
      }
   }

   /****************************************/
   /****************************************/

   REGISTER_STANDARD_SPACE_OPERATIONS_ON_ENTITY(CCrazyflieBatteryEquippedEntity);

   /****************************************/
   /****************************************/

}
