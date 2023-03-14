#pragma once
#include <iostream>
#include "App.h"

using namespace std;

class Button {
private:
    
    int width;
    int height;
    bool hoveringOver;
    bool clicked;
    bool gameVariable;
    string text;
    
    bool toggle;

public:
    
    Button(int width, int height, string value){
        this->width = width;
        this->height = height;
        hoveringOver = false;
        clicked = false;
        text = value;
        toggle = false;
    }
    
    
    int Width(){
        return width;
    }
    
    int Height(){
        return height;
    }
    
    string Text(){
        return text;
    }
    
    bool OnClick(){
        toggle = !toggle;
        return toggle;
    }
};


