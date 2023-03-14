#pragma once

#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Piece {
    
private:
    string type;
    string colour;
    int moveCount;
    int value;
    
public:
    // constructor for an empty square placeholder
    Piece(){
        this->type =  "empty";
        this->colour = "none";
        moveCount = 0;
        value = 0;
    }
    
    Piece(string type, string colour){
        this->type =  type;
        this->colour = colour;
        moveCount = 0;
        value = this->GetValue();
    }
    
    int GetValue(){
        if (this->type != "empty") {
            map<string, int> pieceValues = {{"King", 10000}, {"Queen", 975}, {"Rook", 525}, {"Bishop", 325}, {"Knight", 325}, {"Pawn", 100}};
            return pieceValues[type];
        }
        else{
            return 0;
        }
    }
    
    int GetMVV_LVAindex(){
        map<string, int> MVV_LVAindex = {{"King", 0}, {"Queen", 1}, {"Rook", 2}, {"Bishop", 3}, {"Knight", 4}, {"Pawn", 5}, {"empty", 6}};
        return MVV_LVAindex[type];
    }
    
    int PieceNumber(){
        map<string, int> pieceNumbers = {
            {"wPa", 1}, {"wKn", 2}, {"wBi", 3}, {"wRo", 4}, {"wQu", 5}, {"wKi", 6},
            {"bPa", 7}, {"bKn", 8}, {"bBi", 9}, {"bRo", 10}, {"bQu", 11}, {"bKi", 12}
        };
        return pieceNumbers[this->PieceID()];
    }
    
    string PieceID(){
        string ID = "";
        ID.push_back(colour[0]);
        ID.push_back(type[0]);
        ID.push_back(type[1]);
        return ID;
    }
    
    string Type(){
        return this->type;
    }
    
    void SetType(string type){
        this->type = type;
    }
    
    string Colour(){
        return this->colour;
    }
    
    void SetMoveCount(int n){
        this->moveCount = n;
    }
    
    void IncrementMoveCount(){
        this->moveCount++;
    }
    
    bool GetMoveCount(){
        return this->moveCount;
    }
};
