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
	int width;
	int height;
	time_t startTime;
	time_t askQuitTime;

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
void DisplayInfortmation();
void DisplayControls();
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
	Position tempPos{ ((game.rightText.position.x - game.position.x - game.title.length())/2),(game.position.y)};
	Print(tempPos, game.title, 14);

	tempPos.x = game.rightText.position.x;
	tempPos.y = game.rightText.position.y - 1;
	Print(tempPos, "'C' to open Controls." , 14);

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

void AskQuit() {
	game.askQuitTime = time(NULL);
	game.quitting = true;
	ClearRightText();
	game.rightText.lines.push_back("Are you sure you want to quit?");
	game.rightText.lines.push_back("Press 'Q' again to quit...");
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

void ClearRightText() {
	int i = 0;
	for (int y = game.rightText.position.y; y < (game.rightText.lines.size() + game.rightText.position.y); y++) {
		for (int x = game.rightText.position.x; x < (game.rightText.position.x + game.rightText.lines[i].length()); x++) {
			Position tempPos{ x, y };
			Print(tempPos, " ", 0);
		}
		i++;
	}
	for (string str : game.rightText.lines) {
		if (str == "'C' to open Controls.") { continue; }
		game.rightText.lines.pop_back();
	}
}

void UpdateScreen() {
	for (int i = 0; i < game.rightText.lines.size(); i++) {
		Position tempPos{ game.rightText.position.x, game.rightText.position.y + i };
		Print(tempPos, game.rightText.lines[i], 10);
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