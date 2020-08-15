//  Jonathan Tsai
//  8000357292
//  Assignment 12

//Libraries
#include<iostream>
#include<fstream>
#include<ncurses.h>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<string>

using namespace std;

void initCurses();
void endCurses();

//Circular queue from previous assignment
struct circularQueue{
    int front, back, size;
    int *arr;

    circularQueue(int s){
       front = back = -1;
       size = s;
       arr = new int[s];
    }

    void enQueue(int value){
        if ((front == 0 && back == size-1) || (back == (front-1)%(size-1))){
            printf("\nQueue is Full");
            return;
        }else if (front == -1){
            front = back = 0;
            arr[back] = value;
        }else if (back == size-1 && front != 0){
            back = 0;
            arr[back] = value;
        }else{
            back++;
            arr[back] = value;
        }
    }
    int deQueue(){
        int data = arr[front];
        arr[front] = -1;
        if(front == back){
            front = -1;
            back = -1;
        }
        else if(front == size-1)
            front = 0;
        else
            front++;

        return data;
    }

    bool isEmpty(){
        if(front == -1)
            return true;
        else return false;
    }

    ~circularQueue(){
    delete [] arr;
    }
};


//The class all classes inherits from
class entity_t{
public:
    entity_t():x(0), y(0){}
    int x, y;
    virtual char whatamI(){
        return 'e';
    }
    entity_t( int newX, int newY ){
      x = newX;
      y = newY;
    }
    void setCoordinates(int newX, int newY){
        x = newX;
        y = newY;
    }
};

//So that other classes know it exists
class bomb_t;
class AIDispatcher;

class player_t : public entity_t{
friend class point_t;
friend class bomb_t;
public:
    bool isHuman;
    bomb_t *bomb;
    bool alive;
    bool hasShot;
    char team;
    char lastDirection;

    player_t(char type, int newX, int newY) : entity_t(newX, newY){
      hasShot = false;
      team = type;
      isHuman = true;
      bomb = NULL;
      alive = true;
    }
    char whatamI(){
      if(isHuman == true){
        return 'p';
      }
      else if(isHuman ==  false && team == 't')
        return 'T';
      else if(isHuman == false && team == 'c')
        return 'C';
    }
};

//Bomb class inherited from entity_t
class bomb_t : public entity_t{
  public:
    bool isPlanted;
    bool isCarried;
    bool isDefused;
    bomb_t( int newX, int newY ) : entity_t(newX, newY){
      bool isPlanted = false;
      bool isCarried = false;
      bool isDefused = false;
    }
    char whatamI(){
      return 'b';
    }
};

//projectile class inherited from entity_t
class projectile_t : public entity_t{
public:
  char direction;
  entity_t *owner;
  projectile_t(char whichWay, int x, int y, player_t *p) : entity_t(x, y){
    direction = whichWay;
    owner = p;
  }
  char whatamI(){
    return '*';
  }
};

//charMap class to help define class below main
class charMap {
  public:
    charMap(char *arg);
    charMap(char** c, string m, int w, int h) :
    map(c), mapName(m), width(w), height(w){}
    ~charMap();
    void print();
    char **map;
    string mapName;
    int width;
    int height;
};

//Class that defines the attributes of a point in the specific spot on the map
class point_t {
public:
    char baseType;
    vector<entity_t * > entList;
    bool isBombsite;
    bool isObstacle;
    bool isWall;
    bool isBridgeTunnel;
    int x, y;
    bomb_t * newBomb;
    point_t(){}
    ~point_t(){
    }
    point_t(char mapPoint, int newX, int newY ){
      baseType = mapPoint;
      x = newX;
      y = newY;
      isBombsite = false;
      isObstacle = false;
      isWall = false;
      isBridgeTunnel = false;

      if(mapPoint == 'x'){
        isWall = true;
      } else if(mapPoint == '#'){
        baseType = ' ';
        isBridgeTunnel = true;
      } else if(mapPoint == 'o'){
        isObstacle = true;
      } else if(mapPoint == 'B'){
        baseType = ' ';
        entList.push_back(new bomb_t(newX,newY));
      } else if(mapPoint == 'P' || mapPoint == '1' || mapPoint == '2'
            || mapPoint == '3' ){
        baseType = ' ';
        isBombsite = true;
      } else if( mapPoint == 'C' || mapPoint == 'T'){
        baseType = ' ';
      }
    }

    //Function that prints out every single entity type
    void renderPoint(){
      char render = this -> baseType;
      if(isBombsite){
        render = 'P';
      }
      if(isBridgeTunnel){
        render = '#';
      }
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        if((entList[i])->whatamI() == 'b'){
          render = 'B';
          break;
        }
      }
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        if((entList[i])->whatamI() == '*'){
          render = '*';
          break;
        }
      }
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        if((entList[i])->whatamI() == 'C'){
          if(entList[i] == NULL){
            continue;
          }
          render = 'C';
          break;
        }
      }
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        if((entList[i])->whatamI() == 'T'){
          render = 'T';
          break;
        }
      }
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        if((entList[i])->whatamI() == 'p'){
          render = '@';
          break;
        }
      }
      printw("%c", render);

    }

    //Function that spawns an entity at location
    void initSpawn( player_t *player){
      entList.push_back(player);
    }

    //Remove entity at location
    void deleteEntFromPoint(entity_t *entity){
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        if(entList[i]->whatamI() == entity->whatamI()){
          entList[i] = NULL;
          entList.erase(entList.begin()+i);
          break;
        }
      }
    }

    //Function that "kills" an entity
    void RIP(player_t *player){
      player->alive = false;
      if(player->bomb){
        this->entList.push_back(player->bomb);
        player->bomb->isCarried = false;
      }
      this->deleteEntFromPoint(player);
    }
};


//Dispatches the level and holds attributes about the map
class LevelDispatcher{
  public:
    friend class point_t;
    friend class entity_t;
    vector<vector<point_t> > points;
    player_t * newPlayer;
    player_t * newBot;
    int height;
    int width;
    charMap* mapref;
    int roundTimer;
    int bombTimer;
    bool bombPlanted;
    bool bombDefused;
    int tAlive;
    int cAlive;
    int xT, yT;
    int xC, yC;
    LevelDispatcher(){}
    ~LevelDispatcher(){
    }

    //Function that holds end/win conditions
    void endCondition( int condition ) {
      if(condition == 0){
        return;
      }
      if(condition == 1){
        printw("\n CT WIN, BOMB HAS BEEN DEFUSED \n");
      }
      if(condition == 2){
        refresh();
        printw("\n T WIN, BOMB HAS EXPLODED\n");
      }
      if(condition == 3){
        printw("\n T WIN, ALL COUNTER TERRORIST HAVE BEEN ELIMINATED \n");
      }
      if(condition == 4){
        printw("\n CT WIN, ALL TERRORIST HAVE BEEN ELIMINATED \n");
      }
      if(condition == 5){
        printw("\n CT WIN, ROUND TIMER IS UP AND BOMB HAS NOT BEEN PLANTED BY TERRORIST \n");
      }
    }

    LevelDispatcher(charMap *map){
      mapref = map;
      height = map->height;
      width = map->width;
      points.resize(height, vector<point_t>(width));
        for(int i=0; i < height; i++){
          for(int j=0; j < width; j++){
            if(map->map[i][j] == 'C'){
              xC = i;
              yC = j;
            }
            else if(map->map[i][j] == 'T'){
              xT = i;
              yT = j;
            }
            points[i][j] = point_t(map->map[i][j],i,j);
          }
        }
        roundTimer = 300;
        bombTimer = 120;
        bombPlanted = false;
        bombDefused = false;
        tAlive = 5;
        cAlive = 5;
    }

    //Function that simply renders the map
    void renderMap(){
      for(int i=0; i< height; i++){
        for(int j=0; j< width; j++){
            points[i][j].renderPoint();
        }
        printw("\n");
      }
    }

    //Refreshes the screen with updates information (after clearing
    void clearScreen(){
        refresh();
        clear();
        if (newPlayer->team == 'c'){
            printw("You are a Counter-Terrorist (represented by @) \n");
            printw("Your goal is to defuse the bomb (B) or eliminate all Terrorist! \n");
            printw("Use WASD to move, and the spacebar to shoot. \n");
        } else {
            printw("You are a Terrorist (represented by @) \n");
            printw("Your goal is to plant the bomb (B) or eliminate all Counter-Terrorist! \n");
            printw("Use WASD to move, and the spacebar to shoot. \n");
        }
        if(bombPlanted){
            printw("BOMB TIME REMAINING: %d \n" , bombTimer);
            printw("BOMB IS PLANTED \n");
            bombTick();
        } else{
            printw("ROUND TIME REMAINING: %d \n" , roundTimer);
            secondTick();
        }
    }
    //Menu screen before game beings
    player_t *userMenu(){
      char userInput;

      printw("Enter C to play Counter-Terrorist or enter T to play Terrorist\n");
      userInput = getch();

      while(userInput != 'T' && userInput != 't' && userInput != 'c' && userInput != 'C'){
        printw(" T or C was not entered, please try again. \n");
        printw("Enter C to play Counter-Terrorist or enter T to play Terrorist\n");
        userInput = getch();
      }
      if(userInput == 't' || userInput == 'T'){
        printw("You are a Terrorist (represented by @) \n");
        printw("Your goal is to plant the bomb (B) or eliminate all Counter-Terrorist! \n");
        printw("Use WASD to move, and the spacebar to shoot. \n");
        newPlayer = new player_t('t', xT, yT);
        points[xT][yT].initSpawn(newPlayer);
        for (int i = 0; i < 4; i++){
            newBot = new player_t('t', xT, yT);
            newBot->isHuman = false;
            points[xT][yT].initSpawn(newBot);
        }
        for (int i = 0; i < 5; i++){
            newBot = new player_t('c', xC, yC);
            newBot->isHuman = false;
            points[xC][yC].initSpawn(newBot);
        }
        renderMap();
        return newPlayer;
      }
      else if(userInput == 'c' || userInput =='C'){
        printw("You are a Counter-Terrorist (represented by @) \n");
        printw("Your goal is to defuse the bomb (B) or eliminate all Terrorist! \n");
        printw("Use WASD to move, and the spacebar to shoot. \n");
        newPlayer = new player_t('c', xC, yC);
        points[xC][yC].initSpawn(newPlayer);
        for (int i = 0; i < 4; i++){
            newBot = new player_t('c', xC, yC);
            newBot->isHuman = false;
            points[xC][yC].initSpawn(newBot);
        }
        for (int i = 0; i < 5; i++){
            newBot = new player_t('t', xT, yT);
            newBot->isHuman = false;
            points[xT][yT].initSpawn(newBot);
        }
        renderMap();
        return newPlayer;
      }
    }

    //Reduces round timer
    void secondTick(){
        roundTimer--;
    }
    //Reduces bomb timer
    void bombTick(){
        bombTimer--;
    }

    //Checks if the game is over
    int checkRoundStatus(){
        if (bombDefused){
            endCondition(1);
            return 1;
        } else if (bombPlanted && bombTimer == -1){
            endCondition(2);
            return 2;
        } else if (cAlive == 0){
            endCondition(3);
            return 3;
        } else if (tAlive == 0 && !bombPlanted){
            endCondition(4);
            return 4;
        } else if (!bombPlanted && roundTimer == -1){
            endCondition(5);
            return 5;
        }
        return 0;
    }

};


//Ballistic dispatcher that takes care of projectiles logic
class BallisticDispatcher{
public:
  vector<projectile_t*> projectilesList;
  LevelDispatcher * levelref;
  BallisticDispatcher( LevelDispatcher * level ){
      levelref = level;
  }

  //Spawns a projectiles
  void insertProjectiles(projectile_t* projectiles){
    projectilesList.push_back(projectiles);
  }

  //Refreses projectiless
  void updateAll(){

    for(int i =0; i < projectilesList.size(); i++){
      int x = projectilesList[i]->x;
      int y = projectilesList[i]->y;

      if(projectilesList[i]->direction == 'u'){
        projectilesList[i]->setCoordinates(x-1, y);
      }

      else if(projectilesList[i]->direction == 'l'){
        projectilesList[i]->setCoordinates(x, y-1);
      }

      else if(projectilesList[i]->direction == 'r'){
        projectilesList[i]->setCoordinates(x, y+1);
      }

      else if(projectilesList[i]->direction == 'd'){
        projectilesList[i]->setCoordinates(x+1, y);

      }
      x = projectilesList[i]->x;
      y = projectilesList[i]->y;

      if(levelref->points[x][y].baseType == 'x'){
        levelref->points[x][y].deleteEntFromPoint(projectilesList[i]);
        if(projectilesList[i]->direction == 'u'){
          levelref->points[x+1][y].deleteEntFromPoint(projectilesList[i]);
        } else if(projectilesList[i]->direction == 'l'){
          levelref->points[x][y+1].deleteEntFromPoint(projectilesList[i]);
        } else if(projectilesList[i]->direction =='r'){
          levelref->points[x][y-1].deleteEntFromPoint(projectilesList[i]);
        } else if(projectilesList[i]-> direction == 'd'){
          levelref->points[x-1][y].deleteEntFromPoint(projectilesList[i]);
        }
        delete projectilesList[i];
        projectilesList[i] = NULL;
        projectilesList.erase(projectilesList.begin()+i);
        continue;
      } else if(levelref->points[x][y].entList.empty() ){
        levelref->points[x][y].entList.push_back(projectilesList[i]);
      } else if( levelref->points[x][y].entList.size() > 0){ //When a player should be killed

        for(int i =0; i < levelref->points[x][y].entList.size() ; i++){
          if(levelref->points[x][y].entList[i]->whatamI()=='T'|| levelref->points[x][y].entList[i]->whatamI()=='C' || levelref->points[x][y].entList[i]->whatamI()=='p'){
            if(projectilesList[i]->owner == dynamic_cast<player_t*>(levelref->points[x][y].entList[i])){
              continue;
            } else if(dynamic_cast<player_t*>(levelref->points[x][y].entList[i])->team == 't'){
                levelref->tAlive--;
            } else if (dynamic_cast<player_t*>(levelref->points[x][y].entList[i])->team == 'c'){
                levelref->cAlive--;
            }
            levelref->points[x][y].RIP(dynamic_cast<player_t*>(levelref->points[x][y].entList[i]));
          }
        }
      }

      if(projectilesList[i]->direction == 'u'){
        levelref->points[x+1][y].deleteEntFromPoint(projectilesList[i]);
      } else if(projectilesList[i]->direction == 'l'){
        levelref->points[x][y+1].deleteEntFromPoint(projectilesList[i]);
      } else if(projectilesList[i]->direction =='r'){
        levelref->points[x][y-1].deleteEntFromPoint(projectilesList[i]);
      } else if(projectilesList[i]-> direction == 'd'){
        levelref->points[x-1][y].deleteEntFromPoint(projectilesList[i]);
      }
    }
  }
};

//Class that takes care of all movement in game, move shoot and idle
class MovementDispatcher{
public:
  static char readkeyInput(){
    int ch= getch();
    switch(ch){
      case 'w':
      case 'W':
      case KEY_UP:
          return 'w';

      case 's':
      case 'S':
      case KEY_DOWN:
          return 's';

      case 'a':
      case 'A':
      case KEY_LEFT:
          return 'a';

      case 'd':
      case 'D':
      case KEY_RIGHT:
          return 'd';

      case 'i':
      case 'I':
          return 'i';

      case 'q':
      case 'Q':
          return 'q';

      case 'c':
      case 'C':
          return 'c';

      case ' ':
          return ' ';
      default:
        readkeyInput();
        break;
    }
  }

  //Executes the movement, deals with every single type of moment (left down right up)
  static void makeMove(LevelDispatcher *level, player_t *player, char direction, BallisticDispatcher *projectiles){
    if(player->alive == false){
      return;
    }
    int x = player->x;
    int y = player->y;
    if(direction == 'c'){
      refresh();
      return;
    }
    if(direction == 'i'){
      return;
    }
    if( direction == ' '){
      projectile_t * tempProjectiles = new projectile_t(player->lastDirection, x,y,player);
      projectiles->insertProjectiles(tempProjectiles);
      level->points[x][y].entList.push_back(tempProjectiles);
    }
    if(direction == 'w'){
        if(level->points[x-1][y].baseType == ' ' && level->points[x][y].isBridgeTunnel && (player->lastDirection=='u' ||player->lastDirection=='d' )){
          level->points[x-1][y].entList.push_back(player);
          level->points[x][y].deleteEntFromPoint(player);
          player->setCoordinates(x-1, y);
          player->lastDirection = 'u';
        } else if(level->points[x-1][y].baseType == ' ' &&
            level->points[x][y].isBridgeTunnel &&
            (player->lastDirection!='u' ||player->lastDirection!='d' )){
            return;
        } else if(level->points[x-1][y].baseType == ' ' &&
            !level->points[x][y].isBridgeTunnel){
            level->points[x-1][y].entList.push_back(player);
            level->points[x][y].deleteEntFromPoint(player);
            player->setCoordinates(x-1, y);
            player->lastDirection = 'u';
      } else{
        player->lastDirection = 'u';
        return;
      }
    }

    if(direction == 'a'){
      if(level->points[x][y-1].baseType == ' ' && level->points[x][y].isBridgeTunnel && (player->lastDirection=='r' ||player->lastDirection=='l')){
        level->points[x][y-1].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x, y-1);
        player->lastDirection = 'l';
        }else if(level->points[x][y-1].baseType == ' ' && level->points[x][y].isBridgeTunnel && (player->lastDirection!='r' ||player->lastDirection!='l')){
            return;
        }else if(level->points[x][y-1].baseType == ' ' && !level->points[x][y].isBridgeTunnel){
        level->points[x][y-1].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x, y-1);
        player->lastDirection = 'l';
      } else{
        player->lastDirection = 'l';
        return;
      }
    }

    if(direction == 's'){
      if(level->points[x+1][y].baseType == ' ' && level->points[x][y].isBridgeTunnel && (player->lastDirection=='u' ||player->lastDirection=='d')){
        level->points[x+1][y].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x+1, y);
        player->lastDirection = 'd';
          } else  if( level->points[x+1][y].baseType == ' ' && level->points[x][y].isBridgeTunnel && (player->lastDirection!='u' ||player->lastDirection!='d')){
          return;
        } else if(level->points[x+1][y].baseType == ' ' && !level->points[x][y].isBridgeTunnel){
        level->points[x+1][y].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x+1, y);
        player->lastDirection = 'd';
      } else{
        player->lastDirection = 'd';
        return;
      }
    }

    if(direction == 'd'){
      if(level->points[x][y+1].baseType == ' ' && level->points[x][y].isBridgeTunnel && (player->lastDirection=='r' ||player->lastDirection=='l' )){
        level->points[x][y+1].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x, y+1);
        player->lastDirection = 'r';
        } else if(level->points[x][y+1].baseType == ' ' && level->points[x][y].isBridgeTunnel && (player->lastDirection!='r' ||player->lastDirection!='l' )){
          return;
        } else if(level->points[x][y+1].baseType == ' ' && !level->points[x][y].isBridgeTunnel){
        level->points[x][y+1].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x, y+1);
        player->lastDirection = 'r';
      } else{
        player->lastDirection = 'r';
        return;


      }
    }
    postMoveChecks(level, player);
  }

  //Checks the status of the player based on the point they're sitting on
  static void postMoveChecks(LevelDispatcher*level, player_t*player){
    int x = player->x;
    int y = player->y;

    //Bomb pickup
    if(player->team == 't'){
      for(int i=0; i < level->points[x][y].entList.size(); i++){
        if(level->points[x][y].entList[i]->whatamI() == 'b'){
          player->bomb = dynamic_cast<bomb_t*>(level->points[x][y].entList[i]);
          player->bomb->isCarried = true;
          level->points[x][y].deleteEntFromPoint(level->points[x][y].entList[i]);
        }
      }
    }

    //Bomb planting
    if(player->team == 't' && (player->bomb)){
        if(level->points[x][y].isBombsite){
          level->points[x][y].entList.push_back(player->bomb);
          player->bomb->isCarried = false;
          player->bomb->isPlanted = true;
          player->bomb->setCoordinates(x,y);
          player->bomb = NULL;
          level->bombPlanted = true;
        }
    }

    //Player defusal
    if(player->team == 'c' && level->points[x][y].isBombsite){
      for(int i=0; i < level->points[x][y].entList.size(); i++){
        if(level->points[x][y].entList[i]->whatamI() == 'b'){
          dynamic_cast<bomb_t*>(level->points[x][y].entList[i])->isCarried = false;
          dynamic_cast<bomb_t*>(level->points[x][y].entList[i])->isDefused = true;
          level->bombDefused = true;

        }
      }
    }
  }
};

//AIDispatcher that deals with all of the AI logic
class AIDispatcher{
    public:
    void addHuman(player_t *humanFound){
        human = humanFound;
        return;
    }
    void addBot(player_t *botFound){
        botList.push_back(botFound);
        totalBots++;
    }
    void addBomb(bomb_t *bombFound){
        bomb = bombFound;
    }

    //Reused BFS from assignment 11
    bool BFS(vector<vector<int>> adj_list, int source, int omega, int numberOfVertices, int pre[]){
        //queue for our bfs
        circularQueue Q(numberOfVertices * numberOfVertices);

        //Array that bfs uses to check if a node has been visited
        bool visited[numberOfVertices];

        //Initialize all our passed in arrays
        for (int i = 0; i < numberOfVertices; i++){
            visited[i] = false;
            pre[i] = -1;
        }

        //We start with out source as our "root"
        visited[source] = true;
        Q.enQueue(source);
        //BFS algorithm that checks all pathways starting from our root
        while (!Q.isEmpty()){
            int curr = Q.deQueue();
            for(int i = 0; i < adj_list[curr].size(); i++){
                //If we haven't visited the adjacent node
                //We also then need to set the adj node as visited and put the previous of that node to the current adj list master
                if(visited[adj_list[curr][i]] == false){
                    visited[adj_list[curr][i]] = true;
                    pre[adj_list[curr][i]] = curr;
                    Q.enQueue(adj_list[curr][i]);

                    //ends if we are able to find the destination node (omega)
                    if(adj_list[curr][i] == omega)
                       return true;
                }
            }
        }

        //false if none of the connected nodes are the omega
        return false;
        }

    //Constructor
    AIDispatcher(LevelDispatcher *level, BallisticDispatcher *projectiles){
        numOfSites = 0;
        numOfVertices = level->height * level->width;
        adjList.resize(numOfVertices);
        for (int i = 0; i < numOfVertices; i++){
            int currX = i / level->width;
            int currY = i % level->width;

            int upIndex = i - level->width;
            int downIndex = i + level->width;
            int rightIndex = i + 1;
            int leftIndex = i - 1;

            int upX = currX - 1;
            int downX = currX + 1;
            int rightY = currY + 1;
            int leftY = currY - 1;

            if (level->points[currX][currY].isBombsite){
                bombsiteOptions.push_back(currX * level->width + currY);
                numOfSites++;
            }

            if (upX >= 0 && upX < level->height && upIndex >= 0  && level->points[currX][currY].baseType == ' ' && level->points[upX][currY].baseType == ' '){
                adjList[i].push_back(upIndex);
            }
            if (downX < level->height && downIndex < numOfVertices && level->points[currX][currY].baseType == ' ' && level->points[downX][currY].baseType == ' '){
                adjList[i].push_back(downIndex);
            }
            if (rightY < level->width && rightIndex < numOfVertices && level->points[currX][currY].baseType == ' ' && level->points[currX][rightY].baseType == ' '){
                adjList[i].push_back(rightIndex);
            }
            if (leftY >= 0 && leftY < level->width && leftIndex >= 0 && level->points[currX][currY].baseType == ' ' && level->points[currX][leftY].baseType == ' '){
                adjList[i].push_back(leftIndex);

            }

        }
        levelref = level;
        ballref = projectiles;
        totalBots = 0;
        for(int i = 0; i < level->height; i++)
            for(int j = 0; j < level->width; j++)
                for(int k = 0; k < level->points[i][j].entList.size(); k++){
                    if(level->points[i][j].entList[k]->whatamI() == 'T' || level->points[i][j].entList[k]->whatamI() == 'C'){
                        addBot(dynamic_cast<player_t*>(level->points[i][j].entList[k]));
                    } else if (level->points[i][j].entList[k]->whatamI() == 'p'){
                        addHuman(dynamic_cast<player_t*>(level->points[i][j].entList[k]));
                    } else if (level->points[i][j].entList[k]->whatamI() == 'b'){
                        addBomb(dynamic_cast<bomb_t*>(level->points[i][j].entList[k]));
                    }
                }
    }

    //Rid of all dead bots
    void checkForNewDead(){
        for(int i = 0; i < botList.size(); i++){
            if(!botList[i]->alive)
                delete botList[i];
        }
    }

    std::vector<player_t*> botList;
    std::vector<vector<int>> adjList;
    vector<int> sitePathway;
    vector<int> bombsiteOptions;
    int siteIndex;
    int numOfSites;
    bool chosenSite;
    player_t* human;
    bomb_t* bomb;
    LevelDispatcher* levelref;
    int totalBots;
    int numOfVertices;
    BallisticDispatcher* ballref;

    //Updates all of the AI to do a movement. They either shoot, move, move towards defusing the bomb, or move towards a bomb  (if they have it)
    void updateAll(){
        srand(time(NULL));
        int leftSide;
        int rightSide;
        bool canShoot;
        char dir[5] = {'i', 'w', 'a', 's', 'd'};
        char dirUpDown[2] = {'w', 's'};
        char dirLeftRight[2] = {'a', 'd'};
        int n = botList.size();

        //Shooting humans
        for(int i = 0; i < n; i++){
            canShoot = false;
            botList[i]->hasShot = false;
            if(human->alive && botList[i]->team != human->team && (botList[i]-> x == human->x || botList[i]-> y == human->y)){
                canShoot = true;
                if(botList[i]-> x == human->x){
                    if (botList[i]->y > human->y){
                        leftSide = human->y;
                        rightSide = botList[i]->y;
                        botList[i]->lastDirection = 'l';
                    } else {
                        leftSide = botList[i]->y;
                        rightSide = human->y;
                        botList[i]->lastDirection = 'r';
                    }
                    for(int j = leftSide + 1; j < rightSide; j++){
                            if(levelref->points[human->x][j].isWall){
                                canShoot = false;
                                break;
                                }
                    }
                } else {
                    if (botList[i]->x > human->x){
                        leftSide = human->x;
                        rightSide = botList[i]->x;
                        botList[i]->lastDirection = 'u';
                    } else {
                        leftSide = botList[i]->x;
                        rightSide = human->x;
                        botList[i]->lastDirection = 'd';
                    }
                    for(int j = leftSide + 1; j < rightSide; j++){
                        if(levelref->points[j][human->y].isWall){
                            canShoot = false;
                            break;
                            }
                    }
                }
            }
                if(canShoot){
                    botList[i]->hasShot = true;
                    MovementDispatcher::makeMove(levelref, botList[i], ' ', ballref);
                 }
        }

        //Bot on bot action
        for(int i = 0; i < n - 1; i++){
            for (int j = 0; j < n; j++){
            if(j == i){
                continue;
            }
            canShoot = false;
            if(botList[i]->alive && botList[j]->alive && botList[j]->team != botList[i]->team && (botList[j]-> x == botList[i]->x || botList[j]-> y == botList[i]->y)){
                canShoot = true;
                if(botList[j]-> x == botList[i]->x){
                    if (botList[j]->y > botList[i]->y){
                        leftSide = botList[i]->y;
                        rightSide = botList[j]->y;
                        botList[j]->lastDirection = 'l';
                        botList[i]->lastDirection = 'r';
                    } else {
                        leftSide = botList[j]->y;
                        rightSide = botList[i]->y;
                        botList[j]->lastDirection = 'r';
                        botList[i]->lastDirection = 'l';
                    }
                    for(int k = leftSide + 1; k < rightSide; k++){
                            if(levelref->points[botList[i]->x][k].isWall){
                                canShoot = false;
                                break;
                                }
                    }
                } else {
                    if (botList[j]->x > botList[i]->x){
                        leftSide = botList[i]->x;
                        rightSide = botList[j]->x;
                        botList[j]->lastDirection = 'u';
                        botList[i]->lastDirection = 'd';
                    } else {
                        leftSide = botList[j]->x;
                        rightSide = botList[i]->x;
                        botList[j]->lastDirection = 'd';
                        botList[i]->lastDirection = 'u';
                    }
                    for(int k = leftSide + 1; k < rightSide; k++){
                        if(levelref->points[k][botList[i]->y].isWall){
                            canShoot = false;
                            break;
                            }
                    }
                }
            }
                if(canShoot){
                    if(botList[i]->hasShot == false){
                        MovementDispatcher::makeMove(levelref, botList[j], ' ', ballref);
                        }
                    if(botList[i]->hasShot == false){
                    MovementDispatcher::makeMove(levelref, botList[i], ' ', ballref);
                    }
                 }

        }
        }

        //Moves towards site path if bomb is planted
        for(int i = 0; i < n; i++){
            if(levelref->bombPlanted && botList[i]->hasShot == false){
                int pre[numOfVertices];
                int botIndex = (botList[i]->x * levelref->width) + botList[i]->y;
                int bombIndex = (bomb->x * levelref->width) + bomb->y;
                bool hasPath = BFS(adjList, botIndex, bombIndex, numOfVertices, pre);
                 //path stores the path from end to beginning (pushes bomb first because thats the end)
                vector<int> path;
                path.push_back(bombIndex);
                int previousNode = bombIndex;

                //and then uses that as an index for pre which stores the previous node(s) leading to the bomb, -1 is signifies the end of the path (the source)
                while(pre[previousNode] != -1){
                    path.push_back(pre[previousNode]);
                    previousNode = pre[previousNode];
                }

                if(hasPath){
                if (path[path.size() - 2] == botIndex + 1){
                    MovementDispatcher::makeMove(levelref, botList[i], 'd', ballref);
                } else if (path[path.size() - 2] == botIndex - 1){
                    MovementDispatcher::makeMove(levelref, botList[i], 'a', ballref);
                } else if (path[path.size() - 2] == botIndex + levelref->width){
                    MovementDispatcher::makeMove(levelref, botList[i], 's', ballref);
                } else if (path[path.size() - 2] == botIndex - levelref->width){
                    MovementDispatcher::makeMove(levelref, botList[i], 'w', ballref);
                }
                }else {
                    char aiMove = dir[rand() % 5];
                    MovementDispatcher::makeMove(levelref, botList[i], aiMove, ballref);
            }
            } else if(botList[i]->bomb){ //If bot has the bomb
                if(!chosenSite){
                    siteIndex = bombsiteOptions[rand() % numOfSites];
                    chosenSite = true;
                }
                int pre[numOfVertices];
                int botIndex = (botList[i]->x * levelref->width) + botList[i]->y;
                bool hasPath = BFS(adjList, botIndex, siteIndex, numOfVertices, pre);
                 //path stores the path from end to beginning (pushes bomb first because thats the end)

                sitePathway.push_back(siteIndex);
                int previousNode = siteIndex;

                //and then uses that as an index for pre which stores the previous node(s) leading to the bomb, -1 is signifies the end of the path (the source)
                while(pre[previousNode] != -1){
                    sitePathway.push_back(pre[previousNode]);
                    previousNode = pre[previousNode];
                }

                if(hasPath && !levelref->points[botList[i]->x][botList[i]->y].isBridgeTunnel){
                if (sitePathway[sitePathway.size() - 2] == botIndex + 1){
                    MovementDispatcher::makeMove(levelref, botList[i], 'd', ballref);
                } else if (sitePathway[sitePathway.size() - 2] == botIndex - 1){
                    MovementDispatcher::makeMove(levelref, botList[i], 'a', ballref);
                } else if (sitePathway[sitePathway.size() - 2] == botIndex + levelref->width){
                    MovementDispatcher::makeMove(levelref, botList[i], 's', ballref);
                } else if (sitePathway[sitePathway.size() - 2] == botIndex - levelref->width){
                    MovementDispatcher::makeMove(levelref, botList[i], 'w', ballref);
                }
                } else if(hasPath && levelref->points[botList[i]->x][botList[i]->y].isBridgeTunnel){
                    char lastDirectionInput;
                    if(botList[i]->lastDirection == 'u'){
                        lastDirectionInput = 'w';
                    } else if(botList[i]->lastDirection == 'd'){
                        lastDirectionInput = 's';
                    } else if(botList[i]->lastDirection == 'l'){
                        lastDirectionInput = 'a';
                    } else if(botList[i]->lastDirection == 'r'){
                        lastDirectionInput = 'd';
                    }
                    MovementDispatcher::makeMove(levelref, botList[i], lastDirectionInput, ballref);
                }
            } //-<<while bot has bomb
            else if(botList[i]->hasShot == false){ //They just move if nothing to do depending on if they're in bridge tunnel
                    if(levelref->points[botList[i]->x][botList[i]->y].isBridgeTunnel){
                    char aiMove;
                    if(botList[i]->lastDirection == 'u'){
                        aiMove = dirUpDown[rand() % 2];
                    } else if(botList[i]->lastDirection == 'd'){
                        aiMove = dirUpDown[rand() % 2];
                    } else if(botList[i]->lastDirection == 'l'){
                        aiMove = dirLeftRight[rand() % 2];
                    } else if(botList[i]->lastDirection == 'r'){
                        aiMove = dirLeftRight[rand() % 2];
                    }
                    MovementDispatcher::makeMove(levelref, botList[i], aiMove, ballref);
                } else{
                    char aiMove = dir[rand() % 5];
                    MovementDispatcher::makeMove(levelref, botList[i], aiMove, ballref);
            }
        }
        }
        }
};

int main(int argc, char **argv){
    charMap *map = (argc == 2) ? new charMap(argv[1]) : NULL;
    if(map == NULL){printf("Invalid Map File\n"); return 1;}
    initCurses();

    LevelDispatcher *level = new LevelDispatcher( map );
    player_t *player1 = (level->userMenu());
    BallisticDispatcher *projectiles = new BallisticDispatcher(level);
    AIDispatcher *AI = new AIDispatcher(level, projectiles);
    char ch;

    while((ch = MovementDispatcher::readkeyInput()) != 'q'){
      MovementDispatcher::makeMove(level, player1, ch, projectiles);
      //projectiles->updateAll(); //for double the speed
      AI->updateAll();
      projectiles->updateAll();
      level->clearScreen();
      level->renderMap();
      if (level->checkRoundStatus() != 0)
        break;
    }

    delete player1; player1 = NULL;
    delete level; level = NULL;
    delete map; map = NULL;
    printw("\n GG EZ, NO RE\n");
    endCurses();
    return 0;
}

//Initializes curses
void initCurses(){

	initscr();
	raw();
	keypad(stdscr, true);
	noecho();
	printw("Welcome - Press Q to Exit\n");
}
void endCurses(){
	refresh();
	getch();
	endwin();
}

charMap::charMap(char *arg){
    char temp;
    ifstream fin(arg);
    fin >> mapName;
    fin >> height;
    fin >> temp;
    fin >> width;
    map = new char*[height];
    for(int i=0; i<height; i++){
        map[i] = new char[width];
        for(int j=0; j<width; j++)
            fin >> (map[i][j]) >> noskipws;
        fin >> skipws;
    }
}

charMap::~charMap(){
    if(map == NULL) return;
    for(int i=0; i<height; i++)
        delete [] map[i];
    delete [] map;
}

void charMap::print(){
    printw("Read Map: '%s' with dimensions %dx%d!\n",
            mapName.c_str(), height, width);
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++)
            printw("%c", map[i][j]);
        printw("\n");
    }
}
