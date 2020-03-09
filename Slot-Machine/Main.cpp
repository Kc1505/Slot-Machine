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

	int state = 1;

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
		Position position{40,1};

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
		
		if (GetKeyState('F') & 0x8000) { cout << "here"; game.state = 2; };
		break;

	case 2:
		DisplaySlot();
		break;

	case 3:
		DisplaySlot();
		break;
	}

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
	ClearTextCenter();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("What would you like to do?");
	game.mainText.lines.push_back("1) Play the Game!");
	game.mainText.lines.push_back("2) View my bank acount!");
	game.mainText.lines.push_back("3) Quit!");
}

void DisplaySlot() {
	ClearTextCenter();
	game.mainText.lines.push_back("");
	game.mainText.lines.push_back("If you dont know what to do, press 'I'!");
}

void DisplayInfortmation() {
	ClearRightText();
	game.rightText.lines.push_back("Infortmation:");
	game.rightText.lines.push_back("This is the game of Slots!");
	game.rightText.lines.push_back("You can bet your fake money, to win or lose more fake money!");
}

void DisplayControls() {
	ClearRightText();
	game.rightText.lines.push_back("Controls:");
	game.rightText.lines.push_back("Pressing 'C' will bring up this menu.");
	game.rightText.lines.push_back("Pressing 'I' will bring up the game infortmation.");
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