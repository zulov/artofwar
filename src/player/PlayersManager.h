#pragma once
#include <string>
#include <vector>

constexpr char TEAMS_NUMBER = 3;

class SimulationInfo;
class SceneSaver;
class Player;
struct NewGameForm;
struct NewGamePlayer;
struct dbload_player;
struct dbload_resource;

class PlayersManager {
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
	void update(SimulationInfo* simInfo);
	char getActivePlayerID();
	static std::string getColumns();
	std::vector<Player*>& getAllPlayers();
private:
	Player* activePlayer;
	std::vector<Player*> allPlayers;
	std::vector<Player*> teams[TEAMS_NUMBER];
};
