#pragma once
using namespace std;

#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>
#include <tuple>
#include "Pieces.hpp"
#include "Moves.h"
#include "Castling.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/ostreamwrapper.h"


class Game {
    
private:
    vector<vector<Piece>> board;
    const Piece emptySqPlaceholder = Piece();
    
    stack<Move> movesMade;
    CastlingRights castlingRights = CastlingRights();
    bool whiteToMove;
    bool checkmate;
    bool stalemate;
    bool inCheck;

    tuple<int, int> whiteKingSq = {7, 4};
    tuple<int, int> blackKingSq = {0, 4};
    
public:
    Game(){
        // preparing board
        this->FillBoardVectorsWithEmptySquares();
        this->LoadGame();

        whiteToMove = true;
        checkmate = false;
        stalemate = false;
        inCheck = false;
    }
    
    
    void SetUpBoard(){
        // resizes board to 8x8 and fills it with pieces
        this->FillBoardVectorsWithEmptySquares();
        this->InstantiatePieces();
    }
    
    void ResetGame(){
        this->ClearBoard();
        this->SetUpBoard();
        this->SaveGame();
    }
    
    void ClearBoard(){
        for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){
                board[i][j] = emptySqPlaceholder;
            }
        }
    }
    
    void FillBoardVectorsWithEmptySquares(){
        board.resize(8, vector<Piece>(8, emptySqPlaceholder));
    }
    
    void InstantiatePieces(){
        map <int, string> piecesTable = {{0, "Rook"}, {1, "Knight"}, {2, "Bishop"}};

        for(int i=0; i<board.size(); i++){
            board[1][i] = Piece("Pawn", "black");
            board[6][i] = Piece("Pawn", "white");
            if(i < 3){
                board[0][i] = Piece(piecesTable[i], "black");
                board[0][7-i] = Piece(piecesTable[i], "black");
                board[7][i] = Piece(piecesTable[i], "white");
                board[7][7-i] = Piece(piecesTable[i], "white");
            }
        }
        board[0][3] = Piece("Queen", "black");
        board[7][3] = Piece("Queen", "white");
        board[0][4] = Piece("King", "black");
        board[7][4] = Piece("King", "white");
    }
    
    void UpdateKingPosition(Move &move, bool undo=false){
        if(undo){
            if(move.PieceMoved().Colour() == "white"){
                whiteKingSq = {get<0>(move.StartSq()), get<1>(move.StartSq())};
            }
            else{
                blackKingSq = {get<0>(move.StartSq()), get<1>(move.StartSq())};
            }
        }
        else{
            if(move.PieceMoved().Colour() == "white"){
                whiteKingSq = {get<0>(move.EndSq()), get<1>(move.EndSq())};
            }
            else{
                blackKingSq = {get<0>(move.EndSq()), get<1>(move.EndSq())};
            }
        }
    }
    
    bool KingHasMoved(vector<Piece> pieces){
        for(Piece piece : pieces){
            cout << piece.GetMoveCount() << endl;
            if(piece.Type() == "King" && piece.GetMoveCount() != 0){
                return true;
            }
        }
        return false;
    }
    
    
    bool IsEmptySq(int rank, int file){
        return (board[rank][file].Type().compare("empty") == 0)? true : false;
    }
    
    bool SquareOnBoard(int i, int j){
        // checks if square is within 8x8 board
        if(((0 <= i)&&(i < 8)) && ((0 <= j)&&(j < 8))){
            return true;
        }
        return false;
    }
    
    bool SquareIsAttacked(int rank, int file, string friendlyColour){
        whiteToMove = !whiteToMove;
        vector<Move> enemyMoves = this->PseudoLegalMoves();
        whiteToMove = !whiteToMove;
        for(Move move : enemyMoves){
            if(get<1>(move.EndSq()) == rank && get<0>(move.EndSq()) == file && move.PieceMoved().Colour() != friendlyColour){
                return true;
            }
        }
        return false;
    }

    bool Checkmate(){
        return this->checkmate;
    }
    
    bool Stalemate(){
        return this->stalemate;
    }
    
    bool WhiteToMove(){
        return this->whiteToMove;
    }
    
    void SwitchSides(){
        this->whiteToMove = !whiteToMove;
    }
    
    vector<vector<Piece>> GetBoard(){
        return this->board;
    }
        
    stack<Move> MoveLog(){
        return this->movesMade;
    }
    
    void MakeMove(Move move){
        this->whiteToMove = !whiteToMove;
        tuple<int, int> moveEnd = move.EndSq();
        tuple<int, int> moveStart = move.StartSq();

        Piece pieceMoved = move.PieceMoved();
        board[get<0>(moveEnd)][get<1>(moveEnd)] = pieceMoved;
        board[get<0>(moveStart)][get<1>(moveStart)] = emptySqPlaceholder;
        
        if(pieceMoved.Type() == "King"){
            UpdateKingPosition(move);
            
            if(pieceMoved.Colour() == "white"){
                castlingRights.SetWhiteKingMoved(true);
            }
            else if(pieceMoved.Colour() == "black"){
                castlingRights.SetBlackKingMoved(true);
            }
            
            if(move.Castled()){
                string type = move.CastleType();
                if(type == "WQS"){
                    castlingRights.SetWhiteQueenside(false);
                    board[7][3] = Piece("Rook", "white");
                    board[7][0] = emptySqPlaceholder;
                }
                else if(type == "WKS"){
                    castlingRights.SetWhiteKingside(false);
                    board[7][5] = Piece("Rook", "white");
                    board[7][7] = emptySqPlaceholder;
                }
                else if(type == "BQS"){
                    castlingRights.SetBlackQueenside(false);
                    board[0][3] = Piece("Rook", "black");
                    board[0][0] = emptySqPlaceholder;
                }
                else if(type == "BKS"){
                    castlingRights.SetBlackKingside(false);
                    board[0][5] = Piece("Rook", "black");
                    board[0][7] = emptySqPlaceholder;
                }
            }
        }
        
        if(move.PieceMoved().Type() == "Pawn"){
            if(get<0>(move.EndSq()) == 0 && move.PieceMoved().Colour() == "white"){
                board[get<0>(move.EndSq())][get<1>(move.EndSq())] = Piece("Queen", "white");
            }
            else if(get<0>(move.EndSq()) == 7 && move.PieceMoved().Colour() == "black"){
                board[get<0>(move.EndSq())][get<1>(move.EndSq())] = Piece("Queen", "black");
            }
            
            if(move.EnPassant()){
                int moveDirection = (move.PieceMoved().Colour().compare("white") == 0)? 1 : -1;
                int pawnRank = get<0>(move.EndSq()) + moveDirection;
                int pawnFile = get<1>(move.EndSq());
                
                board[pawnRank][pawnFile] = emptySqPlaceholder;
            }
        }
        
        pieceMoved.IncrementMoveCount();
        movesMade.push(move);
    }
    
    void UndoMove(){
        if(!movesMade.empty()){
            Move move = movesMade.top();
            movesMade.pop();
            
            Piece pieceMoved = move.PieceMoved();
            board[get<0>(move.EndSq())][get<1>(move.EndSq())] = move.PieceCaptured();
            board[get<0>(move.StartSq())][get<1>(move.StartSq())] = pieceMoved;
            
            if(pieceMoved.Type() == "King"){
                UpdateKingPosition(move, true);
                
                (pieceMoved.Colour().compare("white") == 0)? castlingRights.SetWhiteKingMoved(false) : castlingRights.SetBlackKingMoved(false);
                pieceMoved.SetMoveCount(0);
                
                if(move.Castled()){
                    string type = move.CastleType();
                    if(type == "WQS"){
                        castlingRights.SetWhiteQueenside(true);
                        board[7][3] = emptySqPlaceholder;
                        board[7][0] = Piece("Rook", "white");
                    }
                    else if(type == "WKS"){
                        castlingRights.SetWhiteKingside(true);
                        board[7][5] = emptySqPlaceholder;
                        board[7][7] = Piece("Rook", "white");
                    }
                    else if(type == "BQS"){
                        castlingRights.SetBlackQueenside(true);
                        board[0][3] = emptySqPlaceholder;
                        board[0][0] = Piece("Rook", "black");
                    }
                    else if(type == "BKS"){
                        castlingRights.SetBlackKingside(true);
                        board[0][5] = emptySqPlaceholder;
                        board[0][7] = Piece("Rook", "black");
                    }
                }
            }
            
            if(move.EnPassant()){
                int moveDirection = (pieceMoved.Colour().compare("white") == 0)? 1 : -1;
                int pawnRank = get<0>(move.EnPassantSq()) + moveDirection;
                int pawnFile = get<1>(move.EnPassantSq());
                
                Piece pawn = (moveDirection == 1)? Piece("Pawn", "black") : Piece("Pawn", "white");
                board[pawnRank][pawnFile] = pawn;
            }
            
            move.PieceMoved().SetMoveCount(pieceMoved.GetMoveCount()-1);
            this->whiteToMove = !whiteToMove;
            this->inCheck = !inCheck;
        }
    }

    
    bool InCheck(){
        tuple<int, int> kingLocation = (whiteToMove)? whiteKingSq : blackKingSq;
        if(SquareIsAttacked(get<1>(kingLocation), get<0>(kingLocation), (whiteToMove)? "white" : "black")){
            this->inCheck = true;
            return true;
        }
        return false;
    }
    
    bool IsCheckmate(vector<Move> &moves){
        string colourMated = (this->whiteToMove)? "white" : "black";
        if(moves.size() == 0){
            moves = this->GenerateLegalMoves();
        }
        
        for(Move move : moves){
            if(move.PieceMoved().Colour() == colourMated){
                this->checkmate = false;
                return checkmate;
            }
        }
        
        if(this->InCheck()){
            this->checkmate = true;
        }
        return checkmate;
    }
    
    bool IsStalemate(vector<Move> &moves){
        string colourStalemated = (this->whiteToMove)? "white" : "black";
        
        for(Move move : moves){
            if(move.PieceMoved().Colour() == colourStalemated){
                stalemate = false;
                return stalemate;
            }
        }
        
        if(!this->InCheck()){
            stalemate = true;
            return stalemate;
        }
        return stalemate;
    }
    
    bool CreateMove(int i, int j, vector<Move> &moves, tuple<int, int> &startSq, tuple<int, int> &endSq){
        string enemyColour = (whiteToMove)? "black" : "white";
        int startRank = get<0>(startSq);
        int startFile = get<1>(startSq);
        
        if(SquareOnBoard(i, j)){
            if((IsEmptySq(i, j) || board[i][j].Colour() == enemyColour) && board[i][j].Colour() != board[startRank][startFile].Colour()) {
                endSq = {i, j};
                Move move {this->board, startSq, endSq};
                moves.push_back(move);
                return true;
            }
        }
        return false;
    }
    
    
    
    vector<Move> PseudoLegalMoves(bool getCapturesOnly=false){
        vector<Move> moves;
        
        // adding castling moves to vector
        GetCastlingMoves(moves);
        
        // loops through 8x8 board and calculates possible moves for each piece
        for(int rank=0; rank<8; rank++){
            for(int file=0; file<8; file++){
                if((board[rank][file].Colour() == "white" && whiteToMove)||(board[rank][file].Colour() == "black" && !whiteToMove)){
                    if(board[rank][file].Type() == "Pawn"){
                        GetPawnMoves(rank, file, moves);
                    }
                    else if(board[rank][file].Type() == "Knight"){
                        GetKnightMoves(rank, file, moves, getCapturesOnly);
                    }
                    else if(board[rank][file].Type() == "Bishop"){
                        GetBishopMoves(rank, file, moves);
                    }
                    else if(board[rank][file].Type() == "Rook"){
                        GetRookMoves(rank, file, moves);
                    }
                    else if(board[rank][file].Type() == "Queen"){
                        GetRookMoves(rank, file, moves);
                        GetBishopMoves(rank, file, moves);
                    }
                    else if(board[rank][file].Type() == "King"){
                        GetKingMoves(rank, file, moves, getCapturesOnly);
                    }
                }
            }
        }
        return moves;
    }
    

    void GetCastlingMoves(vector<Move> &moves){
        tuple<int, int> startSq;
        tuple<int, int> endSq;
        int whiteRank = 7;
        int blackRank = 0;
        
        vector<string> castlingPieceTypes = {"King", "Rook"};
        vector<Piece> whiteCastlingPieces;
        vector<Piece> blackCastlingPieces;
        
        
        for(string pieceType : castlingPieceTypes){
            for(int file=0; file<8; file++){
                if(board[whiteRank][file].Type() == pieceType){
                    whiteCastlingPieces.push_back(board[whiteRank][file]);
                }
                if(board[blackRank][file].Type() == pieceType){
                    blackCastlingPieces.push_back(board[blackRank][file]);
                }
            }
        }
        
        // updating castling rights
        ValidateCastlingRights(whiteCastlingPieces, blackCastlingPieces);
         
        // white castling moves
        if(castlingRights.WhiteQueenside() && IsEmptySq(7, 1) && IsEmptySq(7, 2)){
            startSq = whiteKingSq;
            endSq = {7, 2};
            Move move {board, startSq, endSq};
            move.SetCastled();
            move.SetCastleType("WQS");
            moves.push_back(move);
        }
        if(castlingRights.WhiteKingside() && IsEmptySq(7, 5) && IsEmptySq(7, 6)){
            startSq = whiteKingSq;
            endSq = {7, 6};
            Move move {board, startSq, endSq};
            move.SetCastled();
            move.SetCastleType("WKS");
            moves.push_back(move);
        }
        
        // black castling moves
        if(castlingRights.BlackQueenside() && IsEmptySq(0, 1) && IsEmptySq(0, 2)){
            startSq = blackKingSq;
            endSq = {0, 2};
            Move move {board, startSq, endSq};
            move.SetCastled();
            move.SetCastleType("BQS");
            moves.push_back(move);
        }
        if(castlingRights.BlackKingside() && IsEmptySq(0, 5) && IsEmptySq(0, 6)){
            startSq = blackKingSq;
            endSq = {0, 6};
            Move move {board, startSq, endSq};
            move.SetCastled();
            move.SetCastleType("BKS");
            moves.push_back(move);
        }
    }
    
    void ValidateCastlingRights(vector<Piece> whiteCastlingPieces, vector<Piece> blackCastlingPieces){
        if(whiteCastlingPieces.size() == 0){
            castlingRights.SetWhiteQueenside(false);
            castlingRights.SetWhiteKingside(false);
        }
        if(blackCastlingPieces.size() == 0){
            castlingRights.SetBlackQueenside(false);
            castlingRights.SetBlackKingside(false);
        }


        // if the castling path is clear and rook is in the correct position
        if(IsEmptySq(7, 7) && castlingRights.WhiteKingside()){
            castlingRights.SetWhiteKingside(false);
        }
        if(IsEmptySq(7, 0) && castlingRights.WhiteQueenside()){
            castlingRights.SetWhiteQueenside(false);
        }
        if(IsEmptySq(0, 7) && castlingRights.BlackKingside()){
            castlingRights.SetBlackKingside(false);
        }
        if(IsEmptySq(0, 0) && castlingRights.BlackQueenside()){
            castlingRights.SetBlackQueenside(false);
        }
    }
    
    
    
    void GetPawnMoves(int rank, int file, vector<Move> &moves, bool capturesOnly=false){
        Piece pawn = board[rank][file];
        
        tuple<int, int> startSq;
        tuple<int, int> endSq;
        
        int sideDirection = (pawn.Colour().compare("white") == 0)? -1 : 1;  // white is going up the board whereas black is going down
        int startRank = (pawn.Colour().compare("white") == 0)? 6 : 1;
        int moveAmount = sideDirection;
        string enemyColour = (pawn.Colour().compare("white") == 0)? "black" : "white";
        string friendlyColour = (pawn.Colour().compare("white"))? "black" : "white";

        if(SquareOnBoard(rank+moveAmount, file) && IsEmptySq(rank+moveAmount, file)){
            startSq = {rank, file};
            endSq = {rank+moveAmount, file};
            Move move {board, startSq, endSq};
            moves.push_back(move);
        }
        
         // double pawn push
        if(SquareOnBoard(rank+(moveAmount*2), file)){
            if(IsEmptySq(rank+moveAmount, file) && IsEmptySq(rank+(moveAmount*2), file) && rank == startRank){
                if(board[rank+(moveAmount*2)][file].Type() == "empty"){
                    startSq = {rank, file};
                    endSq = {rank+(moveAmount*2), file};
                    Move move {board, startSq, endSq};
                    moves.push_back(move);
                }
            }
        }
        
        // captures
        if(SquareOnBoard(rank+moveAmount, file+1)){
            if(board[rank+moveAmount][file+1].Colour() == enemyColour){
                startSq = {rank, file};
                endSq = {rank+moveAmount, file+1};
                Move move {board, startSq, endSq};
                moves.push_back(move);
            }
        }
        
        if(SquareOnBoard(rank+moveAmount, file-1)){
            if(board[rank+moveAmount][file-1].Colour() == enemyColour){
                startSq = {rank, file};
                endSq = {rank+moveAmount, file-1};
                Move move {board, startSq, endSq};
                moves.push_back(move);
            }
        }
        
        
        // en-passant
        if(!movesMade.empty()){
            Move previousMove = movesMade.top();
            if(abs(get<0>(previousMove.EndSq()) - get<0>(previousMove.StartSq())) == 2  && previousMove.PieceMoved().Type() == "Pawn"){
                if(previousMove.PieceMoved().Colour() == enemyColour){
                    
                    if(SquareOnBoard(rank, file+1)){
                        if(board[rank][file+1].Colour() == enemyColour && get<0>(previousMove.EndSq()) == rank && get<1>(previousMove.EndSq()) == file+1){
                            startSq = {rank, file};
                            endSq = {rank+moveAmount, file+1};
                            Move move {board, startSq, endSq};
                            move.SetEnPassantSq(endSq);
                            move.TrueEnPassant();
                            moves.push_back(move);
                        }
                    }
                    
                    if(SquareOnBoard(rank, file-1)){
                        if(board[rank][file-1].Colour() == enemyColour && get<0>(previousMove.EndSq()) == rank && get<1>(previousMove.EndSq()) == file-1){
                            startSq = {rank, file};
                            endSq = {rank+moveAmount, file-1};
                            Move move {board, startSq, endSq};
                            move.SetEnPassantSq(endSq);
                            move.TrueEnPassant();
                            moves.push_back(move);
                        }
                    }
                }
            }
        }
    
    }
    
    void GetKnightMoves(int rank, int file, vector<Move> &moves, bool capturesOnly=false){
        vector<tuple<int, int>> relativePositionVectors = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
        string friendlyColour = (whiteToMove)? "white" : "black";
        string enemyColour = (whiteToMove)? "black" : "white";
        
        // checking all 'L' shaped moves from starting square
        for(tuple<int, int> vector : relativePositionVectors){
            int endRow = rank + get<0>(vector);
            int endCol = file + get<1>(vector);
            
            if(SquareOnBoard(endRow, endCol)){
                Piece endPiece = board[endRow][endCol];
                if(endPiece.Colour() != friendlyColour || (capturesOnly && endPiece.Colour() == enemyColour)){
                    tuple<int, int> startSq = {rank, file};
                    tuple<int, int> endSq = {endRow, endCol};
                    Move move {board, startSq, endSq};
                    moves.push_back(move);
                }
            }
        }
    }
    

    
    void GetKingMoves(int rank, int file, vector<Move> &moves, bool capturesOnly=false){
        vector<tuple<int, int>> relativePositionVectors = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
        string friendlyColour = (whiteToMove)? "white" : "black";
        string enemyColour = (whiteToMove)? "black" : "white";
        
        for(tuple<int, int> vector : relativePositionVectors){
            int endRow = rank + get<0>(vector);
            int endCol = file + get<1>(vector);
            
            if(SquareOnBoard(endRow, endCol)){
                Piece endPiece = board[endRow][endCol];
                if(endPiece.Colour() != friendlyColour || (capturesOnly && endPiece.Colour() == enemyColour)){
                    tuple<int, int> startSq = {rank, file};
                    tuple<int, int> endSq = {endRow, endCol};
                    Move move {board, startSq, endSq};
                    moves.push_back(move);
                }
            }
        }
    }

    
    void GetRookMoves(int rank, int file, vector<Move> &moves){
        tuple<int, int> startSq = {rank, file};
        tuple<int, int> endSq;
        string friendlyColour = (whiteToMove)? "white" : "black";
        
        // east direction
        for(int i=file+1; i<8; i++){
            if(SquareOnBoard(rank, i) && (board[rank][i].Type() == "empty")){
                endSq = {rank, i};
                Move move {board, startSq, endSq};
                moves.push_back(move);
            }
            else{
                if(SquareOnBoard(rank, i) && (board[rank][i].Colour() != friendlyColour)){
                    endSq = {rank, i};
                    Move move {board, startSq, endSq};
                    moves.push_back(move);
                }
                break;
            }
        }
        
        // west direction
        for(int i=file-1; i>=0; i--){
            if(SquareOnBoard(rank, i) && (board[rank][i].Type() == "empty")){
                endSq = {rank, i};
                Move move {board, startSq, endSq};
                moves.push_back(move);
            }
            else{
                if(SquareOnBoard(rank, i) && (board[rank][i].Colour() != friendlyColour)){
                    endSq = {rank, i};
                    Move move {board, startSq, endSq};
                    moves.push_back(move);
                }
                break;
            }
        }
        
        // north direction
        for(int i=rank-1; i>=0; i--){
            if(SquareOnBoard(i, file) && (board[i][file].Type() == "empty")){
                endSq = {i, file};
                Move move {board, startSq, endSq};
                moves.push_back(move);
            }
            else{
                if(SquareOnBoard(i, file) && (board[i][file].Colour() != friendlyColour)){
                    endSq = {i, file};
                    Move move {board, startSq, endSq};
                    moves.push_back(move);
                }
                break;
            }
        }
        
        // south direction
        for(int i=rank+1; i<8; i++){
            if(SquareOnBoard(i, file) && (board[i][file].Type() == "empty")){
                endSq = {i, file};
                Move move {board, startSq, endSq};
                moves.push_back(move);
            }
            else{
                if(SquareOnBoard(i, file) && (board[i][file].Colour() != friendlyColour)){
                    endSq = {i, file};
                    Move move {board, startSq, endSq};
                    moves.push_back(move);
                }
                break;
            }
        }
    }
      
    
    void GetBishopMoves(int rank, int file, vector<Move> &moves, bool capturesOnly=false){
        tuple<int, int> startSq = {rank, file};
        tuple<int, int> endSq;
        
        int i = rank;
        int j = file;
        
        // bottom left to top right diagonal
        while(SquareOnBoard(i, j)){
            i -= 1;
            j += 1;
            
            if(SquareOnBoard(i, j) && board[i][j].Type() != "empty"){
                if(CreateMove(i, j, moves, startSq, endSq)){
                    break;
                }
            }

            // loop breaks when a piece is blocking the diagonal
            if(SquareOnBoard(i, j)){
                if(CreateMove(i, j, moves, startSq, endSq)){
                    continue;
                }
                else{
                    break;
                }
            }
            else{
                break;
            }
        }
        
        // top right to bottom left diagonal
        i = rank;
        j = file;
        while(SquareOnBoard(i, j)){
            i += 1;
            j -= 1;

            if(SquareOnBoard(i, j) && board[i][j].Type() != "empty"){
                if(CreateMove(i, j, moves, startSq, endSq)){
                    break;
                }
            }

            // loop breaks when a piece is blocking the diagonal
            if(SquareOnBoard(i, j)){
                if(CreateMove(i, j, moves, startSq, endSq)){
                    continue;
                }
                else{
                    break;
                }
            }
            else{
                break;
            }
        }

        // bottom right to top left diagonal
        i = rank;
        j = file;
        while(SquareOnBoard(i, j)){
            i -= 1;
            j -= 1;

            if(SquareOnBoard(i, j) && board[i][j].Type() != "empty"){
                if(CreateMove(i, j, moves, startSq, endSq)){
                    break;
                }
            }

            // loop breaks when a piece is blocking the diagonal
            if(SquareOnBoard(i, j)){
                if(CreateMove(i, j, moves, startSq, endSq)){
                    continue;
                }
                else{
                    break;
                }
            }
            else{
                break;
            }
        }


        // top left to bottom right diagonal
        i = rank;
        j = file;
        while(SquareOnBoard(i, j)){
            i += 1;
            j += 1;
            
            if(SquareOnBoard(i, j) && board[i][j].Type() != "empty"){
                if(CreateMove(i, j, moves, startSq, endSq)){
                    break;
                }
            }

            // loop breaks when a piece is blocking the diagonal
            if(SquareOnBoard(i, j)){
                if(CreateMove(i, j, moves, startSq, endSq)){
                    continue;
                }
                else{
                    break;
                }
            }
            else{
                break;
            }
        }
    }
    
    
    void RemoveMoveByID(vector<Move> &moves, Move moveToRemove){
        for(int i = moves.size()-1.0; i>=0; i--){
            if(moves[i].MoveID() == moveToRemove.MoveID()){
                moves.erase(moves.begin()+i);
            }
        }
    }
    
    
    vector<Move> GenerateLegalMoves() {
        vector<Move> legalMoves;
        
        // if we make a move and end up in check, that move was illegal and cannot be made in the first place
        for (Move move : this->PseudoLegalMoves()) {
            this->MakeMove(move);
            this->SwitchSides(); // switches turns back, because "MakeMove" function switches turns automatically
            
            if (!this->InCheck()) {
                legalMoves.push_back(move);
            }
            
            this->SwitchSides();
            this->UndoMove(); // switches turns, because "UndoMove" function switches turns automatically
        }
        return legalMoves;
    }

    
    
    string GetFEN(){
        string fen;
        int emptySpaceCount;
        
        for(int i=0; i<8; i++){
            // reset emptySpaceCount
            emptySpaceCount = 0;
            for(int j=0; j<8; j++){
                Piece piece = board[i][j];
                
                if(piece.Type() == "empty"){
                    emptySpaceCount++;
                    continue;
                }
                else {
                    if(emptySpaceCount != 0){
                        fen += to_string(emptySpaceCount);
                        emptySpaceCount = 0;
                    }
                    if(piece.Colour() == "white"){
                        // capitalising letter for a white piece
                        fen += (piece.Type() == "Knight")? toupper(piece.Type()[1]) : piece.Type()[0];
                    }
                    else{
                        // lower case letter for a black piece
                        fen += (piece.Type() == "Knight")? piece.Type()[1] : tolower(piece.Type()[0]);
                    }
                }
            }
            if(emptySpaceCount != 0){
                fen += to_string(emptySpaceCount);
            }
            if(i % 7 != 0 || i == 0){
                fen += '/';
            }
        }
        
        return fen;
    }
    
    
    vector<string> Split (string s, string delimiter) {
        size_t start = 0, end, delimiterLength = delimiter.length();
        string rank;
        vector<string> rankList;

        while ((end = s.find (delimiter, start)) != string::npos) {
            rank = s.substr (start, end - start);
            start = end + delimiterLength;
            rankList.emplace_back(rank);
        }

        rankList.push_back (s.substr (start));
        return rankList;
    }
    
    void FillPlaces(int row, int startCol, int numberOfPlaces, Piece piece){
        for(int i=0; i<numberOfPlaces; i++){
            board[row][i+startCol] = piece;
        }
    }
    
    void ReadFEN(string fen) {
        vector<string> ranks = Split(fen, "/");
        map<char, string> charPieces = {{'P', "Pawn"}, {'N', "Knight"}, {'B', "Bishop"}, {'R', "Rook"}, {'Q', "Queen"}, {'K', "King"}};
        
        int spaces = 0;
        for (int i = 0; i < ranks.size(); i++) {
            spaces = 0;
            for (int j = 0; j < ranks[i].size(); j++) {
                if (isdigit(ranks[i][j])) {
                    int numSpaces = ranks[i][j] - '0';
                    
                    FillPlaces(i, spaces, numSpaces, emptySqPlaceholder);
                    spaces += numSpaces;
                }
                else {
                    string colour = (isupper(ranks[i][j])) ? "white" : "black";
                    string type = charPieces[toupper(ranks[i][j])];
                    Piece newPiece = Piece(type, colour);
                    board[i][spaces] = newPiece;
                    spaces++;
                }
            }
        }
    }

    
    void LoadGame(){
        FILE* fp = fopen("/Users/anirudhanvijay/Desktop/programs/SDLtest1/SDLtest1/data.json", "r");
        
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        rapidjson::Document document;
        document.ParseStream(is);

        this->ReadFEN(document["fen"].GetString());
        whiteToMove = document["whiteToMove"].GetBool();
        inCheck = document["inCheck"].GetBool();
        castlingRights.SetWhiteQueenside(document["WQS"].GetBool());
        castlingRights.SetWhiteKingside(document["WKS"].GetBool());
        castlingRights.SetWhiteKingMoved(document["whiteKingMoved"].GetBool());
        castlingRights.SetBlackQueenside(document["BQS"].GetBool());
        castlingRights.SetBlackKingside(document["BKS"].GetBool());
        castlingRights.SetBlackKingMoved(document["blackKingMoved"].GetBool());
        
        fclose(fp);
    }
    
    void SaveGame(){
        // creating a writer object
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

        writer.StartObject();
        
        writer.Key("fen");
        writer.String(this->GetFEN().c_str());
        
        writer.Key("whiteToMove");
        writer.Bool(this->whiteToMove);
        
        writer.Key("inCheck");
        writer.Bool(this->inCheck);
        
        writer.Key("WQS");
        writer.Bool(castlingRights.WhiteQueenside());
        
        writer.Key("WKS");
        writer.Bool(castlingRights.WhiteKingside());
        
        writer.Key("whiteKingMoved");
        writer.Bool(castlingRights.WhiteKingHasMoved());
        
        writer.Key("BQS");
        writer.Bool(castlingRights.BlackQueenside());
        
        writer.Key("BKS");
        writer.Bool(castlingRights.BlackKingside());
        
        writer.Key("blackKingMoved");
        writer.Bool(castlingRights.BlackKingHasMoved());
        
        writer.EndObject();

        // opening the JSON file for writing
        FILE* fp = fopen("/Users/anirudhanvijay/Desktop/programs/SDLtest1/SDLtest1/data.json", "w");
        fwrite(buffer.GetString(), 1, buffer.GetSize(), fp);

        fclose(fp);
    }
    
};
