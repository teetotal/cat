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

#define DEFAULT_LAYER_SIZE Size(385, 200)

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace std;

enum ALIGNMENT{
    ALIGNMENT_NONE = 0,
    ALIGNMENT_CENTER
};

//레벨에 따른 이미지 출력을 위한 구조체
struct IMG_LEVEL {
	const string img;
	float level;
};

typedef std::vector<IMG_LEVEL> IMG_LEVEL_VECTOR;

class gui {
public:
    gui(){
        if(hInstance == NULL)
            hInstance = this;
    };
    ~gui(){
        //hInstance = NULL;
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

	Vec2 getPointVec2(int x, int y, ALIGNMENT align = ALIGNMENT_CENTER) {
		Vec2 p;
		gui::inst()->getPoint(x, y, p, align);
		return p;
	};

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
            , Scene *p
            , int fontSize = 0
            , ALIGNMENT align = ALIGNMENT_CENTER
            , const Color3B color = Color3B::BLACK
            , const string img = ""
            , bool isBGImg = true
    );

    Label * addLabelAutoDimension(int x
            , int y
            , const string text
            , Node *p
            , int fontSize = 0
            , ALIGNMENT align = ALIGNMENT_CENTER
            , const Color3B color = Color3B::BLACK
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , const string img = ""
            , bool isBGImg = true
    );

    Label * addLabel(Node *p
            , int x
            , int y
            , const string text
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
    MenuItemFont* addTextButtonAutoDimension(int x
            , int y
            , const string text
            , Node* p
            , const ccMenuCallback& callback
            , int fontSize = 0
            , ALIGNMENT align = ALIGNMENT_CENTER
            , const Color3B color = Color3B::BLACK
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , const string img = ""
            , bool isBGImg = true
    );
    MenuItemFont* addTextButton(int x
            , int y
            , const string text
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

    Sprite* addSprite(int x
            , int y
            , const string img
            , Node* p
            , ALIGNMENT align = ALIGNMENT_CENTER
            , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
    );

    Sprite* addSpriteFixedSize(const Size &spriteSize
            , int x
            , int y
            , const string img
            , Node* p
            , ALIGNMENT align = ALIGNMENT_CENTER
            , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
    );

    Sprite* addSpriteAutoDimension(int x
            , int y
            , const string img
            , Node* p
            , ALIGNMENT align = ALIGNMENT_CENTER
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
    );

    LayerColor * addPopup(LayerColor * &pBG
            , Node * p
            , Size size
            , const string bgImg = ""
            , Color4B bgColor = Color4B::WHITE
    );
    LayerColor * createModalLayer(LayerColor * &pBG
            , Size size
            , const string bgImg = ""
            , Color4B bgColor = Color4B::WHITE
    );

    Layout * createLayout(Size size
            , const string bgImg = ""
            , bool hasBGColor = false
            , Color3B bgColor = Color3B::WHITE
    );

    void addLayoutToScrollView(ScrollView * p
            , Layout * e
            , float margin
            , int newlineInterval = 0
    );

	Size getScrollViewSize(Vec2 p1, Vec2 p2, Size size, Size margin);

    ScrollView * addScrollView(Vec2 p1
            , Vec2 p2
            , Size size
            , Size margin
            , const string bgImg = ""
            , Size innerSize = Size(0, 0)
    );

    LoadingBar * addProgressBar(int x
            , int y
            , const string img
            , Node * p
            , float defaultVal = 0.f
			, Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
			, Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
			, Size origin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
			, Size margin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
    );

	void setScale(Node * p, float targetWidth) {
		float ratio = targetWidth / p->getContentSize().width;
		p->setScale(ratio);
	};

    //이미지 하나를 3배 복제해서 스크롤링, 리턴 없음
    void addBGScrolling(const string img, Node * p, float duration);

	int mModalTouchCnt;

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
