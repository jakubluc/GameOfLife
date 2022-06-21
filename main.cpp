#include <vector>
#include <string>
#include <iostream>

#include <unistd.h>

#include <fstream>

#include <cstdlib>

#include <chrono> // dostarcza literał ms
#include <thread> // dostarcza sleep_for

int chartoint(char &c){
    int x = (int)c;
    if(c == '\000') return 0;
    if(c >= (char)(48) && c <= (char)(57)){
        x = x - 48;
        return x;
    }else return -1;
}

std::string fixInput(){
    std::string stdbuffer = "";
    std::cout << ":";
    std::getline(std::cin,stdbuffer);
    return stdbuffer;
}

int power(int x, int y){
    int buffer = x;
    if(y ==0)return 1;
    for(int i = 1; i < y; i++){
        x *= x;
    }
    return buffer;
}

int strtoint(std::string strin){
    int str_len = strin.length();
    int buffer = 0;
    for(int i =str_len - 1; i >= 0; i--){
        int k = power(10,i);
        int ln = str_len - 1 - i;
        if(chartoint(strin[ln]) == -1) return -1;
        buffer += chartoint(strin[ln])*k;
    }
    return buffer;
}

class CommandSource {
public:
    virtual std::string next_command() = 0;
    virtual ~CommandSource() { };
};

class AlwaysNextTurn : public CommandSource {
public:
    std::string next_command() override;

private:
    int turns_played = 0;
};

std::string AlwaysNextTurn::next_command() {
    using namespace std::chrono_literals;
    std::cout << std::endl;
    std::this_thread::sleep_for(200ms);

    if (turns_played++ > 50) //wtf? okey...
        return "quit";
    return "next";
}

class UserCommands : public CommandSource {
public:
    std::string next_command() override;
private:
    int turns_played = 0;
};

std::string UserCommands::next_command() {
    std::string input = fixInput();
    if(input == "quit"){
        return "quit";
    }else{
        std::cout << std::endl;
        turns_played++;
        return "next";
    }
}

class NewStateField{
public:
    //i havent found a way to pass an obj.. sorry
    virtual bool newStateField(int x, int y, int xmax, int ymax, std::vector<std::vector<bool>> board) = 0;
    virtual ~NewStateField() { };

};

class Plane : public NewStateField {
public:
    bool newStateField(int x, int y, int xmax, int ymax, std::vector<std::vector<bool>> board) override;
    bool insideOfBoard(int x, int y, int xmax, int ymax);
};

bool Plane::insideOfBoard(int x, int y, int xmax, int ymax){
    if(x >= 0 && x < xmax && y >= 0 && y < ymax){
        return 1;
    }else return 0;
}

bool Plane::newStateField(int x, int y, int xmax, int ymax, std::vector<std::vector<bool>> board){
    bool returnalive = 0;

    int alive = 0;
    for(int ix = -1; ix < 2; ix++){
        for(int iy = -1; iy < 2; iy++){
            if(insideOfBoard(x+ix,y+iy,xmax,ymax)){ //for now
                if(!(ix == 0 && iy == 0) && board[x+ix][y+iy] == 1)
                    alive++;
            }
        }
    }
    if(alive == 3) returnalive = 1;
    else{
        if(alive == 2 && board[x][y] == 1) returnalive = 1;
    }
    return returnalive;
}

class Cylinder : public NewStateField {
public:
    bool newStateField(int x, int y, int xmax, int ymax, std::vector<std::vector<bool>> board) override;
    bool insideOfBoard(int x, int xmax);
};

bool Cylinder::insideOfBoard(int x, int xmax){
    if(x >= 0 && x < xmax){
        return 1;
    }else return 0;
}

bool Cylinder::newStateField(int x, int y, int xmax, int ymax, std::vector<std::vector<bool>> board){
    bool returnalive = 0;

    int alive = 0;
    for(int ix = -1; ix < 2; ix++){
        for(int iy = -1; iy < 2; iy++){
            if(y+iy >= 0 && y+iy < ymax){
                if(insideOfBoard(x+ix,xmax)){ //for now
                    if(!(ix == 0 && iy == 0) && board[x+ix][y+iy] == 1)
                        alive++;
                }
            }else{
                if(y+iy < 0){
                    int idkhow = ymax-1;
                    if(insideOfBoard(x+ix,xmax)){ //for now
                        if(!(ix == 0 && iy == 0) && board[x+ix][idkhow] == 1)
                            alive++;
                    }
                }
                if(y+iy >= ymax){
                    int idkhow = 0;
                    if(insideOfBoard(x+ix,xmax)){ //for now
                        if(!(ix == 0 && iy == 0) && board[x+ix][idkhow] == 1)
                            alive++;
                    }
                }
            }
        }
    }
    if(alive == 3) returnalive = 1;
    else{
        if(alive == 2 && board[x][y] == 1) returnalive = 1;
    }
    return returnalive;
}

class Torus : public NewStateField {
public:
    bool newStateField(int x, int y, int xmax, int ymax, std::vector<std::vector<bool>> board) override;
    bool insideOfBoardX(int x, int xmax);
    bool insideOfBoardY(int y, int ymax);
};

bool Torus::insideOfBoardX(int x, int xmax){
    if(x >= 0 && x < xmax){
        return 1;
    }else return 0;
}

bool Torus::insideOfBoardY(int y, int ymax){
    if(y >= 0 && y < ymax){
        return 1;
    }else return 0;
}

bool Torus::newStateField(int x, int y, int xmax, int ymax, std::vector<std::vector<bool>> board){
    bool returnalive = 0;

    int alive = 0;
    for(int ix = -1; ix < 2; ix++){
        for(int iy = -1; iy < 2; iy++){
            int convx = x + ix;
            int convy = y + iy;

            if(convx < 0) convx = xmax - 1;
            if(convx >= xmax) convx = 0;
            if(convy < 0) convy = ymax - 1;
            if(convy >= ymax) convy = 0;

            if(!(ix == 0 && iy == 0) && board[convx][convy] == 1)
                alive++;
        }
    }
    if(alive == 3) returnalive = 1;
    else{
        if(alive == 2 && board[x][y] == 1) returnalive = 1;
    }
    return returnalive;
}

class GameOfLife{
public:
    GameOfLife();

    void loadBoard();
    void saveBoard() const;
    void debug_display() const;

    bool insideOfBoard(int x, int y);

    //bool newStateField(int x, int y);
    std::string type = "Cylinder";
    std::string boardL = "000000000000000000000000000000000000000000000000000000000000000000000000000000\n011000000000000000000011000000000000000000000000000000000000000000000000000000\n011000000111000000000011000000000000000000000000000000000000000000000000000000\n000000000000000000000000110000000000000000000000000000000000000000000000000000\n000000000000000000000000110000000000000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000000000000000000000000000000000010000\n000000000000000000000000000000000000000000000000000000000000000000000000001000\n000000000000000000000000000000000000000000000000000000000000000000000000111000\n000000000000000000000000000000000000000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000000000000000000000000000000000000000";
//public until release
    int xmax;
    int ymax;
    std::vector<std::vector<bool>> board;

    void nextStateBoard(Plane field);
    void nextStateBoard(Cylinder field);
    void nextStateBoard(Torus field);

    void play(CommandSource& chosenCommandSource);
    int boardType;
private:
};



void GameOfLife::nextStateBoard(Plane field){
    std::vector<std::vector<bool>> boardB = board;
    for(int x = 0; x < xmax; x++){
        for(int y = 0; y < ymax; y++){
            boardB[x][y] = field.newStateField(x,y,xmax,ymax,board);
        }
    }
    board = boardB;

}

void GameOfLife::nextStateBoard(Cylinder field){
    std::vector<std::vector<bool>> boardB = board;
    for(int x = 0; x < xmax; x++){
        for(int y = 0; y < ymax; y++){
            boardB[x][y] = field.newStateField(x,y,xmax,ymax,board);
        }
    }
    board = boardB;

}

void GameOfLife::nextStateBoard(Torus field){
    std::vector<std::vector<bool>> boardB = board;
    for(int x = 0; x < xmax; x++){
        for(int y = 0; y < ymax; y++){
            boardB[x][y] = field.newStateField(x,y,xmax,ymax,board);
        }
    }
    board = boardB;

}

GameOfLife::GameOfLife(){
    loadBoard();

}

void GameOfLife::loadBoard(){
    //add load string from file here



    std::vector<std::vector<bool>> boardB;

    GameOfLife::ymax = 0;  // 13 number of things in line
    while(1){
        if(boardL[GameOfLife::ymax] == '\n'){
            //ymax++;
            break;
        }else GameOfLife::ymax++;
    }

    for(int x = 0; x < (int)boardL.length(); x+= ymax){
        std::vector<bool> boardBuffer;
        for(int y = x; y < ymax + x; y++){
            bool buff = chartoint(boardL[y]);
            boardBuffer.push_back(buff);
        }
        x++;
        boardB.push_back(boardBuffer);
    }
    board = boardB;
    GameOfLife::xmax = (int)board.size(); // 5 number of next lines
}

void GameOfLife::saveBoard() const{
    std::string boardS = "";
    for(int x = 0; x < xmax; x++){
        for(int y = 0; y < ymax; y++){
            boardS += (char)(board[x][y] + 48);
        }
        boardS += "\n";
    }
    std::ofstream ifs("savedGameState.txt");
    ifs << type << "\n";
    ifs << xmax << "\n";
    ifs << ymax << "\n";
    ifs << boardS;

    //add upload to file here
}

void GameOfLife::debug_display() const{
    for(int x = 0; x < xmax; x++){
        for(int y = 0; y < ymax; y++){
            std::cout << board[x][y];
        }
        std::cout << "\n";
    }
}
/*
//test this much more
bool GameOfLife::newStateField(int x, int y){
    bool returnalive = 0;

    int alive = 0;
    for(int ix = -1; ix < 2; ix++){
        for(int iy = -1; iy < 2; iy++){
            if(insideOfBoard(x+ix,y+iy)){ //for now
                if(!(ix == 0 && iy == 0) && board[x+ix][y+iy] == 1)
                    alive++;
            }
        }
    }
    if(alive == 3) returnalive = 1;
    else{
        if(alive == 2 && board[x][y] == 1) returnalive = 1;
    }
    return returnalive;
}
*/





void GameOfLife::play(CommandSource& chosenCommandSource){
    std::string next_command = "a";
    Plane boardTypePlane;
    Cylinder boardTypeCylinder;
    Torus boardTypeTorus;
    std::string plane = "Plane"; //dumb way to do this but i dont care
    std::string cylinder = "Cylinder";
    std::string torus = "Torus";
    std::string type = GameOfLife::type;

    while(true){
        next_command = chosenCommandSource.next_command();
        if(next_command == "quit") break;
        if(type == plane){
            nextStateBoard(boardTypePlane);
        }else{
            if(type == cylinder){
                nextStateBoard(boardTypeCylinder);
            }else{
                if(type == torus){
                    nextStateBoard(boardTypeTorus);
                }
            }
        }
        debug_display();
    }
    saveBoard();
}



int main(){

    AlwaysNextTurn auto_command_source;
    UserCommands user_command_source;
    GameOfLife game;

    std::string wybrany_tryb;
    std::string wybrany_tryb_planszy;

    std::ifstream ifs("savedGameState.txt");
    if(ifs.is_open()){
        std::vector <std::string> line;
        std::string lineB;
        int i = 0;
        while(getline(ifs, lineB)) {
            line.push_back(lineB);
        }
        wybrany_tryb_planszy = line[0];
        game.xmax = strtoint(line[1]);
        game.ymax = strtoint(line[2]);

        std::string boardTBL = "";
        for(int i = 3; i < line.size();i++){
            boardTBL += line[i];
            boardTBL += "\n";
        }

        game.boardL = boardTBL;

        ifs.close();

        game.loadBoard();
    }else{
        //game.debug_display();
        std::cout << "Wybierz typ planszy [Plane, Cylinder, Torus]: ";
        std::cin >> wybrany_tryb_planszy; // dla chętnych: można pozyskać tę wartość z parametru programu podanego przy uruchomieniu, zamiast pytać o nią użytkownika już w trakcie działania
    }
    std::cout << "Wybierz źródło komend [user, auto]: ";
    std::cin >> wybrany_tryb; // dla chętnych: można pozyskać tę wartość z parametru programu podanego przy uruchomieniu, zamiast pytać o nią użytkownika już w trakcie działania

    game.type = wybrany_tryb_planszy;

    if (wybrany_tryb == "user")
        game.play(user_command_source);
    else if (wybrany_tryb == "auto")
        game.play(auto_command_source);



}
