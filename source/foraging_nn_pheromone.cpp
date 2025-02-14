#include "foraging_nn_pheromone.h"

/*****
 * The iAnt pheromone needs to keep track of four things:
 *
 * [1] location of the waypoint
 * [2] a trail to the nest
 * [3] simulation time at creation
 * [4] pheromone rate of decay
 *
 * The remaining variables always start with default values.
 *****/
foraging_nn_pheromone::foraging_nn_pheromone(CVector2         newLocation,
                               Real             newTime,
                               Real             newDecayRate)
{
    /* required initializations */
	location    = newLocation;
	lastUpdated = newTime;
	decayRate   = newDecayRate;

    /* standardized initializations */
	weight      = 1.0;
	threshold   = 0.001;
}

/*****
 * The pheromones slowly decay and eventually become inactive. This simulates
 * the effect of a chemical pheromone trail that dissipates over time.
 *****/
void foraging_nn_pheromone::Update(Real time) {
    /* pheromones experience exponential decay with time */
    weight *= exp(-decayRate * (time - lastUpdated));
    lastUpdated = time;
}

void foraging_nn_pheromone::Reset(Real time){
	weight = 1.0;
	lastUpdated = time;
}

/*****
 * Turns off a pheromone and makes it inactive.
 *****/
void foraging_nn_pheromone::Deactivate() {
    weight = 0.0;
}

/*****
 * Return the pheromone's location.
 *****/
CVector2 foraging_nn_pheromone::GetLocation() {
    return location;
}

/*****
 * Return the weight, or strength, of this pheromone.
 *****/
Real foraging_nn_pheromone::GetWeight() {
	return weight;
}

/*****
 * Is the pheromone active and usable?
 * TRUE:  weight >  threshold : the pheromone is active
 * FALSE: weight <= threshold : the pheromone is not active
 *****/
bool foraging_nn_pheromone::IsActive() {
	return (weight > threshold);
}
