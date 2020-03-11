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

class AfterPrint {
public:
	Position position;
	string string;
	int effect;
};

class Game {
public:
	bool running = true;
	bool quitting = false;
	bool displayChanged = false;

	int state = 1;

	int currentMoney = 1000;
	int betAmount = 1;

	int slotNumbers[3][3] = {
		{1,2,3},
		{7,7,7},
		{4,5,6} 
	};

	time_t startTime = 0;
	time_t askQuitTime = 0;

	string title = "Welcome to Slots!";
	
	Position position{0,0};

	class MainText {
	public:
		vector<string> lines;
		vector<string> bankStatements{"Recieved: $1000    From: Mom"};
		Position position{ 0,1 };

	}mainText;

	class RightText {
	public:
		vector<string> lines;
		Position position{45,1};

	}rightText;

	vector<AfterPrint> afterPrint;

}game;


void Start();
void Update();
int IntInput(int min, int max);
void Print(Position pos, string str, int effect);
void Draw(Position pos, string str, int effect);
void GotoXY(Position pos, int multi);
void Input();
void UpdateScreen();
void ClearRightText();
void ClearCenterText();
void DisplayInformation();
void DisplayControls();
void DisplayMenu();
void DisplaySlot();
void AskQuit();
void EnterBet();
void writeMachine();
void WatchSlotMachine();
void DisplayCredits();
void DisplayWinnings();
void DisplayAccount();


int main() {
	game.startTime = time(NULL);
	srand(game.startTime);

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
		if (GetKeyState('2') & 0x8000) { game.state = 6; game.displayChanged = false; };
		if (GetKeyState('3') & 0x8000) { game.running = false; };
		if (GetKeyState('4') & 0x8000) { game.state = 10; game.displayChanged = false; };
		if (GetKeyState('I') & 0x8000) {
			DisplayInformation();
		}
		if (GetKeyState('C') & 0x8000) {
			DisplayControls();
		}

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
				game.betAmount = game.currentMoney;
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
				if (game.currentMoney >= 1) {
					game.betAmount = 1;
				}
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

	case 5:
		DisplayWinnings();
		if (GetKeyState('R') & 0x8000) { game.state = 3; game.displayChanged = false; }
		else if (GetKeyState('S') & 0x8000) { game.state = 2; game.displayChanged = false; }
		break;

	case 6:
		DisplayAccount();

	case 10:
		DisplayCredits();
		break;
	}

	if (GetKeyState('M') & 0x8000 &&  (game.state != 4)) { game.state = 1; game.displayChanged = false; }

	if (GetKeyState('Q') & 0x8000) {
		if (game.quitting) {
			game.running = false;
		}
		else {
			AskQuit();
		}
	}
	

}

void DisplayMenu() {
	if (game.displayChanged) return;
	game.afterPrint.clear();
	ClearCenterText();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("What would you like to do?");
	game.mainText.lines.push_back("1) Play the Game!");
	game.mainText.lines.push_back("2) View my bank acount!");
	game.mainText.lines.push_back("3) Quit :(");
	game.mainText.lines.push_back("4) View the Credits!");

	DisplayInformation();
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("Pressing '1','2','3', or '4' will do something");
	game.rightText.lines.push_back("(As indicated on the left)");

	game.displayChanged = true;
}

void DisplayAccount() {
	if (game.displayChanged) return;
	ClearCenterText();


	for (string string : game.mainText.bankStatements) {
		game.mainText.lines.push_back(string);
	}

	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Current Balance: $" + to_string(game.currentMoney));
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Press 'M' to go back to the Menu");

	DisplayControls();

	game.displayChanged = true;
}

void DisplayCredits() {
	if (game.displayChanged) return;
	ClearCenterText();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("This game was made with love my me:");
	game.mainText.lines.push_back("KeaneCarotenuto@gmail.com");
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("I used ascii art from:");
	game.mainText.lines.push_back("www.oocities.org/spunk1111/electro.htm");
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("It was requested to include the link below:");
	game.mainText.lines.push_back("www.ascii-art.com");
	game.mainText.lines.push_back("Unfortunately this link does not work.");
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Press 'M' to go back to the Menu.");
	game.displayChanged = true;
}

void DisplaySlot() {
	if (game.displayChanged) return;
	ClearCenterText();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("If you dont know what to do, press 'I'!");
	game.mainText.lines.push_back("");
	writeMachine();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Tip: Press 'E' to enter the bet amount");

	DisplayControls();
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("Press 'E' To Start Betting");



	game.displayChanged = true;
}

void EnterBet() {
	if (game.displayChanged) return;
	ClearCenterText();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("If you dont know what to do, press 'I'!");
	game.mainText.lines.push_back("");
	writeMachine();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Bet Amount: " + to_string(game.betAmount));
	game.mainText.lines.push_back("Balance: $" + to_string(game.currentMoney));
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Press 'P' to place the bet!");
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Press 'M' to go back to the Menu");
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
	game.currentMoney -= game.betAmount;
	game.mainText.bankStatements.push_back("Sent    : $" + to_string(game.betAmount) + "    To: Casino");

	ClearCenterText();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("If you dont know what to do, press 'I'!");
	game.mainText.lines.push_back("");
	writeMachine();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Wooo! Lets get that jackpot!");
	
	DisplayControls();
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("All you can do is watch!");

	game.afterPrint.clear();

	for (int i = 0; i < rand() % 15 + 5; i++) {
		for (int x = 0; x < 3; x++) {
			game.slotNumbers[2][x] = game.slotNumbers[1][x];
		}
		for (int x = 0; x < 3; x++) {
			game.slotNumbers[1][x] = game.slotNumbers[0][x];
		}
		for (int x = 0; x < 3; x++) {
			game.slotNumbers[0][x] = rand() % 9 + 1;
		}

		game.afterPrint.push_back({ {10,7}, to_string(game.slotNumbers[0][0]), 14 });
		game.afterPrint.push_back({ {14,7}, to_string(game.slotNumbers[0][1]), 14 });
		game.afterPrint.push_back({ {18,7}, to_string(game.slotNumbers[0][2]), 14 });

		game.afterPrint.push_back({ {10,8}, to_string(game.slotNumbers[1][0]), 14 });
		game.afterPrint.push_back({ {14,8}, to_string(game.slotNumbers[1][1]), 14 });
		game.afterPrint.push_back({ {18,8}, to_string(game.slotNumbers[1][2]), 14 });

		game.afterPrint.push_back({ {10,9}, to_string(game.slotNumbers[2][0]), 14 });
		game.afterPrint.push_back({ {14,9}, to_string(game.slotNumbers[2][1]), 14 });
		game.afterPrint.push_back({ {18,9}, to_string(game.slotNumbers[2][2]), 14 });
		
		UpdateScreen();
		Sleep(rand() % 600 + 100);
	}

	ClearCenterText();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("If you dont know what to do, press 'I'!");
	game.mainText.lines.push_back("");
	writeMachine();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("Spin Complete!");
	UpdateScreen();
	Sleep(1000);

	game.state = 5;
	game.displayChanged = false;
}

void DisplayWinnings() {
	if (game.displayChanged) return;
	ClearCenterText();
	
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("If you dont know what to do, press 'I'!");
	game.mainText.lines.push_back("");
	writeMachine();
	game.mainText.lines.push_back("");

	DisplayControls();
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("All you can do is watch!");

	UpdateScreen();

	DisplayControls();
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("Press 'R' to Retry");
	game.rightText.lines.push_back("Press 'S' to Stop");


	game.afterPrint.clear();

	if (game.slotNumbers[1][0] == 7 &&
		game.slotNumbers[1][1] == 7 &&
		game.slotNumbers[1][2] == 7) {
		
		game.afterPrint.push_back({ {10,13}, "JACKPOT!", 10 });
		game.afterPrint.push_back({ {10,12}, "TRIPLES", 12 });
		game.afterPrint.push_back({ {10,11}, "DOUBLES", 12 });

		game.currentMoney += game.betAmount * 5;

		game.mainText.lines.push_back("You Just Gained $" + to_string(game.betAmount * 5));
		game.mainText.bankStatements.push_back("Recieved: $" + to_string(game.betAmount * 5) + "    From: Casino");
	}
	else if (game.slotNumbers[1][0] == game.slotNumbers[1][1] &&
		game.slotNumbers[1][0] == game.slotNumbers[1][2] &&
		game.slotNumbers[1][1] == game.slotNumbers[1][2]) {

		game.afterPrint.push_back({ {10,13}, "JACKPOT!", 12 });
		game.afterPrint.push_back({ {10,12}, "TRIPLES", 10 });
		game.afterPrint.push_back({ {10,11}, "DOUBLES", 12 });

		game.currentMoney += game.betAmount * 3;

		game.mainText.lines.push_back("You Just Won $" + to_string(game.betAmount * 3));
		game.mainText.bankStatements.push_back("Recieved: $" + to_string(game.betAmount * 3) + "    From: Casino");
	}
	else if (game.slotNumbers[1][0] == game.slotNumbers[1][1] ||
		game.slotNumbers[1][0] == game.slotNumbers[1][2] ||
		game.slotNumbers[1][1] == game.slotNumbers[1][2]) {

		game.afterPrint.push_back({ {10,13}, "JACKPOT!", 12 });
		game.afterPrint.push_back({ {10,12}, "TRIPLES", 12 });
		game.afterPrint.push_back({ {10,11}, "DOUBLES", 10 });

		game.currentMoney += game.betAmount * 2;

		game.mainText.lines.push_back("You Just Won $" + to_string(game.betAmount * 2));
		game.mainText.bankStatements.push_back("Recieved: $" + to_string(game.betAmount * 2) + "    From: Casino");
	}
	else {
		game.afterPrint.push_back({ {10,13}, "JACKPOT!", 12 });
		game.afterPrint.push_back({ {10,12}, "TRIPLES", 12 });
		game.afterPrint.push_back({ {10,11}, "DOUBLES", 12 });

		game.mainText.lines.push_back("You Just Lost $" + to_string(game.betAmount));
	}
	game.mainText.lines.push_back("Balance: $" + to_string(game.currentMoney));


	game.betAmount = 1;


	game.displayChanged = true;

	if (game.currentMoney <= 0) {
		game.mainText.lines.push_back("");
		game.mainText.lines.push_back("OOPS! Please Wait");
		DisplayControls();
		game.rightText.lines.push_back("");
		game.rightText.lines.push_back("You Lost, wait to be kicked out...");
		
		UpdateScreen();

		Sleep(5000);

		ClearCenterText();
		game.afterPrint.clear();

		game.mainText.lines.push_back("");
		game.mainText.lines.push_back("You Just lost the game...");
		game.mainText.lines.push_back("You Ran out of money");
		game.mainText.lines.push_back("The one thing you should NOT do");
		game.mainText.lines.push_back("Sorry, gotta kick you out of the Casino");
		game.mainText.lines.push_back("Goodbye! (in 5 seconds)");
		UpdateScreen();

		Sleep(5000);

		game.currentMoney = 1000;

		game.state = 1;
		game.displayChanged = false;
	}
	else {
		game.mainText.lines.push_back("");
		game.mainText.lines.push_back("Press 'R' to Retry");
		game.mainText.lines.push_back("Press 'S' to Stop betting");
	}
	
}

void writeMachine() {
	game.mainText.lines.push_back("          .-------.");
	game.mainText.lines.push_back("       oO{- SLOTS -}Oo");
	game.mainText.lines.push_back("       .=============. __");
	game.mainText.lines.push_back("       |  " + to_string(game.slotNumbers[0][0]) + "   " + to_string(game.slotNumbers[0][1]) + "   " + to_string(game.slotNumbers[0][2]) + "  |(  )");
	game.mainText.lines.push_back("       |-["+to_string(game.slotNumbers[1][0])+"] [" + to_string(game.slotNumbers[1][1]) + "] [" + to_string(game.slotNumbers[1][2]) + "]-| ||");
	game.mainText.lines.push_back("       |  " + to_string(game.slotNumbers[2][0]) + "   " + to_string(game.slotNumbers[2][1]) + "   " + to_string(game.slotNumbers[2][2]) + "  | ||");
	game.mainText.lines.push_back("       |=============|_||");
	game.mainText.lines.push_back("       |  DOUBLES    |--'");
	game.mainText.lines.push_back("       |  TRIPLES    |");
	game.mainText.lines.push_back("       |  JACKPOT!   |");
	game.mainText.lines.push_back("       |             |");
	game.mainText.lines.push_back("       |      __ === |");
	game.mainText.lines.push_back("       |_____/__\\____|");
	game.mainText.lines.push_back("      /###############\\");
	game.mainText.lines.push_back("     /#################\\");
	game.mainText.lines.push_back("    |JGS================|");
}

void DisplayInformation() {
	ClearRightText();
	game.rightText.lines.push_back("Information:");
	game.rightText.lines.push_back("This is the game of Slots!");
	game.rightText.lines.push_back("You can bet your fake money, to win or lose more fake money!");
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("-The goal of the Game is to gain as much money as you can.");
	game.rightText.lines.push_back("-To do that, bet your current money at the slot machine.");
	game.rightText.lines.push_back("-You start with $1000, given to you by your Mom");
	game.rightText.lines.push_back("      (PS. she thinks it's for school...)");
	game.rightText.lines.push_back("-If you lose all of your money,");
	game.rightText.lines.push_back(" your Mom will surely send you some more!");
	game.rightText.lines.push_back("");
	game.rightText.lines.push_back("Good luck and have fun!");
}

void DisplayControls() {
	ClearRightText();
	game.rightText.lines.push_back("Controls:");
	game.rightText.lines.push_back("Pressing 'C' will bring up this menu.");
	game.rightText.lines.push_back("Pressing 'I' will bring up the game information.");
	game.rightText.lines.push_back("    (only in the menu)");
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

void ClearCenterText() {
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
	
	for (AfterPrint toPrint: game.afterPrint) {
		Print(toPrint.position, toPrint.string, toPrint.effect);
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