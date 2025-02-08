#include "foraging_nn_qt_user_functions.h"
#include <source/foraging_nn_loop_functions.h>
#include <source/foraging_nn_controller.h>
// #include <controllers/foraging_nn/foraging_nn_controller.h>
// #include <argos3/core/simulator/entity/controllable_entity.h>

using namespace argos;

/****************************************/
/****************************************/

CForagingNNQTUserFunctions::CForagingNNQTUserFunctions() :
   loopFunctions((CForagingNNLoopFunctions&)(CSimulator::GetInstance().GetLoopFunctions()))
{
   RegisterUserFunction<CForagingNNQTUserFunctions,CFootBotEntity>(&CForagingNNQTUserFunctions::DrawOnRobot);
   // RegisterUserFunction<CForagingNNQTUserFunctions,CFloorEntity>(&CForagingNNQTUserFunctions::DrawOnArena);
}

/****************************************/
/****************************************/

// void CForagingNNQTUserFunctions::Draw(CFootBotEntity& c_entity) {
//    CForagingNNController& cController = dynamic_cast<CForagingNNController&>(c_entity.GetControllableEntity().GetController());
//    CForagingNNController::SFoodData& sFoodData = cController.GetFoodData();
//    if(sFoodData.HasFoodItem) {
//       DrawCylinder(
//          CVector3(0.0f, 0.0f, 0.3f), 
//          CQuaternion(),
//          0.1f,
//          0.05f,
//          CColor::BLACK);
//    }
// }
void CForagingNNQTUserFunctions::DrawOnRobot(CFootBotEntity& entity) {
   CForagingNNController& c = (CForagingNNController&)(entity.GetControllableEntity().GetController());
   CForagingNNController::SFoodData& sFoodData = c.GetFoodData();
   DrawText(
      CVector3(0.0f, 0.0f, 0.3f),
      entity.GetId(),
      CColor::BLACK
   );
   if(sFoodData.HasFoodItem) {
      DrawCylinder(
         CVector3(0.0f, 0.0f, 0.3f), 
         CQuaternion(),
         0.1f,
         0.05f,
         CColor::BLACK);
   }
}

void CForagingNNQTUserFunctions::DrawInWorld() {
   DrawFood();
   DrawNest();
}

void CForagingNNQTUserFunctions::DrawFood() {
   for(const auto& food : loopFunctions.FoodList) {
      if(!food.IsCollected) {
         Real x = food.Position.GetX();
         Real y = food.Position.GetY();
         CVector3 food_3d(x, y, 0.0f);
         DrawCylinder(food_3d, CQuaternion(), loopFunctions.FoodRadius, 0.05f, CColor::BLACK);
      }
   }
}

void CForagingNNQTUserFunctions::DrawNest() {
   Real x = loopFunctions.NestPosition.GetX();
   Real y = loopFunctions.NestPosition.GetY();
   CVector3 nest_3d(x, y, 0.0f);
   DrawCylinder(
      nest_3d, 
      CQuaternion(),
      loopFunctions.NestRadius,
      0.01f,
      CColor::GRAY80);
   DrawCircle(
      nest_3d,
      CQuaternion(),
      loopFunctions.NestRadius,
      CColor::GRAY50);
}

/****************************************/
/****************************************/

REGISTER_QTOPENGL_USER_FUNCTIONS(CForagingNNQTUserFunctions, "foraging_nn_qt_user_functions")