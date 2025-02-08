#ifndef FORAGING_NN_QT_USER_FUNCTIONS_H
#define FORAGING_NN_QT_USER_FUNCTIONS_H

#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/core/utility/math/ray3.h>

using namespace argos;
using namespace std;

class CForagingNNLoopFunctions;
class CForagingNNController;

class CForagingNNQTUserFunctions : public CQTOpenGLUserFunctions {

public:

   CForagingNNQTUserFunctions();

   virtual ~CForagingNNQTUserFunctions() {}

   void Draw(CFootBotEntity& c_entity);
   void DrawOnRobot(CFootBotEntity& c_entity);
   void DrawInWorld();

private:

   void DrawNest();
   void DrawFood();
   
   CForagingNNLoopFunctions& loopFunctions;
};

#endif