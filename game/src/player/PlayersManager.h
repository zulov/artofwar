#pragma once
#include <vector>

struct FrameInfo;
constexpr char TEAMS_NUMBER = 3;

class SceneSaver;
class Player;
struct NewGameForm;
struct NewGamePlayer;
struct dbload_player;

class PlayersManager {
public:
	PlayersManager() = default;
	~PlayersManager();
	void load(const std::vector<dbload_player*>* players);
	void addPlayer(NewGamePlayer& player);
	void load(NewGameForm* form);
	Player* getActivePlayer() const;
	Player* getPlayer(unsigned char id) const;
	std::vector<Player*>& getTeam(short i);
	void changeActive(unsigned char id);
	void update(FrameInfo* frameInfo);
	char getActivePlayerID() const;
	void activateNext();
	Player* getEnemyFor(unsigned char player) const;

	std::vector<Player*>& getAllPlayers();
private:
	Player* activePlayer;
	std::vector<Player*> allPlayers;
	std::vector<Player*> teams[TEAMS_NUMBER];
};
