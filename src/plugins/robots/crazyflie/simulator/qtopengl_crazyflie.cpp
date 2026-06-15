/**
 * @file <argos3/plugins/robots/crazyflie/simulator/qtopengl_crazyflie.cpp>
 *
 * @author Daniel H. Stolfi based on the Carlo Pinciroli's work
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#include "qtopengl_crazyflie.h"
#include "crazyflie_entity.h"
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/plugins/simulator/entities/led_equipped_entity.h>
#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_obj_model.h>
#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_widget.h>

namespace argos {

   /* Body */
   static const Real BODY_LENGTH         = 0.035f;
   static const Real BODY_HALF_LENGTH    = BODY_LENGTH * 0.5f;
   static const Real BODY_WIDTH          = 0.025f;
   static const Real BODY_HALF_WIDTH     = BODY_WIDTH * 0.5f;
   static const Real BODY_HEIGHT         = 0.008f;
   static const Real BODY_ELEVATION      = 0.01f;

   /* AprilTag camera */
   static const Real CAMERA_HALF_SIDE    = 0.006f;
   static const Real CAMERA_TOP          = BODY_ELEVATION;
   static const Real CAMERA_BOTTOM       = BODY_ELEVATION - 0.010f;

   /* Rod */
   static const Real ROD_SIDE           = 0.092f;
   static const Real ROD_HALF_SIDE      = ROD_SIDE * 0.5f;
   static const Real ROD_DIAMETER       = 0.004f;
   static const Real ROD_RADIUS         = ROD_DIAMETER * 0.5f;
   static const Real ROD_ELEVATION      = BODY_ELEVATION;
   static const Real ROD_ANGLE          = 45.0f;

   /* Rotor */
   static const Real ROTOR_RADIUS        = 0.005f;
   static const Real ROTOR_HEIGHT        = 0.02f;
   static const Real ROTOR_OFFSET        = ROD_HALF_SIDE - (ROTOR_RADIUS * 0.5f);
   static const Real ROTOR_ELEVATION     = 0.0f;

   /* Propeller */
   static const Real PROPELLER_LENGTH = 0.0048f;
   static const Real PROPELLER_HALF_LENGTH = PROPELLER_LENGTH * 0.5f;
   static const Real PROPELLER_WIDTH = 0.008f;
   static const Real PROPELLER_HALF_WIDTH = 0.05f * 0.5f;
   static const Real PROPELLER_HEIGHT = 0.001f;
   static const Real PROPELLER_ELEVATION = ROTOR_ELEVATION + ROTOR_HEIGHT + PROPELLER_HEIGHT * 0.5f;

   /* Led */
   static const Real LED_HEIGHT     = 0.005f;
   static const Real LED_SIDE       = 0.01f;
   static const Real LED_ELEVATION  = BODY_ELEVATION - (LED_HEIGHT * 0.8f);
   static const Real LED_OFFSET     = 0.0f;

   CQTOpenGLCrazyflie::CQTOpenGLCrazyflie() :
      m_unVertices(16) {
      /* Reserve the needed display lists */
      m_unLists = glGenLists(5);
      /* Assign indices for better referencing (later) */
      m_unBodyList            = m_unLists;
      m_unRodList             = m_unLists + 1;
      m_unPropellerList       = m_unLists + 2;
      m_unCpuLEDList          = m_unLists + 3;
      m_unAprilTagCameraList  = m_unLists + 4;

      /* Create the body display list */
      glNewList(m_unBodyList, GL_COMPILE);
      DrawBody();
      glEndList();

      /* Create the rod list */
      glNewList(m_unRodList, GL_COMPILE);
      DrawRods();
      glEndList();

      /* Create the propeller list */
      glNewList(m_unPropellerList, GL_COMPILE);
      DrawPropellers();
      glEndList();

      /* Create the CPU LED list */
      glNewList(m_unCpuLEDList, GL_COMPILE);
      DrawCpuLED();
      glEndList();

      /* Create the AprilTag camera list */
      glNewList(m_unAprilTagCameraList, GL_COMPILE);
      DrawAprilTagCamera();
      glEndList();

   }

   /****************************************/
   /****************************************/

   CQTOpenGLCrazyflie::~CQTOpenGLCrazyflie() {
      glDeleteLists(m_unLists, 5);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::Draw(CCrazyflieEntity& c_entity) {
      SetBodyMaterial();
      /* Place the body */
      glCallList(m_unBodyList);

      SetRodMaterial();
      /* Place the rods */
      glCallList(m_unRodList);

      SetPropellerMaterial();
      /* Place the propellers */
      glCallList(m_unPropellerList);

      const CColor& cColor = c_entity.GetLEDEquippedEntity().GetLED(0).GetColor();
      SetLEDMaterial(cColor.GetRed(),
                     cColor.GetGreen(),
                     cColor.GetBlue());
      /* Place the LED */
      glCallList(m_unCpuLEDList);

      SetCameraMaterial();
      /* Place the downward AprilTag camera */
      glCallList(m_unAprilTagCameraList);

   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::SetLEDMaterial(GLfloat f_red, GLfloat f_green, GLfloat f_blue) {
      const GLfloat pfColor[]     = { f_red, f_green, f_blue, 1.0f };
      const GLfloat pfSpecular[]  = {  0.0f,    0.0f,   0.0f, 1.0f };
      const GLfloat pfShininess[] = {  0.0f                        };
      const GLfloat pfEmission[]  = { f_red, f_green, f_blue, 1.0f };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pfColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,            pfSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,           pfShininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,            pfEmission);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::SetBodyMaterial() {
      const GLfloat pfColor[]     = {   0.028f, 0.228f, 0.126f, 1.0f };
      const GLfloat pfSpecular[]  = {   0.055f, 0.457f, 0.255f, 1.0f };
      const GLfloat pfShininess[] = {  80.0f                   };
      const GLfloat pfEmission[]  = {   0.0f, 0.0f, 0.0f, 1.0f };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pfColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,            pfSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,           pfShininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,            pfEmission);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::SetRodMaterial() {
      const GLfloat pfColor[]     = { 1.0f, 1.0f, 1.0f, 1.0f };
      const GLfloat pfSpecular[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
      const GLfloat pfShininess[] = { 32.0f                   };
      const GLfloat pfEmission[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pfColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,            pfSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,           pfShininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,            pfEmission);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::SetPropellerMaterial() {
      const GLfloat pfColor[]     = {   0.1f, 0.1f, 0.1f, 1.0f };
      const GLfloat pfSpecular[]  = {   0.5f, 0.5f, 0.5f, 1.0f };
      const GLfloat pfShininess[] = {  50.0f                   };
      const GLfloat pfEmission[]  = {   0.0f, 0.0f, 0.0f, 1.0f };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pfColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,            pfSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,           pfShininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,            pfEmission);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::SetCameraMaterial() {
      const GLfloat pfColor[]     = {  0.02f, 0.02f, 0.02f, 1.0f };
      const GLfloat pfSpecular[]  = {  0.10f, 0.10f, 0.10f, 1.0f };
      const GLfloat pfShininess[] = { 16.0f                   };
      const GLfloat pfEmission[]  = {  0.0f,  0.0f,  0.0f, 1.0f };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pfColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,            pfSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,           pfShininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,            pfEmission);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::SetArrowMaterial() {
      const GLfloat pfColor[]     = { 1.0f, 1.0f, 0.0f, 1.0f };
      const GLfloat pfSpecular[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
      const GLfloat pfShininess[] = { 0.0f                   };
      const GLfloat pfEmission[]  = { 1.0f, 1.0f, 0.0f, 1.0f };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pfColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,            pfSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,           pfShininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,            pfEmission);
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::DrawAprilTagCamera() {
      /* Top face */
      glBegin(GL_QUADS);
      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(-CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_TOP);
      glVertex3f( CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_TOP);
      glVertex3f( CAMERA_HALF_SIDE,  CAMERA_HALF_SIDE, CAMERA_TOP);
      glVertex3f(-CAMERA_HALF_SIDE,  CAMERA_HALF_SIDE, CAMERA_TOP);
      glEnd();

      /* Side faces */
      glBegin(GL_QUAD_STRIP);
      glNormal3f(0.0f, -1.0f, 0.0f);
      glVertex3f(-CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_TOP);
      glVertex3f(-CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_BOTTOM);
      glVertex3f( CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_TOP);
      glVertex3f( CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_BOTTOM);
      glNormal3f(1.0f, 0.0f, 0.0f);
      glVertex3f( CAMERA_HALF_SIDE,  CAMERA_HALF_SIDE, CAMERA_TOP);
      glVertex3f( CAMERA_HALF_SIDE,  CAMERA_HALF_SIDE, CAMERA_BOTTOM);
      glNormal3f(0.0f, 1.0f, 0.0f);
      glVertex3f(-CAMERA_HALF_SIDE,  CAMERA_HALF_SIDE, CAMERA_TOP);
      glVertex3f(-CAMERA_HALF_SIDE,  CAMERA_HALF_SIDE, CAMERA_BOTTOM);
      glNormal3f(-1.0f, 0.0f, 0.0f);
      glVertex3f(-CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_TOP);
      glVertex3f(-CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_BOTTOM);
      glEnd();

      /* Bottom face */
      glBegin(GL_QUADS);
      glNormal3f(0.0f, 0.0f, -1.0f);
      glVertex3f( CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_BOTTOM);
      glVertex3f(-CAMERA_HALF_SIDE, -CAMERA_HALF_SIDE, CAMERA_BOTTOM);
      glVertex3f(-CAMERA_HALF_SIDE,  CAMERA_HALF_SIDE, CAMERA_BOTTOM);
      glVertex3f( CAMERA_HALF_SIDE,  CAMERA_HALF_SIDE, CAMERA_BOTTOM);
      glEnd();
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::DrawBody() {

      /* Top face */
      glBegin(GL_QUADS);
      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(-BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION+BODY_HEIGHT);
      glVertex3f( BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION+BODY_HEIGHT);
      glVertex3f( BODY_HALF_LENGTH,  BODY_HALF_WIDTH, BODY_ELEVATION+BODY_HEIGHT);
      glVertex3f(-BODY_HALF_LENGTH,  BODY_HALF_WIDTH, BODY_ELEVATION+BODY_HEIGHT);
      glEnd();
      /* This part covers the faces (South, East, North, West) */
      glBegin(GL_QUAD_STRIP);
      /* Starting vertex */
      glNormal3f(0.0f, 1.0f, 0.0f);
      glVertex3f(-BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION+BODY_HEIGHT);
      glVertex3f(-BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION);
      /* South face */
      glVertex3f( BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION+BODY_HEIGHT);
      glVertex3f( BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION);
      /* East face */
      glNormal3f(-1.0f, 0.0f, 0.0f);
      glVertex3f( BODY_HALF_LENGTH,  BODY_HALF_WIDTH, BODY_ELEVATION+BODY_HEIGHT);
      glVertex3f( BODY_HALF_LENGTH,  BODY_HALF_WIDTH, BODY_ELEVATION);
      /* North face */
      glNormal3f(0.0f, -1.0f, 0.0f);
      glVertex3f(-BODY_HALF_LENGTH,  BODY_HALF_WIDTH, BODY_ELEVATION+BODY_HEIGHT);
      glVertex3f(-BODY_HALF_LENGTH,  BODY_HALF_WIDTH, BODY_ELEVATION);
      /* West face */
      glNormal3f(1.0f, 0.0f, 0.0f);
      glVertex3f(-BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION+BODY_HEIGHT);
      glVertex3f(-BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION);
      glEnd();
      /* Bottom face */
      glBegin(GL_QUADS);
      glNormal3f(0.0f, 0.0f, -1.0f);
      glVertex3f( BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION);
      glVertex3f(-BODY_HALF_LENGTH, -BODY_HALF_WIDTH, BODY_ELEVATION);
      glVertex3f(-BODY_HALF_LENGTH,  BODY_HALF_WIDTH, BODY_ELEVATION);
      glVertex3f( BODY_HALF_LENGTH,  BODY_HALF_WIDTH, BODY_ELEVATION);
      glEnd();
      /* Triangle to set the direction */
      SetArrowMaterial();
      glBegin(GL_TRIANGLES);
      glVertex3f( BODY_HALF_LENGTH * 0.5,                    0.0f, BODY_ELEVATION + BODY_HEIGHT + 0.001f);
      glVertex3f(-BODY_HALF_LENGTH * 0.5,  BODY_HALF_LENGTH * 0.2, BODY_ELEVATION + BODY_HEIGHT + 0.001f);
      glVertex3f(-BODY_HALF_LENGTH * 0.5, -BODY_HALF_LENGTH * 0.2, BODY_ELEVATION + BODY_HEIGHT + 0.001f);
      glEnd();
    }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::DrawRods() {

      MakeRod(ROD_ANGLE);
      MakeRod(-ROD_ANGLE);

   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::DrawPropellers() {

      MakePropeller(ROD_ANGLE, ROTOR_OFFSET);
      MakePropeller(-ROD_ANGLE, ROTOR_OFFSET);
      MakePropeller(ROD_ANGLE, -ROTOR_OFFSET);
      MakePropeller(-ROD_ANGLE, -ROTOR_OFFSET);

   }

   /****************************************/
   /****************************************/
   void CQTOpenGLCrazyflie::MakeRod(GLfloat angle) {
      glPushMatrix();
      glRotatef(angle, 0.0f, 0.0f, 1.0f);

      /* Leg */
      CVector2 cVertex(ROD_RADIUS, 0.0f);
      CRadians cAngle(CRadians::TWO_PI / m_unVertices);
      CVector2 cNormal(-1.0f, 0.0f);
      cVertex.Set(ROD_RADIUS, 0.0f);
      glBegin(GL_QUAD_STRIP);
      for(GLuint i = 0; i <= m_unVertices; i++) {
         glNormal3f(cNormal.GetX(), cNormal.GetY(), -1.0f);
         glVertex3f( ROD_HALF_SIDE, cVertex.GetX(), cVertex.GetY()+ROD_ELEVATION+ROD_RADIUS);
         glVertex3f(-ROD_HALF_SIDE, cVertex.GetX(), cVertex.GetY()+ROD_ELEVATION+ROD_RADIUS);
         cVertex.Rotate(cAngle);
         cNormal.Rotate(cAngle);
      }
      glEnd();

      /* Rotors */
      MakeRotor(ROTOR_OFFSET);
      MakeRotor(-ROTOR_OFFSET);

      glPopMatrix();
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::MakeRotor(GLfloat offset) {
      /* Draw the rotor */
      CVector2 cVertex(ROTOR_RADIUS, 0.0f);
      CRadians cAngle(-CRadians::TWO_PI / m_unVertices);
      /* Bottom part */
      glBegin(GL_POLYGON);
      glNormal3f(0.0f, 0.0f, -1.0f);
      for(GLuint i = 0; i <= m_unVertices; i++) {
         glVertex3f(cVertex.GetX() + offset, cVertex.GetY(), ROTOR_ELEVATION);
         cVertex.Rotate(cAngle);
      }
      glEnd();
      /* Side surface */
      cAngle = -cAngle;
      CVector2 cNormal(1.0f, 0.0f);
      cVertex.Set(ROTOR_RADIUS, 0.0f);
      glBegin(GL_QUAD_STRIP);
      for(GLuint i = 0; i <= m_unVertices; i++) {
         glNormal3f(cNormal.GetX() + offset, cNormal.GetY(), 0.0f);
         glVertex3f(cVertex.GetX() + offset, cVertex.GetY(), ROTOR_ELEVATION + ROTOR_HEIGHT);
         glVertex3f(cVertex.GetX() + offset, cVertex.GetY(), ROTOR_ELEVATION);
         cVertex.Rotate(cAngle);
         cNormal.Rotate(cAngle);
      }
      glEnd();
      /* Top part */
      glBegin(GL_POLYGON);
      glNormal3f(0.0f, 0.0f, 1.0f);
      cVertex.Set(ROTOR_RADIUS, 0.0f);
      for(GLuint i = 0; i <= m_unVertices; i++) {
         glVertex3f(cVertex.GetX() + offset, cVertex.GetY(), ROTOR_ELEVATION + ROTOR_HEIGHT);
         cVertex.Rotate(cAngle);
      }
      glEnd();
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::MakePropeller(GLfloat angle, GLfloat offset) {
      /* Propeller */
      glPushMatrix();
      glRotatef(angle, 0.0f, 0.0f, 1.0f);

      /* Top part */
      glBegin(GL_POLYGON);
      glNormal3f(0.0f, 0.0f, -1.0f);
      glVertex3f(offset + PROPELLER_HALF_LENGTH,  0.0f,                  PROPELLER_ELEVATION);
      glVertex3f(offset,                         PROPELLER_HALF_WIDTH, PROPELLER_ELEVATION);
      glVertex3f(offset - PROPELLER_HALF_LENGTH,  0.0f,                  PROPELLER_ELEVATION);
      glVertex3f(offset,                        -PROPELLER_HALF_WIDTH, PROPELLER_ELEVATION);
      glEnd();
      /* Bottom part */
      glBegin(GL_POLYGON);
      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(offset + PROPELLER_HALF_WIDTH,  0.0f,                  PROPELLER_ELEVATION);
      glVertex3f(offset,                        -PROPELLER_HALF_LENGTH, PROPELLER_ELEVATION);
      glVertex3f(offset - PROPELLER_HALF_WIDTH,  0.0f,                  PROPELLER_ELEVATION);
      glVertex3f(offset,                         PROPELLER_HALF_LENGTH, PROPELLER_ELEVATION);
      glEnd();

      glPopMatrix();
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLCrazyflie::DrawCpuLED() {

      glPushMatrix();
      glTranslatef(0.0f, 0.0f, LED_ELEVATION);

      CVector2 cVertex(0,0);
      CRadians cAngle(-CRadians::TWO_PI / m_unVertices);
      /* Top part */
      glBegin(GL_POLYGON);
      glNormal3f(0.0f, 0.0f, 1.0f);
      cVertex.Set(LED_SIDE, 0.0f);
      for(GLuint i = 0; i <= m_unVertices; i++) {
         glVertex3f(cVertex.GetX(), cVertex.GetY(), 0.0f);
         cVertex.Rotate(cAngle);
      }
      glEnd();
      /* Side surface */
      cAngle = -cAngle;
      CVector2 cNormal(1.0f, 0.0f);
      cVertex.Set(LED_SIDE, 0.0f);
      glBegin(GL_QUAD_STRIP);
      for(GLuint i = 0; i <= m_unVertices; i++) {
         glNormal3f(cNormal.GetX(), cNormal.GetY(), 0.0f);
         glVertex3f(cVertex.GetX(), cVertex.GetY(), LED_HEIGHT);
         glVertex3f(cVertex.GetX(), cVertex.GetY(), 0.0f);
         cVertex.Rotate(cAngle);
         cNormal.Rotate(cAngle);
      }
      glEnd();

      glPopMatrix();
   }

   /****************************************/
   /****************************************/

   class CQTOpenGLOperationDrawCrazyflieNormal : public CQTOpenGLOperationDrawNormal {
   public:
      void ApplyTo(CQTOpenGLWidget& c_visualization,
                   CCrazyflieEntity& c_entity) {
         static CQTOpenGLCrazyflie m_cCrazyflieModel;
         c_visualization.DrawRays(c_entity.GetControllableEntity());
         c_visualization.DrawEntity(c_entity.GetEmbodiedEntity());
         m_cCrazyflieModel.Draw(c_entity);
      }
   };

   /****************************************/
   /****************************************/

   class CQTOpenGLOperationDrawCrazyflieSelected : public CQTOpenGLOperationDrawSelected {
   public:
      void ApplyTo(CQTOpenGLWidget& c_visualization,
                   CCrazyflieEntity& c_entity) {
         c_visualization.DrawBoundingBox(c_entity.GetEmbodiedEntity());
      }
   };

   REGISTER_QTOPENGL_ENTITY_OPERATION(CQTOpenGLOperationDrawNormal, CQTOpenGLOperationDrawCrazyflieNormal, CCrazyflieEntity);

   REGISTER_QTOPENGL_ENTITY_OPERATION(CQTOpenGLOperationDrawSelected, CQTOpenGLOperationDrawCrazyflieSelected, CCrazyflieEntity);

   /****************************************/
   /****************************************/

}
