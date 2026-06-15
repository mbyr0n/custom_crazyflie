/**
 * @file <argos3/plugins/robots/crazyflie/simulator/qtopengl_spiri.h>
 *
 * @author Daniel H. Stolfi based on the Carlo Pinciroli's work
 *
 * ADARS project -- PCOG / SnT / University of Luxembourg
 */

#ifndef QTOPENGL_CRAZYFLIE_H
#define QTOPENGL_CRAZYFLIE_H

namespace argos {
   class CQTOpenGLCrazyflie;
   class CCrazyflieEntity;
}

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

namespace argos {

   class CQTOpenGLCrazyflie {

   public:

      CQTOpenGLCrazyflie();

      virtual ~CQTOpenGLCrazyflie();

      virtual void Draw(CCrazyflieEntity& c_entity);

   protected:

      /** The Crazyflie body */
      void DrawBody();

      /** The Crazyflie rods */
      void DrawRods();

      /** The Crazyflie propellers */
      void DrawPropellers();

      /** The Crazyflie CPU LED */
      void DrawCpuLED();

      /** The Crazyflie downward AprilTag camera */
      void DrawAprilTagCamera();

      /** Sets a LED material */
      void SetLEDMaterial(GLfloat f_red, GLfloat f_green, GLfloat f_blue);

      /** Sets a body material */
      void SetBodyMaterial();

      /** Sets a rod material */
      void SetRodMaterial();

      /** Sets a propeller material */
      void SetPropellerMaterial();

      /** Sets a camera material */
      void SetCameraMaterial();

      /** Sets an arrow material */
      void SetArrowMaterial();

      /** A Crazyflie rod */
      void MakeRod(GLfloat angle);

      /** A Crazyflie rotor */
      void MakeRotor(GLfloat offset);

      /** A Crazyflie propeller */
      void MakePropeller(GLfloat angle, GLfloat offset);

   private:

      /** Start of the display list index */
      GLuint m_unLists;

      /** Body display list */
      GLuint m_unBodyList;

      /** Rod display list */
      GLuint m_unRodList;

      /** Propeller display list */
      GLuint m_unPropellerList;

      /** Propeller cpu led list */
      GLuint m_unCpuLEDList;

      /** AprilTag camera display list */
      GLuint m_unAprilTagCameraList;

      /** Number of vertices to display the round parts */
      GLuint m_unVertices;

   };
}

#endif
