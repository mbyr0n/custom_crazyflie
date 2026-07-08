/**
 * @file <argos3/plugins/robots/crazyflie/simulator/crazyflie_apriltag_default_sensor.cpp>
 */

#include "crazyflie_apriltag_default_sensor.h"
#include "ground_apriltag_entity.h"

#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/core/simulator/entity/controllable_entity.h>
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/core/simulator/physics_engine/physics_engine.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/utility/datatypes/any.h>
#include <argos3/core/utility/math/ray3.h>
#include <argos3/plugins/simulator/entities/tag_entity.h>
#include <argos3/plugins/simulator/media/tag_medium.h>

#include <algorithm>
#include <cctype>

namespace argos {

   /****************************************/
   /****************************************/

   CCrazyflieAprilTagDefaultSensor::CCrazyflieAprilTagDefaultSensor() :
      m_pcEmbodiedEntity(nullptr),
      m_pcControllableEntity(nullptr),
      m_pcTagIndex(nullptr),
      m_fRange(3.0f),
      m_cHalfFOV(CRadians::ZERO),
      m_cCameraOffset(0.0f, 0.0f, -0.01f),
      m_bShowRays(false),
      m_bShowFOV(false) {
      CDegrees cFOVDegrees(110.0f);
      m_cHalfFOV.FromValueInDegrees(cFOVDegrees.GetValue() * 0.5f);
   }

   /****************************************/
   /****************************************/

   void CCrazyflieAprilTagDefaultSensor::SetRobot(CComposableEntity& c_entity) {
      m_pcEmbodiedEntity = &(c_entity.GetComponent<CEmbodiedEntity>("body"));
      m_pcControllableEntity = &(c_entity.GetComponent<CControllableEntity>("controller"));
   }

   /****************************************/
   /****************************************/

   void CCrazyflieAprilTagDefaultSensor::Init(TConfigurationNode& t_tree) {
      try {
          CCI_CrazyflieAprilTagSensor::Init(t_tree);
          GetNodeAttributeOrDefault(t_tree, "range", m_fRange, m_fRange);
          if(m_fRange <= 0.0f) {
             THROW_ARGOSEXCEPTION("The crazyflie AprilTag sensor range must be greater than 0.0");
          }
          CDegrees cFOVDegrees(110.0f);
          GetNodeAttributeOrDefault(t_tree, "fov", cFOVDegrees, cFOVDegrees);
          if(cFOVDegrees.GetValue() <= 0.0f || cFOVDegrees.GetValue() >= 180.0f) {
             THROW_ARGOSEXCEPTION("The crazyflie AprilTag sensor fov must be greater than 0 and less than 180 degrees");
          }
          m_cHalfFOV.FromValueInDegrees(cFOVDegrees.GetValue() * 0.5f);
          std::string strTagMedium;
          GetNodeAttribute(t_tree, "tag_medium", strTagMedium);
          m_pcTagIndex = &(CSimulator::GetInstance().GetMedium<CTagMedium>(strTagMedium).GetIndex());
          GetNodeAttributeOrDefault(t_tree, "show_rays", m_bShowRays, m_bShowRays);
          GetNodeAttributeOrDefault(t_tree, "show_fov", m_bShowFOV, m_bShowFOV);
          GetNodeAttributeOrDefault(t_tree, "camera_offset", m_cCameraOffset, m_cCameraOffset);
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("Error initializing the crazyflie AprilTag default sensor", ex);
      }
      Enable();
   }

   /****************************************/
   /****************************************/

   void CCrazyflieAprilTagDefaultSensor::Update() {
      m_tReadings.clear();

      if(IsDisabled()) {
         return;
      }

      const SAnchor& sBodyAnchor = m_pcEmbodiedEntity->GetOriginAnchor();
      const CQuaternion& cBodyOrientation = sBodyAnchor.Orientation;
      CVector3 cCameraOffset = m_cCameraOffset;
      cCameraOffset.Rotate(cBodyOrientation);
      CVector3 cCameraPosition = sBodyAnchor.Position + cCameraOffset;
      CQuaternion cInvBodyOrientation = cBodyOrientation.Inverse();

      if(m_pcTagIndex != nullptr) {
         class CTagOperation : public CPositionalIndex<CTagEntity>::COperation {
         public:
            CTagOperation(CCrazyflieAprilTagDefaultSensor& c_sensor,
                          const CVector3& c_camera_position,
                          const CQuaternion& c_inv_body_orientation) :
               Sensor(c_sensor),
               CameraPosition(c_camera_position),
               InvBodyOrientation(c_inv_body_orientation) {}

            virtual bool operator()(CTagEntity& c_tag) {
               Sensor.ProcessTag(c_tag, CameraPosition, InvBodyOrientation);
               return true;
            }

         private:
            CCrazyflieAprilTagDefaultSensor& Sensor;
            const CVector3& CameraPosition;
            const CQuaternion& InvBodyOrientation;
         } cTagOperation(*this, cCameraPosition, cInvBodyOrientation);

         m_pcTagIndex->ForEntitiesInSphereRange(cCameraPosition, m_fRange, cTagOperation);
      }

      CSpace::TMapPerType* ptGroundTags = nullptr;
      try {
         ptGroundTags = &CSimulator::GetInstance().GetSpace().GetEntitiesByType("ground_apriltag");
      }
      catch(CARGoSException&) {}
      if(ptGroundTags != nullptr) {
         CSpace::TMapPerType& tGroundTags = *ptGroundTags;
         for(CSpace::TMapPerType::iterator it = tGroundTags.begin(); it != tGroundTags.end(); ++it) {
            CGroundAprilTagEntity* pcTag = any_cast<CGroundAprilTagEntity*>(it->second);
            ProcessGroundTag(*pcTag, cCameraPosition, cInvBodyOrientation);
         }
      }

      if(m_bShowFOV) {
         AddFOVRays(cCameraPosition, cBodyOrientation);
      }
   }

   /****************************************/
   /****************************************/

   void CCrazyflieAprilTagDefaultSensor::Reset() {
      m_tReadings.clear();
   }

   /****************************************/
   /****************************************/

   void CCrazyflieAprilTagDefaultSensor::AddFOVRays(const CVector3& c_camera_position,
                                                    const CQuaternion& c_body_orientation) {
      const Real fHalfSide = m_fRange * Tan(m_cHalfFOV);
      CVector3 arrEndpoints[] = {
         CVector3( fHalfSide, 0.0f,      -m_fRange),
         CVector3(-fHalfSide, 0.0f,      -m_fRange),
         CVector3(0.0f,       fHalfSide, -m_fRange),
         CVector3(0.0f,      -fHalfSide, -m_fRange)
      };

      for(UInt32 i = 0; i < 4; ++i) {
         arrEndpoints[i].Rotate(c_body_orientation);
         arrEndpoints[i] += c_camera_position;
         m_pcControllableEntity->AddCheckedRay(
            false,
            CRay3(c_camera_position, arrEndpoints[i]));
      }
   }

   /****************************************/
   /****************************************/

   void CCrazyflieAprilTagDefaultSensor::ProcessTag(CTagEntity& c_tag,
                                                    const CVector3& c_camera_position,
                                                    const CQuaternion& c_inv_body_orientation) {
      CEntity& cTagRootEntity = c_tag.GetRootEntity();
      CEntity& cCameraRootEntity = m_pcControllableEntity->GetRootEntity();
      if(&cTagRootEntity == &cCameraRootEntity) {
         return;
      }

      CVector3 cRelative = c_tag.GetPosition() - c_camera_position;
      CVector3 cRelativeBody = cRelative;
      cRelativeBody.Rotate(c_inv_body_orientation);

      Real fDistance = cRelativeBody.Length();
      if(fDistance > m_fRange || fDistance == 0.0f) {
         return;
      }
      if(cRelativeBody.GetZ() >= 0.0f) {
         return;
      }
      if(ACos(-cRelativeBody.GetZ() / fDistance) > m_cHalfFOV) {
         return;
      }
      if(c_tag.GetObservableAngle() < CRadians::PI) {
         CVector3 cTagToCamera(c_camera_position - c_tag.GetPosition());
         CVector3 cTagDirection(CVector3::Z);
         cTagDirection.Rotate(c_tag.GetOrientation());
         if(ACos(cTagDirection.DotProduct(cTagToCamera) /
                 (cTagDirection.Length() * cTagToCamera.Length())) >
            c_tag.GetObservableAngle()) {
            return;
         }
      }

      SEmbodiedEntityIntersectionItem sIntersectionItem;
      CRay3 cOcclusionCheckRay(c_camera_position, c_tag.GetPosition());
      if(GetClosestEmbodiedEntityIntersectedByRay(sIntersectionItem,
                                                  cOcclusionCheckRay,
                                                  *m_pcEmbodiedEntity)) {
         CEntity& cIntersectionRootEntity = sIntersectionItem.IntersectedEntity->GetRootEntity();
         if(&cIntersectionRootEntity != &cTagRootEntity &&
            &cIntersectionRootEntity != &cCameraRootEntity) {
            return;
         }
      }

      UInt32 unId = 0;
      try {
         std::string strId(c_tag.GetPayload());
         std::string::iterator itRemove =
            std::remove_if(strId.begin(),
                           strId.end(),
                           [](unsigned char ch) {
                              return std::isdigit(ch) == 0;
                           });
         strId.erase(itRemove, strId.end());
         unId = std::stoul(strId);
      }
      catch(const std::logic_error&) {}

      CRadians cRelativeYaw, cTmp1, cTmp2;
      CQuaternion cRelativeOrientation = c_inv_body_orientation * c_tag.GetOrientation();
      cRelativeOrientation.ToEulerAngles(cRelativeYaw, cTmp1, cTmp2);
      Real fRelativeYaw = NormalizeAngle(cRelativeYaw.GetValue());

      m_tReadings.push_back(
         CCI_CrazyflieAprilTagSensor::SReading(
            unId,
            cRelativeBody.GetX(),
            cRelativeBody.GetY(),
            fRelativeYaw,
            fDistance,
            ATan2(cRelativeBody.GetY(), cRelativeBody.GetX())));

      if(m_bShowRays) {
         m_pcControllableEntity->AddCheckedRay(false, cOcclusionCheckRay);
      }
   }

   /****************************************/
   /****************************************/

   void CCrazyflieAprilTagDefaultSensor::ProcessGroundTag(CGroundAprilTagEntity& c_tag,
                                                          const CVector3& c_camera_position,
                                                          const CQuaternion& c_inv_body_orientation) {
      const SAnchor& sTagAnchor = c_tag.GetEmbodiedEntity().GetOriginAnchor();

      CVector3 cRelative = sTagAnchor.Position - c_camera_position;
      CVector3 cRelativeBody = cRelative;
      cRelativeBody.Rotate(c_inv_body_orientation);

      Real fDistance = cRelativeBody.Length();
      if(fDistance > m_fRange || fDistance == 0.0f) {
         return;
      }
      if(cRelativeBody.GetZ() >= 0.0f) {
         return;
      }
      if(ACos(-cRelativeBody.GetZ() / fDistance) > m_cHalfFOV) {
         return;
      }

      SEmbodiedEntityIntersectionItem sIntersectionItem;
      CRay3 cOcclusionCheckRay(c_camera_position, sTagAnchor.Position);
      if(GetClosestEmbodiedEntityIntersectedByRay(sIntersectionItem,
                                                  cOcclusionCheckRay,
                                                  *m_pcEmbodiedEntity)) {
         return;
      }

      CRadians cRelativeYaw, cTmp1, cTmp2;
      CQuaternion cRelativeOrientation = c_inv_body_orientation * sTagAnchor.Orientation;
      cRelativeOrientation.ToEulerAngles(cRelativeYaw, cTmp1, cTmp2);
      Real fRelativeYaw = NormalizeAngle(cRelativeYaw.GetValue());

      m_tReadings.push_back(
         CCI_CrazyflieAprilTagSensor::SReading(
            c_tag.GetMarkerId(),
            cRelativeBody.GetX(),
            cRelativeBody.GetY(),
            fRelativeYaw,
            fDistance,
            ATan2(cRelativeBody.GetY(), cRelativeBody.GetX())));

      if(m_bShowRays) {
         m_pcControllableEntity->AddCheckedRay(false, cOcclusionCheckRay);
      }
   }

   /****************************************/
   /****************************************/

   Real CCrazyflieAprilTagDefaultSensor::NormalizeAngle(Real f_angle) const {
      CRadians cAngle(f_angle);
      cAngle.SignedNormalize();
      return cAngle.GetValue();
   }

   /****************************************/
   /****************************************/

   REGISTER_SENSOR(CCrazyflieAprilTagDefaultSensor,
                   "crazyflie_apriltag", "default",
                   "Byron Hawkins",
                   "1.0",
                   "A default Crazyflie AprilTag sensor.",
                   "This sensor detects tag_medium tags below the Crazyflie\n"
                   "within a configurable range and field of view.\n",
                   "Usable");

   /****************************************/
   /****************************************/
}
