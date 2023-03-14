#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <stack>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#include "ChessAI.h"
#include "GameClass.hpp"
#include "Moves.h"
#include "/Users/anirudhanvijay/Desktop/programs/SDLtest1/SDLtest1/Menu.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/ostreamwrapper.h"

using namespace std;

class IApp{
public:
    virtual void NewGame() = 0;
    virtual void HideMenu() = 0;
    virtual vector<SDL_Rect> DisplayMenu(vector<Button> &buttons, int startX, int startY) = 0;
};



class ChessApp : IApp{
private:
    Game game = Game();
    // constants
    const int WIDTH = 480;
    const int HEIGHT = 480;
    const int SQUARESIZE = 60;
    const int WINDOWSIZE = 680;
    const int BORDER = 30;
    const double TRANSLATION = 1.667;
    
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    vector<tuple<int, int>> squaresClicked;
    tuple<int, int> squareSelected;
    stack<Move> movesToHighlight;

    bool switchColour = false;
    bool moveMade = false;
    bool highlightPossibleMoves = true;

    const vector<tuple<int, int, int, int>> greenTheme = {{251, 246, 226, 255}, {14, 102, 28, 255}};
    const vector<tuple<int, int, int, int>> brownTheme = {{240, 217, 181, 255}, {163, 122, 89, 255}};

    bool playerIsWhite = true;
    bool playerIsBlack = false;
    bool playerVSplayer = false;
    bool AIvsAI = false;
    bool playervsAI = false;
    
    vector<bool> gameSettings = {false, false, false, false, false, false, false, false, false};
    
public:
    ChessApp(SDL_Renderer* renderer, SDL_Window* window){
        this->renderer = renderer;
        this->window = window;
    }
    
    void ResetGame(){
        movesToHighlight = {};
        game.ResetGame();
    }
    
    void PlayAI() {
        playervsAI = true;
    }

    void PlayAsBlack() {
        playerIsWhite = false;
        playerIsBlack = true;
    }

    void PlayPvP() {
        playerVSplayer = true;
    }

    void PlayAIvsAI() {
        AIvsAI = true;
    }
    
    void NewGame() override {
        bool running = true;
        
        vector<Button> buttons = {
            Button(200, 60, "play AI"),
            Button(200, 60, "play as black"),
            Button(200, 60, "player vs player"),
            Button(200, 60, "AI vs AI"),
            Button(200, 60, "Switch themes"),
            Button(200, 60, "Start game"),
            Button(200, 60, "Easy"),
            Button(200, 60, "Medium"),
            Button(200, 60, "Difficult")
        };
        
    
        // clearing the screen and displaying menu buttons
        HideMenu();
        vector<SDL_Rect> buttonRects;
        
        while (running) {
            buttonRects = DisplayMenu(buttons, 100, 60);

            SDL_Event event;
            if(SDL_WaitEvent(&event)) {
                if(event.type ==  SDL_QUIT){
                    running = false;
                }
                else{
                    switch (event.type) {
                        case SDL_KEYDOWN:
                            
                            if(event.key.repeat == 0){
                                
                                if(event.key.keysym.sym == SDLK_ESCAPE){
                                    // return to main menu
                                    HideMenu();
                                    fill(gameSettings.begin(), gameSettings.end(), false);
                                    running = false;
                                    break;
                                }
                            }
                            break;
                            
                        case SDL_MOUSEBUTTONDOWN:
                            
                            int mouseX, mouseY;
                            SDL_GetMouseState(&mouseX, &mouseY);

                            SDL_Rect mouseRect = { mouseX, mouseY, 1, 1 };
                            
                            for(int i=0; i<buttonRects.size(); i++){
                                if (SDL_HasIntersection(&mouseRect, &buttonRects[i])) {
                                    if(i == 5){
                                        // start game button clicked
                                        ResetGame();
                                        StartGame(gameSettings);
                                    }
                                    gameSettings[i] = buttons[i].OnClick();
                                }
                            }
                            break;
                    }
                    
                }
            }
        }
    }
    
    
    void StartGame(vector<bool> settings){
        bool running = true;
        
        // player selection & game settings
        bool humanTurn = true;  // true for white, false for black or for AI vs AI
        bool AIvsAI = false;  // true for AI vs AI game mode
        bool playervsAI = settings[0];
        
        playerIsBlack = settings[1];
        playerIsWhite = !playerIsBlack;
        playerVSplayer = settings[2];
        AIvsAI = settings[3];
        switchColour = settings[4];
        
        bool easyAI = settings[6];
        bool mediumAI = settings[7];
        bool hardAI = settings[8];
        
        // making sure multiple player options cannot be selected at the same time
        if(AIvsAI && playervsAI){
            playervsAI = false;
        }
        if(playerVSplayer && playervsAI){
            playervsAI = false;
        }
        if(playerVSplayer && AIvsAI){
            AIvsAI = false;
        }
        
        // save settings now to continue game later
        SaveSettings(settings);
        
        if(playerIsWhite){
            humanTurn = true;
        }
        if(!playerIsBlack && !playerIsWhite){
            AIvsAI = true;
        }
        if(AIvsAI || playerIsBlack){
            humanTurn = false;
        }
        if(AIvsAI){
            playervsAI = false;
        }
        if(playerVSplayer){
            humanTurn = true;
        }
        
        
        vector<Move> moves;
        AI computer = AI(2);
        
        if(easyAI){
            computer = AI(1);
        }
        else if(mediumAI){
            computer = AI(2);
        }
        else if(hardAI){
            computer = AI(4);
        }
        
        while(running){
            UpdateBoard(renderer);
        
            SDL_Event event;
            if(SDL_WaitEvent(&event)) {
                if(event.type ==  SDL_QUIT){
                    running = false;
                }
                if(AIvsAI){
                    Move AImove = computer.GetAIMove(game);
                    game.MakeMove(AImove);
                    UpdateBoard(renderer);
                    movesToHighlight.push(AImove);
                }
                else if(!game.Checkmate() && humanTurn){
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    
                    // adjusting x and y due to board borders
                    x -= (WINDOWSIZE-WIDTH);
                    y -= (WINDOWSIZE-HEIGHT);
                    y += SQUARESIZE*TRANSLATION;
                    x += SQUARESIZE*TRANSLATION;
                    
                    // getting the rank and file of the click
                    x /= SQUARESIZE;
                    y /= SQUARESIZE;
                    
                    
                    if(event.type == SDL_MOUSEBUTTONDOWN && MouseCoordinatesOnBoard(x, y)){
                        if(squaresClicked.size() == 0 && game.GetBoard()[y][x].Type() == "empty"){
                            squareSelected = {};
                            squaresClicked = {};
                            continue;
                        }
                        else{
                            squareSelected = {y, x};
                            squaresClicked.push_back(squareSelected);
                        }
                        
                        if(squaresClicked.size() == 2){
                            if(squaresClicked[0] == squaresClicked[1]){
                                squareSelected = {};
                                squaresClicked = {};
                            }
                            else{
                                moveMade = false;
                                tuple<int, int> startSq = squaresClicked[0];
                                tuple<int, int> endSq = squaresClicked[1];
                                
                                moves = game.GenerateLegalMoves();
                                if(moves.size() == 0){
                                    GameTextUpdate();
                                }
                                
                                // checks for checkmate or stalemate
                                game.IsCheckmate(moves);
                                game.IsStalemate(moves);

                                Move humanMove = Move(game.GetBoard(), startSq, endSq);
                                
                                for(Move move : moves){
                                    if(move.MoveID() == humanMove.MoveID()){
                                        game.MakeMove(move);
                                        moveMade = true;
                                        movesToHighlight.push(move);
                                        if((!playerVSplayer || playervsAI) && moveMade){
                                            humanTurn = false;
                                        }
     
                                        game.SaveGame();
                                    }
                                    else{
                                        squareSelected = {};
                                        squaresClicked = {squaresClicked[1]};
                                        continue;
                                    }
                                }
                                // refreshing screen and resetting player's mouse clicks
                                UpdateBoard(renderer);
                                squareSelected = {};
                            }
                        }
                    }
                }
                

                if(event.type == SDL_KEYDOWN){
                    if(event.key.keysym.sym == SDLK_BACKSPACE){
                        // Undoing 2 ply to undo a move for both players
                        game.UndoMove();
                        if(game.MoveLog().size() % 2 == 0){
                            game.UndoMove();
                        }
                    }
                    
                    if(event.key.keysym.sym == SDLK_ESCAPE){
                        HideMenu();
                        game.SaveGame();
                        SaveSettings(settings);
                        fill(gameSettings.begin(), gameSettings.end(), false);
                        return;
                    }
                }
                
                if((!game.Checkmate()) && (!humanTurn || AIvsAI || (playervsAI && moveMade))){
                    moveMade = false;
                    
                    Move AImove = computer.GetAIMove(game);
                    game.MakeMove(AImove);
                    movesToHighlight.push(AImove);
                    game.SaveGame();
                    
                    // update window after AI's move
                    UpdateBoard(renderer);

                    squareSelected = {};
                    squaresClicked = {};
                    
                    if(!AIvsAI || playervsAI){
                        humanTurn = true;
                    }
                    
                }
            }
            // update window
            UpdateBoard(renderer);
        }
        return;
    }

    
    void DrawBoard(SDL_Renderer *renderer){
        // draw border
        SDL_Rect border;
        border.x = (WINDOWSIZE-WIDTH)/2 - BORDER/2;
        border.y = (WINDOWSIZE-HEIGHT)/2 - BORDER/2;
        border.w = 8*SQUARESIZE + BORDER;
        border.h = 8*SQUARESIZE + BORDER;
        SDL_SetRenderDrawColor(renderer, 21, 16, 30, 100);
        SDL_RenderFillRect(renderer, &border);
        
        // draw light coloured tiles
        SDL_Rect darkSquares;
        darkSquares.x = (WINDOWSIZE-WIDTH)/2;
        darkSquares.y = (WINDOWSIZE-HEIGHT)/2;
        darkSquares.w = 8*SQUARESIZE;
        darkSquares.h = 8*SQUARESIZE;
        tuple<int, int, int, int> colour = (switchColour)? brownTheme[1] : greenTheme[1];
        SDL_SetRenderDrawColor(renderer, get<0>(colour), get<1>(colour), get<2>(colour), get<3>(colour));
        SDL_RenderFillRect(renderer, &darkSquares);
        
        colour = (switchColour)? brownTheme[0] : greenTheme[0];
        int startPos = 0;
        for(int y = 0; y < 8; y++){
            for (int x = startPos; x < 8; x+=2) {
                SDL_Rect lightSquare;
                lightSquare.x = (x+TRANSLATION) * SQUARESIZE;
                lightSquare.y = (y+TRANSLATION) * SQUARESIZE;
                lightSquare.w = SQUARESIZE;
                lightSquare.h = SQUARESIZE;
                SDL_SetRenderDrawColor(renderer, get<0>(colour), get<1>(colour), get<2>(colour), get<3>(colour));
                SDL_RenderFillRect(renderer, &lightSquare);
            }
            startPos = 1 - startPos;
        }
    }

    void HighlightMove(SDL_Renderer *renderer){
        if(movesToHighlight.size() != 0){
            Move move = movesToHighlight.top();
            
            tuple<int, int> startSq = move.StartSq();
            tuple<int, int> endSq = move.EndSq();
            
            SDL_Rect start;
            start.x = (get<1>(startSq)+TRANSLATION) * SQUARESIZE;
            start.y = (get<0>(startSq)+TRANSLATION) * SQUARESIZE;
            start.w = SQUARESIZE;
            start.h = SQUARESIZE;
            
            SDL_Rect dest;
            dest.x = (get<1>(endSq)+TRANSLATION) * SQUARESIZE;
            dest.y = (get<0>(endSq)+TRANSLATION) * SQUARESIZE;
            dest.w = SQUARESIZE;
            dest.h = SQUARESIZE;
            
            vector<tuple<int, int, int, int>> highlightThemes = {{247, 247, 131, 195}, {246, 230, 123, 205}};
            tuple<int, int, int, int> colour = (switchColour)? highlightThemes[1] : highlightThemes[0];
            SDL_SetRenderDrawColor(renderer, get<0>(colour), get<1>(colour), get<2>(colour), get<3>(colour));
            SDL_RenderFillRect(renderer, &start);
            SDL_RenderFillRect(renderer, &dest);
        }
    }


    void HighlightSquare(SDL_Renderer *renderer, int square, int colourIndex){
        int rank = square / 8;
        int file = square % 8;
        
        SDL_Rect tile;
        tile.x = (file+TRANSLATION) * SQUARESIZE;
        tile.y = (rank+TRANSLATION) * SQUARESIZE;
        tile.w = SQUARESIZE;
        tile.h = SQUARESIZE;
        
        vector<tuple<int, int, int, int>> highlightThemes = {{173, 3, 3, 170}, {208, 39, 10, 150}, {201, 190, 188, 150},
            {249, 56, 20, 130}, {232, 15, 15, 135}, {247, 247, 131, 170}};
        tuple<int, int, int, int> colour = highlightThemes[colourIndex];

        SDL_SetRenderDrawColor(renderer, get<0>(colour), get<1>(colour), get<2>(colour), get<3>(colour));
        SDL_RenderFillRect(renderer, &tile);
    }


    void DrawPieces(SDL_Renderer *renderer){
        string base = "/Users/anirudhanvijay/Desktop/programs/SDLtest1/SDLtest1/Images/";
        string middle = "";
        string suffix = ".bmp";
        string path;
        
        for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){
                // retreiving piece from board[i][j]
                Piece piece = game.GetBoard()[i][j];
                if(piece.Type() != "empty"){
                    middle = piece.PieceID();
                    path = base + middle + suffix;

                    SDL_Rect dest;
                        dest.x = (j+TRANSLATION)*SQUARESIZE;
                        dest.y = (i+TRANSLATION)*SQUARESIZE;
                        dest.w = SQUARESIZE;
                        dest.h = SQUARESIZE;

                    // rendering piece image onto the screen, then freeing heap memory up
                    SDL_Surface *pieceImage = SDL_LoadBMP(path.c_str());
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, pieceImage);
                    SDL_RenderCopy(renderer, texture, NULL, &dest);
                    SDL_DestroyTexture(texture);
                    SDL_FreeSurface(pieceImage);
                }
                else{
                    continue;
                }
                path = "";
                middle = "";
            }
        }
    }

    void UpdateBoard(SDL_Renderer *renderer){
        // clearing previously presented image and preparing renderer and next frame
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 21, 16, 30, 10);
        SDL_RenderClear(renderer);

        // drawing board tiles onto screen
        DrawBoard(renderer);
        
        // highlighting squares of the previous move made
        HighlightMove(renderer);
        
        // drawing pieces onto the screen
        DrawPieces(renderer);
        GameTextUpdate();
        
        // presenting the frame
        SDL_RenderPresent(renderer);
    }

    static bool MouseCoordinatesOnBoard(int x, int y){
        if(((x < 0) || (y < 0))){
            return false;
        }
        if(((x > 7) || (y > 7))){
            return false;
        }
        return true;
    }

    
    void GameTextUpdate(){
        TTF_Font* font = TTF_OpenFont("/Users/anirudhanvijay/Desktop/programs/SDLtest1/SDLtest1/aphont-font/Aphont-nv0V.ttf", 22);
        SDL_Color white = {255, 255, 255};

        string text = "";
        if(game.Checkmate()){
            text = "Checkmate!";
        }
        if(game.Stalemate()){
            text = "Stalemate!";
        }
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), white);

        // convert it into a texture
        SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, textSurface);

        int textW = 30;
        int textH = 10;
        
        SDL_QueryTexture(Message, NULL, NULL, &textW, &textH);
        SDL_Rect textBoxEvaluation = {223 + SQUARESIZE, 80 + 9*SQUARESIZE, textW, textH};
        SDL_RenderCopy(renderer, Message, NULL, &textBoxEvaluation);

        // freeing up heap memory
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(Message);
        TTF_CloseFont(font);
    }
    
    
    void HideMenu() override {
        SDL_SetRenderDrawColor(renderer, 21, 16, 30, 10);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
    
    vector<SDL_Rect> DisplayMenu(vector<Button>& buttons, int startX, int startY) override {
        vector<SDL_Rect> buttonRects;
        TTF_Font* font = TTF_OpenFont("/Users/anirudhanvijay/Desktop/programs/SDLtest1/SDLtest1/aphont-font/Aphont-nv0V.ttf", 21);
        SDL_Color colour = {255, 255, 255};
        
        int currentY = startY;
        int spacing = 40;
        
        for (int i = 0; i < buttons.size(); i++) {
            if(currentY > 600){
                currentY = startY;
                startX += 300;
            }
            
            Button& button = buttons[i];
            SDL_Rect buttonRect = { startX, currentY, button.Width(), button.Height() };
            buttonRects.push_back(buttonRect);
            currentY += button.Height() + spacing;
        }
        
        // clearing the texture to black
        SDL_SetRenderDrawColor(renderer, 21, 16, 30, 100);
        SDL_RenderClear(renderer);
        
        // button colours
        SDL_SetRenderDrawColor(renderer, 149, 140, 160, 255);
        for (int i = 0; i < buttonRects.size(); i++) {
            SDL_RenderFillRect(renderer, &buttonRects[i]);
        }
        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        for (int i = 0; i < buttons.size(); i++) {
            Button& button = buttons[i];
            SDL_Surface* buttonTextSurface = TTF_RenderText_Blended(font, button.Text().c_str(), colour);
            SDL_Texture* buttonTextTexture = SDL_CreateTextureFromSurface(renderer, buttonTextSurface);
            int buttonTextX = buttonRects[i].x + (buttonRects[i].w - buttonTextSurface->w) / 2;
            int buttonTextY = buttonRects[i].y + (buttonRects[i].h - buttonTextSurface->h) / 2;
            SDL_Rect buttonTextRect = { buttonTextX, buttonTextY, buttonTextSurface->w, buttonTextSurface->h };
            SDL_RenderCopy(renderer, buttonTextTexture, NULL, &buttonTextRect);
            SDL_FreeSurface(buttonTextSurface);
            SDL_DestroyTexture(buttonTextTexture);
        }
        
        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_RenderPresent(renderer);
        return buttonRects;
    }
    
    
    vector<bool> LoadSettings(){
        FILE* fp = fopen("/Users/anirudhanvijay/Desktop/programs/SDLtest1/SDLtest1/settings.json", "r");

        char readBuffer[65536];
        vector<bool> settings = {false, false, false, false, false, false, false, false, false};
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        rapidjson::Document document;
        document.ParseStream(is);

        settings[0] = document["PlayervsAI"].GetBool();
        settings[1] = document["PlayerIsBlack"].GetBool();
        settings[2] = document["PlayerVSplayer"].GetBool();
        settings[3] = document["AIvsAI"].GetBool();
        settings[4] = document["SwitchThemes"].GetBool();
        settings[6] = document["Easy"].GetBool();
        settings[7] = document["Medium"].GetBool();
        settings[8] = document["Difficult"].GetBool();


        fclose(fp);
        return settings;
    }
    
    
    void SaveSettings(vector<bool> settings){
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

        writer.StartObject();
        
        // creating key-value pairs
        writer.Key("PlayervsAI");
        writer.Bool(settings[0]);

        writer.Key("PlayerIsBlack");
        writer.Bool(settings[1]);

        writer.Key("PlayerVSplayer");
        writer.Bool(settings[2]);

        writer.Key("AIvsAI");
        writer.Bool(settings[3]);

        writer.Key("SwitchThemes");
        writer.Bool(settings[4]);


        writer.Key("Easy");
        writer.Bool(settings[6]);

        writer.Key("Medium");
        writer.Bool(settings[7]);

        writer.Key("Difficult");
        writer.Bool(settings[8]);
        
        writer.EndObject();

        // writing data to json file
        FILE* fp = fopen("/Users/anirudhanvijay/Desktop/programs/SDLtest1/SDLtest1/settings.json", "w");
        fwrite(buffer.GetString(), 1, buffer.GetSize(), fp);

        fclose(fp);
    }
};

