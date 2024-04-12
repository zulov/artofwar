#pragma once
#include <vector>

struct FrameInfo;
constexpr char TEAMS_NUMBER = 3;

class SceneSaver;
class Player;
struct NewGameForm;
struct NewGamePlayer;
struct dbload_player;
struct dbload_resource;

class PlayersManager {
public:
	PlayersManager() = default;
	~PlayersManager();
	void load(std::vector<dbload_player*>* players, std::vector<dbload_resource*>* resources);
	void addPlayer(NewGamePlayer& player);
	void load(NewGameForm* form);
	Player* getActivePlayer() const;
	Player* getPlayer(short i);
	std::vector<Player*>& getTeam(short i);
	void changeActive(short i);
	void save(SceneSaver& saver);
	void update(FrameInfo* frameInfo);
	char getActivePlayerID() const;
	static char getEnemyFor(char player);

	std::vector<Player*>& getAllPlayers();
private:
	Player* activePlayer;
	std::vector<Player*> allPlayers;
	std::vector<Player*> teams[TEAMS_NUMBER];
};
