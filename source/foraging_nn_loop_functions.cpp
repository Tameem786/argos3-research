#include "foraging_nn_loop_functions.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <source/foraging_nn_controller.h>

/****************************************/
/****************************************/

CForagingNNLoopFunctions::CForagingNNLoopFunctions() :
   m_cForagingArenaSideX(-0.9f, 1.7f),
   m_cForagingArenaSideY(-1.7f, 1.7f),
   m_pcFloor(NULL),
   m_pcRNG(NULL),
   m_unCollectedFood(0),
   m_nEnergy(0),
   m_unEnergyPerFoodItem(1),
   m_unEnergyPerWalkingRobot(1) {
}

/****************************************/
/****************************************/

void CForagingNNLoopFunctions::Init(TConfigurationNode& t_node) {
   try {
      TConfigurationNode& tForaging = GetNode(t_node, "foraging");
      /* Get a pointer to the floor entity */
      m_pcFloor = &GetSpace().GetFloorEntity();
      /* Get the number of food items we want to be scattered from XML */
      UInt32 unFoodItems;
      GetNodeAttribute(tForaging, "items", unFoodItems);
      /* Get the number of food items we want to be scattered from XML */
      GetNodeAttribute(tForaging, "radius", m_fFoodSquareRadius);
      m_fFoodSquareRadius *= m_fFoodSquareRadius;
      /* Create a new RNG */
      m_pcRNG = CRandom::CreateRNG("argos");
      m_cForagingArenaSideX.Set(-1.5f, 1.5f);
      m_cForagingArenaSideY.Set(-1.5f, 1.5f);
      /* Distribute uniformly the items in the environment */
      // for(UInt32 i = 0; i < unFoodItems; ++i) {
      //    m_cFoodPos.push_back(
      //       CVector2(m_pcRNG->Uniform(m_cForagingArenaSideX),
      //                m_pcRNG->Uniform(m_cForagingArenaSideY)));
      // }
      /* Get the output file name from XML */
      GetNodeAttribute(tForaging, "output", m_strOutput);
      /* Open the file, erasing its contents */
      m_cOutput.open(m_strOutput.c_str(), std::ios_base::trunc | std::ios_base::out);
      m_cOutput << "# clock\twalking\tresting\tcollected_food\tenergy" << std::endl;
      /* Get energy gain per item collected */
      GetNodeAttribute(tForaging, "energy_per_item", m_unEnergyPerFoodItem);
      /* Get energy loss per walking robot */
      GetNodeAttribute(tForaging, "energy_per_walking_robot", m_unEnergyPerWalkingRobot);
      GetNodeAttribute(tForaging, "NestPosition", NestPosition);
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error parsing loop functions!", ex);
   }

   NestPosition = CVector2(0.0f, 0.0f);
   NestRadius = 0.35f;
   FoodRadius = 0.1f;
   isFoodCollected = false;
   SetInitialFoodPositions();
}

/****************************************/
/****************************************/

void CForagingNNLoopFunctions::SetInitialFoodPositions() {
   FoodList.clear();
   for(size_t i = 0; i < 5; i++) {
      CVector2 cPos;
      bool validPos = false;
      do {
         cPos = CVector2(m_pcRNG->Uniform(m_cForagingArenaSideX),
                         m_pcRNG->Uniform(m_cForagingArenaSideY));
         validPos = true;
         if((cPos - NestPosition).Length() < NestRadius+FoodRadius) {
            validPos = false;
            continue;
         }
         for(const SFoodData& food : FoodList) {
            if((cPos - food.Position).Length() < FoodRadius*3) {
               validPos = false;
               break;
            }
         }
      } while(!validPos);
      FoodList.emplace_back(cPos);
   }
}

CColor CForagingNNLoopFunctions::GetFloorColor(const CVector2& c_position_on_plane) {
   if(c_position_on_plane.SquareLength() < NestRadius*NestRadius) {
      return CColor::GRAY50;
   }
   else {
      return CColor::WHITE;
   }
}

void CForagingNNLoopFunctions::Reset() {
   /* Zero the counters */
   m_unCollectedFood = 0;
   m_nEnergy = 0;
   /* Close the file */
   m_cOutput.close();
   /* Open the file, erasing its contents */
   m_cOutput.open(m_strOutput.c_str(), std::ios_base::trunc | std::ios_base::out);
   m_cOutput << "# clock\twalking\tresting\tcollected_food\tenergy" << std::endl;
   /* Distribute uniformly the items in the environment */
   // for(UInt32 i = 0; i < m_cFoodPos.size(); ++i) {
   //    m_cFoodPos[i].Set(m_pcRNG->Uniform(m_cForagingArenaSideX),
   //                      m_pcRNG->Uniform(m_cForagingArenaSideY));
   // }
   isFoodCollected = false;
   SetInitialFoodPositions();
}

/****************************************/
/****************************************/

void CForagingNNLoopFunctions::Destroy() {
   /* Close the file */
   m_cOutput.close();
}

/****************************************/
/****************************************/

void CForagingNNLoopFunctions::SetFoodList() {
   for(size_t i = 0; i < 15; i++) {
      CVector2 cPos;
      cPos.Set(m_pcRNG->Uniform(m_cForagingArenaSideX),
               m_pcRNG->Uniform(m_cForagingArenaSideY));
      FoodList.push_back(cPos);
      FoodColoringList.push_back(CColor::BLACK);
   }
}

void CForagingNNLoopFunctions::PreStep() {
   /* Logic to pick and drop food items */
   /*
    * If a robot is in the nest, drop the food item
    * If a robot is on a food item, pick it
    * Each robot can carry only one food item per time
    */
   UInt32 unWalkingFBs = 0;
   UInt32 unRestingFBs = 0;
   /* Check whether a robot is on a food item */
   CSpace::TMapPerType& m_cFootbots = GetSpace().GetEntitiesByType("foot-bot");

   for(CSpace::TMapPerType::iterator it = m_cFootbots.begin();
       it != m_cFootbots.end();
       ++it) {
      /* Get handle to foot-bot entity and controller */
      CFootBotEntity& cFootBot = *any_cast<CFootBotEntity*>(it->second);
      CForagingNNController& cController = dynamic_cast<CForagingNNController&>(cFootBot.GetControllableEntity().GetController());
      
      /* Count how many foot-bots are in which state */
      if(! cController.IsResting()) ++unWalkingFBs;
      else ++unRestingFBs;
      /* Get the position of the foot-bot on the ground as a CVector2 */
      CVector2 cPos;
      cPos.Set(cFootBot.GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
               cFootBot.GetEmbodiedEntity().GetOriginAnchor().Position.GetY());
      /* Get food data */
      // Handle food pickup
      CForagingNNController::SFoodData& sFoodData = cController.GetFoodData();
      if(!sFoodData.HasFoodItem) {
         for(SFoodData& food : FoodList) {
            if(!food.IsCollected && (cPos - food.Position).SquareLength() < FoodRadius*FoodRadius) {
                  sFoodData.HasFoodItem = true;
                  food.IsCollected = true;
                  break;
            }
         } 
         m_pcFloor->SetChanged();
      } 
      // Handle food drop
      if(sFoodData.HasFoodItem) {
         if((cPos - NestPosition).SquareLength() < NestRadius*NestRadius) {
            sFoodData.HasFoodItem = false;
            sFoodData.FoodItemIdx = 0;
            sFoodData.TotalFoodItems++;
            ++m_unCollectedFood;
            m_pcFloor->SetChanged();
            // SetFoodPosition();
            // isFoodCollected = false;
         }
      }
   }

   /* Update energy expediture due to walking robots */
   m_nEnergy -= unWalkingFBs * m_unEnergyPerWalkingRobot;
   /* Output stuff to file */
   m_cOutput << GetSpace().GetSimulationClock() << "\t"
             << unWalkingFBs << "\t"
             << unRestingFBs << "\t"
             << m_unCollectedFood << "\t"
             << m_nEnergy << std::endl;
}

void CForagingNNLoopFunctions::PostExperiment() {
   size_t total_food_collected = 0;
   CSpace::TMapPerType& footbots = GetSpace().GetEntitiesByType("foot-bot");
   
   for(const auto& bot : footbots) {
      CFootBotEntity& footBot = *any_cast<CFootBotEntity*>(bot.second);
      CForagingNNController& c = (CForagingNNController&)(footBot.GetControllableEntity().GetController());
      // LOG << "Fuel Cost: " << c.GetId() << ":  " << c.GetFuelCost() << endl;
      total_food_collected += c.GetFoodData().TotalFoodItems;
   }
   LOG << "Total food collected: " << total_food_collected << std::endl;
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CForagingNNLoopFunctions, "foraging_nn_loop_functions")