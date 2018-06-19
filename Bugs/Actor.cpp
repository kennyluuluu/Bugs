#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"
#include "Compiler.h"
#include <cstdlib>
#include <cmath>
using namespace std;

Actor::Actor(StudentWorld* world,int ID, int x, int y, Direction dir, int depth)
	:GraphObject(ID, x, y, dir, depth)
{
	m_hasMovedThisTick = false;
	m_Alive = true;
	myWorld = world;
}
bool Actor::isDead() const { 
	return !m_Alive; 
}
void Actor::setDead() {
	m_Alive = false;
}
bool Actor::blocksMovement() const {
	return false;
}
void Actor::getBitten(int amt) { 
	return; 
}
bool Actor::isBitten() const { 
	return false; 
}
void Actor::getPoisoned() { 
	return; 
}
void Actor::getStunned() { 
	return;
}
bool Actor::isStunned() const { 
	return false; 
}
bool Actor::isEdible() const {
	return false;
}
bool Actor::isPheromone(int colony) const { 
	return false; 
}
bool Actor::isEnemy(int colony) const { 
	return false; 
}
bool Actor::isDangerous(int colony) const {
	return false;
}
bool Actor::isAntHill(int colony) const { 
	return false; 
}
bool Actor::didMove() const { 
	return m_hasMovedThisTick; 
}
bool Actor::becomesFoodUponDeath() const {
	return false;
}
void Actor::Movement(bool move) {
	m_hasMovedThisTick = move;
}
StudentWorld* Actor::getWorld() const {
	return myWorld;
}
/*
Actor::~Actor() {
	//cout << getX() << "," << getY() << endl;
}




bool Actor::didImoveYet() const {
	return alreadyMoved;
}

void Actor::changeMoved(bool move) {
	alreadyMoved = move;
}
}*/			// delete

Pebble::Pebble(StudentWorld* sw, int x, int y)
	:Actor(sw, IID_ROCK, x, y, right, 1)
{
}
void Pebble::doSomething() {
	return;
}
bool Pebble::blocksMovement() const{
	return true;
}

TriggerableActor::TriggerableActor(StudentWorld* sw, int imageID, int x, int y)
	:Actor(sw,imageID,x,y,right, 2)
{
}
bool TriggerableActor::isDangerous(int colony) const {
	return true;
}

WaterPool::WaterPool(StudentWorld* sw, int x, int y)
	:TriggerableActor(sw, IID_WATER_POOL, x, y)
{
}
void WaterPool::doSomething() {
	getWorld()->stunAllStunnableAt(getX(), getY());
}

Poison::Poison(StudentWorld* sw, int x, int y)
	:TriggerableActor(sw, IID_POISON, x, y)
{
}
void Poison::doSomething() {
	getWorld()->poisonAllPoisonableAt(getX(), getY());
}

EnergyHolder::EnergyHolder(StudentWorld* sw, int imageID, int startX, int startY, Direction startDir, int depth, int energy)
	:Actor(sw, imageID, startX,startY,startDir,depth)
{
	m_energy = energy;
}
int EnergyHolder::getEnergy() const {
	return m_energy;
}
void EnergyHolder::updateEnergy(int amt) {
	m_energy = amt;
}
int EnergyHolder::pickupAndEatFood(int amt) {
	Food* f = static_cast<Food*>(getWorld()->getEdibleAt(getX(), getY()));
	if (f != nullptr) {
		int temp = f->getEnergy();
		if (amt < temp) {
			f->updateEnergy(temp - amt);
			updateEnergy(getEnergy() + amt);
			return amt;
		}
		if (amt >= temp) {
			updateEnergy(getEnergy() + temp);
			f->updateEnergy(0);
			f->setDead();
			return temp;
		}
	}
	return 0;
}

AntHill::AntHill(StudentWorld* sw, int startX, int startY, int colony, Compiler* program) 
	:EnergyHolder(sw,IID_ANT_HILL,startX,startY,right,2,8999)
{
	m_colony = colony;
	m_program = program;
}
void AntHill::doSomething() {
	updateEnergy(getEnergy() - 1);

	if (getEnergy() <= 0) {
		setDead();
		return;
	}

	if (pickupAndEatFood(10000) > 0) {
		return;
	}

	if (getEnergy() >= 2000) {
		if (m_colony == 0)
			getWorld()->addActor(new Ant(getWorld(), IID_ANT_TYPE0, getX(), getY(), m_colony, m_program));
		if (m_colony == 1)
			getWorld()->addActor(new Ant(getWorld(), IID_ANT_TYPE1, getX(), getY(), m_colony, m_program));
		if (m_colony == 2)
			getWorld()->addActor(new Ant(getWorld(), IID_ANT_TYPE2, getX(), getY(), m_colony, m_program));
		if (m_colony == 3)
			getWorld()->addActor(new Ant(getWorld(), IID_ANT_TYPE3, getX(), getY(), m_colony, m_program));
		updateEnergy(getEnergy() - 1500);
		getWorld()->increaseScore(m_colony);
	}
}
bool AntHill::isAntHill(int colony) const {
	if (colony == m_colony)
		return true;
	else
		return false;
}

Pheromone::Pheromone(StudentWorld* sw, int imageID, int startX, int startY, int colony)
	:EnergyHolder(sw, imageID, startX, startY, right, 2, 256)
{
	m_colony = colony;
}
void Pheromone::doSomething() {
	updateEnergy(getEnergy() - 1);
	if (getEnergy() <= 0)
		setDead();
}
bool Pheromone::isPheromone(int colony) const {
	return true;
}
void Pheromone::increaseStrength() {
	updateEnergy(getEnergy() + 256);
	if (getEnergy() > 768)
		updateEnergy(768);
}

Food::Food(StudentWorld* sw, int startX, int startY, int energy)
	:EnergyHolder(sw, IID_FOOD, startX, startY, right, 2, energy)
{
}
void Food::doSomething() {
	return;
}
bool Food::isEdible() const{
	return true;
}

Insect::Insect(StudentWorld* world, int imageID, int startX, int startY, int energy)
	:EnergyHolder(world,imageID,startX,startY,randomDir(),1,energy)
{
	alreadyStunned = false;
	alreadyBitten = false;
	m_sleep = 0;
}
bool Insect::doSomethingOfAllInsects() {
	updateEnergy(getEnergy() - 1);
	if (getEnergy() <= 0) {
		addFood(100);
		setDead();
		return true;
	}
	if (m_sleep > 0) {
		m_sleep--;
		return true;
	}
	return false;
}
GraphObject::Direction Insect::randomDir() {
	int i = randInt(1, 4);
	switch (i) {
	case 1:
		return up;
		break;
	case 2:
		return right;
		break;
	case 3:
		return down;
		break;
	case 4:
		return left;
		break;
	default:
		return none;
	}
}
void Insect::getBitten(int amt) {
	if (alreadyBitten == true)
		return;
	else {
		updateEnergy(getEnergy() - amt);
		if (getEnergy() <= 0) {
			setDead();
			addFood(100);
		}
	}
	alreadyBitten = true;
}
bool Insect::isBitten() const {
	return alreadyBitten;
}
void Insect::getPoisoned() {
	updateEnergy(getEnergy() - 150);
	if (getEnergy() <= 0) {
		setDead();
		addFood(100);
	}
}
void Insect::getStunned() {
	m_sleep += 2;
	alreadyStunned = true;
}
bool Insect::isStunned() const {
	return alreadyStunned;
}
bool Insect::isEnemy(int colony) const {
	return true;
}
bool Insect::isDangerous(int colony) const {
	return true;
}
void Insect::addFood(int amt) {
	if (amt <= 0)
		return;
	Food* f = static_cast<Food*>(getWorld()->getEdibleAt(getX(), getY()));
	if (f != nullptr)
		f->updateEnergy(f->getEnergy() + amt);
	else
		getWorld()->addActor(new Food(getWorld(), getX(), getY(), amt));
}
bool Insect::becomesFoodUponDeath() const {
	return true;
}
bool Insect::moveForwardIfPossible() {
	bool possible = false;
	Movement(true);
	switch (getDirection()) {
	case up:
	{
		possible = getWorld()->canMoveTo(getX(), getY() - 1);
		if (possible) {
			moveTo(getX(), getY() - 1);
			alreadyBitten = false;
			alreadyStunned = false;
		}
		return possible;
		break;
	}
	case right:
	{
		possible = getWorld()->canMoveTo(getX() + 1, getY());
		if (possible) {
			moveTo(getX() + 1, getY());
			alreadyBitten = false;
			alreadyStunned = false;
		}
		return possible;
		break;
	}
	case down:
	{
		possible = getWorld()->canMoveTo(getX(), getY() + 1);
		if (possible) {
			moveTo(getX(), getY() + 1);
			alreadyBitten = false;
			alreadyStunned = false;
		}
		return possible;
		break;
	}
	case left:
	{
		possible = getWorld()->canMoveTo(getX() - 1, getY());
		if (possible) {
			moveTo(getX() - 1, getY());
			alreadyBitten = false;
			alreadyStunned = false;
		}
		return possible;
		break;
	}
	default:
		return false;
	}
}
void Insect::increaseSleepTicks(int amt) {
	m_sleep += amt;
	if (m_sleep < 0)
		m_sleep = 0;
}

Ant::Ant(StudentWorld* sw, int imageID, int startX, int startY, int colony, Compiler* program)
	:Insect(sw,imageID,startX,startY,1500)
{
	wasBlocked = false;
	m_foodHolding = 0;
	m_colony = colony;
	m_program = program;
	m_lastRandomNumber = 0;
	ic = 0;
}
void Ant::doSomething() {
	if (doSomethingOfAllInsects())
		return;
	if (!Interpreter())
		setDead();
}
bool Ant::Interpreter() {
	string error;
	if (!m_program->compile(m_program->getColonyName(), error)) {
		return false;
	}
	Compiler::Command cmd;
	for (int i = 0; i < 10; i++) {
		if (!m_program->getCommand(ic, cmd)) {
			return false;
		}
		switch (cmd.opcode) {
		case Compiler::moveForward:
		{
			if (moveForwardIfPossible()) {
				wasBlocked = false;
			}
			else
				wasBlocked = true;
			ic++;
			return true;
			break;
		}
		case Compiler::eatFood:
		{
			if (m_foodHolding >= 100) {
				m_foodHolding -= 100;
				updateEnergy(getEnergy() + 100);
			}
			else {
				updateEnergy(getEnergy() + m_foodHolding);
				m_foodHolding = 0;
			}
			ic++;
			return true;
			break;
		}
		case Compiler::dropFood:
		{
			addFood(m_foodHolding);
			m_foodHolding = 0;
			ic++;
			return true;
			break;
		}
		case Compiler::bite:
		{
			getWorld()->biteEnemyAt(this, m_colony, 15);
			ic++;
			return true;
			break;
		}
		case Compiler::pickupFood: 
		{
			pickUpFood();
			ic++;
			return true;
			break;
		}
		case Compiler::emitPheromone:				
		{
			emitPheromone();
			ic++;
			return true;
			break;
		}
		case Compiler::faceRandomDirection:
		{
			setDirection(randomDir());
			ic++;
			return true;
			break;
		}
		case Compiler::rotateClockwise:
		{
			switch (getDirection()) {
			case up:
				setDirection(right);
				break;
			case right:
				setDirection(down);
				break;
			case down:
				setDirection(left);
				break;
			case left:
				setDirection(up);
				break;
			default:
				setDirection(none);
			}
			ic++;
			return true;
			break;
		}
		case Compiler::rotateCounterClockwise:
		{
			switch (getDirection()) {
			case up:
				setDirection(left);
				break;
			case right:
				setDirection(up);
				break;
			case down:
				setDirection(right);
				break;
			case left:
				setDirection(down);
				break;
			default:
				setDirection(none);
			}
			ic++;
			return true;
			break;
		}
		case Compiler::generateRandomNumber:
		{
			m_lastRandomNumber = randInt(0, stoi(cmd.operand1));
			ic++;
			break;
		}
		case Compiler::goto_command:
		{
			ic = stoi(cmd.operand1);
			break;
		}
		case Compiler::if_command: {
			switch (stoi(cmd.operand1)) {
			case Compiler::last_random_number_was_zero:
			{
				if (m_lastRandomNumber == 0)
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			case Compiler::i_am_carrying_food:
			{
				if (m_foodHolding > 0)
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			case Compiler::i_am_hungry:
			{
				if (getEnergy() <= 25)
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			case Compiler::i_am_standing_with_an_enemy:
			{
				if (getWorld()->isEnemyAt(getX(),getY(),m_colony))									
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			case Compiler::i_am_standing_on_food:
			{
				if (getWorld()->getEdibleAt(getX(), getY()) != nullptr)
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			case Compiler::i_am_standing_on_my_anthill:
			{
				if (getWorld()->isAntHillAt(getX(), getY(), m_colony))
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			case Compiler::i_smell_pheromone_in_front_of_me:
			{
				int XinFront;
				int YinFront;
				getXYInFrontOfMe(XinFront, YinFront);
				if (getWorld()->getPheromoneAt(XinFront,YinFront,m_colony) != nullptr)													
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			case Compiler::i_smell_danger_in_front_of_me:
			{
				int XinFront;
				int YinFront;
				getXYInFrontOfMe(XinFront, YinFront);
				if (getWorld()->isDangerAt(XinFront,YinFront,m_colony))
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			case Compiler::i_was_bit:
			{
				if (isBitten())
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			case Compiler::i_was_blocked_from_moving:
			{
				if (wasBlocked)											
					ic = stoi(cmd.operand2);
				else
					ic++;
				break;
			}
			}
			break;
		}
		default:
			cout << cmd.opcode;
	}
	}
	return true;
}
bool Ant::isEnemy(int colony) const {
	if (colony == m_colony)
		return false;
	return true;
}
bool Ant::isDangerous(int colony) const {
	return isEnemy(colony);
}
void Ant::getXYInFrontOfMe(int& x, int& y) {
	Direction d = getDirection();
	switch (d) {
	case up:
		x = getX();
		y = getY() - 1;
		break;
	case right:
		x = getX() + 1;
		y = getY();
		break;
	case down:
		x = getX();
		y = getY() + 1;
		break;
	case left:
		x = getX() - 1;
		y = getY();
		break;
	default:
		break;
	}
	return;
}

void Ant::pickUpFood() {
	Food* f = static_cast<Food*>(getWorld()->getEdibleAt(getX(), getY()));
	if (f->getEnergy() >= 400) {
		if (m_foodHolding <= 1400) {
			m_foodHolding += 400;
			f->updateEnergy(f->getEnergy() - 400);
			if (f->getEnergy() == 0)
				f->setDead();
		}
		else {
			f->updateEnergy(f->getEnergy() - (1800 - m_foodHolding));
			m_foodHolding = 1800;
		}
	}
	else {
		if (m_foodHolding <= 1400) {
			m_foodHolding += f->getEnergy();
			f->updateEnergy(0);
			f->setDead();
		}
		else
		{
			f->updateEnergy(getEnergy() - (1800 - m_foodHolding));
			m_foodHolding += f->getEnergy();
			if (m_foodHolding > 1800)
				m_foodHolding = 1800;
			if (f->getEnergy() <= 0) {
				f->updateEnergy(0);
				f->setDead();
			}
		}
	}
}
void Ant::emitPheromone() {
	Pheromone* P = static_cast<Pheromone*>(getWorld()->getPheromoneAt(getX(), getY(), m_colony));
	if (P != nullptr)
		P->increaseStrength();
	else {
		switch (m_colony) {
		case 0:
			getWorld()->addActor(new Pheromone(getWorld(), IID_PHEROMONE_TYPE0, getX(), getY(), m_colony));
			break;
		case 1:
			getWorld()->addActor(new Pheromone(getWorld(), IID_PHEROMONE_TYPE1, getX(), getY(), m_colony));
			break;
		case 2:
			getWorld()->addActor(new Pheromone(getWorld(), IID_PHEROMONE_TYPE2, getX(), getY(), m_colony));
			break;
		case 3:
			getWorld()->addActor(new Pheromone(getWorld(), IID_PHEROMONE_TYPE3, getX(), getY(), m_colony));
			break;
		}

	}
}

Grasshopper::Grasshopper(StudentWorld* sw, int imageID, int startX, int startY, int energy)
	:Insect(sw, imageID, startX, startY, energy)
{
}
void Grasshopper::doSomething() {
	if (doSomethingOfAllInsects())
		return;

	if (BabyOrAdultPart())
		return;

	if (pickupAndEatFood(200) > 0) {
		if (randInt(1, 2) == 1) {
			increaseSleepTicks(2);
			return;
		}
	}
	if (distanceToWalk == 0) {
		setDirection(randomDir());
		distanceToWalk += randInt(2, 10);
	}
	if (moveForwardIfPossible())
		distanceToWalk--;
	else
		distanceToWalk = 0;
	increaseSleepTicks(2);
}

BabyGrasshopper::BabyGrasshopper(StudentWorld* sw, int startX, int startY)
	:Grasshopper(sw, IID_BABY_GRASSHOPPER, startX, startY, 500)
{
}
bool BabyGrasshopper::BabyOrAdultPart() {
	if (getEnergy() >= 1600) {
		getWorld()->addActor(new AdultGrasshopper(getWorld(), getX(), getY()));
		setDead();
		return true;
	}
	return false;
}

AdultGrasshopper::AdultGrasshopper(StudentWorld* sw, int startX, int startY)
	:Grasshopper(sw, IID_ADULT_GRASSHOPPER, startX, startY, 1600)
{
}
bool AdultGrasshopper::BabyOrAdultPart() {
	if (randInt(1, 3) == 1) {
		getWorld()->biteEnemyAt(this, 5, 50);
		increaseSleepTicks(2);
		return true;
	}
	if (randInt(1, 10) == 1) {
		int x = 0, y = 0;
		do {
			double radius = randInt(0, 10);
			int randomRadian = randInt(0, 360) * 2 * 3.14159 / 360;
			x = radius*cos(randomRadian);
			y = radius*sin(randomRadian);
		} while (!getWorld()->canMoveTo(getX() + x, getY() + y));
		moveTo(getX() + x, getY() + y);
		increaseSleepTicks(2);
		return true;
	}
	return false;
}
void AdultGrasshopper::getBitten(int amt) {
	Insect::getBitten(amt);
	if (randInt(1, 2) == 1)
		getWorld()->biteEnemyAt(this, 5, 50);
}
void AdultGrasshopper::getPoisoned() {
	return;
}
void AdultGrasshopper::getStunned() {
	return;
}

