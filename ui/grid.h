//
// Created by daejung on 2018-07-24.
//

#ifndef PROJ_ANDROID_GRID_H
#define PROJ_ANDROID_GRID_H

#include "cocos2d.h"

#define GRID_X 9
#define GRID_Y 7
#define GRID_DEFAULT_FONT_SIZE 14
#define GRID_MARGIN 10

using namespace cocos2d;
using namespace std;
enum ALIGNMENT{
    ALIGNMENT_NONE = 0,
    ALIGNMENT_CENTER
};

typedef void (*Callback1)(Ref* pSender);

class grid {
public:
    grid(){};
    ~grid(){};
    void init(float originX, float originY, float width, float height, const char* font = "fonts/Marker Felt.ttf", int fontSize = GRID_DEFAULT_FONT_SIZE);
    bool getPoint(int x, int y, float &pointX, float &pointY, ALIGNMENT align = ALIGNMENT_NONE);
    bool drawGrid(Scene * p);
    bool drawPoint(Scene * p);
    Label * addLabel(int x
            , int y
            , const string &text
            , Scene *p
            , int fontSize = 0
            , ALIGNMENT align = ALIGNMENT_CENTER
            , const Color3B color = Color3B::BLACK);
    MenuItem* addTextButton(int x
            , int y
            , const char * text
            , Scene* p
            , const ccMenuCallback& callback
            , int fontSize = 0
            , ALIGNMENT align = ALIGNMENT_CENTER
            , const Color3B color = Color3B::BLACK);
private:
    float mOriginX, mOriginY;
    float mWidth;
    float mHeight;

    float mGridWidth, mGridHeight;
    const char* mDefaultFont;
    int mDefaultFontSize;

    const Vec2 getNoneAnchorPoint(){
        return Vec2(0,1.75);
    };
};


#endif //PROJ_ANDROID_GRID_H
