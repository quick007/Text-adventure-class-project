#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
using namespace std;

// README: Each room has a one sentance description and the player can look
// around in all 4 directions. This adds up to 5 sentances per room, and it lets
// me add more interactivity. I think this is a better experiance than just
// having a long list of sentances for each room.

// Items
int coins;
int swordLevel;

// Stats
int xp;
int direction;
int room;
bool grace;
vector<int> lootedChests;

bool running = true;

// encounters & interactives

const string CHEST = "chest";
const string TRADER = "trader";
const string FORCE = "force";
const string FOUNTAIN = "fountain";
const string DOOR = "door";
const string REAL_EXIT = "real exit";
const string FAKE_EXIT = "fake exit";

const string ENEMY = "enemy";
const string BOSS = "boss";

// room defenitions

const string rooms[9][5][2] = {
    // room 0
    {// Room description
     {"You're in a clearing in the forest. It seems ordinary at first, but you "
      "feel like you're being watched...",
      ENEMY},
     // Direction 0 (north)
     {"You find a chest hidden amongst the trees.", CHEST},
     // Direction 1 (east)
     {"The clearing you're in continues along a short path.", "1"},
     // Direction 2 (south)
     {"The forest stretches out into the distance.", ""},
     // Direction 3 (west)
     {"The forest stretches out into the distance.", ""}},
    // room 1
    {// Room description
     {"You're in a small clearing in a large forest.", ""},
     // Direction 0 (north)
     {"You see a long trail leading north to what appears to be a small "
      "village in the distance.",
      FORCE},
     // Direction 1 (east)
     {"The clearing you're in continues along a short path.", "2"},
     // Direction 2 (south)
     {"A chest is hidden in the undergrowth amongst the trees.", CHEST},
     // Direction 3 (west)
     {"An ordinary looking field stretches out before you. Something feels off "
      "about it...",
      "0"}},
    // room 2
    {// Room description
     {"You're in a clearing in the forest. It's surprisingly peaceful.", ""},
     // Direction 0 (north)
     {"A trader stands in the middle of the clearing, offering his wares.",
      TRADER},
     // Direction 1 (east)
     {"A large, dark opening leads into the earth. It looks like a dungeon.",
      "3"},
     // Direction 2 (south)
     {"A path disappears into the forest. The air feels heavy here...", FORCE},
     // Direction 3 (west)
     {"A path leads back to the previous clearing.", "1"}},
    // room 3
    {// Room description
     {"You're in a dark, damp dungeon on a staircase. The air is thick with "
      "the smell of "
      "decay.",
      ""},
     // Direction 0 (north)
     {"You find a chest here, hidden in the shadows.", CHEST},
     // Direction 1 (east)
     {"You see a light in the distance at the bottom of the steps.", "4"},
     // Direction 2 (south)
     {"The dungeon continues downwards, but it's too dark to see what's ahead.",
      ""},
     // Direction 3 (west)
     {"The dungeon opens into the previous clearing.", "2"}},
    // room 4
    {// Room description
     {"You've finally stopped going down. This is a large hallway. It's dark "
      "and damp, and you can hear the sound of dripping water.",
      ""},
     // Direction 0 (north)
     {"A fountain glows with an ethereal light. It's surrounded by a circle of "
      "stones.",
      FOUNTAIN},
     // Direction 1 (east)
     {"The hallway continues, and you can see a faint light in the distance.",
      "5"},
     // Direction 2 (south)
     {"The hallway is blocked by a wall.", ""},
     // Direction 3 (west)
     {"The hallway leads back to the previous room.", "3"}},
    // room 5
    {// Room description
     {"You enter a large atrium with a high ceiling. The walls are adorned "
      "with intricate carvings of a forgotten language. A faint light shines "
      "from the center of the room.",
      ""},
     // Direction 0 (north)
     {"A hallway leads north, but it's blocked off by a large, sealed door.",
      ""},
     // Direction 1 (east)
     {"A chest sits in a corner of the room.", CHEST},
     // Direction 2 (south)
     {"A path leads south, and you see a sign that reads Boss.", "6"},
     // Direction 3 (west)
     {"The hallway leads back to the previous room.", "4"}},
    // room 6
    {// Room description
     {"You're in a large, open chamber. The air is thick with the smell of "
      "damp earth and decay. A large, rusted iron door stands in the center of "
      "the room.",
      ""},
     // Direction 0 (north)
     {"A long, winding hallway leads north. You can see a faint light at the "
      "end of the hallway.",
      "5"},
     // Direction 1 (east)
     {"The chamber seems to fade into the darkness ahead.",
      ""},
     // Direction 2 (south)
     {"You find a chest hidden in the shadows.", CHEST},
     // Direction 3 (west)
     {"An imposing door seems to beckon you to enter.", "7"}},
    // room 7
    {// Room description
     {"You enter a large, circular room. The air is thick with the smell of "
      "magic and decay.",
      ENEMY},
     // Direction 0 (north)
     {"The room seems to fade into the darkness ahead.", ""},
     // Direction 1 (east)
     {"A locked door blocks your path to the east.", DOOR},
     // Direction 2 (south)
     {"A path leads south to the boss room. Be careful...", "8"},
     // Direction 3 (west)
     {"You see a wall with similar carvings as the chamber.", ""}},
    // room 8
    {// Room description
     {"You enter a large, circular room. The air is thick with the smell of "
      "magic and decay. You can see a large, imposing figure standing in the "
      "center of the room. This must be the boss.",
      BOSS},
     // Direction 0 (north)
     {"A long, dark hallway leads north.", "7"},
     // Direction 1 (east)
     {"The room seems to fade into the darkness ahead.", ""},
     // Direction 2 (south)
     {"The room is blocked by a wall.", REAL_EXIT},
     // Direction 3 (west)
     {"You find a hidden exit. It's not clear where it leads...", FAKE_EXIT}}};

void resetGame() {
  coins = 0;
  swordLevel = 0;
  xp = 20;
  direction = 1;
  room = 8;
  grace = true;
  lootedChests.clear();

  running = true;
}

void clear() { cout << "\033c"; }

void displayBar(int progress) {
  cout << "[";
  for (int i = 0; i < progress / 2; i++) {
    cout << "█";
  }
  for (int i = progress / 2; i < 5; i++) {
    cout << " ";
  }
  cout << "]" << endl;
}

string getArrow(int direction) {
  switch (direction) {
  case 0:
    return "↑ (North)";
  case 1:
    return "→ (East)";
  case 2:
    return "↓ (South)";
  case 3:
    return "← (West)";
  }
  return "";
}

void displayStats() {
  cout << endl << endl << "**Stats**" << endl;
  cout << "Coins: " << coins << ", Sword Level: " << swordLevel
       << ", Room: " << room << endl;
  cout << "XP: ";
  displayBar(xp);
  cout << "Direction: " << getArrow(direction);
  cout << endl << "-----------------------------" << endl << endl;
}

void showDirectionOptions() {
  cout << "Options:" << endl;
  cout << "1. Turn left" << endl;
  cout << "2. Turn right" << endl;

  string forwardOption = rooms[room][direction + 1][1];
  string forwardDescription = rooms[room][direction + 1][0];

  cout << "3. Move forward - " << ((forwardOption == REAL_EXIT && grace) ? "The grace of the god opens the exit" : forwardDescription) << endl;

  if (forwardOption == CHEST) {
    cout << "4. Open the chest in front of you." << endl;
  } else if (forwardOption == TRADER) {
    cout << "4. Talk to the trader." << endl;
  } else if (forwardOption == FOUNTAIN) {
    cout << "4. Interact with the fountain." << endl;
  } else if (forwardOption == DOOR) {
    cout << "4. Open the door." << endl;
  } else if (forwardOption == REAL_EXIT && grace) {
    cout << "4. Use the *real* exit." << endl;
  } else if (forwardOption == FAKE_EXIT) {
    if (grace) {
      cout << "4. Use the *fake* exit (the goddess has revealed the true exit)."
           << endl;
    } else {
      cout << "4. Use the exit." << endl;
    }
  } else if (forwardOption == ENEMY) {
    cout << "4. Fight the enemy." << endl;
  } else if (forwardOption == BOSS) {
    cout << "4. Fight the boss." << endl;
  }
}

int main() {
  cout << "Welcome, brave adventurer!" << endl;
  cout << "Press enter to start." << endl;
  string input;
  getline(cin, input);
  clear();
  resetGame();
  cout << "You wake up in a small clearing..." << endl << endl << endl;
  while (running) {
    displayStats();
    showDirectionOptions();

    cout << endl << "What do you want to do? (say a number)" << endl;
    getline(cin, input);
    string forwardOption = rooms[room][direction + 1][1];
    string forwardDescription = rooms[room][direction + 1][0];

    if (input == "1") {
      clear();
      cout << "Turned left." << endl;
      direction = (direction + 3) % 4;
    } else if (input == "2") {

      clear();
      cout << "Turned right." << endl;
      direction = (direction + 1) % 4;
    } else if (input == "3") {
      // Move forward
      string forwardOption = rooms[room][direction + 1][1];
      
      if (forwardOption != "") {
        if (forwardOption == FORCE) {
          clear();
          cout << "A myserious force prevents you from going furthur." << endl;
        } else {
          try {
            if (stoi(forwardOption) >= 0 && stoi(forwardOption) <= 8) {
              room = stoi(forwardOption);
              string forwardRoomOption = rooms[room][0][1];
              clear();
              cout << "You move forward to a new area. " << rooms[room][0][0]
                   << endl;
              if (forwardRoomOption == ENEMY) {
                // I found this on stack overflow, seems to work
                this_thread::sleep_for(chrono::milliseconds(2500));
                clear();
                cout << "You encounter an enemy skeleton!" << endl;
                this_thread::sleep_for(chrono::milliseconds(1000));
                clear();
                cout << "You defeat the enemy skeleton! + 5 xp" << endl;
                xp += 5;

              } else if (forwardRoomOption == BOSS) {
                this_thread::sleep_for(chrono::milliseconds(2500));
                clear();
                if (xp >= 10 || swordLevel >= 1) {
                  cout << "You fight the boss and win!" << endl;
                } else {
                  cout << "You lose against the boss and die" << endl;
                  break;
                }
              }
            }
          } catch (exception e) {
            clear();
            cout << "There is something in front of you" << endl;
          }
        }
      } else {
        clear();
        cout << "There's nowhere to go ahead. Try turning left/right" << endl;
      }
    } else if (input == "4") {
      string forwardOption = rooms[room][direction + 1][1];
      if (forwardOption == CHEST) {
        bool found = false;
        for (int i = 0; i < lootedChests.size(); i++) {
          if (lootedChests[i] == room)
            found = true;
        }
        if (found) {
          clear();
          cout << "You already looted this chest." << endl;
        } else {
          const int coin = (rand() % (15 - 5 + 1)) + 5;
          coins += coin;
          lootedChests.push_back(room);
          clear();
          cout << "You open the chest and find " << coin << " coins!" << endl;
        }
      } else if (forwardOption == TRADER) {
        clear();
        if (coins > 10) {
          cout << "You talk to the trader pay 10 for a higher level sword."
               << endl;
          coins -= 10;
          swordLevel++;
        } else {
          cout << "You don't have enough coins to buy a sword." << endl;
        }
      } else if (forwardOption == FORCE) {
        clear();
        cout << "You use force to proceed through the obstacle." << endl;
      } else if (forwardOption == FOUNTAIN) {
        clear();
        cout << "As you approach the foutain, you feel the gods grace "
                "enveloping you"
             << endl;
        grace = true;
      } else if (forwardOption == DOOR) {
        clear();
        cout << "You open the door and proceed." << endl;
      } else if (forwardOption == REAL_EXIT && grace) {
        clear();
        cout << "You used the real exit beat the game! Good job" << endl;
        break;
      } else if (forwardOption == FAKE_EXIT) {
        clear();
        cout << "The boss tricked you. This isn't the final exit, but a trap. "
                "You fall into a pit of spikes."
             << endl;
        break;
      } else {
        clear();
        cout << "There is nothing to interact with here." << endl;
      }
    } else {
      clear();
      cout << "Invalid option. Try again." << endl;
    }
  }
}
