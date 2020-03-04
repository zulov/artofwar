#pragma once
#include <string>
#include <vector>

#define TEAMS_NUMBER 5

struct NewGameForm;
class SceneSaver;
class Player;
struct NewGamePlayer;
struct dbload_player;
struct dbload_resource;

class PlayersManager
{
public:
	PlayersManager();
	~PlayersManager();
	void load(std::vector<dbload_player*>* players, std::vector<dbload_resource*>* resources);
	void addPlayer(NewGamePlayer& player);
	void load(NewGameForm* form);
	Player* getActivePlayer() const;
	Player* getPlayer(short i);
	std::vector<Player*>& getTeam(short i);
	void changeActive(short i);
	void save(SceneSaver& saver);
	void update();
	char getActivePlayerID();
	static std::string getColumns();
	std::vector<Player*>& getAllPlayers();
private:
	Player* activePlayer;
	std::vector<Player*> allPlayers;
	std::vector<Player*> teams[TEAMS_NUMBER];
};
