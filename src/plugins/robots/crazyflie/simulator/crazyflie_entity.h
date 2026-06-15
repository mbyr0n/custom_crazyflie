/**
 * @file <argos3/plugins/robots/crazyflie/simulator/crazyflie_entity.h>
 *
 * @author Daniel H. Stolfi based on the Carlo Pinciroli's work
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#ifndef CRAZYFLIE_ENTITY_H
#define CRAZYFLIE_ENTITY_H

namespace argos {
   class CControllableEntity;
   class CEmbodiedEntity;
   class CCrazyflieEntity;
   class CQuadRotorEntity;
   class CLEDEquippedEntity;
   class CRABEquippedEntity;
   class CCrazyflieCameraEquippedEntity;
   class CCrazyflieBatteryEquippedEntity;
}

#include <argos3/core/simulator/entity/composable_entity.h>

namespace argos {

   class CCrazyflieEntity : public CComposableEntity {

   public:

      ENABLE_VTABLE();

   public:

      CCrazyflieEntity();

      CCrazyflieEntity(const std::string& str_id,
                   const std::string& str_controller_id,
                   const CVector3& c_position = CVector3(),
                   const CQuaternion& c_orientation = CQuaternion(),
                   Real f_rab_range = 3.0f,
                   size_t un_rab_data_size = 10,
                   const CRadians& c_perspcam_aperture = ToRadians(CDegrees(33.75f)),
                   Real f_perspcam_focal_length = 0.035f,
                   Real f_perspcam_range = 3.0f);

      virtual void Init(TConfigurationNode& t_tree);
      virtual void Reset();
      virtual void UpdateComponents();

      inline CControllableEntity& GetControllableEntity() {
         return *m_pcControllableEntity;
      }

      inline CEmbodiedEntity& GetEmbodiedEntity() {
         return *m_pcEmbodiedEntity;
      }

      inline CQuadRotorEntity& GetQuadRotorEntity() {
         return *m_pcQuadRotorEntity;
      }

      inline CRABEquippedEntity& GetRABEquippedEntity() {
         return *m_pcRABEquippedEntity;
      }

      inline CLEDEquippedEntity& GetLEDEquippedEntity() {
         return *m_pcLEDEquippedEntity;
      }

      inline CCrazyflieBatteryEquippedEntity& GetBatterySensorEquippedEntity() {
          return *m_pcBatteryEquippedEntity;
      }

      virtual std::string GetTypeDescription() const {
         return "crazyflie";
      }

   private:

      CControllableEntity*              m_pcControllableEntity;
      CEmbodiedEntity*                  m_pcEmbodiedEntity;
      CQuadRotorEntity*                 m_pcQuadRotorEntity;
      CLEDEquippedEntity*               m_pcLEDEquippedEntity;
      CRABEquippedEntity*               m_pcRABEquippedEntity;
      CCrazyflieCameraEquippedEntity*   m_pcPerspectiveCameraEquippedEntity;
      CCrazyflieBatteryEquippedEntity*  m_pcBatteryEquippedEntity;
   };

}

#endif
