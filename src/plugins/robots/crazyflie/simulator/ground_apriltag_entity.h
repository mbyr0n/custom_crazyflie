/**
 * @file <argos3/plugins/robots/crazyflie/simulator/ground_apriltag_entity.h>
 */

#ifndef GROUND_APRILTAG_ENTITY_H
#define GROUND_APRILTAG_ENTITY_H

namespace argos {
   class CGroundAprilTagEntity;
   class CEmbodiedEntity;
}

#include <argos3/core/simulator/entity/composable_entity.h>

namespace argos {

   class CGroundAprilTagEntity : public CComposableEntity {

   public:

      ENABLE_VTABLE();

   public:

      CGroundAprilTagEntity();

      CGroundAprilTagEntity(const std::string& str_id,
                            UInt32 un_marker_id,
                            const CVector3& c_position,
                            const CQuaternion& c_orientation = CQuaternion(),
                            Real f_size = 0.16f);

      virtual void Init(TConfigurationNode& t_tree);
      virtual void Reset();
      virtual void UpdateComponents();

      inline UInt32 GetMarkerId() const {
         return m_unMarkerId;
      }

      inline Real GetSize() const {
         return m_fSize;
      }

      inline CEmbodiedEntity& GetEmbodiedEntity() {
         return *m_pcEmbodiedEntity;
      }

      inline const CEmbodiedEntity& GetEmbodiedEntity() const {
         return *m_pcEmbodiedEntity;
      }

      virtual std::string GetTypeDescription() const {
         return "ground_apriltag";
      }

   private:

      UInt32 m_unMarkerId;
      Real m_fSize;
      CEmbodiedEntity* m_pcEmbodiedEntity;
   };
}

#endif
