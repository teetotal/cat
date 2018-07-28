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
#define GRID_INVALID_VALUE -1
#define DEFAULT_OPACITY 150

using namespace cocos2d;
using namespace std;
enum ALIGNMENT{
    ALIGNMENT_NONE = 0,
    ALIGNMENT_CENTER
};

class gui {
public:
    gui(){
        hInstance = this;
    };
    ~gui(){
        hInstance = NULL;
    };
    static gui * inst(){
        return hInstance;
    };
    void init(const char* font = "fonts/Marker Felt.ttf"
            , int fontSize = GRID_DEFAULT_FONT_SIZE
            , Color4F bgColor = Color4F::WHITE
    );
    bool getPoint(int x, int y, float &pointX, float &pointY, ALIGNMENT align = ALIGNMENT_NONE
                  , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                  , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                  , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
                  , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
    );
    bool drawGrid(Node * p
            , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
    );
    bool drawPoint(Node * p);
    Label * addLabel(int x
            , int y
            , const string &text
            , Node *p
            , int fontSize = 0
            , ALIGNMENT align = ALIGNMENT_CENTER
            , const Color3B color = Color3B::BLACK
            , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
    );
    MenuItem* addTextButton(int x
            , int y
            , const char * text
            , Node* p
            , const ccMenuCallback& callback
            , int fontSize = 0
            , ALIGNMENT align = ALIGNMENT_CENTER
            , const Color3B color = Color3B::BLACK
            , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
    );

    LayerColor * addPopup(LayerColor * &pBG, Node * p, Size size, const string bgImg = "", Color4B bgColor = Color4B::WHITE);
private:
    float mOriginX, mOriginY;
    float mVisibleX, mVisibleY;
    float mWidth;
    float mHeight;

    const char* mDefaultFont;
    int mDefaultFontSize;

    static gui * hInstance;

    const Vec2 getNoneAnchorPoint(){
        return Vec2(0,1.75);
    };
};


#endif //PROJ_ANDROID_GRID_H
