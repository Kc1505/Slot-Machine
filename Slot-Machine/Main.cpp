#include <iostream>
#include <windows.h>
#include <time.h>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <io.h>
#include <fcntl.h>

#pragma region "Initialisation"
using namespace std;

//Create position class to be used for other objects.
class Position {
public:
	int x;
	int y;
};

//create a class to be used to store text to be printed over existing text (to reduce screen load times).
class AfterPrint {
public:
	Position position;
	wstring string;
	int effect;
};

//Main class that stores most game values to be used throughout the code.
class Game {
public:
	//Bools used to control the state of the program.
	bool running = true;
	bool quitting = false;
	bool displayChanged = false;

	//This int is used the most often to determine the main states of the game.
	int state = 1;

	//Money and bet amount of player.
	int currentMoney = 2000;
	int betAmount = 1;

	//The current slot numbers at any given point. (used for rolling effect, and checking win).
	int slotNumbers[3][3] = {
		{1,2,3},
		{7,7,7},
		{4,5,6} 
	};

	//Time values to control exit prompt delay.
	time_t startTime = 0;
	time_t askQuitTime = 0;

	wstring title = L"Welcome to Slots!";
	
	//position of the game window.
	Position position{0,0};

	//The Text that is used to display the main game on the left of the console.
	class MainText {
	public:
		vector<wstring> lines;
		vector<wstring> bankStatements{L"Recieved: $2000    From: Mom"};
		Position position{ 0,1 };

	}mainText;

	//The Text that is used to display the infortmation and controls on the right of the console.
	class RightText {
	public:
		vector<wstring> lines;
		Position position{50,1};

	}rightText;

	//storing values to be printed after main screen render.
	vector<AfterPrint> afterPrint;

}game;
#pragma endregion


#pragma region  "Declaring Function"

//Declaring all of the functions used in the program.
void Start();
void Update();
void Print(Position pos, wstring str, int effect);
void GotoXY(Position pos, int multi);
void Input();
void UpdateScreen(bool onlyAfter = 0);
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

#pragma endregion


#pragma region  "Start Functions"
//Starting Mian function.
int main() {
	_setmode(_fileno(stdout), _O_U16TEXT);

	Start();

	while (game.running) {
		Update();
		Sleep(10);
	}

	return 0;
}

//Function that is executed only once before the game actually starts, used to initialise some values.
void Start() {
	game.startTime = time(NULL);
	srand(game.startTime);

	Position tempPos{ (((game.rightText.position.x) - (game.position.x) - static_cast<int>(game.title.length()))/2),(game.position.y)};
	Print(tempPos, game.title, 14);

	tempPos.x = game.rightText.position.x;
	tempPos.y = game.rightText.position.y - 1;
	Print(tempPos, L"'C' to open Controls." , 10);
}

//The Main Update Function, called after every 100ms + execute time.
void Update() {
	if (game.quitting && time(NULL) - game.askQuitTime  >= 5) {
		game.quitting = false;
		DisplayControls();
	}

	//Checks for user input.
	Input();
}
#pragma endregion


#pragma region "Input Function"
//Function taht checks for user input.
void Input() {
	switch (game.state) {
	case 1: //Menu State of the game.
		DisplayMenu();
		
		if (GetKeyState('1') & 0x8000) { game.state = 2; game.displayChanged = false; };
		if (GetKeyState('2') & 0x8000) { game.state = 10; game.displayChanged = false; };
		if (GetKeyState('3') & 0x8000) { game.running = false; };
		if (GetKeyState('4') & 0x8000) { game.state = 6; game.displayChanged = false; };
		if (GetKeyState('I') & 0x8000) {
			DisplayInformation();
		}
		if (GetKeyState('C') & 0x8000) {
			DisplayControls();
		}

		break;

	case 2: //State to Display screen before betting starts.
		DisplaySlot();

		if (GetKeyState('E') & 0x8000) { game.state = 3; game.displayChanged = false; };
		break;

	case 3: //State to Let user choose and place thier bet amount.
		EnterBet();

		if (GetKeyState('P') & 0x8000) { game.state = 4; game.displayChanged = false; };

		//Increasing bet amount by 100.
		if (GetKeyState(VK_UP) & 0x8000) {
			int fail = false;
			if (game.betAmount + 100 <= game.currentMoney) {
				game.betAmount += 100;
			}
			else {
				game.betAmount = game.currentMoney;
				for (int i = 0; i < game.mainText.lines.size(); i++) {
					if (game.mainText.lines[i] != L"You dont have enough money to go higher!") {
						fail = true;
					}

				}
			}
			game.displayChanged = false;
			EnterBet();
			if (fail) game.mainText.lines.push_back(L"You dont have enough money to go higher!");;
			
		};
		//Decreasing bet amount by 100.
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
					if (game.mainText.lines[i] != L"Cant go lower!") {
						fail = true;
					}

				}
			}
			game.displayChanged = false;
			EnterBet();
			if (fail) game.mainText.lines.push_back(L"Cant go lower!");;

		};
		//Increasing bet amount by 1.
		if (GetKeyState(VK_RIGHT) & 0x8000) {
			int fail = false;
			if (game.betAmount + 1 <= game.currentMoney) {
				game.betAmount += 1;
			}
			else {
				for (int i = 0; i < game.mainText.lines.size(); i++) {
					if (game.mainText.lines[i] != L"You dont have enough money to go higher!") {
						fail = true;
					}

				}
			}
			game.displayChanged = false;
			EnterBet();
			if (fail) game.mainText.lines.push_back(L"You dont have enough money to go higher!");;

		};
		//Decreasing bet amount by 1.
		if (GetKeyState(VK_LEFT) & 0x8000) {
			int fail = false;
			if (game.betAmount - 1 >= 1) {
				game.betAmount -= 1;
			}
			else {
				for (int i = 0; i < game.mainText.lines.size(); i++) {
					if (game.mainText.lines[i] != L"Cant go lower!") {
						fail = true;
					}

				}
			}
			game.displayChanged = false;
			EnterBet();
			if (fail) game.mainText.lines.push_back(L"Cant go lower!");;

		};
		break;

	case 4: //State that simple shows slow machine spinning.
		WatchSlotMachine();
		break;

	case 5: //State used to show the result of the slot machine spinning (how much the player won or lost)
		DisplayWinnings();
		if (GetKeyState('R') & 0x8000) { game.state = 3; game.displayChanged = false; }
		else if (GetKeyState('S') & 0x8000) { game.state = 2; game.displayChanged = false; }
		break;

	case 6: //State that Displays the log of transactions that the player has made.
		DisplayAccount();

	case 10: //State that Displays the credits.
		DisplayCredits();
		break;
	}

	//Goes back to menu state
	if (GetKeyState('M') & 0x8000 &&  (game.state != 4)) { game.state = 1; game.displayChanged = false; }

	//Asks the user if they want to quit, then quits if yes.
	if (GetKeyState('Q') & 0x8000) {
		if (game.quitting) {
			game.running = false;
		}
		else {
			AskQuit();
		}
	}
	

}
#pragma endregion


#pragma region "Center/Left Side Display Functions"
//Displays all of the text relevant to the menu.
void DisplayMenu() {
	if (game.displayChanged) return;
	game.afterPrint.clear();
	ClearCenterText();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"What would you like to do?");
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"1) Play the Game!");
	game.mainText.lines.push_back(L"2) View the Credits!");
	game.mainText.lines.push_back(L"3) Quit :(");
	game.mainText.lines.push_back(L"4) View my bank acount!");
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Balance: $" + to_wstring(game.currentMoney));

	DisplayInformation();
	game.rightText.lines.push_back(L"");
	game.rightText.lines.push_back(L"Pressing '1','2','3', or '4' will do something");
	game.rightText.lines.push_back(L"(As indicated on the left)");

	UpdateScreen();
	game.displayChanged = true;
}

//Displays all of the text relevant to the user transactions.
void DisplayAccount() {
	if (game.displayChanged) return;
	ClearCenterText();


	for (wstring string : game.mainText.bankStatements) {
		game.mainText.lines.push_back(string);
	}

	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Current Balance: $" + to_wstring(game.currentMoney));
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Press 'M' to go back to the Menu");

	DisplayControls();

	UpdateScreen();
	game.displayChanged = true;
}

//Displays all of the text relevant to the credits screen.
void DisplayCredits() {
	if (game.displayChanged) return;
	ClearCenterText();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"This game was made with love by me:");
	game.mainText.lines.push_back(L"KeaneCarotenuto@gmail.com");
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"I used ascii art from:");
	game.mainText.lines.push_back(L"www.oocities.org/spunk1111/electro.htm");
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"It was requested to include the link below:");
	game.mainText.lines.push_back(L"www.ascii-art.com");
	game.mainText.lines.push_back(L"Unfortunately this link does not work.");
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Press 'M' to go back to the Menu.");

	UpdateScreen();
	game.displayChanged = true;
}

//Displays all of the text relevant to the starting slot image.
void DisplaySlot() {
	if (game.displayChanged) return;
	ClearCenterText();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"You Approach the Slot Machine");
	game.mainText.lines.push_back(L"");
	writeMachine();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Tip: Press 'E' to enter the bet amount");

	DisplayControls();
	game.rightText.lines.push_back(L"");
	game.rightText.lines.push_back(L"Press 'E' To Start Betting");
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Press 'M' to go back to the Menu");

	UpdateScreen();
	game.displayChanged = true;
}

//Displays all of the text relevant to the player entering their bet.
void EnterBet() {
	if (game.displayChanged) return;
	ClearCenterText();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"You begin to enter your bet amount");
	game.mainText.lines.push_back(L"");
	writeMachine();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Bet Amount: " + to_wstring(game.betAmount));
	game.mainText.lines.push_back(L"Balance: $" + to_wstring(game.currentMoney));
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Press 'P' to place the bet!");
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Press 'M' to go back to the Menu");
	DisplayControls();
	game.rightText.lines.push_back(L"");
	game.rightText.lines.push_back(L"Press 'up arrow key' to increase the bet amount by 100");
	game.rightText.lines.push_back(L"Press 'down arrow key' to decrease the bet amount 100");
	game.rightText.lines.push_back(L"Press 'right arrow key' to increase the bet amount 1");
	game.rightText.lines.push_back(L"Press 'left arrow key' to decrease the bet amount 1");
	game.rightText.lines.push_back(L"Press 'P' to place the bet!");

	UpdateScreen();
	game.displayChanged = true;
}

//Displays the slot machine generating random numbers, and then spinning.
void WatchSlotMachine() {
	if (game.displayChanged) return;
	game.currentMoney -= game.betAmount;
	game.mainText.bankStatements.push_back(L"Sent    : $" + to_wstring(game.betAmount) + L"    To: Casino");

	ClearCenterText();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"You Watch as the Machine spins");
	game.mainText.lines.push_back(L"");
	writeMachine();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Wooo! Lets get that jackpot!");
	
	DisplayControls();
	game.rightText.lines.push_back(L"");
	game.rightText.lines.push_back(L"All you can do is watch!");

	game.afterPrint.clear();

	//Section that actually shows the numbers spinning.
	for (int i = 0; i < rand() % 15 + 5; i++) {
		for (int x = 0; x < 3; x++) {
			game.slotNumbers[2][x] = game.slotNumbers[1][x];
		}
		for (int x = 0; x < 3; x++) {
			game.slotNumbers[1][x] = game.slotNumbers[0][x];
		}
		for (int x = 0; x < 3; x++) {
			game.slotNumbers[0][x] = rand() % 6 + 2;
		}

		game.afterPrint.push_back({ {10,7}, to_wstring(game.slotNumbers[0][0]), game.slotNumbers[0][0] });
		game.afterPrint.push_back({ {14,7}, to_wstring(game.slotNumbers[0][1]), game.slotNumbers[0][1] });
		game.afterPrint.push_back({ {18,7}, to_wstring(game.slotNumbers[0][2]), game.slotNumbers[0][2] });

		game.afterPrint.push_back({ {10,8}, to_wstring(game.slotNumbers[1][0]), game.slotNumbers[1][0] });
		game.afterPrint.push_back({ {14,8}, to_wstring(game.slotNumbers[1][1]), game.slotNumbers[1][1] });
		game.afterPrint.push_back({ {18,8}, to_wstring(game.slotNumbers[1][2]), game.slotNumbers[1][2] });

		game.afterPrint.push_back({ {10,9}, to_wstring(game.slotNumbers[2][0]), game.slotNumbers[2][0] });
		game.afterPrint.push_back({ {14,9}, to_wstring(game.slotNumbers[2][1]), game.slotNumbers[2][1] });
		game.afterPrint.push_back({ {18,9}, to_wstring(game.slotNumbers[2][2]), game.slotNumbers[2][2] });
		
		UpdateScreen(true);
		Sleep(400);
	}

	ClearCenterText();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"The machine stops Spinning");
	game.mainText.lines.push_back(L"");
	writeMachine();
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"Spin Complete!");
	UpdateScreen();
	Sleep(1000);

	game.state = 5;

	game.displayChanged = false;
}

//Displays the result of the player's bet.
void DisplayWinnings() {
	if (game.displayChanged) return;
	ClearCenterText();
	
	game.mainText.lines.push_back(L"");
	game.mainText.lines.push_back(L"You look at the results of your bet");
	game.mainText.lines.push_back(L"");
	writeMachine();
	game.mainText.lines.push_back(L"");

	DisplayControls();
	game.rightText.lines.push_back(L"");
	game.rightText.lines.push_back(L"All you can do is watch!");

	UpdateScreen();

	DisplayControls();
	game.rightText.lines.push_back(L"");
	game.rightText.lines.push_back(L"Press 'R' to Retry");
	game.rightText.lines.push_back(L"Press 'S' to Stop");


	game.afterPrint.clear();

	//Checks what the result is and displays it to the player.
	if (game.slotNumbers[1][0] == 7 &&
		game.slotNumbers[1][1] == 7 &&
		game.slotNumbers[1][2] == 7) {
		
		game.afterPrint.push_back({ {10,13}, L"JACKPOT!x10", 10 });
		game.afterPrint.push_back({ {10,12}, L"TRIPLES x5", 12 });
		game.afterPrint.push_back({ {10,11}, L"DOUBLES x3", 12 });

		game.currentMoney += game.betAmount * 10;

		game.mainText.lines.push_back(L"You Just Gained $" + to_wstring(game.betAmount * 10));
		game.mainText.bankStatements.push_back(L"Recieved: $" + to_wstring(game.betAmount * 10) + L"    From: Casino");
	}
	else if (game.slotNumbers[1][0] == game.slotNumbers[1][1] &&
		game.slotNumbers[1][0] == game.slotNumbers[1][2] &&
		game.slotNumbers[1][1] == game.slotNumbers[1][2]) {

		game.afterPrint.push_back({ {10,13}, L"JACKPOT!x10", 12 });
		game.afterPrint.push_back({ {10,12}, L"TRIPLES x5", 10 });
		game.afterPrint.push_back({ {10,11}, L"DOUBLES x3", 12 });

		game.currentMoney += game.betAmount * 5;

		game.mainText.lines.push_back(L"You Just Won $" + to_wstring(game.betAmount * 5));
		game.mainText.bankStatements.push_back(L"Recieved: $" + to_wstring(game.betAmount * 5) + L"    From: Casino");
	}
	else if (game.slotNumbers[1][0] == game.slotNumbers[1][1] ||
		game.slotNumbers[1][0] == game.slotNumbers[1][2] ||
		game.slotNumbers[1][1] == game.slotNumbers[1][2]) {

		game.afterPrint.push_back({ {10,13}, L"JACKPOT!x10", 12 });
		game.afterPrint.push_back({ {10,12}, L"TRIPLES x5", 12 });
		game.afterPrint.push_back({ {10,11}, L"DOUBLES x3", 10 });

		game.currentMoney += game.betAmount * 3;

		game.mainText.lines.push_back(L"You Just Won $" + to_wstring(game.betAmount * 3));
		game.mainText.bankStatements.push_back(L"Recieved: $" + to_wstring(game.betAmount * 3) + L"    From: Casino");
	}
	else {
		game.afterPrint.push_back({ {10,13}, L"JACKPOT!x10", 12 });
		game.afterPrint.push_back({ {10,12}, L"TRIPLES x5", 12 });
		game.afterPrint.push_back({ {10,11}, L"DOUBLES x3", 12 });

		game.mainText.lines.push_back(L"You Just Lost $" + to_wstring(game.betAmount));
	}
	game.mainText.lines.push_back(L"Balance: $" + to_wstring(game.currentMoney));

	game.afterPrint.push_back({ {10,7}, to_wstring(game.slotNumbers[0][0]), game.slotNumbers[0][0] });
	game.afterPrint.push_back({ {14,7}, to_wstring(game.slotNumbers[0][1]), game.slotNumbers[0][1] });
	game.afterPrint.push_back({ {18,7}, to_wstring(game.slotNumbers[0][2]), game.slotNumbers[0][2] });

	game.afterPrint.push_back({ {10,8}, to_wstring(game.slotNumbers[1][0]), game.slotNumbers[1][0] });
	game.afterPrint.push_back({ {14,8}, to_wstring(game.slotNumbers[1][1]), game.slotNumbers[1][1] });
	game.afterPrint.push_back({ {18,8}, to_wstring(game.slotNumbers[1][2]), game.slotNumbers[1][2] });

	game.afterPrint.push_back({ {10,9}, to_wstring(game.slotNumbers[2][0]), game.slotNumbers[2][0] });
	game.afterPrint.push_back({ {14,9}, to_wstring(game.slotNumbers[2][1]), game.slotNumbers[2][1] });
	game.afterPrint.push_back({ {18,9}, to_wstring(game.slotNumbers[2][2]), game.slotNumbers[2][2] });

	game.betAmount = 1;


	game.displayChanged = true;

	//Checks if the player has lost all of thier money, kicks them to the menu and gives them another $1k.
	if (game.currentMoney <= 0) {
		game.mainText.lines.push_back(L"");
		game.mainText.lines.push_back(L"OOPS! Please Wait");
		DisplayControls();
		game.rightText.lines.push_back(L"");
		game.rightText.lines.push_back(L"You Lost, wait to be kicked out...");
		
		UpdateScreen();

		Sleep(5000);

		ClearCenterText();
		game.afterPrint.clear();

		game.mainText.lines.push_back(L"");
		game.mainText.lines.push_back(L"You Just lost the game...");
		game.mainText.lines.push_back(L"You Ran out of money");
		game.mainText.lines.push_back(L"");
		game.mainText.lines.push_back(L"Sorry, gotta kick you out of the Casino");
		game.mainText.lines.push_back(L"Goodbye! (in 5 seconds)");
		UpdateScreen();

		Sleep(5000);

		game.currentMoney = 2000;
		game.mainText.bankStatements.push_back(L"Recieved: $2000    From: Mom");


		game.state = 1;
		game.displayChanged = false;
	}
	else {
		game.mainText.lines.push_back(L"");
		game.mainText.lines.push_back(L"Press 'R' to Retry");
		game.mainText.lines.push_back(L"Press 'S' to Stop betting");
	}
	UpdateScreen();
}
#pragma endregion


#pragma region "ASCII Slot Machine Function"
//Used to display the actual slot machine, instead of writng it out every time.
void writeMachine() {
	game.mainText.lines.push_back(L"         ╔═════════╗");
	game.mainText.lines.push_back(L"       ▀█║- SLOTS -║█▀");
	game.mainText.lines.push_back(L"       ┌─┼─┬─┬─┬─┬─┼─┐ __");
	game.mainText.lines.push_back(L"       │ │" + to_wstring(game.slotNumbers[0][0]) + L"│ │" + to_wstring(game.slotNumbers[0][1]) + L"│ │" + to_wstring(game.slotNumbers[0][2]) + L"│ │(  )");
	game.mainText.lines.push_back(L"       ├─┤"+to_wstring(game.slotNumbers[1][0])+ L"├─┤" + to_wstring(game.slotNumbers[1][1]) + L"├─┤" + to_wstring(game.slotNumbers[1][2]) + L"├─┤ ││");
	game.mainText.lines.push_back(L"       │ │" + to_wstring(game.slotNumbers[2][0]) + L"│ │" + to_wstring(game.slotNumbers[2][1]) + L"│ │" + to_wstring(game.slotNumbers[2][2]) + L"│ │ ││");
	game.mainText.lines.push_back(L"       ├─┴─┴─┴─┴─┴─┴─┼─┘│");
	game.mainText.lines.push_back(L"       │  DOUBLES x3 ├──┘");
	game.mainText.lines.push_back(L"       │  TRIPLES x5 │");
	game.mainText.lines.push_back(L"       │  JACKPOT!x10│");
	game.mainText.lines.push_back(L"       │             │");
	game.mainText.lines.push_back(L"       │ ┌─────────┐ │");
	game.mainText.lines.push_back(L"      ┌┤ └─────────┘ ├┐");
	game.mainText.lines.push_back(L"     ┌┘└─────────────┘└┐");
	game.mainText.lines.push_back(L"    ┌┘╬╬╬╬╬╬╬╬╬╬╬╬╬╬╬╬╬└┐");
	game.mainText.lines.push_back(L"    └───────────────────┘");
}
#pragma endregion


#pragma region "Right Side Display Functions"
//Displays game information on the right.
void DisplayInformation() {
	ClearRightText();
	game.rightText.lines.push_back(L"Information:");
	game.rightText.lines.push_back(L"This is the game of Slots!");
	game.rightText.lines.push_back(L"You can bet your fake money, to win or lose more fake money!");
	game.rightText.lines.push_back(L"");
	game.rightText.lines.push_back(L"-The goal of the Game is to gain as much money as you can.");
	game.rightText.lines.push_back(L"-To do that, bet your current money at the slot machine.");
	game.rightText.lines.push_back(L"-You start with $2000, given to you by your Mom");
	game.rightText.lines.push_back(L"      (PS. she thinks it's for school...)");
	game.rightText.lines.push_back(L"-If you lose all of your money,");
	game.rightText.lines.push_back(L" your Mom will surely send you some more!");
	game.rightText.lines.push_back(L"");
	game.rightText.lines.push_back(L"Good luck and have fun!");

	UpdateScreen();
}

//Displays general controls on the right.
void DisplayControls() {
	ClearRightText();
	game.rightText.lines.push_back(L"Controls:");
	game.rightText.lines.push_back(L"Pressing 'C' will bring up this menu.");
	game.rightText.lines.push_back(L"Pressing 'I' will bring up the game information.");
	game.rightText.lines.push_back(L"    (only in the menu)");
	game.rightText.lines.push_back(L"Pressing 'M' will go back to the Menu.");
	game.rightText.lines.push_back(L"Pressing 'Q' will quit the game.");

	UpdateScreen();
}

//Asks the user if they want to quit on the right.
void AskQuit() {
	game.askQuitTime = time(NULL);
	game.quitting = true;
	ClearRightText();
	game.rightText.lines.push_back(L"Are you sure you want to quit?");
	game.rightText.lines.push_back(L"Press 'Q' again to quit...");

	UpdateScreen();
}
#pragma endregion


#pragma region "Clear Section Functions"
					//-----------------		Clear Functions     -----------------\\

//Clears all of the text on the right side of the screen.
void ClearRightText() {
	int i = 0;
	int lines = static_cast<int>(game.rightText.lines.size());

	for (int y = game.rightText.position.y; y < (lines + game.rightText.position.y); y++) {
		int lineLength = game.rightText.lines[i].length();
		wstring toPrint;
		for (int x = 0; x < lineLength; x++) {
			toPrint += L" ";
		}
		Position tempPos{ game.rightText.position.x, y };
		Print(tempPos, toPrint, 0);
		i++;
	}
	for (wstring str : game.rightText.lines) {
		game.rightText.lines.pop_back();
	}
}

//Clears all of the text on the left/center side of the screen.
void ClearCenterText() {
	int i = 0;
	int lines = static_cast<int>(game.mainText.lines.size());
	for (int y = game.mainText.position.y; y < (lines + game.mainText.position.y); y++) {
		int lineLength = game.mainText.lines[i].length();
		wstring toPrint;
		for (int x = 0; x < lineLength; x++) {
			toPrint += L" ";
		}
		Position tempPos{ game.mainText.position.x, y };
		Print(tempPos, toPrint, 0);
		i++;
	}
	for (wstring str : game.mainText.lines) {
		game.mainText.lines.pop_back();
	}
}
#pragma endregion


#pragma region "Screen Update Function"
//Function that decides what to write where.
void UpdateScreen(bool onlyAfter) {
	if (!onlyAfter) {
		for (int i = 0; i < game.rightText.lines.size(); i++) {
			Position tempPos{ game.rightText.position.x, game.rightText.position.y + i };
			Print(tempPos, game.rightText.lines[i], 10);
		}

		for (int i = 0; i < game.mainText.lines.size(); i++) {
			Position tempPos{ game.mainText.position.x, game.mainText.position.y + i };
			Print(tempPos, game.mainText.lines[i], 14);
		}
	}

	for (AfterPrint toPrint: game.afterPrint) {
		Print(toPrint.position, toPrint.string, toPrint.effect);
	}
}
#pragma endregion


#pragma region "Printing Functions"
//Used to print out text at the specified coordinate, with the specified effect.
void Print(Position pos, wstring str, int effect) {
	GotoXY(pos, 1);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), effect);
	wcout << str;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

//Used to move the Console Cursor to a point on the screen for more accurate text management.
void GotoXY(Position pos, int multi) {
	COORD point;
	point.X = (pos.x + game.position.x) * multi;
	point.Y = pos.y + game.position.y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), point);
}
#pragma endregion