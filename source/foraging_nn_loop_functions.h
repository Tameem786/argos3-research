#ifndef FORAGING_NN_LOOP_FUNCTIONS_H
#define FORAGING_NN_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/rng.h>
#include <vector>

using namespace argos;
using namespace std;

class CForagingNNLoopFunctions : public CLoopFunctions {

public:

   CForagingNNLoopFunctions();
   virtual ~CForagingNNLoopFunctions() {}

   virtual void Init(TConfigurationNode& t_tree);
   virtual void Reset();
   virtual void Destroy();
   virtual CColor GetFloorColor(const CVector2& c_position_on_plane);
   virtual void PreStep();
   // virtual void PostStep();
   virtual void PostExperiment();
   // vector<CVector2> GetFoodList() { return FoodList; }
   vector<CColor> GetFoodColoringList() { return FoodColoringList; }
   void SetFoodList();
   CVector2 NestPosition;
   CVector2 FoodPosition;
   Real NestRadius;
   Real FoodRadius;
   bool isFoodCollected;

   void SetFoodPosition();
   const CVector2& GetFoodPosition() const { return FoodPosition; }
   const CVector2& GetNestPosition() const { return NestPosition; }

   struct SFoodData {
      CVector2 Position;
      bool IsCollected;
      SFoodData(const CVector2& pos): Position(pos), IsCollected(false) {}
   };

   vector<SFoodData> FoodList;
   void SetInitialFoodPositions();
protected:
   vector<CColor> FoodColoringList;

private:

   Real m_fFoodSquareRadius;
   CRange<Real> m_cForagingArenaSideX, m_cForagingArenaSideY;
   std::vector<CVector2> m_cFoodPos;
   CFloorEntity* m_pcFloor;
   CRandom::CRNG* m_pcRNG;

   std::string m_strOutput;
   std::ofstream m_cOutput;

   UInt32 m_unCollectedFood;
   SInt64 m_nEnergy;
   UInt32 m_unEnergyPerFoodItem;
   UInt32 m_unEnergyPerWalkingRobot;


};

#endif