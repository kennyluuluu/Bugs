#include "StudentWorld.h"
#include "Field.h"
#include <string>
#include <vector>
#include <list>
#include "Compiler.h"
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir)
{
	winningAntNumber = -1;
	colonyNames.resize(4);
}

int StudentWorld::init()
{
	Ticks = 2000;
	Field f;
	string fieldFile = getFieldFilename();
	string error;
	if(f.loadField(fieldFile, error) != Field::LoadResult::load_success)
	{
		setError(fieldFile + " " + error);
		return false;				// something bad happened!
	}

	Compiler *compilerForEntrant0, *compilerForEntrant1, *compilerForEntrant2, *compilerForEntrant3;
	AntHill *ah0, *ah1, *ah2, *ah3;
	fileNames = getFilenamesOfAntPrograms();

	compilerForEntrant0 = new Compiler;
	if (!compilerForEntrant0->compile(fileNames[0], error)) {
		setError(fileNames[0] + " " + error);
		return GWSTATUS_LEVEL_ERROR;
	}
	colonyNames[0] = compilerForEntrant0->getColonyName();
	
	compilerForEntrant1 = new Compiler;
	if (!compilerForEntrant1->compile(fileNames[1], error)) {
		setError(fileNames[1] + " " + error);
		return GWSTATUS_LEVEL_ERROR;
	}
	colonyNames[1] = compilerForEntrant1->getColonyName();
	
	compilerForEntrant2 = new Compiler;
	if (!compilerForEntrant2->compile(fileNames[2], error)) {
		setError(fileNames[2] + " " + error);
		return GWSTATUS_LEVEL_ERROR;
	}
	colonyNames[2] = compilerForEntrant2->getColonyName();

	compilerForEntrant3 = new Compiler;
	if (!compilerForEntrant3->compile(fileNames[3], error)) {
		setError(fileNames[3] + " " + error);
		return GWSTATUS_LEVEL_ERROR;
	}
	colonyNames[3] = compilerForEntrant3->getColonyName();

	Bugs.resize(VIEW_WIDTH);
	for (int i = 0; i < VIEW_HEIGHT; i++) {
		Bugs[i].resize(VIEW_HEIGHT);
	}
	
	for (int y = 0; y < VIEW_HEIGHT; y++) {
		for (int x = 0; x < VIEW_WIDTH; x++) {
			Field::FieldItem item = f.getContentsOf(x, y);				// note it’s x,y and not y,x!!!
			if (item == Field::FieldItem::rock)
				Bugs[x][y].push_front(new Pebble(this, x, y));
			if (item == Field::FieldItem::grasshopper)
				Bugs[x][y].push_back(new BabyGrasshopper(this, x, y));
			if (item == Field::FieldItem::food)
				Bugs[x][y].push_back(new Food(this, x, y, 6000));
			if (item == Field::FieldItem::water)
				Bugs[x][y].push_front(new WaterPool(this, x, y));
			if (item == Field::FieldItem::poison)
				Bugs[x][y].push_front(new Poison(this, x, y));
			if (item == Field::FieldItem::anthill0) {
				ah0 = new AntHill(this, x, y, 0, compilerForEntrant0);
				Bugs[x][y].push_front(ah0);
			}
			if (item == Field::FieldItem::anthill1) {
				ah1 = new AntHill(this, x, y, 1, compilerForEntrant1);
				Bugs[x][y].push_front(ah1);
			}
			if (item == Field::FieldItem::anthill2) {
				ah2 = new AntHill(this, x, y, 2, compilerForEntrant2);
				Bugs[x][y].push_front(ah2);
			}
			if (item == Field::FieldItem::anthill3) {
				ah3 = new AntHill(this, x, y, 3, compilerForEntrant3);
				Bugs[x][y].push_front(ah3);
			}
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	Ticks--;

	Bugs.resize(VIEW_WIDTH);
	for (int i = 0; i < VIEW_HEIGHT; i++) {
		Bugs[i].resize(VIEW_HEIGHT);
	}

	for (int y = 0; y < VIEW_HEIGHT; y++) {
		for (int x = 0; x < VIEW_WIDTH; x++) {
			for (list<Actor*>::iterator IT = Bugs[x][y].begin(); IT != Bugs[x][y].end();) {
				int oldX = (*IT)->getX();
				int oldY = (*IT)->getY();
				if (!(*IT)->isDead() && !(*IT)->didMove()) {
					(*IT)->doSomething();
				}
				
				if (oldX != (*IT)->getX() || oldY != (*IT)->getY()) {
					Actor* temp = *IT;
					Bugs[temp->getX()][temp->getY()].push_back(*IT);
					IT = Bugs[x][y].erase(IT);
				}
				else 
					IT++;
			}
		}
	}
	for (int y = 0; y < VIEW_HEIGHT; y++) {
		for (int x = 0; x < VIEW_WIDTH; x++) {
			for (list<Actor*>::iterator IT = Bugs[x][y].begin(); IT != Bugs[x][y].end();) {
				(*IT)->Movement(false);
				IT++;
			}
		}
	}
	for (int y = 0; y < VIEW_HEIGHT; y++) {
		for (int x = 0; x < VIEW_WIDTH; x++) {
			for (list<Actor*>::iterator IT = Bugs[x][y].begin(); IT != Bugs[x][y].end();) {
				if ((*IT)->isDead()) {
					Actor* delMe = *IT;
					delete delMe;
					IT = Bugs[x][y].erase(IT);
				}
				else IT++;
			}
		}
	}
	displayText();
	if (Ticks == 0) {
		if (winningAntNumber != -1) {
			setWinner(colonyNames[winningAntNumber]);
			return GWSTATUS_PLAYER_WON;
		}
		else
			return GWSTATUS_NO_WINNER;
	}
	return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::canMoveTo(int x, int y) const {
	if (x < 0 || x > 63 || y < 0 || y > 63)
		return false;
	list<Actor*>::const_iterator IT = Bugs[x][y].cbegin();
	while (IT != Bugs[x][y].cend()) {
		if ((*IT)->blocksMovement())
			return false;
		IT++;
	}
	return true;
}

void StudentWorld::addActor(Actor* a) {
	int x = a->getX();
	int y = a->getY();
	Bugs[x][y].push_back(a);
}

Actor* StudentWorld::getEdibleAt(int x, int y) const {
	list<Actor*>::const_iterator IT = Bugs[x][y].cbegin();
	while (IT != Bugs[x][y].cend()) {
		if ((*IT)->isEdible())
			return *IT;
		IT++;
	}
	return nullptr;
}

Actor* StudentWorld::getPheromoneAt(int x, int y, int colony) const {
	list<Actor*>::const_iterator IT = Bugs[x][y].cbegin();
	while (IT != Bugs[x][y].cend()) {
		if ((*IT)->isPheromone(colony))
			return *IT;
		IT++;
	}
	return nullptr;
}

bool StudentWorld::isEnemyAt(int x, int y, int colony) const {
	list<Actor*>::const_iterator IT = Bugs[x][y].cbegin();
	while (IT != Bugs[x][y].cend()) {
		if ((*IT)->isEnemy(colony))
			return true;
		IT++;
	}
	return false;
}

bool StudentWorld::isDangerAt(int x, int y, int colony) const {
	list<Actor*>::const_iterator IT = Bugs[x][y].cbegin();
	while (IT != Bugs[x][y].cend()) {
		if ((*IT)->isDangerous(colony))
			return true;
		IT++;
	}
	return false;
}

bool StudentWorld::isAntHillAt(int x, int y, int colony) const {
	list<Actor*>::const_iterator IT = Bugs[x][y].cbegin();
	while (IT != Bugs[x][y].cend()) {
		if ((*IT)->isAntHill(colony))
			return true;
		IT++;
	}
	return false;
}

void StudentWorld::poisonAllPoisonableAt(int x, int y) {
	for (list<Actor*>::iterator IT = Bugs[x][y].begin(); IT != Bugs[x][y].end(); IT++) {
		if (!(*IT)->isEdible() && (*IT)->becomesFoodUponDeath()) {
			(*IT)->getPoisoned();
			}
		}
}

void StudentWorld::biteEnemyAt(Actor* me, int colony, int biteDamage) {
	for (list<Actor*>::iterator IT = Bugs[me->getX()][me->getY()].begin(); IT != Bugs[me->getX()][me->getY()].end(); IT++) {
		if ((*IT)->isDangerous(colony) && *IT != me) {
				(*IT)->getBitten(biteDamage);
		}
	}
}

void StudentWorld::stunAllStunnableAt(int x, int y) {
	for (list<Actor*>::iterator IT = Bugs[x][y].begin(); IT != Bugs[x][y].end(); IT++) {
		if ((*IT)->becomesFoodUponDeath() && !(*IT)->isStunned()) {
			(*IT)->getStunned();
		}
	}
}

void StudentWorld::increaseScore(int colony) {
	colonyScores[colony]++;
	if (Ticks > 1994)
		return;
	switch (colony) {
	case 0:
		if (colonyScores[0] > colonyScores[1] && colonyScores[0] > colonyScores[2] && colonyScores[0] > colonyScores[3])
			winningAntNumber = 0;
		break;
	case 1:
		if (colonyScores[1] > colonyScores[0] && colonyScores[1] > colonyScores[2] && colonyScores[1] > colonyScores[3])
			winningAntNumber = 1;
		break;
	case 2:
		if (colonyScores[2] > colonyScores[0] && colonyScores[2] > colonyScores[1] && colonyScores[2] > colonyScores[3])
			winningAntNumber = 2;
		break;
	case 3:
		if (colonyScores[3] > colonyScores[0] && colonyScores[3] > colonyScores[1] && colonyScores[3] > colonyScores[2])
			winningAntNumber = 3;
		break;
	}
}

void StudentWorld::displayText() {
	string NumAnts0 = to_string(colonyScores[0]);
	string NumAnts1 = to_string(colonyScores[1]);
	string NumAnts2 = to_string(colonyScores[2]);
	string NumAnts3 = to_string(colonyScores[3]);
	if (colonyScores[0] < 10)
		NumAnts0 = "0" + NumAnts0;
	if (colonyScores[1] < 10)
		NumAnts1 = "0" + NumAnts1;
	if (colonyScores[2] < 10)
		NumAnts2 = "0" + NumAnts2;
	if (colonyScores[3] < 10)
		NumAnts3 = "0" + NumAnts3;
	string s = "Ticks: ";
	if (Ticks < 10)
		s += "   " + to_string(Ticks) + " - ";
	else if (Ticks < 100)
		s += "  " + to_string(Ticks) + " - ";
	else if (Ticks < 1000)
		s += " " + to_string(Ticks) + " - ";
	else if (Ticks >= 1000)
		s += to_string(Ticks) + " - ";
	switch (winningAntNumber) {
	case 0:
		s += colonyNames[0] + "*: " + NumAnts0 + "  " + colonyNames[1] + ": " + NumAnts1 + "  " + colonyNames[2] + ": " + NumAnts2 + "  " + colonyNames[3] + ": " + NumAnts3;
		break;
	case 1:
		s += colonyNames[0] + ": " + NumAnts0 + "  " + colonyNames[1] + "*: " + NumAnts1 + "  " + colonyNames[2] + ": " + NumAnts2 + "  " + colonyNames[3] + ": " + NumAnts3;
		break;
	case 2:
		s += colonyNames[0] + ": " + NumAnts0 + "  " + colonyNames[1] + ": " + NumAnts1 + "  " + colonyNames[2] + "*: " + NumAnts2 + "  " + colonyNames[3] + ": " + NumAnts3;
		break;
	case 3:
		s += colonyNames[0] + ": " + NumAnts0 + "  " + colonyNames[1] + ": " + NumAnts1 + "  " + colonyNames[2] + ": " + NumAnts2 + "  " + colonyNames[3] + "*: " + NumAnts3;
		break;
	default:
		s += colonyNames[0] + ": " + NumAnts0 + "  " + colonyNames[1] + ": " + NumAnts1 + "  " + colonyNames[2] + ": " + NumAnts2 + "  " + colonyNames[3] + ": " + NumAnts3;
	}
	setGameStatText(s);
	cout << "YAS5";
}

void StudentWorld::cleanUp() {
	for (int y = 0; y < VIEW_HEIGHT; y++) {
		for (int x = 0; x < VIEW_WIDTH; x++) {
			for (list<Actor*>::iterator IT = Bugs[x][y].begin(); IT != Bugs[x][y].end();) {
				Actor* temp = *IT;
				IT = Bugs[x][y].erase(IT);
				delete temp;
			}
		}
	}
}

StudentWorld::~StudentWorld() {
	cleanUp();
}