#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>
#include<conio.h>
#include"Menus.h"

using namespace std;

/* 
This will be a simple program to easily track gameplay stats for the video game Fall Guys. It will make it simple to start a round, choose the game type you get put into,
choose the game, then choose your team if applicable and whether you win or lose the round.
*/

const string filePath = "Stats.txt";		// constant file path for the Stats file to be written to
const int	numColumns = 12,				// constant to determine the number of columns used to print stats
statsX = 25,		// Number of maps
statsY = 10,		// Number of stats stored for a map -  Total Rounds Won / Total Rounds Lost / Red Wins / Red Losses / Green Wins / Green Losses / Blue Wins / Blue Losses / Yellow Wins / Yellow Losses
soloLargeMaps = 7,	// Number of maps that are Large Solo Maps
soloSmallMaps = 7,	// Number of maps that are Small Solo Maps
teamMaps = 7,		// Number of maps that are Team Maps
finalsMaps = 4;		// Number of maps that are Finals Maps


int statsColumnSizes[numColumns],		// Array of int to hold column sizes for output table (12 Columns Total) - Map / Rounds Won / Rounds Lost / Win Rate	/ RW / RL / GW / GL / BW / BL / YW / YL 
	sessionColumnSizes[numColumns];		// Array of int to hold column sizes for output table for session stats
int allStats[statsX][statsY];			// Array to hold all of the stats stored - 25 Maps each with					  Rounds Won / Rounds Lost				/ RW / RL / GW / GL / BW / BL / YW / YL
int sessionStats[statsX][statsY];		// Array to hold all of the stats for current session

int gamesPlayed = 0, gamesWon = 0,					// Total number of games played and total number of games won (final rounds won)
	sessionGamesPlayed = 0, sessionGamesWon = 0;	// Total number of games played and total wins in the current session

/* Function Declarations */
void loadFile(int*, int*, int*);		// Attempts to load a file containing data for stats. Parameters will point to an integer for the amount of games played, integer for the amount of games won, and the beginning of an array for storing stats.
void writeStatsFile(int, int, int*);	// Attempts to write to the file containig data for stats. Parameters will be an integer for the amount of games played, integer for the amount of games won, and a pointer to the beginning of an array containing stats.
void setColumnSizes(int*, int*);		// The first parameter is a pointer for the beginning of an array of integers to be set to determine the sizes of columns for output tables based on the second parameter of a pointer to a double array of stats.


/* Function Declarations */
bool loadFile(void);			// Loads the file holding stats
void writeStatsFile(void);		// Populates the stats file with the information from the allStats array
void setColumnSizes();			// Sets the column sizes for output table
void newSession(void);			// Empties the session stats and resets session games played and games won
void (void);				// Empties the game stats
void logRound(int, int, int);	// Logs the last round
void lostLastRound(int, int);	// Edits the last round played to be a loss
string getMapName(int);			// Returns the name of the map for the given map number
string getTeamColor(int);		// Returns the string for the team color for the given number
void outputStats(int);			// Displays stats in a table format
int getSizeOfStat(int);			// Used to determine the amount of characters a stat is 
string setSpace(int);

bool loadFile() {
	string line;			// Used to store the string of a line
	string tempString = "";	// Used to hold the value parsed from the file
	string delimiter = ",";	// Delimiter used to parse lines from the file
	size_t pos = 0;			// Used to parse the lines from the file
	int arrayX = 0;			// Used to step through the array within the while loop for reading lines of the file
	
	cout << "Starting to load file..." << endl;

	ifstream readStatsFile (filePath);
	if (readStatsFile.is_open()) {										// Determines if the file is open
		cout << "File is open..." << endl;
		if (readStatsFile.peek() == ifstream::traits_type::eof()) {		// Determines if the file is an empty file
			cout << "File was empty, creating file..." << endl;			
			readStatsFile.close();										// Closes the file since it is empty, so we can open the file to write to instead and populate the file
			writeStatsFile();											// Populates the file with the allStats array, which would be empty in this case
			cout << "File created." << endl;
			return 1;
		}
		cout << "File was not empty, reading file..." << endl;
		getline(readStatsFile, line);
		pos = line.find(delimiter);
		tempString = line.substr(0, pos);
		gamesPlayed = stoi(tempString);
		line.erase(0, pos + delimiter.length());
		gamesWon = stoi(line);
		while (getline(readStatsFile, line)) {
			if (arrayX < statsX) {
				for (int y = 0; y < statsY; y++) {
					if (y < statsY - 1) {
						pos = line.find(delimiter);
						tempString = line.substr(0, pos);
						allStats[arrayX][y] = stoi(tempString);
						line.erase(0, pos + delimiter.length());
					}
					else {
						allStats[arrayX][y] = stoi(line);
					}
				}
			}
			arrayX++;
		}
		cout << "File read and stats created." << endl;
		readStatsFile.close();
		cout << "File closed." << endl;
		return 1;
	}
	else {
		cout << "Unable to open file." << endl;
		return 0;
	}
}

/* Used to populate the Stats file with the current allStats array */
void writeStatsFile() {
	ofstream statsFile(filePath);
	statsFile << gamesPlayed << "," << gamesWon << endl;
	for (int x = 0; x < statsX; x++) {
		for (int y = 0; y < statsY; y++) {
			if (y < statsY - 1) {
				statsFile << allStats[x][y] << ",";
			}
			else {
				statsFile << allStats[x][y];
				if (x < statsX - 1) {
					statsFile << endl;
				}
			}
		}
	}
	statsFile.close();
}


/* Runs the main program and runs the loop to continue gathering game statistics */
int main() {
	int roundType = 0,											// Game type (the map being played)
		roundMap = 0,											// Game map number (1-25)
		teamColor = 0,											// Color of the team the user is on (used for team games)
		numRound = 1,											// Used for the number of rounds in a game
		roundResult = 0;										// Used to store the result of the round - 1 for win, 0 for loss
		
	int lastRoundMap = 0,
		lastRoundColor = 0;

	bool	exitProgram = false,								// Determines if program should exit or not, true indicating that we should exit
			startSession = false,								// Will keep the session running
			startStats = false,									// Whether user wants to view stats
			startEdits = false,									// Whether user wants to edit stats
			startGame = false,									// Whether user started a game - Set to false when a game ends
			startRound = false;									// Whether user started a round

	if (!loadFile()) {
		return -1;
	}

	setColumnSizes();
	cout << endl;

	do {
		switch (mainMenu()) {
		case 1: cout << "Starting session..." << endl;
			sessionGamesPlayed = 0;
			sessionGamesWon = 0;
			newSession();
			startSession = true;
			break;
		case 2: cout << "Getting stats..." << endl;
			startStats = true;
			break;
		case 3: cout << "Editing stats..." << endl;
			startEdits = true;
			break;
		case 4: cout << "Exiting program..." << endl;
			exitProgram = true;
			break;
		}
		while (startSession) {
			if (startGameMenu() == 1) {
				numRound = 1;
				do {
					roundType = 0; roundMap = 0; teamColor = 0;	startRound = false; roundResult = false; // Reset variables

					roundType = selectGameType(numRound);		// Menu to choose which type of map is being played this round, or to return to the previous menu
					
					switch (roundType) {
					case 1:	roundMap = selectLargeSoloMap();	// Case 1 of Game type is for Large Solo Maps
						if (roundMap != 8) {					// This is the option to return to the previous menu
							startRound = true;
						}
						break;
					case 2:	roundMap = selectSmallSoloMap();	// Case 2 of Game type is for Small Solo Maps
						if (roundMap != 8) {					// This is the option to return to the previous menu
							startRound = true;
						}
						break;
					case 3:	roundMap = selectTeamMap();			// Case 3 of Game type is for Team Maps
						if (roundMap != 8) {					// This is the option to return to the previous menu
							startRound = true;
						}
						break;
					case 4:	
						if (numRound > 1) {
							roundMap = selectFinalsMap();		// Case 4 of Game type is for Finals Maps
							if (roundMap != 5) {				// This is the option to return to the previous menu
								startRound = true;
								numRound = 5;
							}
							break;
						}
					case 5:									// Case 5 is to go back to previous menu 
						if (numRound > 1) {
							startRound = false;
							startGame = false;
							gamesPlayed++;
							sessionGamesPlayed++;
							lostLastRound(lastRoundMap, lastRoundColor);
						}
						continue;		// Breaks the loop to return to the previous menu
						break;
					}

					if (startRound) {						// Will be true if user successfully chose Round Type and Map
						if (roundType == 3) {
							if (roundMap == 7) {
								teamColor = selectTeamColor(4);
							}
							else if (roundMap == 2 || roundMap == 5) {
								teamColor = selectTeamColor(2);
							}
							else {
								teamColor = selectTeamColor(3);
							}
						}
						roundResult = roundResultsMenu(roundType == 4);
						switch (roundResult) {
						case 0:						// Eliminated or Lost
							sessionGamesPlayed++;
							gamesPlayed++;
							startGame = false;
							break;
						case 1:						// Qualified or Won
							if (roundType == 4) {		// Was the final round
								startGame = false;
								gamesPlayed++;
								gamesWon++;
								sessionGamesPlayed++;
								sessionGamesWon++;
							}
							else {						// Not the final round
								startGame = true;
							}
							break;
						case 2:						// Chose the wrong map
							startRound = false;
							continue;
							break;
						case 3:						// Disconnected or left the game (not lost)
							if (numRound > 1) {
								sessionGamesPlayed++;
								gamesPlayed++;
								writeStatsFile();
							}
							startGame = false;
							continue;				// End this iteration
							break;
						}

						numRound++;
						lastRoundMap = (roundType - 1) * 7 + roundMap - 1;
						lastRoundColor = teamColor;
						logRound(lastRoundMap, lastRoundColor, roundResult);
					}

					if (numRound > 5) {
						startGame = false;
					}
				} while (startGame);
			}
			else {
				if (endSessionMenu() == 1) {
					startSession = false;
					outputStats(2);
				}
			}
		}

		while (startStats) {
			switch (viewStatsMenu()) {
			case 1:
				cout << endl;
				cout << "Number of games played: " << gamesPlayed << endl;
				cout << "Number of games won:    " << gamesWon << endl;
				break;
			case 2:
				outputStats(1);
				break;
			case 3:
				outputStats(2);
				break;
			case 4:
				outputStats(3);
				break;
			case 5:
				outputStats(4);
				break;
			case 6:
				startStats = false;
				break;
			}
		}

		while (startEdits) {
			roundType = 0; roundMap = 0; teamColor = 0;
			switch(editStatsMenu()){
			case 1:
				roundType = selectMapTypeEditMenu();
				switch (roundType) {
				case 1: 
					roundMap = selectLargeSoloMap();
					if (roundMap == 8) {
						continue;			// Ends the current iteration of the while loop to return to the previous menu
					}
					break;
				case 2:
					roundMap = selectSmallSoloMap();
					if (roundMap == 8) {
						continue;			// Ends the current iteration of the while loop to return to the previous menu
					}
					break;
				case 3: 
					roundMap = selectTeamMap();
					if (roundMap == 8) {
						continue;			// Ends the current iteration of the while loop to return to the previous menu
					}
					if (roundMap == 7) {
						teamColor = selectTeamColor(4);
					}
					else if (roundMap == 2 || roundMap == 5) {
						teamColor = selectTeamColor(2);
					}
					else {
						teamColor = selectTeamColor(3);
					}
					break;
				case 4:
					roundMap = selectFinalsMap();
					if (roundMap == 5) {
						continue;			// Ends the current iteration of the while loop to return to the previous menu
					}
					break;
				case 5:
					continue;				// Ends the current iteration of the while loop
					break;
				}

				roundMap = (roundType - 1) * 7 + roundMap - 1;

				switch (editTypeMenu()) {
					/* Remove 1 win and add 1 loss to the map */
				case 1:		// Remove 1 win and add 1 loss
					if (allStats[roundMap][0] + allStats[roundMap][1] > 0) {
						if (allStats[roundMap][0] > 0) {
							allStats[roundMap][0]--;
							allStats[roundMap][1]++;
							if (roundType == 4) {
								gamesWon--;
							}

							cout << endl << "Removed 1 win and added 1 loss for the " << getMapName(roundMap) << " map";

							if (teamColor != 0) {
								allStats[roundMap][1 + teamColor * 2]--;
								allStats[roundMap][2 + teamColor * 2]++;
								cout << " for the " << getTeamColor(teamColor) << " team";
							}

							cout << "." << endl;
						}
						else {
							cout << endl << "Can not decrease amount of wins below 0." << endl;
						}
					}
					else {
						cout << endl << "No games were played on " << getMapName(roundMap) << " to be edited." << endl;
					}
					break;

					/* Add 1 win and remove 1 loss to the map */
				case 2:		// Add 1 win and remove 1 loss
					if (allStats[roundMap][0] + allStats[roundMap][1] > 0) {
						if (allStats[roundMap][1] > 0) {
							allStats[roundMap][0]++;
							allStats[roundMap][1]--;
							if (roundType == 4) {
								gamesWon++;
							}

							cout << endl << "Added 1 win and removed 1 loss for the " << getMapName(roundMap) << " map";

							if (teamColor != 0) {
								allStats[roundMap][1 + teamColor * 2]++;
								allStats[roundMap][2 + teamColor * 2]--;
								cout << " for the " << getTeamColor(teamColor) << " team";
							}

							cout << "." << endl;
						}
						else {
							cout << endl << "Can not decrease amount of losses below 0." << endl;
						}
					}
					else {
						cout << endl << "No games were played on " << getMapName(roundMap) << " to be edited." << endl;
					}
					break;

					/* Add 1 win to the map */
				case 3:		// Add 1 win
					allStats[roundMap][0]++;

					cout << endl << "Added 1 win for the " << getMapName(roundMap) << " map";

					if (teamColor != 0) {
						allStats[roundMap][1 + teamColor * 2]++;
						cout << " for the " << getTeamColor(teamColor) << " team";
					}

					cout << "." << endl;
					break;

					/* Remove 1 win from the map */
				case 4:		// Remove 1 win
					if (allStats[roundMap][0] > 0) {
						allStats[roundMap][0]--;

						cout << endl << "Added 1 win for the " << getMapName(roundMap) << " map";

						if (teamColor != 0) {
							allStats[roundMap][1 + teamColor * 2]--;
							cout << " for the " << getTeamColor(teamColor) << " team";
						}

						cout << "." << endl;
					} 
					else {
						cout << endl << "Can not decrease amount of wins below 0." << endl;
					}
					break;

					/* Add 1 loss to the map */
				case 5:		// Add 1 loss
					allStats[roundMap][1]++;

					cout << endl << "Added 1 loss for the " << getMapName(roundMap) << " map";

					if (teamColor != 0) {
						allStats[roundMap][2 + teamColor * 2]++;
						cout << " for the " << getTeamColor(teamColor) << " team";
					}

					cout << "." << endl;
					break;

					/* Remove 1 loss from the map */
				case 6:		// Remove 1 loss
					if (allStats[roundMap][1] > 0) {
						allStats[roundMap][1]--;

						cout << endl << "Removed 1 loss for the " << getMapName(roundMap) << " map";

						if (teamColor != 0) {
							allStats[roundMap][2 + teamColor * 2]--;
							cout << " for the " << getTeamColor(teamColor) << " team";
						}

						cout << "." << endl;
					}
					else {
						cout << endl << "Can not decrease amount of losses below 0." << endl;
					}
					break;

					/* Cancels the edit option */
				case 7:
					continue;				// Ends the current iteration of the while loop
					break;
				}
				writeStatsFile();
				break;

			case 2:
				switch (editGamesMenu()) {
				case 1:			// Decrease number of games played
					if (gamesPlayed > 0) {
						gamesPlayed--;
						cout << endl << "Decreased the number of games played." << endl;
					}
					else {
						cout << endl << "Can not decrease the number of games played below 0." << endl;
					}
					break;

				case 2:			// Increase number of games played
					gamesPlayed++;
					cout << endl << "Increased the number of games played." << endl;
					break;

				case 3:			// Decrease number of games won
					if (gamesWon > 0) {
						gamesWon--;
						cout << endl << "Decreased the number of games won." << endl;
					}
					else {
						cout << endl << "Can not decrease the number of games won below 0." << endl;
					}
					break;

				case 4:			// Increase number of games won
					gamesWon++;
					cout << endl << "Increased the number of games won." << endl;
					break;

				case 5:
					continue;				// Ends the current iteration of the while loop
					break;
				}
				writeStatsFile();
				break;

			case 3:
				startEdits = false;
				break;
			}
		}

	} while (!exitProgram);

	return 0;
}

/* Sets the column sizes used to create the table to output stats */
void setColumnSizes() {
	string tempString = "";
	int statsColumnSize = 1;
	int sessionColumnSize = 1;
	int tempSize = 0;
	int tempInt = 0;

	for (int i = 0; i < statsX; i++) {
		if (tempString.length() < getMapName(i).length()) {
			tempString = getMapName(i);
		}
	}
	statsColumnSizes[0] = tempString.length() + 2;
	sessionColumnSizes[0] = statsColumnSizes[0];

	for (int y = 0; y < statsY; y++) {
		statsColumnSize = 1;
		sessionColumnSize = 1;

		for (int x = 0; x < statsX; x++) {
			tempSize = getSizeOfStat(allStats[x][y]);
			if (tempSize > statsColumnSize) {
				statsColumnSize = tempSize;
			}

			tempSize = getSizeOfStat(sessionStats[x][y]);
			if (tempSize > statsColumnSize) {
				sessionColumnSize = tempSize;
			}
		}
		if (y > 1) {
			statsColumnSizes[y+2] = statsColumnSize + 2;
			sessionColumnSizes[y+2] = sessionColumnSize + 2;
		}
		else {
			statsColumnSizes[y+1] = statsColumnSize + 2;
			sessionColumnSizes[y+1] = sessionColumnSize + 2;
		}
	}
	statsColumnSizes[3] = 9;		// Index for Win Rate ( xx.xx% ) 
	sessionColumnSizes[3] = 9;		// Index for Win Rate
}

void newSession() {
	sessionGamesPlayed = 0;
	sessionGamesWon = 0;
	fill(&sessionStats[0][0], &sessionStats[0][0] + sizeof(sessionStats) / sizeof(sessionStats[0][0]), 0);
}


void logRound(int roundMap, int roundTeam, int roundResult) {
	sessionStats[roundMap][1 - roundResult]++;
	allStats[roundMap][1 - roundResult]++;
	if (roundTeam != 0) {
		sessionStats[roundMap][1 + roundTeam * 2 - roundResult]++;
		allStats[roundMap][1 + roundTeam * 2 - roundResult]++;
	}
	writeStatsFile();

	cout << endl;
	if (roundResult && roundMap > 21) {
		cout << "Winner on " << getMapName(roundMap);
	}
	else if (roundResult) {
		cout << "Qualified on " << getMapName(roundMap);
	}
	else {
		cout << "Eliminated on " << getMapName(roundMap);
	}

	if (roundTeam != 0) {
		cout << " on " << getTeamColor(roundTeam) << " team";
	}

	cout << "!" << endl;
	
	if (roundMap > 21) {
		cout << "You have won ";
	}
	else {
		cout << "You have qualified ";
	}
	cout << allStats[roundMap][0] << " times out of " << allStats[roundMap][0] + allStats[roundMap][1] << " attempts on this map." << endl;
}

void lostLastRound(int roundMap, int roundTeam) {
	sessionStats[roundMap][0]--;	// Subtract from wins
	sessionStats[roundMap][1]++;	// Add to losses
	allStats[roundMap][0]--;		// Subtract from wins 
	allStats[roundMap][1]++;		// Add to losses
	if (roundTeam != 0) {
		sessionStats[roundMap][1 + roundTeam * 2]--;	// Subtract from team wins
		sessionStats[roundMap][2 + roundTeam * 2]++;	// Add to team losses
		allStats[roundMap][1 + roundTeam * 2]--;	// Subtract from team wins
		allStats[roundMap][2 + roundTeam * 2]++;	// Add to team losses
	}
	writeStatsFile(); 

	cout << "Adjusted previous round to be a loss." << endl;
}

string getMapName(int roundMap) {
	string mapName = "";
	switch (roundMap) {
	case 0: mapName = "Dizzy Heights";
		break;
	case 1: mapName = "Door Dash";
		break;
	case 2: mapName = "Gate Crash";
		break;
	case 3: mapName = "Hit Parade";
		break;
	case 4: mapName = "See Saw";
		break;
	case 5: mapName = "Slime Climb";
		break;
	case 6: mapName = "The Whirlygig";
		break;
	case 7: mapName = "Block Party";
		break;
	case 8: mapName = "Fruit Chute";
		break;
	case 9: mapName = "Jump Club";
		break;
	case 10: mapName = "Perfect Match";
		break;
	case 11: mapName = "Roll Out";
		break;
	case 12: mapName = "Tail Tag";
		break;
	case 13: mapName = "Tip Toe";
		break;
	case 14: mapName = "Egg Scramble";
		break;
	case 15: mapName = "Fall Ball";
		break;
	case 16: mapName = "Hoarders";
		break;
	case 17: mapName = "Hoopsie Daisy";
		break;
	case 18: mapName = "Jinxed";
		break;
	case 19: mapName = "Rock 'n' Roll";
		break;
	case 20: mapName = "Team Tail Tag";
		break;
	case 21: mapName = "Fall Mountain";
		break;
	case 22: mapName = "Hex-a-gone";
		break;
	case 23: mapName = "Jump Showdown";
		break;
	case 24: mapName = "Royal Fumble";
		break;


	}
	return mapName;
}

string getTeamColor(int roundColor) {
	string teamColor = "";
	switch (roundColor) {
	case 1: teamColor = "Red";
		break;
	case 2: teamColor = "Green";
		break;
	case 3: teamColor = "Blue";
		break;
	case 4: teamColor = "Yellow";
		break;
	}
	return teamColor;
}

/* Used to print stats in a table format */
void outputStats(int option) {				// Option 1: All Stats, Option 2: Session Stats, Option 3: All Team Game Stats, Option 4: Session Team Game Stats
	int start = 0,					// Used to determine where to start in the for loop
		end = 0,					// Used to determine where to end the for loop
		numSoloGames = 14,			// The amount of solo maps there are
		numTeamGames = 7,			// The amount of team maps there are
		tableWidth = 0,				// Used to determine the width of the table
		tableColumns = 0,			// Number of columns that will be in the table
		tempStat = 0,				// Used to store a stat to be displayed
		columnSize = 0,				// Used to store the size of the current column
		statSize = 0,				// Used to store the size of the current stat
		indent = 0;
	bool emptyTable = true;			// Used to determine if the table being output would be empty - used for session stats 
	float f = 100.00;				// Used for setting percentages for win rates
	int columnSizes[numColumns];	// Used for column sizes populated based on which 
	string columnTitles[numColumns] = {
		"Map",
		"W",
		"L",
		"W%",
		"RW",
		"RL",
		"GW",
		"GL",
		"BW",
		"BL",
		"YW",
		"YL",
	};
	string	tableDivider = "",		// Divider to be at the top and bottom of the table or seperate pieces of the table
			line = "";				


	if (option == 3 || option == 4) {
		start = numSoloGames;		// Team Games start at the 
		end = numSoloGames + numTeamGames;
		tableColumns = 12;
	} 
	else {
		tableColumns = 4;
		end = statsX;
	}

	setColumnSizes();

	tableWidth = tableColumns + 1;
	for (int i = 0; i < tableColumns; i++) {
		if (option == 1 || option == 3) {
			tableWidth += statsColumnSizes[i];
			columnSizes[i] = statsColumnSizes[i];
		}
		else if (option == 2 || option == 4) {
			tableWidth += sessionColumnSizes[i];
			columnSizes[i] = sessionColumnSizes[i];
		}
	}

	for (int i = 0; i < tableWidth; i++) {
		tableDivider += "-";
	}
	
	cout << endl << tableDivider << endl;
	cout << "|";
	for (int i = 0; i < tableColumns; i++) {
		indent = (columnSizes[i] - columnTitles[i].length()) / 2;
		cout << setSpace(indent) << columnTitles[i] << setSpace((indent + ((columnSizes[i] - columnTitles[i].length()) % 2))) << "|";
	}
	cout << endl << tableDivider << endl;

	cout << std::setprecision(2) << std::fixed;
	for (int x = start; x < end; x++) {
		if ((option == 2 || option == 4) && (sessionStats[x][0] + sessionStats[x][1]) == 0) {
			continue;
		}

		emptyTable = false;

		cout << "|" << setSpace(1) << getMapName(x) << setSpace(columnSizes[0] - getMapName(x).length() - 1) << "|";
		for (int y = 0; y < 2; y++) {
			tempStat = 0;
			if (option == 1 || option == 3) {
				tempStat = allStats[x][y];
			}
			else if (option == 2 || option == 4) {
				tempStat = sessionStats[x][y];
			}
			statSize = getSizeOfStat(tempStat);
			cout << setSpace(columnSizes[y + 1] - statSize - 1) << tempStat << setSpace(1) << "|";
		}

		f = 100.00;
		if (option == 1 || option == 3) {
			if ((allStats[x][0] + allStats[x][1]) != 0) {
				f = (float(allStats[x][0]) / (float(allStats[x][0]) + float(allStats[x][1]))) * f;
			} 
			else {
				f = 0.0;
			}
		}
		else if (option == 2 || option == 4) {
			if ((sessionStats[x][0] + sessionStats[x][1]) != 0) {
				f = (float(sessionStats[x][0]) / (float(sessionStats[x][0]) + float(sessionStats[x][1]))) * f;
			}
			else {
				f = 0.0;
			}
		}
		statSize = getSizeOfStat(int(f));
		cout << setSpace(columnSizes[3] - statSize - 5) << f << "%" << setSpace(1) << "|";

		if (option == 3 || option == 4) {
			for (int y = 2; y < tableColumns - 2; y++) {
				tempStat = 0;
				if (option == 1 || option == 3) {
					tempStat = allStats[x][y];
				}
				else if (option == 2 || option == 4) {
					tempStat = sessionStats[x][y];
				}
				statSize = getSizeOfStat(tempStat);
				cout << setSpace(columnSizes[y + 2] - statSize - 1) << tempStat << setSpace(1) << "|";
			}
		}
		cout << endl;
	}
	if (emptyTable) {
		if (option == 2) {
			cout << "| No stats for the session." << setSpace(tableWidth - 28) << "|" << endl;
		}
		else if (option == 4) {
			cout << "| No team game stats for the session." << setSpace(tableWidth - 38) << "|" << endl;
		}
	}

	cout << tableDivider << endl << endl;
}

/* Used to determine how many characters a stat will take to display */
int getSizeOfStat(int x) {
	if (x > 999) {
		return 4;
	}
	else if (x > 99) {
		return 3;
	}
	else if (x > 9) {
		return 2;
	}
	else {
		return 1;
	}
}

string setSpace(int x) {
	string tempString = "";
	for (int i = 0; i < x; i++) {
		tempString += " ";
	}
	return tempString;
}