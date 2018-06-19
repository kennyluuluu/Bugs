#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "Compiler.h"
#include <vector>
#include <list>

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* world, int ID, int x, int y, Direction dir, int depth);
	virtual void doSomething() = 0;
	bool isDead() const;							// returns !m_Alive
	void setDead();									// m_Alive = false
	virtual bool blocksMovement() const;			// return false
	virtual void getBitten(int amt);				// return
	virtual bool isBitten() const;					// return false
	virtual void getPoisoned();						// return
	virtual void getStunned();						// return
	virtual bool isStunned() const;					// return false
	virtual bool isEdible() const;					// return false
	virtual bool isPheromone(int colony) const;		// return false
	virtual bool isEnemy(int colony) const;			// return false
	virtual bool isDangerous(int colony) const;		// return false
	virtual bool isAntHill(int colony) const;		// return false
	virtual bool didMove() const;					// return hasMovedThisTick
	virtual bool becomesFoodUponDeath() const;		// return false
	void Movement(bool move);
	StudentWorld* getWorld() const;					// return myWorld
private:
	bool m_hasMovedThisTick;
	bool m_Alive = true;
	StudentWorld* myWorld;
};

class Pebble : public Actor
{
public:
	Pebble(StudentWorld* sw, int x, int y);
	virtual void doSomething();
	virtual bool blocksMovement() const;
};

class TriggerableActor : public Actor
{
public:
	TriggerableActor(StudentWorld* sw, int imageID, int x, int y);
	virtual bool isDangerous(int colony) const;
};

class WaterPool : public TriggerableActor
{
public:
	WaterPool(StudentWorld* sw, int x, int y);
	virtual void doSomething();
};

class Poison : public TriggerableActor
{
public:
	Poison(StudentWorld* sw, int x, int y);
	virtual void doSomething();
};

class EnergyHolder : public Actor
{
public:
	EnergyHolder(StudentWorld* sw, int imageID, int startX, int startY, Direction startDir, int depth, int energy);
	int getEnergy() const;						// Get this actor's amount of energy (for a Pheromone, same as strength).
	void updateEnergy(int amt);					// Adjust this actor's amount of energy upward or downward.
	int pickupAndEatFood(int amt);				// Have this actor pick up an amount of food and eat it.
private:
	int m_energy;
};

class AntHill : public EnergyHolder
{
public:
	AntHill(StudentWorld* sw, int startX, int startY, int colony, Compiler* program);
	virtual void doSomething();
	virtual bool isAntHill(int colony) const;
private:
	int m_colony;
	Compiler* m_program;
};

class Pheromone : public EnergyHolder
{
public:
	Pheromone(StudentWorld* sw, int imageID, int startX, int startY, int colony);
	virtual void doSomething();
	virtual bool isPheromone(int colony) const;
	void increaseStrength();								// Increase the strength (i.e., energy) of this pheromone.
private:
	int m_colony;
};

class Food : public EnergyHolder
{
public:
	Food(StudentWorld* sw, int startX, int startY, int energy);
	virtual void doSomething();
	virtual bool isEdible() const;
};

class Insect : public EnergyHolder
{
public:
	Insect(StudentWorld* world, int imageID,int startX, int startY, int energy);
	Direction randomDir();
	bool doSomethingOfAllInsects();
	virtual void getBitten(int amt);
	virtual bool isBitten() const;
	virtual void getPoisoned();
	virtual void getStunned();						
	virtual bool isStunned() const;		
	virtual bool isEnemy(int colony) const;
	virtual bool isDangerous(int colon) const;
	void addFood(int amt);
	virtual bool becomesFoodUponDeath() const;
	virtual bool moveForwardIfPossible();	// Move this insect one step forward if possible, and return true; otherwise, return false without moving.
	void increaseSleepTicks(int amt);		// Increase the number of ticks this insect will sleep by the indicated amount.
private:
	bool alreadyStunned;
	bool alreadyBitten;
	int m_sleep;
};

class Ant : public Insect
{
public:
	Ant(StudentWorld* sw, int imageID, int startX, int startY, int colony, Compiler* program);
	virtual void doSomething();
	bool Interpreter();
	virtual bool isEnemy(int colony) const;
	virtual bool isDangerous(int colony) const;
	void getXYInFrontOfMe(int& x, int& y) ;
	void pickUpFood();
	void emitPheromone();
private:
	bool wasBlocked;
	int m_foodHolding;
	int m_colony;
	Compiler* m_program;
	int m_lastRandomNumber;
	int ic;
};

class Grasshopper : public Insect
{
public:
	Grasshopper(StudentWorld* sw, int imageID, int startX, int startY, int energy);
	virtual void doSomething();
private:
	virtual bool BabyOrAdultPart() = 0;
	int distanceToWalk;
};

class BabyGrasshopper : public Grasshopper
{
public:
	BabyGrasshopper(StudentWorld* sw, int startX, int startY);
private:
	virtual bool BabyOrAdultPart();
};

class AdultGrasshopper : public Grasshopper
{
public:
	AdultGrasshopper(StudentWorld* sw, int startX, int startY);
	virtual void getBitten(int amt);
	virtual void getPoisoned();
	virtual void getStunned();
private:
	virtual bool BabyOrAdultPart();
};


#endif // ACTOR_H_
