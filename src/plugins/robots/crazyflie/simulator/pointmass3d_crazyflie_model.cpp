/**
 * @file <argos3/plugins/robots/crazyflie/simulator/pointmass3d_crazyflie_model.h>
 *
 * @author Daniel H. Stolfi based on the Carlo Pinciroli's work
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#include "pointmass3d_crazyflie_model.h"
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/utility/math/cylinder.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>

namespace argos {

   static const Real     BODY_HEIGHT   = 0.020f;
   static const Real     BODY_SIDE     = 0.065f;
   static const Real     BODY_DIAGONAL = BODY_SIDE * ::sqrt(2);
   static const Real     BODY_RADIUS   = BODY_DIAGONAL / 2.0f;
   static const Real     BODY_MASS     = 0.030f;
   static const Real     BODY_INERTIA  = 0.012f;
   static const CVector3 POS_K_P(0.00005f, 0.00005f, 0.0005f);  // 0.001
   static const CVector3 POS_K_D(0.061f,   0.061f,   0.120f);  // 0.240
   static const Real     YAW_K_P = 0.0005f;
   static const Real     YAW_K_D = 0.05f;
   static const CVector3 VEL_K_P(2.0f,  2.0f,  2.0f);
   static const CVector3 VEL_K_D(0.01f, 0.01f, 0.01f);
   static const Real     ROT_K_P = 0.4f;
   static const Real     ROT_K_D = 0.005f;

   /****************************************/
   /****************************************/

   CPointMass3DCrazyflieModel::CPointMass3DCrazyflieModel(CPointMass3DEngine& c_engine,
                                                          CCrazyflieEntity& c_crazyflie) :
                                                          CPointMass3DModel(c_engine, c_crazyflie.GetEmbodiedEntity()),
                                                          m_cQuadRotorEntity(c_crazyflie.GetQuadRotorEntity()),
                                                          m_fBodyHeight(BODY_HEIGHT),
                                                          m_fArmLength(BODY_RADIUS),
                                                          m_fBodyMass(BODY_MASS),
                                                          m_fBodyInertia(BODY_INERTIA),
                                                          m_cPosKP(POS_K_P),
                                                          m_cPosKD(POS_K_D),
                                                          m_fYawKP(YAW_K_P),
                                                          m_fYawKD(YAW_K_D),
                                                          m_cVelKP(VEL_K_P),
                                                          m_cVelKD(VEL_K_D),
                                                          m_fRotKP(ROT_K_P),
                                                          m_fRotKD(ROT_K_D),
                                                          m_cMaxForce(CVector3(1000.0f, 1000.0f, 1000.0f)),
                                                          m_fMaxTorque(1000.0f) {
                  Reset();
         /* Register the origin anchor update method */
         RegisterAnchorMethod(GetEmbodiedEntity().GetOriginAnchor(),
                              &CPointMass3DCrazyflieModel::UpdateOriginAnchor);
         /* Get initial rotation */
         CRadians cTmp1, cTmp2;
         GetEmbodiedEntity().GetOriginAnchor().Orientation.ToEulerAngles(m_cYaw, cTmp1, cTmp2);

   }

   /****************************************/
   /****************************************/

   void CPointMass3DCrazyflieModel::UpdateRABAnchor(SAnchor& s_anchor) {
      s_anchor.Orientation = GetEmbodiedEntity().GetOriginAnchor().Orientation;
      s_anchor.Position = s_anchor.OffsetPosition;
      s_anchor.Position += GetEmbodiedEntity().GetOriginAnchor().Position;
   }


   /****************************************/
   /****************************************/

   static Real SymmetricClamp(Real f_max,
                              Real f_value) {
      if(f_value >  f_max) return  f_max;
      if(f_value < -f_max) return -f_max;
      return f_value;
   }



   /****************************************/
   /****************************************/

   void CPointMass3DCrazyflieModel::Reset() {
      CPointMass3DModel::Reset();
      m_pfLinearError[0] = 0.0f;
      m_pfLinearError[1] = 0.0f;
      m_pfLinearError[2] = 0.0f;
      m_fRotError = 0.0f;
   }

   /****************************************/
   /****************************************/

   void CPointMass3DCrazyflieModel::UpdateFromEntityStatus() {
      m_sDesiredPositionData = m_cQuadRotorEntity.GetPositionControlData();
      m_sDesiredSpeedData = m_cQuadRotorEntity.GetSpeedControlData();
      m_sDesiredSpeedData.Velocity = m_sDesiredSpeedData.Velocity * 10.0;
      m_sDesiredSpeedData.RotSpeed = m_sDesiredSpeedData.RotSpeed * 10.0;
   }

   /****************************************/
   /****************************************/

   void CPointMass3DCrazyflieModel::Step() {
      /*
       * Update positional information
       */
      /* Integration step */
      m_cPosition += m_cVelocity * m_cPM3DEngine.GetPhysicsClockTick();
      m_cYaw      += m_cRotSpeed * m_cPM3DEngine.GetPhysicsClockTick();
      /* Limit the quad-rotor position within the arena limits */
      const CRange<CVector3>& cLimits = CSimulator::GetInstance().GetSpace().GetArenaLimits();
      m_cPosition.SetX(
         Min(Max(m_cPosition.GetX(),
                 cLimits.GetMin().GetX() + m_fArmLength),
             cLimits.GetMax().GetX() - m_fArmLength));
      m_cPosition.SetY(
         Min(Max(m_cPosition.GetY(),
                 cLimits.GetMin().GetY() + m_fArmLength),
             cLimits.GetMax().GetY() - m_fArmLength));
      m_cPosition.SetZ(
         Min(Max(m_cPosition.GetZ(),
                 cLimits.GetMin().GetZ()),
             cLimits.GetMax().GetZ() - m_fBodyHeight));
      /* Normalize the yaw */
      m_cYaw.UnsignedNormalize();
      /*
       * Update velocity information
       */
      m_cVelocity += (m_cPM3DEngine.GetPhysicsClockTick() / m_fBodyMass)    * m_cAcceleration;
      m_cRotSpeed += (m_cPM3DEngine.GetPhysicsClockTick() / m_fBodyInertia) * m_cTorque;
      // TODO std::cout << m_cAcceleration.GetX() << "," << m_cVelocity.GetX() << std::endl;
      /*
       * Update control information
       */
      if(m_cQuadRotorEntity.GetControlMethod() == CQuadRotorEntity::POSITION_CONTROL)
         PositionalControl();
      else
         SpeedControl();
      /*
       * Update force/torque information
       */
      m_cAcceleration.SetX(m_cLinearControl.GetX());
      m_cAcceleration.SetY(m_cLinearControl.GetY());
      m_cAcceleration.SetZ(m_cLinearControl.GetZ() + m_fBodyMass * m_cPM3DEngine.GetGravity());
      m_cTorque.SetValue(m_fRotationalControl);
   }

   /****************************************/
   /****************************************/

   void CPointMass3DCrazyflieModel::CalculateBoundingBox() {
      GetBoundingBox().MinCorner.Set(
         GetEmbodiedEntity().GetOriginAnchor().Position.GetX() - m_fArmLength,
         GetEmbodiedEntity().GetOriginAnchor().Position.GetY() - m_fArmLength,
         GetEmbodiedEntity().GetOriginAnchor().Position.GetZ());
      GetBoundingBox().MaxCorner.Set(
         GetEmbodiedEntity().GetOriginAnchor().Position.GetX() + m_fArmLength,
         GetEmbodiedEntity().GetOriginAnchor().Position.GetY() + m_fArmLength,
         GetEmbodiedEntity().GetOriginAnchor().Position.GetZ() + m_fBodyHeight);
   }

   /****************************************/
   /****************************************/

   bool CPointMass3DCrazyflieModel::CheckIntersectionWithRay(Real& f_t_on_ray,
                                                             const CRay3& c_ray) const {
      CCylinder m_cShape(m_fArmLength,
                         m_fBodyHeight,
                         m_cPosition,
                         CVector3::Z);
      return m_cShape.Intersects(f_t_on_ray, c_ray);
   }

   /****************************************/
   /****************************************/

   void CPointMass3DCrazyflieModel::PositionalControl() {
      /* Linear control */
      m_cLinearControl.Set(
         SymmetricClamp(m_cMaxForce.GetX(), PDControl(
                           m_sDesiredPositionData.Position.GetX() - m_cPosition.GetX(),
                           m_cPosKP.GetX(),
                           m_cPosKD.GetX(),
                           m_pfLinearError[0])),
         SymmetricClamp(m_cMaxForce.GetY(), PDControl(
                           m_sDesiredPositionData.Position.GetY() - m_cPosition.GetY(),
                           m_cPosKP.GetY(),
                           m_cPosKD.GetY(),
                           m_pfLinearError[1])),
         SymmetricClamp(m_cMaxForce.GetZ(), PDControl(
                           m_sDesiredPositionData.Position.GetZ() - m_cPosition.GetZ(),
                           m_cPosKP.GetZ(),
                           m_cPosKD.GetZ(),
                           m_pfLinearError[2]) - m_fBodyMass * m_cPM3DEngine.GetGravity()));
      /* Rotational control */
      m_fRotationalControl =
         SymmetricClamp(m_fMaxTorque, PDControl(
                           (m_sDesiredPositionData.Yaw - m_cYaw).SignedNormalize().GetValue(),
                           m_fYawKP,
                           m_fYawKD,
                           m_fRotError));
   }

   /****************************************/
   /****************************************/

   void CPointMass3DCrazyflieModel::SpeedControl() {
      /* Linear control */
      m_cLinearControl.Set(
         SymmetricClamp(m_cMaxForce.GetX(), PDControl(
                           m_sDesiredSpeedData.Velocity.GetX() - m_cVelocity.GetX(),
                           m_cVelKP.GetX(),
                           m_cVelKD.GetX(),
                           m_pfLinearError[0])),
         SymmetricClamp(m_cMaxForce.GetY(), PDControl(
                           m_sDesiredSpeedData.Velocity.GetY() - m_cVelocity.GetY(),
                           m_cVelKP.GetY(),
                           m_cVelKD.GetY(),
                           m_pfLinearError[1])),
         SymmetricClamp(m_cMaxForce.GetZ(), PDControl(
                           m_sDesiredSpeedData.Velocity.GetZ() - m_cVelocity.GetZ(),
                           m_cVelKP.GetZ(),
                           m_cVelKD.GetZ(),
                           m_pfLinearError[2]) - m_fBodyMass * m_cPM3DEngine.GetGravity()));
      /* Rotational control */
      m_fRotationalControl =
         SymmetricClamp(m_fMaxTorque, PDControl(
                           (m_sDesiredSpeedData.RotSpeed - m_cRotSpeed).GetValue(),
                           m_fRotKP,
                           m_fRotKD,
                           m_fRotError));
   }

   /****************************************/
   /****************************************/

   Real CPointMass3DCrazyflieModel::PDControl(Real f_cur_error,
                                              Real f_k_p,
                                              Real f_k_d,
                                              Real& f_old_error) {
      Real fOutput =
         f_k_p * f_cur_error +                                                      /* proportional term */
         f_k_d * (f_cur_error - f_old_error) / m_cPM3DEngine.GetPhysicsClockTick(); /* derivative term */
      f_old_error = f_cur_error;
      return fOutput;
   }

   /****************************************/
   /****************************************/

   void CPointMass3DCrazyflieModel::UpdateOriginAnchor(SAnchor& s_anchor) {
      s_anchor.Position = m_cPosition;
      s_anchor.Orientation = CQuaternion(m_cYaw, CVector3::Z);
   }

   /****************************************/
   /****************************************/

   REGISTER_STANDARD_POINTMASS3D_OPERATIONS_ON_ENTITY(CCrazyflieEntity, CPointMass3DCrazyflieModel);

   /****************************************/
   /****************************************/

}
