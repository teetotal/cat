//
// Created by daejung on 2018-07-24.
//

#ifndef PROJ_ANDROID_GRID_H
#define PROJ_ANDROID_GRID_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#define GRID_X 9
#define GRID_Y 7
#define GRID_DEFAULT_FONT_SIZE 14
#define GRID_MARGIN 10
#define GRID_INVALID_VALUE -1
#define DEFAULT_OPACITY 150

using namespace cocos2d;
using namespace cocos2d::ui;
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
    float getRealPixel(float x);
    float getSizeFromRealPixel(float x);

    bool getPoint(int x, int y, Vec2 &point, ALIGNMENT align = ALIGNMENT_NONE
            , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
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
            , const string img = ""
            , bool isBGImg = true
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
            , const string img = ""
            , bool isBGImg = true
    );

    LayerColor * addPopup(LayerColor * &pBG, Node * p, Size size, const string bgImg = "", Color4B bgColor = Color4B::WHITE);
    Layout * createLayout(Size size, const string bgImg = "", bool hasBGColor = false, Color3B bgColor = Color3B::WHITE);
    void addLayoutToScrollView(ScrollView * p, Layout * e, float margin, ScrollView::Direction d = ScrollView::Direction::HORIZONTAL);
    ScrollView * addScrollView(Vec2 p1, Vec2 p2, Size size, Size margin, const string bgImg = "");
    LoadingBar * addProgressBar(int x, int y, const string img, Node * p, float defaultVal = 0.f);

private:
    float mOriginX, mOriginY;
    float mVisibleX, mVisibleY;
    float mWidth;
    float mHeight;

    Size mResolution;

    const char* mDefaultFont;
    int mDefaultFontSize;

    static gui * hInstance;

    const Vec2 getNoneAnchorPoint(){
        return Vec2(0,1.75);
    };

    void getPoint(int x, int y
            , float &pointX_Center, float &pointY_Center
            , float &pointX_None, float &pointY_None
            , ALIGNMENT align
            , Size dimension
            , Size grid
            , Size origin
            , Size margin);
};


#endif //PROJ_ANDROID_GRID_H
