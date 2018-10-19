//
// Created by Jung, DaeCheon on 19/10/2018.
//

#ifndef PROJ_ANDROID_UI_COLOR_H
#define PROJ_ANDROID_UI_COLOR_H

#include "ui.h"

class ui_color {
public:
    ui_color() {
    };
    virtual ~ui_color(){
    };
    
    static ui_color * inst(){
        if(hInst == NULL){
            hInst = new ui_color();
        }
        
        return hInst;
    };
    
    struct COLOR_RGB{
        int R;
        int G;
        int B;
        string name;
    };
    
    typedef vector<COLOR_RGB> COLOR_VECTOR;
    
    COLOR_VECTOR * get(){
        return &mVec;
    };
    void add(COLOR_RGB &color){
        mVec.push_back(color);
    };
    void add(int R, int G, int B, string name);
    COLOR_RGB getColor(int idx);
    Color3B getColor3B(int idx);
    Color4F getColor4F(int idx);
    
    
    
private:
    COLOR_VECTOR mVec;
    static ui_color * hInst;
};


#endif //PROJ_ANDROID_UI_COLOR_H
