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
    
    void init(Node * p, float degrees, bool isDebugModeBottom = false, bool isDebugModeWall = false);
    //바닥 좌표, 타일
    void addBottom(int posDiv, int drawDiv, Color4F color1, Color4F color2);
    //벽
    void addWall(int div, Color4F color1, Color4F color2, Color4F color3, Color4F color4);
    //벽
    void addWall(int div, Color4F color1, Color4F color2){
        addWall(div, getDarkColor(color1), getDarkColor(color2), color1, color2);
    }
    
    //raw func
    void createBottom(bool isDraw, int div, POSITION_VECTOR &vec, Color4F color1 = Color4F::BLACK, Color4F color2 = Color4F::GRAY);
    //just draw
    void drawBottom(int div, Color4F color1, Color4F color2){
        mTempVec.clear();
        createBottom(true, div, mTempVec, color1, color2);
    };
    
    //raw func
    void createWall(bool isDraw, int div, POSITION_VECTOR * vecLeft, POSITION_VECTOR * vecRight, Color4F left1 = Color4F::BLACK, Color4F left2 = Color4F::BLACK, Color4F right1 = Color4F::GRAY, Color4F right2 = Color4F::GRAY);
    //auto dark
    void createWall(int div, POSITION_VECTOR * vecLeft, POSITION_VECTOR * vecRight, Color4F color1, Color4F color2){
        createWall(true, div, vecLeft, vecRight, getDarkColor(color1), getDarkColor(color2), color1, color2);
    };
    void drawGuidLine();
    
    //objecs -------------
    void addObjectBottom(Sprite * sprite, int idx){
        addObject(sprite, mBottomVec, mBottomSpriteVec, idx);
    };
    void addObjectLeft(Sprite * sprite, int idx){
        addObject(sprite, mLeftVec, mLeftSpriteVec, idx);
    };
    void addObjectRight(Sprite * sprite, int idx){
        addObject(sprite, mRightVec, mRightSpriteVec, idx);
    };
    
    //touch -------------
    enum TOUCHED_SIDE{
        TOUCHED_SIDE_BOTTOM,
        TOUCHED_SIDE_LEFT,
        TOUCHED_SIDE_RIGHT,
        TOUCHED_SIDE_MAX
    };
    
    struct TOUCHED_INFO{
        TOUCHED_SIDE side;
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
    
private:
#define DARK_RATION 0.837

    bool mDebugModeBottom, mDebugModeWall;
    Node * mMainLayoput;
    float mDegrees;
    float mH;
    Vec2 mCenter;
    POSITION_VECTOR mTempVec, mBottomVec, mLeftVec, mRightVec;
    int mBottomDivCnt, mWallDivCnt;
    vector<Sprite*> mBottomSpriteVec, mLeftSpriteVec, mRightSpriteVec;
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
    
    void addObject(Sprite * sprite, POSITION_VECTOR &posVec, vector<Sprite*> &vec, int idx);
    
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
            case TOUCHED_SIDE_BOTTOM:
                return mBottomSpriteVec[p.idx];
            case TOUCHED_SIDE_LEFT:
                return mLeftSpriteVec[p.idx];
            case TOUCHED_SIDE_RIGHT:
                return mRightSpriteVec[p.idx];
            default:
                return NULL;
        }
    };
    
    vector<Sprite*> * getSpriteVec(TOUCHED_INFO &p){
        switch(p.side){
            case TOUCHED_SIDE_BOTTOM:
                return &mBottomSpriteVec;
            case TOUCHED_SIDE_LEFT:
                return &mLeftSpriteVec;
            case TOUCHED_SIDE_RIGHT:
                return &mRightSpriteVec;
            default:
                return NULL;
        }
    };
    
    POSITION_VECTOR * getPosVec(TOUCHED_INFO &p){
        switch(p.side){
            case TOUCHED_SIDE_BOTTOM:
                return &mBottomVec;
            case TOUCHED_SIDE_LEFT:
                return &mLeftVec;
            case TOUCHED_SIDE_RIGHT:
                return &mRightVec;
            default:
                return NULL;
        }
    };
    
    Size getGridSize(TOUCHED_INFO &p){
        switch(p.side){
            case TOUCHED_SIDE_BOTTOM:
                return mBottomGridSize;
            default:
                return mWallGridSize;
        }
    };
    
};

#endif //PROJ_ANDROID_UI_DECO_H
