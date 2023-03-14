#pragma once

struct CastlingRights {
private:
    bool blackQueensideCastling;
    bool blackKingsideCastling;
    bool whiteQueensideCastling;
    bool whiteKingsideCastling;
    bool whiteKingHasMoved;
    bool blackKingHasMoved;
    
public:
    CastlingRights(){
        this->whiteQueensideCastling = true;
        this->whiteKingsideCastling = true;
        this->blackQueensideCastling = true;
        this->blackKingsideCastling = true;
        this->whiteKingHasMoved = false;
        this->blackKingHasMoved = false;
    }
    
    bool WhiteQueenside(){
        return this->whiteQueensideCastling;
    }
    
    bool WhiteKingside(){
        return this->whiteKingsideCastling;
    }
    
    bool BlackQueenside(){
        return this->blackQueensideCastling;
    }
    
    bool BlackKingside(){
        return this->blackKingsideCastling;
    }
    
    void SetWhiteQueenside(bool canCastle){
        this->whiteQueensideCastling = canCastle;
    }
    
    void SetWhiteKingside(bool canCastle){
        this->whiteKingsideCastling = canCastle;
    }
    
    void SetBlackQueenside(bool canCastle){
        this->blackQueensideCastling = canCastle;
    }
    
    void SetBlackKingside(bool canCastle){
        this->blackKingsideCastling = canCastle;
    }
    
    bool WhiteKingHasMoved(){
        return this->whiteKingHasMoved;
    }
    
    bool BlackKingHasMoved(){
        return this->blackKingHasMoved;
    }
    
    void SetWhiteKingMoved(bool hasMoved){
        this->whiteKingHasMoved = hasMoved;
    }
    
    void SetBlackKingMoved(bool hasMoved){
        this->blackKingHasMoved = hasMoved;
    }
};
