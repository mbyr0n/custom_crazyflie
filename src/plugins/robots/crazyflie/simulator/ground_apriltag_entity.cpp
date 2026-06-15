/**
 * @file <argos3/plugins/robots/crazyflie/simulator/ground_apriltag_entity.cpp>
 */

#include "ground_apriltag_entity.h"

#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/simulator/physics_engines/pointmass3d/pointmass3d_engine.h>

namespace argos {

   /****************************************/
   /****************************************/

   CGroundAprilTagEntity::CGroundAprilTagEntity() :
      CComposableEntity(nullptr),
      m_unMarkerId(0),
      m_fSize(0.16f),
      m_pcEmbodiedEntity(nullptr) {
   }

   /****************************************/
   /****************************************/

   CGroundAprilTagEntity::CGroundAprilTagEntity(const std::string& str_id,
                                                UInt32 un_marker_id,
                                                const CVector3& c_position,
                                                const CQuaternion& c_orientation,
                                                Real f_size) :
      CComposableEntity(nullptr, str_id),
      m_unMarkerId(un_marker_id),
      m_fSize(f_size),
      m_pcEmbodiedEntity(nullptr) {
      try {
         m_pcEmbodiedEntity = new CEmbodiedEntity(this, "body_0", c_position, c_orientation);
         m_pcEmbodiedEntity->SetMovable(false);
         AddComponent(*m_pcEmbodiedEntity);
         UpdateComponents();
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("Failed to initialize entity \"" << GetId() << "\".", ex);
      }
   }

   /****************************************/
   /****************************************/

   void CGroundAprilTagEntity::Init(TConfigurationNode& t_tree) {
      try {
         CComposableEntity::Init(t_tree);

         GetNodeAttribute(t_tree, "marker_id", m_unMarkerId);
         GetNodeAttributeOrDefault(t_tree, "size", m_fSize, m_fSize);

         if(NodeExists(t_tree, "body")) {
             m_pcEmbodiedEntity = new CEmbodiedEntity(this);
             AddComponent(*m_pcEmbodiedEntity);
             m_pcEmbodiedEntity->Init(GetNode(t_tree, "body"));
             m_pcEmbodiedEntity->SetMovable(false);
          }
          else {
             CVector3 cPosition;
             CQuaternion cOrientation;
             GetNodeAttribute(t_tree, "position", cPosition);
             GetNodeAttributeOrDefault(t_tree, "orientation", cOrientation, cOrientation);
             m_pcEmbodiedEntity = new CEmbodiedEntity(this, "body_0", cPosition, cOrientation);
             m_pcEmbodiedEntity->SetMovable(false);
             AddComponent(*m_pcEmbodiedEntity);
          }

         UpdateComponents();
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("Failed to initialize entity \"" << GetId() << "\".", ex);
      }
   }

   /****************************************/
   /****************************************/

   void CGroundAprilTagEntity::Reset() {
      CComposableEntity::Reset();
      UpdateComponents();
   }

   /****************************************/
   /****************************************/

   void CGroundAprilTagEntity::UpdateComponents() {
   }

   /****************************************/
   /****************************************/

   class CPointMass3DOperationAddGroundAprilTag : public CPointMass3DOperationAddEntity {
   public:
      SOperationOutcome ApplyTo(CPointMass3DEngine& c_engine,
                                CGroundAprilTagEntity& c_entity) {
         return SOperationOutcome(true);
      }
   };

   REGISTER_POINTMASS3D_OPERATION(CPointMass3DOperationAddEntity,
                                  CPointMass3DOperationAddGroundAprilTag,
                                  CGroundAprilTagEntity);

   /****************************************/
   /****************************************/

   class CPointMass3DOperationRemoveGroundAprilTag : public CPointMass3DOperationRemoveEntity {
   public:
      SOperationOutcome ApplyTo(CPointMass3DEngine& c_engine,
                                CGroundAprilTagEntity& c_entity) {
         return SOperationOutcome(true);
      }
   };

   REGISTER_POINTMASS3D_OPERATION(CPointMass3DOperationRemoveEntity,
                                  CPointMass3DOperationRemoveGroundAprilTag,
                                  CGroundAprilTagEntity);

   /****************************************/
   /****************************************/

   REGISTER_ENTITY(CGroundAprilTagEntity,
                   "ground_apriltag",
                   "Byron Hawkins",
                   "1.0",
                   "A fixed ground AprilTag marker.",
                   "A ground AprilTag marker entity stores a marker id, physical size,\n"
                   "and pose for use by simulator sensors.\n\n"
                   "REQUIRED XML CONFIGURATION\n\n"
                   "  <arena ...>\n"
                   "    ...\n"
                   "    <ground_apriltag id=\"tag0\" marker_id=\"0\" position=\"0,0,0\" />\n"
                   "    ...\n"
                   "  </arena>\n\n"
                   "The 'id' attribute is necessary and must be unique among the entities.\n"
                   "The 'marker_id' attribute identifies the AprilTag marker.\n"
                   "The 'position' attribute specifies the marker position in the arena.\n\n"
                   "OPTIONAL XML CONFIGURATION\n\n"
                   "The 'orientation' attribute specifies the marker orientation in Z,Y,X\n"
                   "Euler angles expressed in degrees. It defaults to 0,0,0.\n"
                   "The 'size' attribute specifies the marker side length in meters. It\n"
                   "defaults to 0.16.\n",
                   "Usable");

   /****************************************/
   /****************************************/

   REGISTER_STANDARD_SPACE_OPERATIONS_ON_COMPOSABLE(CGroundAprilTagEntity);

   /****************************************/
   /****************************************/
}
