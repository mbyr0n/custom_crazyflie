/**
 * @file <argos3/plugins/robots/crazyflie/simulator/crazyflie_entity.cpp>
 *
 * @author Daniel H. Stolfi based on the Carlo Pinciroli's work
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#include "crazyflie_entity.h"

#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/entity/controllable_entity.h>
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/plugins/simulator/entities/led_equipped_entity.h>
#include <argos3/plugins/simulator/entities/quadrotor_entity.h>
#include <argos3/plugins/simulator/entities/rab_equipped_entity.h>
#include "crazyflie_camera_equipped_entity.h"
#include "crazyflie_battery_equipped_entity.h"

namespace argos {

   /****************************************/
   /****************************************/

   static const Real HEIGHT            = 0.010f;
   static const Real BODY_HEIGHT       = 0.008f;
   static const Real BODY_ELEVATION    = HEIGHT + BODY_HEIGHT;
   static const Real RAB_ELEVATION     = BODY_ELEVATION;
   static const Real LED_CPU_ELEVATION = 0.0f;

   /****************************************/
   /****************************************/

   CCrazyflieEntity::CCrazyflieEntity() :
      CComposableEntity(nullptr),
      m_pcControllableEntity(nullptr),
      m_pcEmbodiedEntity(nullptr),
      m_pcQuadRotorEntity(nullptr),
      m_pcLEDEquippedEntity(nullptr),
      m_pcRABEquippedEntity(nullptr),
      m_pcPerspectiveCameraEquippedEntity(NULL),
      m_pcBatteryEquippedEntity(nullptr) {
   }

   /****************************************/
   /****************************************/

   CCrazyflieEntity::CCrazyflieEntity(const std::string& str_id,
                              const std::string& str_controller_id,
                              const CVector3& c_position,
                              const CQuaternion& c_orientation,
                              Real f_rab_range,
                              size_t un_rab_data_size,
                              const CRadians& c_perspcam_aperture,
                              Real f_perspcam_focal_length,
                              Real f_perspcam_range) :
      CComposableEntity(nullptr, str_id),
      m_pcControllableEntity(nullptr),
      m_pcEmbodiedEntity(nullptr),
      m_pcQuadRotorEntity(nullptr),
      m_pcLEDEquippedEntity(nullptr),
      m_pcRABEquippedEntity(nullptr),
      m_pcPerspectiveCameraEquippedEntity(nullptr),
      m_pcBatteryEquippedEntity(nullptr) {
      try {
         /*
          * Create and init components
          */
         /*
          * Embodied entity
          * Better to put this first, because many other entities need this one
          */
         m_pcEmbodiedEntity = new CEmbodiedEntity(this, "body_0", c_position, c_orientation);
         AddComponent(*m_pcEmbodiedEntity);
         /* Quadrotor entity */
         m_pcQuadRotorEntity = new CQuadRotorEntity(this, "quadrotor_0");
         AddComponent(*m_pcQuadRotorEntity);
         /* LED equipped entity  */
         m_pcLEDEquippedEntity = new CLEDEquippedEntity(this, "leds_0");
         m_pcLEDEquippedEntity->AddLED(
            CVector3(0.0f, 0.0f, LED_CPU_ELEVATION),
            m_pcEmbodiedEntity->GetOriginAnchor());
         AddComponent(*m_pcLEDEquippedEntity);
         /* RAB equipped entity */
         SAnchor& cRABAnchor = m_pcEmbodiedEntity->AddAnchor(
            "rab",
            CVector3(0.0f, 0.0f, RAB_ELEVATION));
         m_pcRABEquippedEntity = new CRABEquippedEntity(
            this,
            "rab_0",
            un_rab_data_size,
            f_rab_range,
            cRABAnchor,
            *m_pcEmbodiedEntity);
         AddComponent(*m_pcRABEquippedEntity);
         m_pcEmbodiedEntity->EnableAnchor("rab");
         /* Perspective camera equipped entity */
         m_pcEmbodiedEntity->EnableAnchor("perspective_camera");
         m_pcPerspectiveCameraEquippedEntity = new CCrazyflieCameraEquippedEntity(this,
                                                                                  "perspective_camera_0",
                                                                                  c_perspcam_aperture,
                                                                                  f_perspcam_focal_length,
                                                                                  f_perspcam_range,
                                                                                  320, 320,
                                                                                  m_pcEmbodiedEntity->GetOriginAnchor());
         AddComponent(*m_pcPerspectiveCameraEquippedEntity);
         /* Battery equipped entity */
         m_pcBatteryEquippedEntity = new CCrazyflieBatteryEquippedEntity(this, "battery_0");
         AddComponent(*m_pcBatteryEquippedEntity);
         /* Controllable entity
            It must be the last one, for actuators/sensors to link to composing entities correctly */
         m_pcControllableEntity = new CControllableEntity(this, "controller_0");
         AddComponent(*m_pcControllableEntity);
         m_pcControllableEntity->SetController(str_controller_id);
         /* Update components */
         UpdateComponents();
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("Failed to initialize entity \"" << GetId() << "\".", ex);
      }
   }

   /****************************************/
   /****************************************/

   void CCrazyflieEntity::Init(TConfigurationNode& t_tree) {
      try {
         /*
          * Init parent
          */
         CComposableEntity::Init(t_tree);
         /*
          * Create and init components
          */
         /*
          * Embodied entity
          * Better to put this first, because many other entities need this one
          */
         m_pcEmbodiedEntity = new CEmbodiedEntity(this);
         AddComponent(*m_pcEmbodiedEntity);
         m_pcEmbodiedEntity->Init(GetNode(t_tree, "body"));
         /* Quadrotor entity */
         m_pcQuadRotorEntity = new CQuadRotorEntity(this, "quadrotor_0");
         AddComponent(*m_pcQuadRotorEntity);
         /* LED equipped entity  */
         m_pcLEDEquippedEntity = new CLEDEquippedEntity(this, "leds_0");
         m_pcLEDEquippedEntity->AddLED(
            CVector3(0.0f, 0.0f, LED_CPU_ELEVATION),
            m_pcEmbodiedEntity->GetOriginAnchor());
         AddComponent(*m_pcLEDEquippedEntity);
         /* RAB equipped entity */
         Real fRABRange = 3.0f;
         GetNodeAttributeOrDefault(t_tree, "rab_range", fRABRange, fRABRange);
         UInt32 unRABDataSize = 10;
         GetNodeAttributeOrDefault(t_tree, "rab_data_size", unRABDataSize, unRABDataSize);
         SAnchor& cRABAnchor = m_pcEmbodiedEntity->AddAnchor("rab", CVector3(0.0f, 0.0f, RAB_ELEVATION));
         m_pcRABEquippedEntity = new CRABEquippedEntity(
            this,
            "rab_0",
            unRABDataSize,
            fRABRange,
            cRABAnchor,
            *m_pcEmbodiedEntity);
         AddComponent(*m_pcRABEquippedEntity);
         m_pcEmbodiedEntity->EnableAnchor("rab");
         /* Perspective camera equipped entity */
         Real fPerspCamFocalLength = 0.035;
         Real fPerspCamRange = 3.0;
         CDegrees cAperture(33.75f);
         m_pcPerspectiveCameraEquippedEntity = new CCrazyflieCameraEquippedEntity(this,
                                                                                  "perspective_camera_0",
                                                                                  ToRadians(cAperture),
                                                                                  fPerspCamFocalLength,
                                                                                  fPerspCamRange,
                                                                                  320, 320,
                                                                                  m_pcEmbodiedEntity->GetOriginAnchor());
         AddComponent(*m_pcPerspectiveCameraEquippedEntity);
         /* Battery equipped entity */
         m_pcBatteryEquippedEntity = new CCrazyflieBatteryEquippedEntity(this, "battery_0");
         if(NodeExists(t_tree, "crazyflie_battery"))
            m_pcBatteryEquippedEntity->Init(GetNode(t_tree, "crazyflie_battery"));
         AddComponent(*m_pcBatteryEquippedEntity);
         /* Controllable entity
            It must be the last one, for actuators/sensors to link to composing entities correctly */
         m_pcControllableEntity = new CControllableEntity(this);
         AddComponent(*m_pcControllableEntity);
         m_pcControllableEntity->Init(GetNode(t_tree, "controller"));
         /* Update components */
         UpdateComponents();
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("Failed to initialize entity \"" << GetId() << "\".", ex);
      }
   }

   /****************************************/
   /****************************************/

   void CCrazyflieEntity::Reset() {
      /* Reset all components */
      CComposableEntity::Reset();
      /* Update components */
      UpdateComponents();
   }

   /****************************************/
   /****************************************/

#define UPDATE(COMPONENT) if(COMPONENT->IsEnabled()) COMPONENT->Update();

   void CCrazyflieEntity::UpdateComponents() {
      UPDATE(m_pcRABEquippedEntity);
      UPDATE(m_pcLEDEquippedEntity);
      UPDATE(m_pcBatteryEquippedEntity);
   }

   /****************************************/
   /****************************************/

   REGISTER_ENTITY(CCrazyflieEntity,
                   "crazyflie",
                   "Daniel H. Stolfi based on the Carlo Pinciroli's work",
                   "1.0",
                   "The crazyflie drone, developed by Bitcraze.",
                   "The crazyflie is a quad-rotor developed by Bitcraze. It is a\n"
                   "fully autonomous drone with four propellers and a single bottom led.\n\n"
                   "REQUIRED XML CONFIGURATION\n\n"
                   "  <arena ...>\n"
                   "    ...\n"
                   "    <crazyflie id=\"cf0\">\n"
                   "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
                   "      <controller config=\"mycntrl\" />\n"
                   "    </crazyflie>\n"
                   "    ...\n"
                   "  </arena>\n\n"
                   "The 'id' attribute is necessary and must be unique among the entities. If two\n"
                   "entities share the same id, initialization aborts.\n"
                   "The 'body/position' attribute specifies the position of the bottom point of the\n"
                   "crazyflie in the arena.\n"
                   "The 'body/orientation' attribute specifies the orientation of the crazyflie. All\n"
                   "rotations are performed with respect to the bottom point. The order of the\n"
                   "angles is Z,Y,X, which means that the first number corresponds to the rotation\n"
                   "around the Z axis, the second around Y and the last around X. This reflects\n"
                   "the internal convention used in ARGoS, in which rotations are performed in\n"
                   "that order. Angles are expressed in degrees. When the drone is unrotated, it\n"
                   "is oriented along the X axis.\n"
                   "The 'controller/config' attribute is used to assign a controller to the\n"
                   "crazyflie. The value of the attribute must be set to the id of a previously\n"
                   "defined controller. Controllers are defined in the <controllers> XML subtree.\n\n"
                   "OPTIONAL XML CONFIGURATION\n\n"
                   "You can set the emission range of the range-and-bearing system. By default, a\n"
                   "message sent by an crazyflie can be received up to 3m. By using the 'rab_range'\n"
                   "attribute, you can change it to, i.e., 4m as follows:\n\n"
                   "  <arena ...>\n"
                   "    ...\n"
                   "    <crazyflie id=\"cf0\" rab_range=\"4\">\n"
                   "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
                   "      <controller config=\"mycntrl\" />\n"
                   "    </crazyflie>\n"
                   "    ...\n"
                   "  </arena>\n\n"
                   "You can also set the data sent at each time step through the range-and-bearing\n"
                   "system. By default, a message sent by a crazyflie is 10 bytes long. By using the\n"
                   "'rab_data_size' attribute, you can change it to, i.e., 20 bytes as follows:\n\n"
                   "  <arena ...>\n"
                   "    ...\n"
                   "    <crazyflie id=\"cf0\" rab_data_size=\"20\">\n"
                   "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
                   "      <controller config=\"mycntrl\" />\n"
                   "    </crazyflie>\n"
                   "    ...\n"
                   "  </arena>\n\n"
                   "You can also configure the battery of the robot. By default, the battery never\n"
                   "depletes. You can also define the battery's initial charge.\n\n"
                   "  <arena ...>\n"
                   "    ...\n"
                   "    <crazyflie id=\"cf0\"\n"
                   "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
                   "      <controller config=\"mycntrl\" />\n"
                   "      <crazyflie_battery start_charge=\"0.8\"/>\n"
                   "    </crazyflie>\n"
                   "    ...\n"
                   "  </arena>\n\n"
                   ,
                   "Usable"
      );

   /****************************************/
   /****************************************/

   REGISTER_STANDARD_SPACE_OPERATIONS_ON_COMPOSABLE(CCrazyflieEntity);

   /****************************************/
   /****************************************/

}
