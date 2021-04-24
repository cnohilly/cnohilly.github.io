#include<iostream>
#include<iomanip>
#include<conio.h>
#include<string>
#include<vector>


/* Menu Functions Declarations */
int getInput(int);
int createMenu(std::vector<std::string>&);
int mainMenu(void);
int startGameMenu(void);
int endSessionMenu(void);
int selectGameType(int);
int selectLargeSoloMap(void);
int selectSmallSoloMap(void);
int selectTeamMap(void);
int selectFinalsMap(void);
int selectTeamColor(int);

/* Gets the input for a menu restricted by the number of options in the menu */
int getInput(int numChoices) {										// Used to get the player's input at a menu
	int input = 0;													// Assigned the value that the player wants to select
	char choices = 49 + numChoices;									// Used as the upper bound for the while loop
	char buttonPress = 0;											// The button the player is attempting to input
	std::cout << "Press the number of your choice." << std::endl;
	while (!('1' <= buttonPress && buttonPress < choices)) {		// Has to be a number that is in the current menu
		switch (buttonPress = _getch()) {							// Use getch to get player's keypress and assign the number they are pressing
		case '1': input = 1;
			break;
		case '2': input = 2;
			break;
		case '3': input = 3;
			break;
		case '4': input = 4;
			break;
		case '5': input = 5;
			break;
		case '6': input = 6;
			break;
		case '7': input = 7;
			break;
		case '8': input = 8;
			break;
		case '9': input = 9;
			break;
		}
	}
	return input;
}

/* Used to create menus by taking a std::vector of std::strings to iterate through */
int createMenu(std::vector<std::string>& menuVector) {
	int numChoices = 0;
	std::cout << std::endl;
	std::cout << menuVector.at(0) << std::endl;								// The first line of the std::vector is what the menu is requesting
	for (int i = 1; i < menuVector.size(); i++) {							// Iterates through the std::vector for each option of the menu
		numChoices++;														// Increments the amount of choices in the given menu
		std::cout << "  " << numChoices << ". " << menuVector.at(i) << std::endl;		// Prints the current choice number and the choice at the given index of the std::vector
	}
	return getInput(numChoices);		// Asks for the user's input with the given number of choices
}

/* Main menu. Used to determine what the user would like to do.  */
int mainMenu() {
	std::vector<std::string> menuVector {
		"How would you like to proceed?",
		"Start a session",
		"Check Stats",
		"Edit Stats",
		"Exit program"
	};
	return createMenu(menuVector);
}

/* Start Game menu. Used to ask if the user would like to start a new game */
int startGameMenu() {
	std::vector<std::string> menuVector{
		"Would you like to start a new game?",
		"Yes",
		"No"
	};
	return createMenu(menuVector);
}

/* End Session menu. Used to ask if the user would like to end the session */
int endSessionMenu() {
	std::vector<std::string> menuVector{
		"End the session?",
		"Yes",
		"No"
	};
	return createMenu(menuVector);
}

/* Game Type menu. Used to select the game type of the current round */
int selectGameType(int roundNum) {
	std::vector<std::string> menuVector;
	menuVector.push_back("Select the map type for the round:");	
	if (roundNum < 5) {								// If it is not the final round possible
		menuVector.push_back("Large Solo Map");
		menuVector.push_back("Small Solo Map");
		menuVector.push_back("Team Game");
	}
	if (roundNum > 1) {								// If it is not the first round of the game
		menuVector.push_back("Finals");
	}

	if (roundNum == 1) {							// If it is the first round 
		menuVector.push_back("Back to previous menu");
	}
	else {											// If it is not the first round
		menuVector.push_back("Lost last round");
	}

	if (roundNum < 5) {								// If it is not the final round
		return createMenu(menuVector);	 
	}
	else {											// If it is the final round
		return createMenu(menuVector) + 3;
	}
}

/* Large Solo Maps menu. Used to select which Large Solo Map is being played in the current round */
int selectLargeSoloMap() {
	std::vector<std::string> menuVector{
	"Select the map:",
	"Dizzy Heights",
	"Door Dash",
	"Gate Crash",
	"Hit Parade",
	"See Saw",
	"Slime Climb",
	"The Whirlygig",
	"Back to previous menu"
	};
	return createMenu(menuVector);
}

/* Small Solo Maps menu. Used to select which Small Solo Map is being played in the current round */
int selectSmallSoloMap() {
	std::vector<std::string> menuVector{
		"Select the map:",
		"Block Party",
		"Fruit Chute",
		"Jump Club",
		"Perfect Match",
		"Roll Out",
		"Tail Tag",
		"Tip Toe",
		"Back to previous menu"
	};
	return createMenu(menuVector);
}

/* Team Map menu. Used to select which Team Game Map is being played in the current round */
int selectTeamMap() {
	std::vector<std::string> menuVector{
		"Select the map:",
		"Egg Scramble",
		"Fall Ball",
		"Hoarders",
		"Hoopsie Daisy",
		"Jinxed",
		"Rock 'n' Roll",
		"Team Tail Tag",
		"Back to previous menu"	
	};
	return createMenu(menuVector);
}

/* Finals Map menu. Used to select which Finals Map is being played in the current round */
int selectFinalsMap() {
	std::vector<std::string> menuVector{
		"Select the map:",
		"Fall Mountain",
		"Hex-a-gone",
		"Jump Showdown",
		"Royal Fumble",
		"Back to previous menu"
	};
	return createMenu(menuVector);
}

/* Select Team Color menu. Used to choose which team is being played in the current round, if applicable */
int selectTeamColor(int numTeams) {
	std::vector<std::string> menuVector;
	menuVector.push_back("Select your team color:");
	if (numTeams > 2) {					// Red only if it is a game type with 3 teams
		menuVector.push_back("Red");
	}
	if (numTeams > 3) {					// Green only if it is a game type with 4 teams
		menuVector.push_back("Green");
	}
	menuVector.push_back("Blue");		// Blue is always in team games
	menuVector.push_back("Yellow");		// Yellow is always in team games

	int input = 0;
	switch (numTeams) {						
	case 2:					// 2 Teams
		return createMenu(menuVector) + 2;
		break;
	case 3:					// 3 Teams
		input = createMenu(menuVector);
		if (input == 1) {
			return input;
		}
		else {
			return input + 1;
		}
		break;
	case 4:					// 4 Teams
		return createMenu(menuVector);
		break;
	}
}

/* Used to get the result of a round */
int roundResultsMenu(bool finalRound) {
	std::vector<std::string> menuVector;
	menuVector.push_back("What was the result of the round?");
	if (finalRound) {					// If it is the final round
		menuVector.push_back("Lost");
		menuVector.push_back("Won");
	} 
	else {								// If it is not the final round
		menuVector.push_back("Eliminated");
		menuVector.push_back("Qualified");
	}
	menuVector.push_back("Selected wrong map");				// Selected the wrong map and need to reselect the round
	menuVector.push_back("Disconnected or left game");		// Used for if you've disconnected or left the game (not having lost or played)
	return createMenu(menuVector) - 1;
}

/* Used to ask the user which set of stats they would like to view */
int viewStatsMenu() {
	std::vector<std::string> menuVector{
		"Which set of stats would you like to view?",
		"Games played and games won",
		"All stats",
		"Last session stats",
		"All team game stats",
		"Last session team game stats",
		"Stop viewing stats"
	};
	return createMenu(menuVector);
}

int editStatsMenu() {
	std::vector<std::string> menuVector{
		"What would you like to edit?",
		"Stats for a map",
		"Games played or won",
		"Return to previous menu"
	};
	return createMenu(menuVector);
}

int selectMapTypeEditMenu() {
	std::vector<std::string> menuVector {
		"Which map type would you like to edit?",
		"Large Solo Map",
		"Small Solo Map",
		"Team Game Map",
		"Finals Map",
		"Cancel edit"
	};
	return createMenu(menuVector);
}

int editTypeMenu() {
	std::vector<std::string> menuVector{
		"What would you like to edit?",
		"Remove 1 win and add 1 loss",
		"Add 1 win and remove 1 loss",
		"Add 1 win",
		"Remove 1 win",
		"Add 1 loss",
		"Remove 1 loss",
		"Cancel edit"
	};
	return createMenu(menuVector);
}

int editGamesMenu() {
	std::vector<std::string> menuVector{
		"What would you like to edit?",
		"Decrease number of games played",
		"Increase number of games played",
		"Decrease number of games won",
		"Increase number of games won",
		"Cancel edit"
	};
	return createMenu(menuVector);
}
