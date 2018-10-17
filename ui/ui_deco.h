//
// Created by Jung, DaeCheon on 05/08/2018.
//

#ifndef PROJ_ANDROID_UI_DECO_H
#define PROJ_ANDROID_UI_DECO_H

#include "ui.h"

class ui_deco {
public:
    ui_deco() {
		
    };
    virtual ~ui_deco(){
        CCLOG("~ui_deco released");
    };
    
    
    typedef vector<Vec2> POSITION_VECTOR;
    
    void init(Node * p, float degrees);
    void addBottom(bool isDraw, int div, POSITION_VECTOR &vec, Color4F color1 = Color4F::BLACK, Color4F color2 = Color4F::GRAY);
    
    void addWall(bool isDraw, int div, POSITION_VECTOR * vecLeft, POSITION_VECTOR * vecRight, Color4F left1 = Color4F::BLACK, Color4F left2 = Color4F::BLACK, Color4F right1 = Color4F::GRAY, Color4F right2 = Color4F::GRAY);
    void addWall(int div, POSITION_VECTOR * vecLeft, POSITION_VECTOR * vecRight, Color4F color1, Color4F color2){
        addWall(true, div, vecLeft, vecRight, getDarkColor(color1), getDarkColor(color2), color1, color2);
    };
    void drawGuidLine();
    
private:
#define DARK_RATION 0.837

    Node * mMainLayoput;
    float mDegrees;
    float mH;
    Vec2 mCenter;
    struct WALL_POSITIONS{
        Vec2 top;
        Vec2 bottom;
        Vec2 left;
        Vec2 leftBottom;
        Vec2 right;
        Vec2 rightBottom;
    } mWallPostions;
    
    Color4F getDarkColor(Color4F color){
        Color4F c;
        c.r = DARK_RATION * color.r;
        c.g = DARK_RATION * color.g;
        c.b = DARK_RATION * color.b;
        c.a = color.a;
        return c;
    };
};

#endif //PROJ_ANDROID_UI_DECO_H
