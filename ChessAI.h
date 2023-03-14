#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "Moves.h"
#include "GameClass.hpp"
#include "Pieces.hpp"
#include <fstream>
#include <string>
using namespace std;

class baseAI{
    
protected:
    int DEPTH;
    int ply = 0;
    map<char, int> columns = {{'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4}, {'f', 5}, {'g', 6}, {'h', 7}};
    map<string, double> PieceValues = {{"King", 10000}, {"Queen", 975}, {"Rook", 525}, {"Bishop", 335}, {"Knight", 320}, {"Pawn", 100}};
    Move bestMoveSoFar {};
    Move bestMoveFinal {};
    
public:
    
    baseAI(int depth){
        DEPTH = depth;
    }
    
    double MiniMax(Game game, int depth){
        if(depth==0){
            return BoardEvaluation(game);
        }

        // initialising the value to negative "infinity"
        int maximumScore = -1000000;
        vector<Move> moves = game.PseudoLegalMoves();

        for(Move move : moves){
            ply++;
            game.MakeMove(move);
            double score = -MiniMax(game, depth-1);
            ply--;

            game.UndoMove();

            if(score > maximumScore){
                maximumScore = score;

                // if move is a root node
                if(ply==0){
                    bestMoveSoFar = move;
                }
            }
        }
        // initialise move to return
        bestMoveFinal = bestMoveSoFar;

        return maximumScore;
    }
    
    // abstract functions
    virtual double QuiescenceSearch(Game game, double alpha, double beta, int depth) = 0;
    
    virtual double BoardEvaluation(Game game) = 0;
    
    virtual Move GetAIMove(Game game) = 0;
};


class AI : baseAI{
private:
    map<char, int> columns = {{'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4}, {'f', 5}, {'g', 6}, {'h', 7}};

    // MVV_LVA indexed by [victim][attacker]
    int MVV_LVA[7][7] = {
        {0,  0,  0,  0,  0,  0,  0}, // victim King
        {50, 51, 52, 53, 54, 55, 0}, // victim Queen
        {40, 41, 42, 43, 44, 45, 0}, // victim Rook
        {30, 31, 32, 33, 34, 35, 0}, // victim Bishop
        {20, 21, 22, 23, 24, 25, 0}, // victim Knight
        {10, 11, 12, 13, 14, 15, 0}, // victim Pawn
        {0,  0,  0,  0,  0,  0,  0}, // Empty Square
    };

    // killer moves indexed by [id][ply]
    Move emptyMove {};
    vector<vector<Move>> killerMoves = {};

    // history moves indexed by [piece][square]
    vector<vector<int>> historyMoves = {};

    vector<vector<double>> KnightScores = {{-50,-40,-30,-30,-30,-30,-40,-50},
                                           {-40,-20,  0,  0,  0,  0,-20,-40},
                                           {-30,  0, 15, 15, 15, 15,  0,-30},
                                           {-30,  5, 15, 20, 20, 15,  5,-30},
                                           {-30,  0, 15, 20, 20, 15,  0,-30},
                                           {-30,  5, 15, 15, 15, 15,  5,-30},
                                           {-40,-20,  0,  5,  5,  0,-20,-40},
                                           {-50,-40,-30,-30,-30,-30,-40,-50}};

    vector<vector<double>> BishopScores = {{-20,-10,-10,-10,-10,-10,-10,-20},
                                          {-10, 10, 10, 10, 10, 10, 10,-10},
                                          {-10,  0, 10, 10, 10, 10,  0,-10},
                                          {-10,  8,  5, 10, 10,  5,  8,-10},
                                          {-10,  8,  5, 10, 10,  5,  8,-10},
                                          {-10,  0, 10, 10, 10, 10,  0,-10},
                                          {-10, 10, 10, 10, 10, 10, 10,-10},
                                          {-20,-10,-10,-10,-10,-10,-10,-20}};

    vector<vector<double>> QueenScores = {{-20,-10,-10,-5,-5,-10,-10,-20},
                                       {-10,  0,  0,  0,  0,  0,  0, -10},
                                       {-10,  0,  5,  5,  5,  5,  0, -10},
                                       {-5,   0,  5,  5,  5,  5,  0,  -5},
                                       {-5,   0,  5,  5,  5,  5,  0,  -5},
                                       {-10,  0,  5,  5,  5,  5 , 0, -10},
                                       {-10,  0,  0,  0,  0,  0,  0, -10},
                                       {-20,-10,-10,-5,-5, -10, -10, -20}};

    vector<vector<double>> RookScores = {{0,  0,  0,  5,  5,  0,  0,  0},
                                         {5,  10, 10, 10, 10, 10, 10, 5},
                                         {-5,  0,  0,  0,  0,  0,  0,-5},
                                         {-5,  0,  0,  0,  0,  0,  0,-5},
                                         {-5,  0,  0,  0,  0,  0,  0,-5},
                                         {-5,  0,  0,  0,  0,  0,  0,-5},
                                         {5 , 10, 10, 10, 10, 10, 10, 5},
                                         {0,  0,  0,  5,  5,  0,  0,  0}};

    vector<vector<double>> WhitePawnScores = {{0 ,  0,  0,  0,  0,  0,  0,  0},
                                              {50, 50, 50, 50, 50, 50, 50, 50},
                                              {10, 10, 20, 30, 30, 20, 10, 10},
                                              {5,  5, 10, 25, 25,  10,  5,  5},
                                              {0,  0,  0, 20, 20,   0,  0,  0},
                                              {5, -5, -10,  0,  0, -10, -5, 5},
                                              {5, 10, 10, -20, -20, 10, 10, 5},
                                              {0,  0,  0,  0,   0,  0,  0,  0}};

    vector<vector<double>> BlackPawnScores = {{0,  0,  0,  0,  0,  0,  0,  0},
                                              {5, 10, 10, -20,-20, 10, 10, 5},
                                              {5, -5, -10, 0,  0, -10, -5, 5},
                                              {0 , 0,  0, 20, 20,  0,  0,  0},
                                              {5,  5,  10, 25, 25, 10,  5, 5},
                                              {10, 10, 20, 30, 30, 20, 10, 10},
                                              {50, 50, 50, 50, 50, 50, 50, 50},
                                              {0,  0,  0,  0,  0,  0,  0,  0}};

    vector<vector<double>> BlackKingScores = {{20, 30, 10,  0,  0,  10, 30, 20},
                                              { 0,  0,  -5, -5, -5, -5,  0,  0},
                                              {-10,-20,-20,-20,-20,-20,-20,-10},
                                              {-20,-30,-30,-40,-40,-30,-30,-20},
                                              {-30,-40,-40,-50,-50,-40,-40,-30},
                                              {-30,-40,-40,-50,-50,-40,-40,-30},
                                              {-30,-40,-40,-50,-50,-40,-40,-30},
                                              {-30,-40,-40,-50,-50,-40,-40,-30}};

    vector<vector<double>> WhiteKingScores = {{-30,-40,-40,-50,-50,-40,-40,-30},
                                              {-30,-40,-40,-50,-50,-40,-40,-30},
                                              {-30,-40,-40,-50,-50,-40,-40,-30},
                                              {-30,-40,-40,-50,-50,-40,-40,-30},
                                              {-20,-30,-30,-40,-40,-30,-30,-20},
                                              {-10,-20,-20,-20,-20,-20,-20,-10},
                                              { 0,  0,  -5, -5, -5, -5,  0,  0},
                                              {20, 30, 10,  0,  0, 10, 30, 20}};

    
public:
    
    AI(int depth) : baseAI(depth){
        this->DEPTH = depth;
    }

    string MoveToString(Game &game, Move &move){
        map<int, char> columnsReversed;
        for (map<char, int>::iterator i = columns.begin(); i != columns.end(); ++i)
            columnsReversed[i->second] = i->first;

        string moveString = "";
        // start square
        moveString += columnsReversed[get<1>(move.StartSq())];
        moveString += to_string(8-get<0>(move.StartSq()));
        //end square
        moveString += columnsReversed[get<1>(move.EndSq())];
        moveString += to_string(8-get<0>(move.EndSq()));
        return moveString;
    }


    // abstract function from base class completed
    double QuiescenceSearch(Game game, double alpha, double beta, int depth) override {
        double standPat = BoardEvaluation(game);

        if(depth==0){
            return standPat;
        }
        else{
            if(standPat >= beta){
                return beta;
            }
            if(standPat > alpha){
                alpha = standPat;
            }

            vector<Move> captureMoves = game.GenerateLegalMoves();
            if(captureMoves.size() != 0){
                OrderMoves(captureMoves);
            }

            for(Move move : captureMoves){
                if(move.IsCapture()){
                    game.MakeMove(move);
                    ply++;
                    double score = -QuiescenceSearch(game, -beta, -alpha, depth-1);
                    ply--;
                    game.UndoMove();

                    if(score >= beta){
                        return beta;
                    }
                    if(score > alpha){
                        alpha = score;
                    }
                }
                else{
                    continue;
                }
            }
            return alpha;
        }
    }



    double AlphaBeta(Game game, int depth, double alpha, double beta){
        
        if(depth==0){
            return QuiescenceSearch(game, alpha, beta, 4);
        }
        
        double previousAlpha = alpha;

        vector<Move> moves = game.GenerateLegalMoves();
        if(game.IsCheckmate(moves)){
            return -100000 + ply;
        }
        else if(game.IsStalemate(moves)){
            return 0;
        }
        OrderMoves(moves);

        for(Move move : moves){
            ply++;

            game.MakeMove(move);
            double score = -AlphaBeta(game, depth-1, -beta, -alpha);
            ply--;

            game.UndoMove();

            if(score >= beta){
                killerMoves[1][ply] = killerMoves[0][ply];
                killerMoves[0][ply] = move;
                return beta;
            }

            if(score > alpha){
                alpha = score;
                historyMoves[move.PieceMoved().PieceNumber()-1][move.EndSqID()] += depth;
                
                if(ply==0){
                    bestMoveSoFar = move;
                    cout << MoveToString(game, move) << ", score: " << score << endl;
                }
            }
        }
        if(previousAlpha != alpha){
            bestMoveFinal = bestMoveSoFar;
        }

        return alpha;
    }


    Move GetAIMove(Game game) override {
        Move emptyMove {};
        killerMoves.resize(2, vector<Move>(64, emptyMove));
        historyMoves.resize(12, vector<int>(89, 0));

        // Iterative deepening implementation
        for(int depth=1; depth <= DEPTH; depth++){
            AlphaBeta(game, depth, -1000000, 1000000);
        }
        
        return bestMoveFinal;
    }


    vector<int> ScoreAllMoves(vector<Move> &moves){  // using heuristics to guess good moves before they are searched
        vector<int> scores;

        // assigning each move a score
        for(Move move : moves){
            scores.push_back(MVV_LVA[move.PieceCaptured().GetMVV_LVAindex()][move.PieceMoved().GetMVV_LVAindex()]+100000);
            if(!move.IsCapture()){
                if(killerMoves[0][ply].MoveID() == move.MoveID()){
                    scores.push_back(100000);
                }
                else if(killerMoves[1][ply].MoveID() == move.MoveID()){
                    scores.push_back(50000);
                }
                else{
                    Piece piece = move.PieceMoved();
                    if(piece.Type() != "empty"){
                        int historyScore = historyMoves[piece.PieceNumber()-1][move.EndSqID()];
                        scores.push_back(historyScore);
                        
                    }
                }
            }
        }
        return scores;
    }

    
    void Merge(vector<int> &scores, vector<Move> &moves, int left, int mid, int right) {
        vector<int> leftScores(mid - left + 1);
        vector<Move> leftMoves(mid - left + 1);
        vector<int> rightScores(right - mid);
        vector<Move> rightMoves(right - mid);
        
        for(int i = 0; i < mid - left + 1; i++){
            leftScores[i] = scores[left + i];
            leftMoves[i] = moves[left + i];
        }
        
        for(int i = 0; i < right - mid; i++){
            rightScores[i] = scores[mid + 1 + i];
            rightMoves[i] = moves[mid + 1 + i];
        }
        
        int i = 0;
        int j = 0;
        int k = left;
        
        while(i < mid - left + 1 && j < right - mid){
            if(leftScores[i] >= rightScores[j]){
                scores[k] = leftScores[i];
                moves[k] = leftMoves[i];
                i++;
            }
            else{
                scores[k] = rightScores[j];
                moves[k] = rightMoves[j];
                j++;
            }
            k++;
        }
        
        while(i < mid - left + 1){
            scores[k] = leftScores[i];
            moves[k] = leftMoves[i];
            i++;
            k++;
        }
        
        while(j < right - mid){
            scores[k] = rightScores[j];
            moves[k] = rightMoves[j];
            j++;
            k++;
        }
    }

    void MergeSort(vector<int> &scores, vector<Move> &moves, int left, int right) {
        if(left >= right){
            return;
        }
        
        int mid = left + (right - left) / 2;
        MergeSort(scores, moves, left, mid);
        MergeSort(scores, moves, mid + 1, right);
        Merge(scores, moves, left, mid, right);
    }

    void MergeSort(vector<int> &scores, vector<Move> &moves) {
        MergeSort(scores, moves, 0, moves.size() - 1);
    }

    
    
    void OrderMoves(vector<Move> &moves){  // scoring and ordering moves to get the AI to search "better" moves first
        vector<int> scores = ScoreAllMoves(moves);
        if(scores.size() != 0 && moves.size() != 0){
            MergeSort(scores, moves);
        }
        else{
            return;
        }
    }


    double BoardEvaluation(Game game) override {  // evaluating the current board
        double totalScore = 0;
        if(game.Checkmate()){
            return (game.WhiteToMove())? -100000000 : 100000000;
        }
        else if(game.Stalemate()){
            return 0;
        }

        for(int rank=0; rank<8; rank++){
            for(int file=0; file<8; file++){
                if(game.GetBoard()[rank][file].Type() != "empty"){
                    double pieceScore = 0;
                    if(game.GetBoard()[rank][file].Type() == "Knight"){
                        pieceScore = KnightScores[rank][file];
                    }
                    else if(game.GetBoard()[rank][file].Type() == "Bishop"){
                        pieceScore = BishopScores[rank][file];
                    }
                    else if(game.GetBoard()[rank][file].Type() == "Rook"){
                        pieceScore = RookScores[rank][file];
                    }
                    else if(game.GetBoard()[rank][file].Type() == "Queen"){
                        pieceScore = QueenScores[rank][file];
                    }
                    else if(game.GetBoard()[rank][file].Type() == "Pawn" && game.GetBoard()[rank][file].Colour() == "white"){
                        pieceScore = WhitePawnScores[rank][file];
                    }
                    else if(game.GetBoard()[rank][file].Type() == "Pawn" && game.GetBoard()[rank][file].Colour() == "black"){
                        pieceScore = BlackPawnScores[rank][file];
                    }
                    else if(game.GetBoard()[rank][file].Type() == "King" && game.GetBoard()[rank][file].Colour() == "white"){
                        pieceScore = WhiteKingScores[rank][file];
                    }
                    else if(game.GetBoard()[rank][file].Type() == "King" && game.GetBoard()[rank][file].Colour() == "black"){
                        pieceScore = BlackKingScores[rank][file];
                    }
                    if(game.GetBoard()[rank][file].Colour() == "white"){
                        totalScore += (PieceValues[game.GetBoard()[rank][file].Type()] + (pieceScore));
                    }
                    else if(game.GetBoard()[rank][file].Colour() == "black"){
                        totalScore -= (PieceValues[game.GetBoard()[rank][file].Type()] + (pieceScore));
                    }
                }
            }
        }

        return (game.WhiteToMove())? totalScore : -totalScore;
    }
};
