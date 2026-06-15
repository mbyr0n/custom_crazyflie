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

namespace argos {

   /****************************************/
   /****************************************/

   CCrazyflieAprilTagDefaultSensor::CCrazyflieAprilTagDefaultSensor() :
      m_pcEmbodiedEntity(nullptr),
      m_pcControllableEntity(nullptr),
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

      CSpace::TMapPerType* ptTags = nullptr;
      try {
         ptTags = &CSimulator::GetInstance().GetSpace().GetEntitiesByType("ground_apriltag");
      }
      catch(CARGoSException&) {
         return;
      }

      SEmbodiedEntityIntersectionItem sIntersectionItem;
      CRay3 cOcclusionCheckRay;
      cOcclusionCheckRay.SetStart(cCameraPosition);

      CSpace::TMapPerType& tTags = *ptTags;
      for(CSpace::TMapPerType::iterator it = tTags.begin(); it != tTags.end(); ++it) {
         CGroundAprilTagEntity* pcTag = any_cast<CGroundAprilTagEntity*>(it->second);
         const SAnchor& sTagAnchor = pcTag->GetEmbodiedEntity().GetOriginAnchor();

         CVector3 cRelative = sTagAnchor.Position - cCameraPosition;
         CVector3 cRelativeBody = cRelative;
         cRelativeBody.Rotate(cInvBodyOrientation);

         Real fDistance = cRelativeBody.Length();
         if(fDistance > m_fRange || fDistance == 0.0f) {
            continue;
         }
         if(cRelativeBody.GetZ() >= 0.0f) {
            continue;
         }
         if(ACos(-cRelativeBody.GetZ() / fDistance) > m_cHalfFOV) {
            continue;
         }
         cOcclusionCheckRay.SetEnd(sTagAnchor.Position);
         if(GetClosestEmbodiedEntityIntersectedByRay(sIntersectionItem,
                                                     cOcclusionCheckRay,
                                                     *m_pcEmbodiedEntity)) {
            continue;
         }

         CRadians cRelativeYaw, cTmp1, cTmp2;
         CQuaternion cRelativeOrientation = cInvBodyOrientation * sTagAnchor.Orientation;
         cRelativeOrientation.ToEulerAngles(cRelativeYaw, cTmp1, cTmp2);
         Real fRelativeYaw = NormalizeAngle(cRelativeYaw.GetValue());

         m_tReadings.push_back(
            CCI_CrazyflieAprilTagSensor::SReading(
               pcTag->GetMarkerId(),
               cRelativeBody.GetX(),
               cRelativeBody.GetY(),
               fRelativeYaw,
               fDistance,
               ATan2(cRelativeBody.GetY(), cRelativeBody.GetX())));

         if(m_bShowRays) {
            m_pcControllableEntity->AddCheckedRay(
               false,
               CRay3(cCameraPosition, sTagAnchor.Position));
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
                   "This sensor detects ground_apriltag entities below the Crazyflie\n"
                   "within a configurable range and field of view.\n",
                   "Usable");

   /****************************************/
   /****************************************/
}
