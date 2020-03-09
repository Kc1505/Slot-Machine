#include <iostream>
#include <windows.h>
#include <time.h>
#include <string>
#include <vector>

using namespace std;

class Position {
public:
	int x;
	int y;
};

class Game {
public:
	bool running = true;
	bool quitting = false;
	bool displayChanged = false;

	int state = 1;

	int currentMoney = 1000;
	int betAmount = 100;

	time_t startTime = 0;
	time_t askQuitTime = 0;

	string title = "Welcome to Slots!";
	
	Position position{0,0};

	class MainTest {
	public:
		vector<string> lines;
		Position position{ 0,1 };

	}mainText;

	class RightText {
	public:
		vector<string> lines;
		Position position{45,1};

	}rightText;

}game;

////////Need to actually make the game now!


void Start();
void Update();
int IntInput(int min, int max);
void Print(Position pos, string str, int effect);
void Draw(Position pos, string str, int effect);
void GotoXY(Position pos, int multi);
void Input();
void UpdateScreen();
void ClearRightText();
void ClearTextCenter();
void DisplayInfortmation();
void DisplayControls();
void DisplayMenu();
void DisplaySlot();
void AskQuit();
void EnterBet();
void writeMachine();
void WatchSlotMachine();


int main() {
	game.startTime = time(NULL);

	Start();

	while (game.running) {
		Update();
		Sleep(100);
	}

	return 0;
}

void Start() {
	Position tempPos{ (((game.rightText.position.x) - (game.position.x) - static_cast<int>(game.title.length()))/2),(game.position.y)};
	Print(tempPos, game.title, 14);

	tempPos.x = game.rightText.position.x;
	tempPos.y = game.rightText.position.y - 1;
	Print(tempPos, "'C' to open Controls." , 10);
}

void Update() {
	if (game.quitting && time(NULL) - game.askQuitTime  >= 5) {
		game.quitting = false;
		DisplayControls();
	}

	Input();

	UpdateScreen();
}

void Input() {
	switch (game.state) {
	case 1:
		DisplayMenu();
		
		if (GetKeyState('1') & 0x8000) { game.state = 2; game.displayChanged = false; };
		break;

	case 2:
		DisplaySlot();

		if (GetKeyState('E') & 0x8000) { game.state = 3; game.displayChanged = false; };
		break;

	case 3:
		EnterBet();

		if (GetKeyState('P') & 0x8000) { game.state = 4; game.displayChanged = false; };

		if (GetKeyState(VK_UP) & 0x8000) {
			int fail = false;
			if (game.betAmount + 100 <= game.currentMoney) {
				game.betAmount += 100;
			}
			else {
				for (int i = 0; i < game.mainText.lines.size(); i++) {
					if (game.mainText.lines[i] != "You dont have enough money to go higher!") {
						fail = true;
					}

				}
			}
			game.displayChanged = false;
			EnterBet();
			if (fail) game.mainText.lines.push_back("You dont have enough money to go higher!");;
			
		};
		if (GetKeyState(VK_DOWN) & 0x8000) {
			int fail = false;
			if (game.betAmount - 100 >= 1) {
				game.betAmount -= 100;
			}
			else {
				for (int i = 0; i < game.mainText.lines.size(); i++) {
					if (game.mainText.lines[i] != "Cant go lower!") {
						fail = true;
					}

				}
			}
			game.displayChanged = false;
			EnterBet();
			if (fail) game.mainText.lines.push_back("Cant go lower!");;

		};
		if (GetKeyState(VK_RIGHT) & 0x8000) {
			int fail = false;
			if (game.betAmount + 1 <= game.currentMoney) {
				game.betAmount += 1;
			}
			else {
				for (int i = 0; i < game.mainText.lines.size(); i++) {
					if (game.mainText.lines[i] != "You dont have enough money to go higher!") {
						fail = true;
					}

				}
			}
			game.displayChanged = false;
			EnterBet();
			if (fail) game.mainText.lines.push_back("You dont have enough money to go higher!");;

		};
		if (GetKeyState(VK_LEFT) & 0x8000) {
			int fail = false;
			if (game.betAmount - 1 >= 1) {
				game.betAmount -= 1;
			}
			else {
				for (int i = 0; i < game.mainText.lines.size(); i++) {
					if (game.mainText.lines[i] != "Cant go lower!") {
						fail = true;
					}

				}
			}
			game.displayChanged = false;
			EnterBet();
			if (fail) game.mainText.lines.push_back("Cant go lower!");;

		};
		break;

	case 4:
		WatchSlotMachine();
		break;
	}

	if (GetKeyState('M') & 0x8000) { game.state = 1; game.displayChanged = false; }

	if (GetKeyState('Q') & 0x8000) {
		if (game.quitting) {
			game.running = false;
		}
		else {
			AskQuit();
		}
	}
	if (GetKeyState('I') & 0x8000) {
		DisplayInfortmation();
	}
	if (GetKeyState('C') & 0x8000) {
		DisplayControls();
	}
	

}

void DisplayMenu() {
	if (game.displayChanged) return;
	ClearTextCenter();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("What would you like to do?");
	game.mainText.lines.push_back("1) Play the Game!");
	game.mainText.lines.push_back("2) View my bank acount!");
	game.mainText.lines.push_back("3) Quit!");
	game.displayChanged = true;
}

void DisplaySlot() {
	if (game.displayChanged) return;
	ClearTextCenter();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("If you dont know what to do, press 'I'!");
	game.mainText.lines.push_back("");
	writeMachine();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Tip: Press 'E' to enter the bet amount");
	game.mainText.lines.push_back("Tip: Press 'B' to bet the selected amount");


	game.displayChanged = true;
}

void EnterBet() {
	if (game.displayChanged) return;
	ClearTextCenter();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("If you dont know what to do, press 'I'!");
	game.mainText.lines.push_back("");
	writeMachine();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Money: " + to_string(game.currentMoney));
	game.mainText.lines.push_back("Bet Amount: " + to_string(game.betAmount));
	DisplayControls();
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("Press 'up arrow key' to increase the bet amount by 100");
	game.rightText.lines.push_back("Press 'down arrow key' to decrease the bet amount 100");
	game.rightText.lines.push_back("Press 'right arrow key' to increase the bet amount 1");
	game.rightText.lines.push_back("Press 'left arrow key' to decrease the bet amount 1");
	game.rightText.lines.push_back("Press 'P' to place the bet!");

	game.displayChanged = true;
}

void WatchSlotMachine() {
	if (game.displayChanged) return;
	ClearTextCenter();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("If you dont know what to do, press 'I'!");
	game.mainText.lines.push_back("");
	writeMachine();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Wooo! Lets get that jackpot!");

	game.displayChanged = true;
}

void writeMachine() {
	game.mainText.lines.push_back("          .-------.");
	game.mainText.lines.push_back("       oO{-JACKPOT-}Oo");
	game.mainText.lines.push_back("       .=============. __");
	game.mainText.lines.push_back("       |  1   2   3  |(  )");
	game.mainText.lines.push_back("       |-[7] [7] [7]-| ||");
	game.mainText.lines.push_back("       |  4   5   6  | ||");
	game.mainText.lines.push_back("       |=============|_||");
	game.mainText.lines.push_back("       | 222 ::::::: |--'");
	game.mainText.lines.push_back("       | 333 ::::::: |");
	game.mainText.lines.push_back("       | $$$ ::::::: |");
	game.mainText.lines.push_back("       |             |");
	game.mainText.lines.push_back("       |      __ === |");
	game.mainText.lines.push_back("       |_____/__\\____|");
	game.mainText.lines.push_back("      /###############\\");
	game.mainText.lines.push_back("     /#################\\");
	game.mainText.lines.push_back("    |===================|");
}

void DisplayInfortmation() {
	ClearRightText();
	game.rightText.lines.push_back("Infortmation:");
	game.rightText.lines.push_back("This is the game of Slots!");
	game.rightText.lines.push_back("You can bet your fake money, to win or lose more fake money!");
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("-The goal of the Game is to gain as much money as you can.");
	game.rightText.lines.push_back("-To do that, bet your current money at the slot machine.");
	game.rightText.lines.push_back("-If you lose all of your money, game over.");
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("Good luck and have fun!");
}

void DisplayControls() {
	ClearRightText();
	game.rightText.lines.push_back("Controls:");
	game.rightText.lines.push_back("Pressing 'C' will bring up this menu.");
	game.rightText.lines.push_back("Pressing 'I' will bring up the game infortmation.");
	game.rightText.lines.push_back("Pressing 'M' will go back to the Menu.");
	game.rightText.lines.push_back("Pressing 'Q' will quit the game.");

}

void AskQuit() {
	game.askQuitTime = time(NULL);
	game.quitting = true;
	ClearRightText();
	game.rightText.lines.push_back("Are you sure you want to quit?");
	game.rightText.lines.push_back("Press 'Q' again to quit...");
}

void ClearRightText() {
	int i = 0;
	for (int y = game.rightText.position.y; y < (static_cast<int>(game.rightText.lines.size()) + game.rightText.position.y); y++) {
		for (int x = game.rightText.position.x; x < (game.rightText.position.x + game.rightText.lines[i].length()); x++) {
			Position tempPos{ x, y };
			Print(tempPos, " ", 0);
		}
		i++;
	}
	for (string str : game.rightText.lines) {
		game.rightText.lines.pop_back();
	}
}

void ClearTextCenter() {
	int i = 0;
	for (int y = game.mainText.position.y; y < (game.mainText.lines.size() + game.mainText.position.y); y++) {
		for (int x = game.mainText.position.x; x < (game.mainText.position.x + game.mainText.lines[i].length()); x++) {
			Position tempPos{ x, y };
			Print(tempPos, " ", 0);
		}
		i++;
	}
	for (string str : game.mainText.lines) {
		game.mainText.lines.pop_back();
	}
}

void UpdateScreen() {
	for (int i = 0; i < game.rightText.lines.size(); i++) {
		Position tempPos{ game.rightText.position.x, game.rightText.position.y + i };
		Print(tempPos, game.rightText.lines[i], 10);
	}

	for (int i = 0; i < game.mainText.lines.size(); i++) {
		Position tempPos{ game.mainText.position.x, game.mainText.position.y + i };
		Print(tempPos, game.mainText.lines[i], 14);
	}
	
}

int IntInput(int min, int max) {
	int output;
	cin >> output;
	while (!cin.good() || (output < min || output > max))
	{
		cin.clear();
		cin.ignore(INT_MAX, '\n');
		cout << "\n Please only enter integers greater than " << min - 1 << ", and less than " << max + 1 << ":";
		cin >> output;
	}
	return output;
}

void Print(Position pos,string str, int effect) {
	GotoXY(pos, 1);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), effect);
	cout << str;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void Draw(Position pos, string str, int effect) {
	GotoXY(pos, 2);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), effect);
	cout << str << str;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void GotoXY(Position pos, int multi) {
	COORD point;
	point.X = (pos.x + game.position.x) * multi;
	point.Y = pos.y + game.position.y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), point);
}