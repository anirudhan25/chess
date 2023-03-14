#pragma once

#include <iostream>
#include <tuple>
#include "Pieces.hpp"
using namespace std;


class Move{

private:
    tuple<int, int> startSq;
    tuple<int, int> endSq;
    tuple<int, int> enPassantSq;

    Piece pieceMoved;
    Piece pieceCaptured;

    bool castled;
    bool pawnPromoted;
    bool enPassant;
    
    int moveID;
    int moveScore;
    string castleType;

public:
    Move(){
        moveID = 8888;
    }
    
    Move(vector<vector<Piece>> board, tuple<int, int> start, tuple<int, int> end){
        this->castled = false;
        this->pawnPromoted = false;
        this->startSq = start;
        this->endSq = end;
        this->pieceMoved = board[get<0>(startSq)][get<1>(startSq)];
        this->pieceCaptured = board[get<0>(endSq)][get<1>(endSq)];
        this->enPassant = false;
        this->enPassantSq = {};
        this->moveID = get<0>(startSq)*1000 + get<1>(startSq)*100 + get<0>(endSq)*10 + get<1>(endSq);
        this->moveScore = 0;
    }

    tuple<int, int> StartSq() {return this->startSq;}
    tuple<int, int> EndSq() {return this->endSq;}

    Piece PieceMoved() {return this->pieceMoved;}
    Piece PieceCaptured() {return this->pieceCaptured;}

    bool Castled() {return this->castled;}
    void SetCastled() {castled=true;}
    void SetCastleType(string type) {this->castleType = type;}
    string CastleType() {return this->castleType;}
    bool PawnPromoted() {return this->pawnPromoted;}
    void TruePromotion() {pawnPromoted=true;}
    
    int MoveID(){return this->moveID;}
    int MoveScore(){return this->moveScore;}
    void SetMoveScore(int score){this->moveScore = score;}
    bool IsCapture(){return (pieceCaptured.Type() != "empty")? true : false;}
    void TrueEnPassant(){this->enPassant = true;}
    bool EnPassant(){return this->enPassant;}
    
    int EndSqID(){
        int endSqID = this->moveID - (get<0>(this->startSq)*1000 + get<1>(this->startSq)*100);
        return endSqID;
    }
    
    tuple<int, int> EnPassantSq(){
        return this->enPassantSq;
    }
    
    void SetEnPassantSq(tuple<int, int> square){
        this->enPassantSq = square;
    }
    
};

