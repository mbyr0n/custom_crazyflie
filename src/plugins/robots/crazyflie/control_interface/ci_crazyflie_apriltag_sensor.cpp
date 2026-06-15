#include "ci_crazyflie_apriltag_sensor.h"

#ifdef ARGOS_WITH_LUA
#include <argos3/core/wrappers/lua/lua_utility.h>
#endif

namespace argos {

   /****************************************/
   /****************************************/

   CCI_CrazyflieAprilTagSensor::SReading::SReading() :
      Id(0),
      X(0.0f),
      Y(0.0f),
      Yaw(0.0f),
      Distance(0.0f),
      Angle(CRadians::ZERO) {}

   /****************************************/
   /****************************************/

   CCI_CrazyflieAprilTagSensor::SReading::SReading(UInt32 un_id,
                                                   Real f_x,
                                                   Real f_y,
                                                   Real f_yaw,
                                                   Real f_distance,
                                                   const CRadians& c_angle) :
      Id(un_id),
      X(f_x),
      Y(f_y),
      Yaw(f_yaw),
      Distance(f_distance),
      Angle(c_angle) {}

   /****************************************/
   /****************************************/

   CCI_CrazyflieAprilTagSensor::CCI_CrazyflieAprilTagSensor() {}

   /****************************************/
   /****************************************/

#ifdef ARGOS_WITH_LUA
   void CCI_CrazyflieAprilTagSensor::CreateLuaState(lua_State* pt_lua_state) {
      CLuaUtility::OpenRobotStateTable(pt_lua_state, "apriltag");
      for(size_t i = 0; i < m_tReadings.size(); ++i) {
         const SReading& s_reading = m_tReadings[i];
         CLuaUtility::StartTable(pt_lua_state, i+1);
         lua_pushstring(pt_lua_state, "id");
         lua_pushinteger(pt_lua_state, s_reading.Id);
         lua_settable(pt_lua_state, -3);
         CLuaUtility::AddToTable(pt_lua_state, "x", s_reading.X);
         CLuaUtility::AddToTable(pt_lua_state, "y", s_reading.Y);
         CLuaUtility::AddToTable(pt_lua_state, "yaw", s_reading.Yaw);
         CLuaUtility::AddToTable(pt_lua_state, "distance", s_reading.Distance);
         CLuaUtility::AddToTable(pt_lua_state, "angle", s_reading.Angle.GetValue());
         CLuaUtility::EndTable(pt_lua_state);
      }
      CLuaUtility::CloseRobotStateTable(pt_lua_state);
   }
#endif

   /****************************************/
   /****************************************/

#ifdef ARGOS_WITH_LUA
   void CCI_CrazyflieAprilTagSensor::ReadingsToLuaState(lua_State* pt_lua_state) {
      lua_getfield(pt_lua_state, -1, "apriltag");
      size_t unLastReadingNum = lua_rawlen(pt_lua_state, -1);
      for(size_t i = 0; i < m_tReadings.size(); ++i) {
         const SReading& s_reading = m_tReadings[i];
         CLuaUtility::StartTable(pt_lua_state, i+1);
         lua_pushstring(pt_lua_state, "id");
         lua_pushinteger(pt_lua_state, s_reading.Id);
         lua_settable(pt_lua_state, -3);
         CLuaUtility::AddToTable(pt_lua_state, "x", s_reading.X);
         CLuaUtility::AddToTable(pt_lua_state, "y", s_reading.Y);
         CLuaUtility::AddToTable(pt_lua_state, "yaw", s_reading.Yaw);
         CLuaUtility::AddToTable(pt_lua_state, "distance", s_reading.Distance);
         CLuaUtility::AddToTable(pt_lua_state, "angle", s_reading.Angle.GetValue());
         CLuaUtility::EndTable(pt_lua_state);
      }
      if(m_tReadings.size() < unLastReadingNum) {
         for(size_t i = m_tReadings.size()+1; i <= unLastReadingNum; ++i) {
            lua_pushnumber(pt_lua_state, i);
            lua_pushnil(pt_lua_state);
            lua_settable(pt_lua_state, -3);
         }
      }
      lua_pop(pt_lua_state, 1);
   }
#endif

   /****************************************/
   /****************************************/

}
