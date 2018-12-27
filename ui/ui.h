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

#define DEFAULT_LAYER_SIZE Size(420, 200)

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

#define POPUP_LIST(_PARENT_NODE, _GRID_SIZE, _NEWLINE, _START_VEC2, _END_VEC2, _MARGIN, _NODE_MARGIN, _NODE_SIZE, _FOR, _CONTINUE, _IMG, _CALLBACK, _SZ0, _SZ1, _SZ2, _SZ3, _SZ4, _IMG_POINTER) \
    Color3B fontColor0 = Color3B::BLACK; \
    Color3B fontColor1 = Color3B::BLACK; \
    Color3B fontColor2 = Color3B::BLACK; \
    Color3B fontColor3 = Color3B::BLACK; \
    Color3B fontColor4 = Color3B::BLACK; \
    bool isEnable = true; \
	int nCnt = 0; \
	for _FOR { \
		_CONTINUE \
		nCnt++; \
	} \
	Size __nodeSize = _NODE_SIZE; \
	Size sizeOfScrollView = gui::inst()->getScrollViewSize(_START_VEC2, _END_VEC2, _PARENT_NODE->getContentSize(), Size(-1,-1), Size::ZERO, _MARGIN); \
	if(_NEWLINE > 0) { \
		__nodeSize.width = (sizeOfScrollView.width  - (float)((_NEWLINE + 1) * _NODE_MARGIN)) / (float)_NEWLINE; \
	} else { \
		__nodeSize.height = sizeOfScrollView.height; \
	} \
	Size innerSize = (_NEWLINE > 0) ? Size(sizeOfScrollView.width, ((nCnt / _NEWLINE) + 1) * (__nodeSize.height + _NODE_MARGIN)) : Size((__nodeSize.width  + _NODE_MARGIN) * nCnt, __nodeSize.height + _NODE_MARGIN);\
    ScrollView * sv = gui::inst()->addScrollView(_START_VEC2, _END_VEC2, _PARENT_NODE->getContentSize(), Size(-1,-1), Size::ZERO, _MARGIN, "", innerSize); \
	for _FOR { \
		_CONTINUE \
		Layout* l = gui::inst()->createLayout(__nodeSize, "", true, Color3B::WHITE); \
		l->setOpacity(192); \
		int heightIdx = 0; \
		LayerColor * colorLayer = _IMG_POINTER; \
        if(colorLayer != NULL){ \
            Vec2 pos = gui::inst()->getPointVec2(0, 2, ALIGNMENT_CENTER, l->getContentSize(), _GRID_SIZE, Size::ZERO, Size::ZERO);\
            pos.x -= 10;\
            pos.y -= 10;\
            colorLayer->setPosition(pos); \
            l->addChild(colorLayer);\
        } else{ \
            if(_IMG.compare(gui::inst()->EmptyString) != 0) { \
                Menu * pSpriteMenu; \
                auto pMenuSprite = gui::inst()->addSpriteButtonRaw(pSpriteMenu, 0, 2, _IMG, _IMG, l, _CALLBACK, ALIGNMENT_CENTER, l->getContentSize(), _GRID_SIZE, Size::ZERO, Size::ZERO); \
                gui::inst()->setScale(pMenuSprite, 20); \
            } \
        } \
        gui::inst()->addTextButtonAutoDimension(0, 1, _SZ0, l, _CALLBACK, 10, ALIGNMENT_CENTER, fontColor0, _GRID_SIZE, Size::ZERO, Size::ZERO); \
		heightIdx = 1;\
        auto btn1 = gui::inst()->addTextButtonAutoDimension(1, heightIdx++, _SZ1, l, _CALLBACK, 12, ALIGNMENT_NONE, fontColor1, _GRID_SIZE, Size::ZERO, Size::ZERO); \
		auto btn2 = gui::inst()->addTextButtonAutoDimension(1, heightIdx++, _SZ2, l, _CALLBACK, 12, ALIGNMENT_NONE, fontColor2, _GRID_SIZE, Size::ZERO, Size::ZERO); \
		auto btn3 = gui::inst()->addTextButtonAutoDimension(1, heightIdx++, _SZ3, l, _CALLBACK, 12, ALIGNMENT_NONE, fontColor3, _GRID_SIZE, Size::ZERO, Size::ZERO); \
		auto btn4 = gui::inst()->addTextButtonAutoDimension(1, heightIdx++, _SZ4, l, _CALLBACK, 12, ALIGNMENT_NONE, fontColor4, _GRID_SIZE, Size::ZERO, Size::ZERO); \
        if(!isEnable) { \
            btn1->setEnabled(isEnable); \
            btn2->setEnabled(isEnable); \
            btn3->setEnabled(isEnable); \
            btn4->setEnabled(isEnable); \
        } \
		gui::inst()->addLayoutToScrollView(sv, l, _NODE_MARGIN, _NEWLINE); \
	} \
    _PARENT_NODE->removeChildByTag(99, true); \
    _PARENT_NODE->addChild(sv, 1, 99); 

class gui {
public:
    gui(){
		if (hInstance == NULL) {		
			EmptyString = " ";
			hInstance = this;
		}
            
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

    Vec2 getPointVec2(int x, int y, ALIGNMENT align = ALIGNMENT_CENTER
                      , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                      , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                      , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
                      , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)) {
		Vec2 p;
		gui::inst()->getPoint(x, y, p, align, dimension, grid, origin, margin);
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
    
    Sprite * addBG(const string bgImg, Node * parent);
    
    Label * addLabel(int x
            , int y
            , const string &text
            , Node *p
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
    
    Label * createLabel(int x
                     , int y
                     , const string text
                     , int fontSize = 0
                     , ALIGNMENT align = ALIGNMENT_CENTER
                     , const Color3B color = Color3B::BLACK
                     , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                     , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                     , Size origin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
                     , Size margin = Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
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
    MenuItemLabel* addTextButtonAutoDimension(int x
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
	MenuItemLabel * addTextButton(int x
		, int y
		, const string text
		, Node* p
		, const ccMenuCallback& callback
		, int fontSize = 0
		, ALIGNMENT align = ALIGNMENT_CENTER
		, const Color3B color = Color3B::BLACK
		, Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, Size origin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, Size margin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, const string img = ""
		, bool isBGImg = true
	) {
		Menu * pMenu = NULL;
		return addTextButtonRaw(pMenu, x, y, text, p, callback, fontSize, align, color, dimension, grid, origin, margin, img, isBGImg);
	};

    MenuItemLabel* addTextButtonRaw(
			Menu* &pMenu
			, int x
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

	MenuItemImage * addSpriteButton(int x
		, int y
		, const string normalImg
		, const string selectImg
		, Node *p
		, const ccMenuCallback &callback
		, ALIGNMENT align = ALIGNMENT_CENTER
		, Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, Size origin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, Size margin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
	) {
		Menu * pMenu = NULL;
		return addSpriteButtonRaw(pMenu, x, y, normalImg, selectImg, p, callback, align, dimension, grid, origin, margin);
	};

	MenuItemImage * addSpriteButtonRaw(
		Menu* &pMenu
		, int x
		, int y
		, const string normalImg
		, const string selectImg
		, Node *p
		, const ccMenuCallback &callback
		, ALIGNMENT align = ALIGNMENT_CENTER
		, Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, Size origin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
		, Size margin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
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
    
    Sprite * getfittedSprite(const string sz, Node * node) {
        Sprite * sprite = Sprite::create(sz);
        sprite->setPosition(Vec2(node->getContentSize().width / 2, node->getContentSize().height / 2));
        sprite->setContentSize(node->getContentSize());
        return sprite;
    };

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

	Size getScrollViewSize(Vec2 p1, Vec2 p2, Size size, Size grid, Size origin, Size margin);

    ScrollView * addScrollView(Vec2 p1
            , Vec2 p2
            , Size size, Size grid, Size origin, Size margin
            , const string bgImg = ""
            , Size innerSize = Size(0, 0)
			, Node * parent = NULL
    );

    LoadingBar * addProgressBar(int x
            , int y
            , const string img
            , Node * p
            , float width
            , float defaultVal = 0.f
			, Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
			, Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
			, Size origin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
			, Size margin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
			, LoadingBar::Direction direction = LoadingBar::Direction::LEFT
    );

	Sequence * createActionFocus() { 
		float raiseDuration = 0.3;
		float returnDuration = 0.1;
		float scale = 1.5f;

		return Sequence::create(
			ScaleTo::create(raiseDuration, scale), ScaleTo::create(returnDuration, 1), NULL
		);
	};

	RepeatForever * createActionBlink() {
		return RepeatForever::create(Sequence::create(ScaleTo::create(0.3, 1.2), ScaleTo::create(0.3, 1), NULL));
	}

	void actionHarvest(Node * p, string img, int cnt, Vec2 from, Vec2 to, Size size) {		
		for (int n = 0; n < cnt; n++) {

			auto sprite = Sprite::create(img);
			sprite->setPosition(from);
			sprite->setContentSize(size);
			p->addChild(sprite);

			auto delay1 = DelayTime::create(n * 0.03);
			auto delay2 = delay1->clone();
			auto seq1 = Sequence::create(delay1, MoveTo::create(0.5, to), RemoveSelf::create(), NULL);
			auto seq2 = Sequence::create(delay2, ScaleTo::create(0.2, 1.5), ScaleTo::create(0.3, 0.5), NULL);

			sprite->runAction(Spawn::create(seq1, seq2, NULL));
		}
	}


	//수량 UI Layer
	Layout * addQuantityLayer(Node * p, Size size, Size margin
		, Sprite* &sprite, Label * &label, Label * &labelQuantity, Label* &labelPrice
		, const string szSubmit
		, const ccMenuCallback& callbackDec
		, const ccMenuCallback& callbackInc
		, const ccMenuCallback& callbackSubmit);

    float getScale(Node * p, float targetWidth){
        return targetWidth / p->getContentSize().width;
    };
    float getScaleByHeight(Node * p, float targetHeight){
        return targetHeight / p->getContentSize().height;
    };
    //set Scale
	void setScale(Node * p, float targetWidth) {
		//float ratio = targetWidth / p->getContentSize().width;
		p->setScale(getScale(p, targetWidth));
	};
    void setScaleByHeight(Node * p, float targetHeight) {
        p->setScale(getScaleByHeight(p, targetHeight));
    };

	//정 중앙 포인트 가져오기
	Vec2 getCenterFromSize(Size size) {
		return Vec2(size.width / 2, size.height / 2);
	};
	Vec2 getCenter() {
		Size size = Director::getInstance()->getVisibleSize();
		Vec2 point = Vec2(size.width / 2, size.height / 2);
		point.x += Director::getInstance()->getVisibleOrigin().x;
		point.y += Director::getInstance()->getVisibleOrigin().y;
		return point;
	};

    //이미지 하나를 3배 복제해서 스크롤링, 리턴 없음
    void addBGScrolling(const string img, Node * p, float duration);

	//particle
	ParticleSystemQuad * createParticle(string szParticle, int x, int y) {
		return createParticle(szParticle, getPointVec2(x, y));
	};
	ParticleSystemQuad * createParticle(string szParticle, Vec2 position) {
		auto particle = ParticleSystemQuad::create(szParticle);
		particle->setPosition(position);

		return particle;
	};

	void addToCenter(Node * p, Node * pParent) {
		p->setAnchorPoint(Vec2(0.5, 0.5));
		p->setPosition(getCenter());
		pParent->addChild(p);
	};
    DrawNode * drawCircle(Node * p, Vec2 pos, float radius, Color4F color);
    DrawNode * drawTriangle(Node * p, Vec2 a, Vec2 b, Vec2 c, Color4F color);
    DrawNode * drawRect(Node * p, Vec2 pos, Size size, Color4F color);
    DrawNode * drawRect(Node * p, Vec2 pos1, Vec2 pos2, Vec2 pos3, Vec2 pos4, Color4F color);
    DrawNode * drawDiamond(Node * p, Vec2 pos, Size size, Color4F color);
    float drawDiamond(Node * p, Vec2 center, float h, float degrees, Color4F color);
    float getTanLen(float h, float degrees) {
//        const double pi = std::acos(-1);
        //180 X = degree
        //return h / std::tan(degrees * pi / 180);
        
        return h/ tan(CC_DEGREES_TO_RADIANS(degrees));
    }
    void addTiles(Node * p, Rect dimension, vector<Vec2> &vec
                  , Vec2 start
                  , float h
                  , float degrees
                  , bool isBGColor = false
                  , Color4F color1 = Color4F::BLACK
                  , Color4F color2 = Color4F::GRAY
                  , bool isLeft = true
                  , bool isRight = true
                  , Vec2 debugPos = Vec2::ZERO
                  , Vec2 debugPos2 = Vec2::ZERO
                  );
    void addWalls(bool isLeft, Node * p, Rect dimension, vector<Vec2> *vec, Vec2 pos, float h, float len
                  , bool isBGColor = false
                  , Color4F color1 = Color4F::BLACK
                  , Color4F color2 = Color4F::GRAY );
    void drawParallelogram(Node * p, Vec2 top1, Vec2 top2, Vec2 bottom1, Vec2 bottom2, Color4F color);
    
    //벡터 외적. 음수 = false
    bool vectorCross(Vec2 a, Vec2 b, Vec2 c, Vec2 x);
    
    string getStar(int point){
        //★☆
        /*
         1-25 ☆
         26-40 ★
         41-60 ★☆
         61-75 ★★
         76-90 ★★☆
         91- ★★★
         */
        if(point <=0)
            return " ";
        else if(point > 0 && point <= 25)
            return "☆";
        else if(point > 25 && point <= 40)
            return "★";
        else if(point > 40 && point <= 60)
            return "★☆";
        else if(point > 60 && point <= 75)
            return "★★";
        else if(point > 75 && point <= 90)
            return "★★☆";
        else
            return "★★★";
        
    };
    
    Size getVisibleSize() {
        return Size(mVisibleSize.x, mVisibleSize.y);
    };
    //tile 영역 체크에서 사용
    Rect getResizedRect(const Rect rect, float scale, bool isAlignToCenter = true);
    
	int mModalTouchCnt;
	string EmptyString;

	float mOriginX, mOriginY;
	float mVisibleX, mVisibleY;
	float mWidth;
	float mHeight;

    Vec2 mOrigin;
    Vec2 mVisibleSize;
	Size mResolution;
private:
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
    
    bool isExistVec2(vector<Vec2> &vec, Vec2 point);
};


#endif //PROJ_ANDROID_GRID_H
