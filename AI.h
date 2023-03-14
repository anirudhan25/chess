//#pragma once
//#include <iostream>
//#include <vector>
//#include <map>
//#include <algorithm>
//#include <string>
//#include "game.hpp"
//using namespace std;
//
//#define U64 unsigned long long
//
//class AI{
//    
//protected:
//    // search depth values
//    int depth;
//    int ply;
//    static const int maximumPly = 64;
//    
//    // aspiration window for search
//    int windowReduction = 50;
//    
//    // killer & history move ordering
//    bool orderMoves;
//    int killerMoves[2][64];
//    int historyMoves[12][maximumPly];
//    // PV table
//    int PVlength[maximumPly];
//    int PVtable[64][64];
//    
//    // Null Move Pruning related values
//    bool applyNMP;
//    const int reductionValue = 2;
//    
//    // Late Move Reduction related values
//    bool applyLMR;
//    const int LMRvalue = 3;
//    const int fullDepthMoves = 4;
//    
//    // evaluation masks
//    U64 fileMasks[64];
//    U64 rankMasks[64];
//    U64 isolatedPawnMasks[64];
//    U64 passedWhitePawnMasks[64];
//    U64 passedBlackPawnMasks[64];
//
//    // evaluation penalties / bonuses
//    const int doubledPawnPenalty = -20;
//    const int isolatedPawnPenalty = -20;
//    static constexpr int passedPawnBonus[8] = {0, 10, 30, 50, 75, 100, 150, 200};
//    static constexpr int pieceScore[12] = {100, 300, 350, 525, 975, 10000, 100, 300, 350, 525, 975, 10000};
//    static constexpr int materialScore[12] = {100, 300, 350, 525, 975, 10000, -100, -300, -350, -525, -975, -10000};
//    static constexpr int mirrorPiece[12] = {p, n, b, r, q, k, P, N, B, R, Q, K};
//    const int semiOpenFileScore = 10;
//    const int openFileScore = 15;
//    const int rookRankBonus = 15;
//    const int kingSafetyMultiplier = 5;
//    static constexpr double endgameMaterialStart = (pieceScore[R] * 2) + pieceScore[B] + pieceScore[N];
//    // to check whether the game is in the endgame stage or not
//    bool endgame = false;
//    bool adaptiveEndgameEvaluation = true;
//    string evaluation = "";
//    
//
//    // MVV LVA [attacker][victim] capture-based heuristic for move ordering
//    const int MVV_LVA[12][12] = {
//         105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
//         104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
//         103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
//         102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
//         101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
//         100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,
//
//         105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
//         104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
//         103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
//         102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
//         101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
//         100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
//    };
//
//    int pawnScore[64] =
//    {
//        90,  90,  90,  90,  90,  90,  90,  90,
//        30,  30,  30,  40,  40,  30,  30,  30,
//        20,  20,  20,  30,  30,  30,  20,  20,
//        10,  10,  10,  20,  20,  10,  10,  10,
//         5,   5,  10,  25,  25,   10,   5,   5,
//         0,   0,   0,   5,   5,   0,   0,   0,
//         0,   0,  -5, -10, -10,  -5,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0
//    };
//
//    // knight positional score
//    const int knightScore[64] =
//    {
//        -5,   0,   0,   0,   0,   0,   0,  -5,
//        -5,   0,   0,  10,  10,   0,   0,  -5,
//        -5,   5,  20,  20,  20,  20,   5,  -5,
//        -5,  10,  20,  30,  30,  20,  10,  -5,
//        -5,  10,  20,  30,  30,  20,  10,  -5,
//        -5,   5,  20,  10,  10,  20,   5,  -5,
//        -5,   0,   0,   0,   0,   0,   0,  -5,
//        -5, -10,   0,   0,   0,   0, -10,  -5
//    };
//
//    // bishop positional score
//    const int bishopScore[64] =
//    {
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,  10,  10,   0,   0,   0,
//         0,   0,  10,  20,  20,  10,   0,   0,
//         0,   0,  10,  20,  20,  10,   0,   0,
//         0,  10,   0,   0,   0,   0,  10,   0,
//         0,  30,   0,  10,  10,   0,  30,   0,
//         0,   0, -10,   0,   0, -10,   0,   0
//
//    };
//
//    int rookScore[64] =
//    {
//        50,  50,  50,  50,  50,  50,  50,  50,
//        50,  50,  50,  50,  50,  50,  50,  50,
//         0,   0,  10,  20,  20,  10,   0,   0,
//         0,   0,  10,  20,  20,  10,   0,   0,
//         0,   0,  10,  20,  20,  10,   0,   0,
//         0,   0,  10,  20,  20,  10,   0,   0,
//         0,   0,  10,  20,  20,  10,   0,   0,
//         0,   0,   0,  20,  20,   0,   0,   0
//
//    };
//
//    int kingScore[64] =
//    {
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,  -5,  -5,  -5,  -5,   0,   0,
//         0,   0,   20,  -5, -15, -5,   20,  0
//    };
//
//    int blackKingScore[64] =
//    {
//         0,   0,  25,   0,  -15,  0,  25,   0,
//         0,  -5,  -5,  -5,  -5,  -5,  -5,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0,
//         0,   0,   0,   0,   0,   0,   0,   0
//    };
//
//    int pawnEndgameScore[64] =
//    {
//        0,   0,   0,   0,   0,   0,    0,    0,
//        178, 173, 158, 134, 147, 132, 165, 187,
//        94, 100,  85,  67,  56,  53,  82,   84,
//        32,  24,  13,   5,  -2,   4,  17,   17,
//        13,   9,  -3,  -7,  -7,  -8,   3,   -1,
//        4,   7,  -6,   1,   0,  -5,  -1,    -8,
//        13,   8,   8,  10,  13,   0,   2,   -7,
//        0,   0,   0,   0,   0,    0,    0,   0,
//    };
//
//    int rookEndgameScore[64] =
//    {
//        13, 10, 18, 15, 12, 12,   8,   5,
//        11, 13, 13, 11, -3,  3,   8,   3,
//        7,  7,  7,  5,  4,  -3,  -5,  -3,
//        4,  3, 13,  1,  2,   1,  -1,   2,
//        3,  5,  8,  4, -5,  -6,  -8, -11,
//        -4, 0, -5, -1, -7, -12,  -8, -16,
//        -6, -6, 0,  2, -9,  -9, -11,  -3,
//        -9,  2, 3, -1, -5, -13,   4, -20,
//    };
//
//    int kingEndgameScore[64] =
//    {
//        74, -35, -18, -18, -11,  15,   4, -17,
//        -12, 17,  14,  17,  17,  38,  23,  11,
//        10,  17,  23,  15,  20,  45,  44,  13,
//        -8,  22,  24,  27,  26,  33,  26,   3,
//        -18,  -4,  21,  24,  27,  23,  9, -11,
//        -19,  -3,  11,  21,  23,  16,  7,  -9,
//        -27, -11,   4,  13,  14,   4, -5, -17,
//        -53, -34, -21, -11, -28, -14, -24, -43
//    };
//
//    // mirror positional score tables for opposite side
//    const int mirroredSquare[128] =
//    {
//        A1, B1, C1, D1, E1, F1, G1, H1,
//        A2, B2, C2, D2, E2, F2, G2, H2,
//        A3, B3, C3, D3, E3, F3, G3, H3,
//        A4, B4, C4, D4, E4, F4, G4, H4,
//        A5, B5, C5, D5, E5, F5, G5, H5,
//        A6, B6, C6, D6, E6, F6, G6, H6,
//        A7, B7, C7, D7, E7, F7, G7, H7,
//        A8, B8, C8, D8, E8, F8, G8, H8
//    };
//    
//    
//public:
//    // constructor (custom AI difficulty)
//    AI(int depth, bool orderMoves, bool applyNMP, bool applyLMR, bool adaptiveEndgameEvaluation, Game &game){
//        this->depth = depth;
//        this->orderMoves = orderMoves;
//        this->applyNMP = applyNMP;
//        this->applyLMR = applyLMR;
//        this->adaptiveEndgameEvaluation = adaptiveEndgameEvaluation;
//        this->InitialiseEvaluationMasks(game);
//    }
//    // default AI constructor (medium difficulty)
//    AI(Game &game){
//        this->depth = 4;
//        this->orderMoves = true;
//        this->applyNMP = true;
//        this->applyLMR = true;
//        this->adaptiveEndgameEvaluation = false;
//        this->InitialiseEvaluationMasks(game);
//    }
//    
//    // AI assistant preset
//    AI(Game &game, bool assistant=false){
//        this->depth = 4;
//        this->orderMoves = true;
//        this->applyNMP = true;
//        this->applyLMR = true;
//        this->adaptiveEndgameEvaluation = false;
//        this->InitialiseEvaluationMasks(game);
//        
//        if(assistant){
//            // make the assistant stronger
//            this->depth = 8;
//            this->adaptiveEndgameEvaluation = true;
//        }
//    }
//    
//    enum {easy, medium, hard};
//    
//    string ViewDepth(){
//        return std::to_string(this->depth);
//    }
//    
//    string ViewEvaluation(){
//        return this->evaluation;
//    }
//    
//    int MaterialCountNoPawns(Game &game, int side){
//        int whiteMaterial = 0;
//        int blackMaterial = 0;
//        
//        for(int i=0; i<64; i++){
//            int rank = i/8;
//            int file = i%8;
//            
//            for(int piece=P; piece<k; piece++){
//                if(p < piece && piece < k && game.DisplayBoard()[rank][file] == piece && side == black){  // not including kings or pawns
//                    blackMaterial += pieceScore[piece];
//                }
//                if(P < piece && piece < K && game.DisplayBoard()[rank][file] == piece && side == white){  // not including kings or pawns
//                    whiteMaterial += pieceScore[piece];
//                }
//            }
//        }
//        return (side == white)? whiteMaterial : blackMaterial;
//    }
//
//    double EndgamePhaseWeight (int materialCountWithoutPawns) {
//        const double multiplier = 1 / endgameMaterialStart;
//        return 1 - fmin(1, materialCountWithoutPawns * multiplier);
//    }
//
//
//    U64 SetFileRankMask(Game &game, int rank, int file){
//        U64 mask = 0ULL;
//        
//        for(int i=0; i<8; i++){
//            for(int j=0; j<8; j++){
//                int square = i*8 + j;
//                
//                if(file != -1){
//                    if(file == j){
//                        mask |= SetBit(mask, square);
//                    }
//                }
//                else if(rank != -1){
//                    if(rank == i){
//                        mask |= SetBit(mask, square);
//                    }
//                }
//            }
//        }
//        
//        return mask;
//    }
//
//    void InitialiseEvaluationMasks(Game &game){
//        // evaluating isolated pawns
//        for(int rank=0; rank<8; rank++){
//            for(int file=0; file<8; file++){
//                int square = rank*8 + file;
//                
//                rankMasks[square] |= SetFileRankMask(game, rank, -1);
//                fileMasks[square] |= SetFileRankMask(game, -1, file);
//                
//                isolatedPawnMasks[square] |= SetFileRankMask(game, -1, file-1);
//                isolatedPawnMasks[square] |= SetFileRankMask(game, -1, file+1);
//            }
//        }
//        
//        // evaluating passed pawns
//        for(int rank=0; rank<8; rank++){
//            for(int file=0; file<8; file++){
//                int square = rank*8 + file;
//                
//                passedWhitePawnMasks[square] |= SetFileRankMask(game, -1, file-1);
//                passedWhitePawnMasks[square] |= SetFileRankMask(game, -1, file);
//                passedWhitePawnMasks [square] |= SetFileRankMask(game, -1, file+1);
//                
//                passedBlackPawnMasks[square] |= SetFileRankMask(game, -1, file-1);
//                passedBlackPawnMasks[square] |= SetFileRankMask(game, -1, file);
//                passedBlackPawnMasks[square] |= SetFileRankMask(game, -1, file+1);
//                
//                for(int i=0; i<(8-rank); i++){
//                    // reset all redundant bits (behind the passed pawn)
//                    passedWhitePawnMasks[square] &= ~rankMasks[(7-i) * 8 + file];
//                }
//                for(int i=0; i<rank+1; i++){
//                    // reset all redundant bits (behind the passed pawn)
//                    passedBlackPawnMasks[square] &= ~rankMasks[i * 8 + file];
//                }
//            }
//        }
//    }
//    
//    
//    int EvaluateKingSafety(Game &game, bool evaluateWhiteKing){
//        int score = 0;
//        // attackers
//        int knightAttackPenalty = 30;
//        int bishopAttackPenalty = 50;
//        int rookAttackPenalty = 90;
//        int queenAttackPenalty = 125;
//        // defenders
//        int pawnShieldBonus = 40;
//        int noPawnShieldPenalty = 75;
//        
//        int whiteKingSquare = game.GetLeastSignificantFirstBitIndex(game.Bitboards()[K]);  // find square of white king
//        int blackKingSquare = game.GetLeastSignificantFirstBitIndex(game.Bitboards()[k]);  // find square of black king
//        int kingSquare;
//        
//        U64 knightAttacks;
//        U64 bishopAttacks;
//        U64 rookAttacks;
//        U64 queenAttacks;
//        U64 enemyKnights;
//        U64 enemyBishops;
//        U64 enemyRooks;
//        U64 enemyQueens;
//        
//        
//        // pieces attacked
//        if(evaluateWhiteKing){
//            kingSquare = whiteKingSquare;
//            knightAttacks = game.KnightAttacks()[whiteKingSquare];
//            bishopAttacks = game.GetBishopAttacks(whiteKingSquare, game.BoardOccupancies()[both]);
//            rookAttacks = game.GetRookAttacks(whiteKingSquare, game.BoardOccupancies()[both]);
//            queenAttacks = game.GetQueenAttacks(whiteKingSquare, game.BoardOccupancies()[both]);
//            enemyKnights = game.Bitboards()[n] & knightAttacks;
//            enemyBishops = game.Bitboards()[b] & bishopAttacks;
//            enemyRooks = game.Bitboards()[r] & rookAttacks;
//            enemyQueens = game.Bitboards()[q] & queenAttacks;
//            
//        }
//        else{
//            kingSquare = blackKingSquare;
//            knightAttacks = game.KnightAttacks()[blackKingSquare];
//            bishopAttacks = game.GetBishopAttacks(blackKingSquare, game.BoardOccupancies()[both]);
//            rookAttacks = game.GetRookAttacks(blackKingSquare, game.BoardOccupancies()[both]);
//            queenAttacks = game.GetQueenAttacks(blackKingSquare, game.BoardOccupancies()[both]);
//            enemyKnights = game.Bitboards()[N] & knightAttacks;
//            enemyBishops = game.Bitboards()[B] & bishopAttacks;
//            enemyRooks = game.Bitboards()[R] & rookAttacks;
//            enemyQueens = game.Bitboards()[Q] & queenAttacks;
//        }
//        
//        // add penalties for enemy pieces that can attack the white king
//        if(enemyKnights){
//            score -= game.CountBits(enemyKnights) * knightAttackPenalty;
//        }
//        if(enemyBishops){
//            score -= game.CountBits(enemyBishops) * bishopAttackPenalty;
//        }
//        if(enemyRooks){
//            score -= game.CountBits(enemyRooks) * rookAttackPenalty;
//        }
//        if(enemyQueens){
//            score -= game.CountBits(enemyQueens) * queenAttackPenalty;
//        }
//        
//    
//        // check for friendly pawns that are in front of the king and on the same file as the king
//        if(!this->endgame){
//            U64 friendlyPawns;
//            int numPawnsInFront = 0;
//            
//            if(evaluateWhiteKing){
//                friendlyPawns = game.Bitboards()[P] & fileMasks[kingSquare];
//            }
//            else{
//                friendlyPawns = game.Bitboards()[p] & fileMasks[kingSquare];
//            }
//            
//            if(friendlyPawns){
//                // count the number of friendly pawns that are in front of the king
//                numPawnsInFront = game.CountBits(friendlyPawns & game.MaskKingAttacks(kingSquare));
//                // add a bonus for each friendly pawn that is in front of the king
//                score += numPawnsInFront * pawnShieldBonus;
//            }
//            
//            // if the king does not have a pawn shield, add a penalty
//            if(numPawnsInFront == 0 && endgame){
//                score -= noPawnShieldPenalty;
//            }
//        }
//        
//        // count the number of friendly pieces that are close to the king
//        int numCloseFriendlyPieces = 0;
//        int friendlyDefendersBonus = 10;
//        U64 friendlyPieces;
//        
//        if(evaluateWhiteKing){
//            friendlyPieces = game.BoardOccupancies()[white];
//        }
//        else{
//            friendlyPieces = game.BoardOccupancies()[black];
//        }
//        // find the rank and file of the king
//        int kingRank = kingSquare / 8;
//        int kingFile = kingSquare % 8;
//        
//        // check for friendly pieces within a 5-square wide and 3-square tall area around the king
//        for(int rank = kingRank - 1; rank <= kingRank + 1; rank++){
//            for(int file = kingFile - 2; file <= kingFile + 2; file++){
//                if(rank >= 0 && rank < 8 && file >= 0 && file < 8){
//                    int square = rank * 8 + file;
//                    if((friendlyPieces & (1ULL << square)) != 0){
//                        numCloseFriendlyPieces++;
//                    }
//                }
//            }
//        }
//
//        // add a bonus based on the number of friendly pieces that are close to the king
//        score += numCloseFriendlyPieces * friendlyDefendersBonus;
//        
//        return score;
//    }
//    
//    U64 GetPieceAttacks(Game &game, int square, int piece, U64 occupied) {
//
//        switch(piece){
//            case P:
//                return game.PawnAttacks()[white][square];
//            case N:
//                return game.KnightAttacks()[square];
//            case B:
//                return game.GetBishopAttacks(square, occupied);
//            case R:
//                return game.GetRookAttacks(square, occupied);
//            case Q:
//                return game.GetQueenAttacks(square, occupied);
//            case K:
//                return game.KingAttacks()[square];
//                
//            case p:
//                return game.PawnAttacks()[black][square];
//            case n:
//                return game.KnightAttacks()[square];
//            case b:
//                return game.GetBishopAttacks(square, occupied);
//            case r:
//                return game.GetRookAttacks(square, occupied);
//            case q:
//                return game.GetQueenAttacks(square, occupied);
//            case k:
//                return game.KingAttacks()[square];
//                
//            default:
//                return 0;
//        }
//    }
//    
//    inline int GetPawnChainBonus(Game &game) {
//        int bonus = 0;
//        const int chainBonus = 10;
//        U64 whitePawns = game.Bitboards()[P];
//        U64 blackPawns = game.Bitboards()[p];
//        U64 pawns = whitePawns | blackPawns;
//
//        // Iterate over each pawn on the board
//        while (pawns) {
//            int square = game.GetLeastSignificantFirstBitIndex(pawns);
//            U64 pawn = 1ULL << square;
//            bool isWhite = (whitePawns & pawn) != 0;
//            U64 neighbors = (isWhite ? game.PawnAttacks()[black][square] : game.PawnAttacks()[white][square]);
//            int count = game.CountBits(neighbors & pawns);
//
//            // Add bonus for each neighboring pawn
//            bonus += count * chainBonus;
//
//            // Remove current pawn from the list
//            pawns &= ~pawn;
//        }
//
//        return bonus;
//    }
//
//    
//    int EvaluateMobility(Game &game, int square, int piece) {
//        int mobilityScore = 0;
//        const int mobilityBonus = 20;
//        
//        U64 legalMoves = GetPieceAttacks(game, square, piece, game.BoardOccupancies()[both]);
//        int numberOfLegalMoves = game.CountBits(legalMoves);
//        mobilityScore += numberOfLegalMoves * mobilityBonus;
//        return mobilityScore;
//    }
//    
//    
//    int ManhattanDistance(int startSq, int endSq) {
//        return abs(startSq/8 - endSq/8) + abs(startSq%8 - endSq%8);
//    }
//
//    int EndgameKingPromiximity(Game &game, int whiteKingSq, int blackKingSq) {
//        // Give a bonus to the kings based on their proximity to each other
//        int kingProximityBonus = 500 - (5 * ManhattanDistance(whiteKingSq, blackKingSq));
//
//        // Give a bonus to our side based on the distance of the enemy king from a corner
//        int enemyKingRow, enemyKingCol;
//        if(game.Side() == white){
//            enemyKingRow = 0;
//            enemyKingCol = 0;
//        }
//        else{
//            enemyKingRow = 7;
//            enemyKingCol = 7;
//        }
//
//        int minDistance = 1000000;
//        for(int i = 0; i < 2; i++){
//            for(int j = 0; j < 2; j++){
//                minDistance = min(minDistance, ManhattanDistance(blackKingSq, (enemyKingRow + i) * 8 + (enemyKingCol + j)));
//            }
//        }
//
//        int enemyKingProximityBonus = (4 - minDistance) * 300;
//
//        return kingProximityBonus + enemyKingProximityBonus;
//    }
//
//          
//    
//    int Evaluation(Game &game){
//        int score = 0;
//        U64 bitboard;
//        int piece, square;
//        
//        // iterate through all pieces
//        for(int bbPiece = P; bbPiece <= k; bbPiece++){
//            // select the bitboard for the current piece
//            bitboard = game.Bitboards()[bbPiece];
//            
//            while(bitboard){
//                piece = bbPiece;
//                square = game.GetLeastSignificantFirstBitIndex(bitboard);
//                score += materialScore[piece];
//                
//                // score positional piece scores
//               switch (piece)
//               {
//                   // evaluate white piece
//                   case P:
//                   {
//                       // piece position
//                       score += pawnScore[square];
//                       // doubled pawns
//                       int doubledPawns = game.CountBits(game.Bitboards()[P] & fileMasks[square]);
//                       score += doubledPawnPenalty * doubledPawns;  // increases penalty for tripled pawns and so on..
//                       // isolated pawns
//                       if((isolatedPawnMasks[square] & game.Bitboards()[P]) == 0){
//                           score += isolatedPawnPenalty;
//                       }
//                       // passed pawn bonus
//                       if((passedWhitePawnMasks[square] & game.Bitboards()[p]) == 0){
//                           score += passedPawnBonus[8-(square/8)];
//                       }
//                       
//                       // mobility
//                       score += EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case N:
//                   {
//                       // piece position
//                       score += knightScore[square];
//                       // piece mobility
//                       score += EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case B:
//                   {
//                       // piece position
//                       score += bishopScore[square];
//                       // piece mobility
//                       score += EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case R:
//                   {
//                       // piece position
//                       score += rookScore[square];
//
//                       // rook on semi-open file bonus
//                       if((game.Bitboards()[P] & fileMasks[square]) == 0){
//                           score += semiOpenFileScore;
//                       }
//                       // rook on open file bonus
//                       if(((game.Bitboards()[P] | game.Bitboards()[p]) & fileMasks[square]) == 0){
//                           score += openFileScore;
//                       }
//                       // rook on 7th rank bonus
//                       if(square/8 == 1){
//                           score += rookRankBonus;
//                       }
//                       
//                       // piece mobility
//                       score += EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case Q:
//                   {
//                       // piece mobility
//                       score += EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case K:
//                   {
//                       // piece position
//                       score += kingScore[square];
//                       
//                       // king safety bonus
//                       score += EvaluateKingSafety(game, true);
//                       
//                       // king on semi-open file penalty
//                       if((game.Bitboards()[P] & fileMasks[square]) == 0){
//                           score -= semiOpenFileScore;
//                       }
//                       // king on open file penalty
//                       if(((game.Bitboards()[P] | game.Bitboards()[p]) & fileMasks[square]) == 0){
//                           score -= openFileScore;
//                       }
//                       break;
//                       
//                       // piece mobility
//                       if(this->endgame){
//                           score += EvaluateMobility(game, square, piece);
//                       }
//                       break;
//                   }
//
//                   // evaluate black pieces
//                   case p:
//                   {
//                       // piece position
//                       score -= pawnScore[square];
//                       // doubled pawns
//                       int doubledPawns = game.CountBits(game.Bitboards()[p] & fileMasks[square]);
//                       score += doubledPawnPenalty * doubledPawns;  // increases penalty for tripled pawns and so on..
//                       // isolated pawns
//                       if((isolatedPawnMasks[square] & game.Bitboards()[p]) == 0){
//                           score += isolatedPawnPenalty;
//                       }
//                       // passed pawn bonus
//                       if((passedBlackPawnMasks[square] & game.Bitboards()[P]) == 0){
//                           score -= passedPawnBonus[(square/8) + 1];
//                       }
//                       
//                       // piece mobility
//                       score -= EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case n:
//                   {
//                       // piece position
//                       score -= knightScore[mirroredSquare[square]];
//                       // piece mobility
//                       score -= EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case b:
//                   {
//                       // piece position
//                       score -= bishopScore[mirroredSquare[square]];
//                       // piece mobility
//                       score -= EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case r:
//                   {
//                       // piece position
//                       score -= rookScore[mirroredSquare[square]];
//                       // rook on semi-open file bonus
//                       if((game.Bitboards()[p] & fileMasks[square]) == 0){
//                           score -= semiOpenFileScore;
//                       }
//                       // rook on open file bonus
//                       if(((game.Bitboards()[P] | game.Bitboards()[p]) & fileMasks[square]) == 0){
//                           score -= openFileScore;
//                       }
//                       // rook on 2nd rank bonus
//                       if(square/8 == 6){
//                           score -= rookRankBonus;
//                       }
//                       
//                       // piece mobility
//                       score -= EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case q:
//                   {
//                       // piece mobility
//                       score -= EvaluateMobility(game, square, piece);
//                       break;
//                   }
//                   case k:
//                   {
//                       // piece position
//                       score -= blackKingScore[square];
//                       
//                       // king safety bonus
//                       score -= EvaluateKingSafety(game, false);
//                       
//                       // king on semi-open file penalty
//                       if((game.Bitboards()[p] & fileMasks[square]) == 0){
//                           score += semiOpenFileScore;
//                       }
//                       // king on open file penalty
//                       if(((game.Bitboards()[P] | game.Bitboards()[p]) & fileMasks[square]) == 0){
//                           score += openFileScore;
//                       }
//                       
//                       // piece mobility
//                       if(this->endgame){
//                           score -= EvaluateMobility(game, square, piece);
//                       }
//                       break;
//                   }
//               }
//                
//                game.ClearBit(bitboard, square);
//            }
//        }
//        
//        // evalauting pawn chains
//        score += GetPawnChainBonus(game);
//        
//        // king endgame promixity to one-another / edge of board
//        if(this->endgame){
//            score += EndgameKingPromiximity(game, game.GetLeastSignificantFirstBitIndex(game.Bitboards()[K]), game.GetLeastSignificantFirstBitIndex(game.Bitboards()[k]));
//        }
//        
//        // return final static evaluation
//        return (game.Side()==white)? score : -score;
//    }
//    
//    bool BlackPassedPawnOn7thRank(Game &game, int move){
//        if(((passedBlackPawnMasks[game.GetMoveEndSq(move)] & game.Bitboards()[P]) == 0)
//           && (game.GetMoveEndSq(move)/8 == 5 || game.GetMoveEndSq(move)/8 == 6)){
//            return true;
//        }
//        return false;
//    }
//    
//    bool WhitePassedPawnOn2ndRank(Game &game, int move){
//        if(((passedWhitePawnMasks[game.GetMoveEndSq(move)] & game.Bitboards()[p]) == 0)
//           && (game.GetMoveEndSq(move)/8 == 2 || game.GetMoveEndSq(move)/8 == 1)){
//            return true;
//        }
//        return false;
//    }
//    
//    
//    void BubbleSort(vector<int> &scores, vector<int> &moves)
//    {
//        bool swapped = false;
//        for(int i = 0; i < moves.size() - 1; i++){
//            swapped = false;
//            for(int j = 0; j < moves.size() - i - 1; j++){
//                if(scores[j] < scores[j + 1]){
//                    swap(scores[j], scores[j + 1]);
//                    swap(moves[j], moves[j + 1]);
//                    swapped = true;
//                }
//            }
//            if(!swapped){
//                break;
//            }
//        }
//    }
//
//
//    vector<int> ScoreMoves(Game &game, vector<int> &moves){
//        vector<int> scores;
//        
//        for(int move : moves){
//            int pieceCaptured = -1;
//            
//            // pick up bitboard piece index ranges depending on side
//            int startPiece, endPiece;
//            // white to move
//            if (game.Side() == white) {startPiece = p; endPiece = k;}
//            else {startPiece = P; endPiece = K;}
//
//            // loop over bitboards opposite to the current side to move
//            for (int bb_piece = startPiece; bb_piece <= endPiece; bb_piece++)
//            {
//                // if there's a piece on the target square
//                if (GetBit(game.Bitboards()[bb_piece], game.GetMoveEndSq(move)))
//                {
//                    // remove it from corresponding bitboard
//                    pieceCaptured = bb_piece;
//                    break;
//                }
//            }
//            
//            if(game.IsCapture(move)){  // captures
//                scores.push_back(MVV_LVA[game.GetPieceMoved(move)][pieceCaptured] + 10000);
//            }
//            else{
//                if(killerMoves[0][ply] == move){
//                    scores.push_back(9000);
//                }
//                else if(killerMoves[1][ply] == move){
//                    scores.push_back(8000);
//                }
//                else{
//                    scores.push_back(historyMoves[game.GetPieceMoved(move)][game.GetMoveEndSq(move)]);
//                }
//                
//            }
//        }
//        return scores;
//    }
//
//    void OrderAllMoves(Game &game, vector<int> &moves){
//        vector<int> scores = ScoreMoves(game, moves);
//        BubbleSort(scores, moves);
//    }
//
//    int Quiescence(Game &game, int alpha, int beta){
//        int standPat = Evaluation(game);
//        
//        if (ply > maximumPly - 1)
//            // evaluate position
//            return Evaluation(game);
//        
//        if(standPat >= beta){
//            // node fails high
//            return beta;
//        }
//        
//        
//        if(standPat > alpha){
//            // PV node
//            alpha = standPat ;
//        }
//        
//        int moveCount = 0;
//        vector<int> moves;
//        game.GenerateMoves(moves);
//        
//        for(int move : moves){
//            moveCount++;
//            // preserve board state
//            game.CopyBoard();
//            this->ply++;
//            
//            if(game.MakeMove(move, capturesOnly) == 0){
//                this->ply--;
//                game.RestoreBoard();
//                continue;
//            }
//
//            int score = -Quiescence(game, -beta, -alpha);
//            game.RestoreBoard();
//            this->ply--;
//            
//            if(score >= beta){
//                // node fails high
//                return beta;
//            }
//            
//            
//            
//            if(score > alpha){
//                alpha = score;
//                
//                // PV node
//                PVtable[ply][ply] = move;
//                // copy move from deeper search
//                for(int nextPly = ply + 1; nextPly < PVlength[ply + 1]; nextPly++){
//                    PVtable[ply][nextPly] = PVtable[ply + 1][nextPly];
//                }
//                // adjust pv length
//                PVlength[ply] = PVlength[ply + 1];
//            }
//        }
//        // node fails low
//        return alpha;
//    }
//
//    int AlphaBeta(Game &game, int alpha, int beta, int depth){
//        int numberOfMovesSearched = 0;
//        PVlength[this->ply] = this->ply;
//        
//        if(depth==0){
//            return Quiescence(game, alpha, beta);
//        }
//        
//        
//        // we are in too deep
//        if(this->ply > maximumPly-1){
//            return Evaluation(game);
//        }
//        
//        int kingSq;
//        if(game.Side()==white){
//            kingSq = game.GetLeastSignificantFirstBitIndex(game.Bitboards()[K]);
//        }
//        else{
//            kingSq = game.GetLeastSignificantFirstBitIndex(game.Bitboards()[k]);
//        }
//        int inCheck = game.IsSquareAttacked(kingSq, (game.Side() ^ 1));
//        
//        // check extension,
//        if(inCheck){
//            depth++;
//        }
//        
//        // NULL MOVE PRUNING
//        if(depth >= this->reductionValue+1 && !inCheck && this->ply !=0 && this->applyNMP){
//            game.CopyBoard();
//            game.SwitchSides();
//            game.ClearEnpassantSq();
//
//            
//            int score = -AlphaBeta(game, -beta, -beta + 1, depth - 1 - this->reductionValue);
//            game.RestoreBoard();
//            
//            if(score >= beta){
//                // fail hard beta cut-off
//                return beta;
//            }
//        }
//        
//        int legalMoves = 0;
//        vector<int> moves;
//        game.GenerateMoves(moves);
//        OrderAllMoves(game, moves);
//        
//        for(int move : moves){
//            // preserve board state
//            game.CopyBoard();
//            this->ply++;
//            
//            if(game.MakeMove(move, allMoves) == 0){
//                this->ply--;
//                game.RestoreBoard();
//                continue;
//            }
//            legalMoves++;
//
//            int score = 0;
//
//            
//            
//            if(numberOfMovesSearched == 0){
//                score = -AlphaBeta(game, -beta, -alpha, depth-1);
//            }
//            else{
//                // Late Move Reduction condition
//                if(applyLMR && numberOfMovesSearched >= this->fullDepthMoves && depth >= this->LMRvalue && !inCheck && !game.IsCapture(move) && !game.GetPromotedPiece(move)){
//                    // search the move with a reduced depth
//                    score = -AlphaBeta(game, -(alpha+1), -alpha, depth - (this->LMRvalue-1));
//                }
//                else{
//                    // resets alpha to ensure that a full depth search is done
//                    score = alpha + 1;
//                }
//                
//                if(score > alpha){  // a good move was found while attempting to perform LMR so we will search it at a higher depth
//                    score = -AlphaBeta(game, -(alpha+1), -alpha, depth - (this->LMRvalue-2));
//                    
//                    if(score > alpha && score < beta){  // LMR has failed so we search normally
//                        score = -AlphaBeta(game, -beta, -alpha, depth-1);
//                    }
//                }
//            }
//            
//            game.RestoreBoard();
//            this->ply--;
//            numberOfMovesSearched++;
//            
//            if(score >= beta){
//                if(!game.IsCapture(move)){
//                    killerMoves[1][ply] = killerMoves[0][ply];
//                    killerMoves[0][ply] = move;
//                }
//                // node fails high
//                return beta;
//            }
//            
//            if(score > alpha){
//                alpha = score;
//                
//                if(!game.IsCapture(move)){
//                    historyMoves[game.GetPieceMoved(move)][game.GetMoveEndSq(move)] += depth;
//                }
//                
//                // PV node
//                PVtable[ply][ply] = move;
//                // copy move from deeper search
//                for(int nextPly = ply + 1; nextPly < PVlength[ply + 1]; nextPly++){
//                    PVtable[ply][nextPly] = PVtable[ply + 1][nextPly];
//                }
//                // adjust pv length
//                PVlength[ply] = PVlength[ply + 1];
//            }
//        }
//        
//        if(legalMoves == 0){  // no more moves available in the current position
//            if(inCheck){
//                return -49000+this->ply;  // the +ply is necessary to prevent the AI from missing forced checkmates at higher depths
//            }
//            else{
//                // stalemate
//                return 0;
//            }
//        }
//
//        // node fails low (no better move found)
//        return alpha;
//    }
//
//
//    // iterative deepening
//    virtual int GetBestMove(Game &game){
//        // clear data structures
//        memset(killerMoves, 0, sizeof(killerMoves));
//        memset(historyMoves, 0, sizeof(historyMoves));
//        memset(PVtable, 0, sizeof(PVtable));
//        memset(PVlength, 0, sizeof(PVlength));
//
//        int eval = 0;
//        int bestMove;
//        
//        if((MaterialCountNoPawns(game, white) + MaterialCountNoPawns(game, black)) < 1300 && adaptiveEndgameEvaluation){
//            this->endgame = true;  // making sure the AI knows it is in the endgame now
//            this->applyLMR = false;  // we do not want to activate Late Move Reduction during the endgame
//            this->depth = 11;
//            
//            // altering piece position scores for the endgame
//            memcpy(kingScore, kingEndgameScore, sizeof(kingEndgameScore));
//            memcpy(blackKingScore, kingEndgameScore, sizeof(kingEndgameScore));
//            memcpy(rookScore, rookEndgameScore, sizeof(rookEndgameScore));
//            memcpy(pawnScore, pawnEndgameScore, sizeof(pawnEndgameScore));
//        }
//
//        cout << "Depth " << this->depth << endl;
//        for(int currentDepth = 1; currentDepth <= this->depth; currentDepth++){
//            int alpha = -50000;
//            int beta = 50000;
//            
//            eval = AlphaBeta(game, alpha, beta, currentDepth);
//            if((eval <= alpha) || (eval >= beta)){  // we fell out of the window so we re-do the search with a full window
//                alpha = -50000;
//                beta = 50000;
//                continue;
//            }
//            // focus window size for next search
//            alpha = eval - this->windowReduction;
//            beta = eval + this->windowReduction;
//        }
//
//        bestMove = this->PVtable[0][0];
//
//
//        if(eval >= 48999 || eval <= -48999){
//            cout << "Checkmate!" << endl;
//            this->evaluation = "Checkmate!";
//        }
//        else if(eval == 0){
//            cout << "Stalemate!" << endl;
//            this->evaluation = "Stalemate!";
//        }
//        else{
//            cout << "Evaluation: " << -eval << endl;
//            this->evaluation = std::to_string(-eval/100);
//        }
//        
//        return bestMove;
//    }
//};
//
//
//class Assistant : AI{
//public:
//    Assistant(Game &game) : AI(game, true){
//        cout << "assistant depth: " << this->depth << endl;
//    }
//    
//    int GetBestMove(Game &game) override {
//        cout << "Evaluating..." << endl;
//        // clear data structures
//        memset(killerMoves, 0, sizeof(killerMoves));
//        memset(historyMoves, 0, sizeof(historyMoves));
//        memset(PVtable, 0, sizeof(PVtable));
//        memset(PVlength, 0, sizeof(PVlength));
//
//        int eval = 0;
//        int bestMove;
//        
//        if((MaterialCountNoPawns(game, white) + MaterialCountNoPawns(game, black)) < 1300 && adaptiveEndgameEvaluation){
//            this->endgame = true;  // making sure the AI knows it is in the endgame now
//            this->applyLMR = false;  // we do not want to activate Late Move Reduction during the endgame
//            this->depth = 10;  // making the engine search further during an endgame
//            
//            // altering piece position scores for the endgame
//            memcpy(kingScore, kingEndgameScore, sizeof(kingEndgameScore));
//            memcpy(blackKingScore, kingEndgameScore, sizeof(kingEndgameScore));
//            memcpy(rookScore, rookEndgameScore, sizeof(rookEndgameScore));
//            memcpy(pawnScore, pawnEndgameScore, sizeof(pawnEndgameScore));
//        }
//
//        for(int currentDepth = 1; currentDepth <= this->depth; currentDepth++){
//            int alpha = -50000;
//            int beta = 50000;
//            
//            eval = AlphaBeta(game, alpha, beta, currentDepth);
//            
//            if((eval <= alpha) || (eval >= beta)){  // we fell out of the window so we re-do the search with a full window
//                alpha = -50000;
//                beta = 50000;
//                continue;
//            }
//            // focus window size for next search
//            alpha = eval - this->windowReduction;
//            beta = eval + this->windowReduction;
//        }
//
//        bestMove = this->PVtable[0][0];
//        cout << "best move:" << endl;
//        cout << squareToString[game.GetMoveStartSq(bestMove)] << "," << squareToString[game.GetMoveEndSq(bestMove)] << endl;
//        
//        return bestMove;
//    }
//    
//};
