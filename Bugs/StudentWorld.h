#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <list>

class Actor;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	virtual int init();

	virtual int move();

	bool canMoveTo(int x, int y) const;				// Can an insect move to x,y?

	void addActor(Actor* a);						// Add an actor to the world

	// If an item that can be picked up to be eaten is at x,y, return a
	// pointer to it; otherwise, return a null pointer.  (Edible items are
	// only ever going be food.)
	Actor* getEdibleAt(int x, int y) const;

	// If a pheromone of the indicated colony is at x,y, return a pointer
	// to it; otherwise, return a null pointer.
	Actor* getPheromoneAt(int x, int y, int colony) const;

	bool isEnemyAt(int x, int y, int colony) const;				// Is an enemy of an ant of the indicated colony at x,y?

	bool isDangerAt(int x, int y, int colony) const;			// Is something dangerous to an ant of the indicated colony at x,y?

	bool isAntHillAt(int x, int y, int colony) const;			// Is the anthill of the indicated colony at x,y?

	// Bite an enemy of an ant of the indicated colony at me's location
	// (other than me; insects don't bite themselves).  Return true if an
	// enemy was bitten.
	void biteEnemyAt(Actor* me, int colony, int biteDamage);

	void poisonAllPoisonableAt(int x, int y);						// Poison all poisonable actors at x,y.

	void stunAllStunnableAt(int x, int y);							// Stun all stunnable actors at x,y.

	void increaseScore(int colony);									// Record another ant birth for the indicated colony.

	void displayText();

	virtual void cleanUp();

	virtual ~StudentWorld();

private:
	std::vector<std::vector<std::list<Actor*>>> Bugs;
	std::vector<std::string> fileNames;
	std::vector<std::string> colonyNames;
	int Ticks;
	int colonyScores[4];
	int winningAntNumber;
};

#endif // STUDENTWORLD_H_
