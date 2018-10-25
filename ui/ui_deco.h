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
    static ui_deco * inst(){
        if(hInst == NULL){
            hInst = new ui_deco;
        }
        
        return hInst;
    };
    
    typedef vector<Vec2> POSITION_VECTOR;
    //touch -------------
    enum SIDE{
        SIDE_BOTTOM = 0,
        SIDE_LEFT,
        SIDE_RIGHT,
        SIDE_MAX
    };
    
    struct OBJECT {
        int id; // for item id
        Sprite * sprite;
        SIDE side;
        int idx; // for position
        
        OBJECT(int id, Sprite * sprite, SIDE side, int idx){
            this->id = id;
            this->sprite = sprite;
            this->side = side;
            this->idx = idx;
        };
    };
    
    void init(Node * p, float degrees, bool isDebugModeBottom = false, bool isDebugModeWall = false);
    int getDefaultBottomIdx(){
        return (int)(mBottomVec.size() / 2);
    };
    //복사
    void clone(Node * pParentNode, ui_deco * p){
        init(pParentNode, p->mDegrees);
        addBottom(p->mBottomDivCnt, p->mDrawBottomDivCnt, p->mBottomColor1, p->mBottomColor2);
        addWall(p->mWallDivCnt, p->mWallColor1, p->mWallColor2);
        drawGuidLine();
    };
    //바닥 좌표, 타일
    void addBottom(int posDiv, int drawDiv, Color4F color1, Color4F color2);
    //벽
    void addWall(int div, Color4F color1, Color4F color2);
    //벽
    void addWall(int div, Color4F color){
        addWall(div, color, getDarkColor(color));
    }
    
    void changeColorBottom(Color4F color1, Color4F color2);
    void changeColorBottom(Color4F color){
        changeColorBottom(color, color);
    };
    
    void changeColorWall(Color4F color1, Color4F color2);
    void changeColorWall(Color4F color){
        changeColorWall(color, getDarkColor(color));
    };
    
    //raw func
    void createBottom(bool isDraw, int div, POSITION_VECTOR &vec, Color4F color1 = Color4F::BLACK, Color4F color2 = Color4F::GRAY);
    //just draw
    void drawBottom(int div, Color4F color1, Color4F color2){
        mTempVec.clear();
        createBottom(true, div, mTempVec, color1, color2);
    };
    
    //raw func
    void createWall(bool isDraw, int div, POSITION_VECTOR * vecLeft, POSITION_VECTOR * vecRight, Color4F color1 = Color4F::GRAY, Color4F color2 = Color4F::BLACK);
    //auto dark
    void createWall(int div, POSITION_VECTOR * vecLeft, POSITION_VECTOR * vecRight, Color4F color){
        createWall(true, div, vecLeft, vecRight, color, getDarkColor(color));
    };
    void drawGuidLine();
    
    //objecs -------------
    void addObjectBottom(OBJECT &obj){
        addObject(obj, mBottomVec, mBottomSpriteVec);
    };
    void addObjectLeft(OBJECT &obj){
        addObject(obj, mLeftVec, mLeftSpriteVec);
    };
    void addObjectRight(OBJECT &obj){
        addObject(obj, mRightVec, mRightSpriteVec);
    };
    
    
    struct TOUCHED_INFO{
        SIDE side;
        int idx;
        time_t firstTouchTime;
        void copy(TOUCHED_INFO *p){
            this->side = p->side;
            this->idx = p->idx;
            this->firstTouchTime = p->firstTouchTime;
        };
    } mTouchedInfo;
    
    void touchBegan(Vec2 pos);
    void touchEnded(Vec2 pos);
    void touchMoved(Vec2 pos);
    
    
    //utils
    Size getBottomGridSize(){
        float fH =  mMainLayoput->getContentSize().height / (float)mBottomDivCnt;
        return Size(gui::inst()->getTanLen(fH, mDegrees) * 2, fH * 2);
    };
    Size getWallGridSize(){
        float fH =  mMainLayoput->getContentSize().height / (float)mWallDivCnt;
        return Size(gui::inst()->getTanLen(fH, mDegrees), fH);
    };
    
    float mDegrees;
    int mBottomDivCnt, mDrawBottomDivCnt, mWallDivCnt;
    Color4F mBottomColor1, mBottomColor2, mWallColor1, mWallColor2;
    
private:
#define DARK_RATION 0.837
    
    enum LAYER{
        LAYER_WALL = 0,
        LAYER_WALL_TEMP,
        LAYER_BOTTOM,
        LAYER_BOTTOM_TEMP,
        LAYER_GUIDELINE,
        LAYER_OBJECT,
        LAYER_MAX
    };

    bool mDebugModeBottom, mDebugModeWall;
    Node * mParentLayoput, * mMainLayoput;
    Layout * mLayout[LAYER_MAX];
    
    float mH;
    Vec2 mCenter;
    POSITION_VECTOR mTempVec, mBottomVec, mLeftVec, mRightVec;
    vector<OBJECT> mBottomSpriteVec, mLeftSpriteVec, mRightSpriteVec;
    Size mBottomGridSize, mWallGridSize;
    
    struct WALL_POSITIONS{
        Vec2 top;
        Vec2 bottom;
        Vec2 left;
        Vec2 leftBottom;
        Vec2 right;
        Vec2 rightBottom;
    } mWallPostions;
    
    Color4F getDarkColor(Color4F color);
    
    void addObject(OBJECT &obj, POSITION_VECTOR &posVec, vector<OBJECT> &vec);
    
    static bool sortTouchVec(Vec2 a, Vec2 b){
        if(b.y < a.y)
            return true;
        else if(b.y == a.y && b.x > a.x)
            return true;
        
        return false;
    };
    
    static bool sortTouchVecLeft(Vec2 a, Vec2 b){
//        CCLOG("%f, %f - %f, %f", a.x, a.y, b.x, b.y);
        if(b.y < a.y)
            return true;
        else if(b.y == a.y && b.x < a.x)
            return true;
        
        return false;
    };
    
    //touch
    Vec2 mTouchStart;
    Label * mTouchPointLabel;
    
    TOUCHED_INFO mTouchedInfoLast;
    
    Sprite * getSprite(TOUCHED_INFO &p){
        switch(p.side){
            case SIDE_BOTTOM:
                return mBottomSpriteVec[p.idx].sprite;
            case SIDE_LEFT:
                return mLeftSpriteVec[p.idx].sprite;
            case SIDE_RIGHT:
                return mRightSpriteVec[p.idx].sprite;
            default:
                return NULL;
        }
    };
    
    vector<OBJECT> * getSpriteVec(TOUCHED_INFO &p){
        switch(p.side){
            case SIDE_BOTTOM:
                return &mBottomSpriteVec;
            case SIDE_LEFT:
                return &mLeftSpriteVec;
            case SIDE_RIGHT:
                return &mRightSpriteVec;
            default:
                return NULL;
        }
    };
    
    POSITION_VECTOR * getPosVec(TOUCHED_INFO &p){
        switch(p.side){
            case SIDE_BOTTOM:
                return &mBottomVec;
            case SIDE_LEFT:
                return &mLeftVec;
            case SIDE_RIGHT:
                return &mRightVec;
            default:
                return NULL;
        }
    };
    
    Size getGridSize(TOUCHED_INFO &p){
        switch(p.side){
            case SIDE_BOTTOM:
                return mBottomGridSize;
            default:
                return mWallGridSize;
        }
    };
    
    static ui_deco * hInst;
    
};

#endif //PROJ_ANDROID_UI_DECO_H
