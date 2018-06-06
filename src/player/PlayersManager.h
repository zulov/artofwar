#pragma once
#include "Player.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include <string>
#include <vector>

#define TEAMS_NUMBER 5


class SceneSaver;
struct dbload_player;
struct dbload_resource;

class PlayersManager
{
public:
	PlayersManager();
	~PlayersManager();
	void load(std::vector<dbload_player*>* players, std::vector<dbload_resource*>* resources);
	void addPlayer(NewGamePlayer player);
	void load(NewGameForm* form);
	Player* getActivePlayer();
	Player* getPlayer(short i);
	std::vector<Player*>& getTeam(short i);
	void changeActive(short i);
	void save(SceneSaver& saver);
	static std::string getColumns();
	std::vector<Player*>& getAllPlayers();
private:
	Player* activePlayer;
	std::vector<Player*> allPlayers;
	std::vector<Player*> teams[TEAMS_NUMBER];
};
