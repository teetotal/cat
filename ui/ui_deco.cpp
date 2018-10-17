//
// Created by Jung, DaeCheon on 16/10/2018.
//

#include "ui_deco.h"

void ui_deco::init(Node * p, float degrees){
    mMainLayoput = p;
    mDegrees = degrees;
    mH = mMainLayoput->getContentSize().height * 0.5;
    mCenter = Vec2(mMainLayoput->getContentSize().width / 2, mMainLayoput->getContentSize().height / 2);
    
    mWallPostions.top = Vec2(mCenter.x, mMainLayoput->getContentSize().height);
    mWallPostions.bottom = Vec2(mCenter.x, mH);
    mWallPostions.left = Vec2(0, mH);
    mWallPostions.leftBottom = Vec2(0, 0);
    mWallPostions.right = Vec2(mMainLayoput->getContentSize().width, mH);
    mWallPostions.rightBottom = Vec2(mMainLayoput->getContentSize().width, 0);
    
}
void ui_deco::addBottom(bool isDraw, int div, POSITION_VECTOR &vec, Color4F color1, Color4F color2) {
    const float fH = mMainLayoput->getContentSize().height / (float)div;
    Rect dimension = Rect(0, -1 * mH, mMainLayoput->getContentSize().width, mMainLayoput->getContentSize().height);
    gui::inst()->addTiles(mMainLayoput, dimension, vec, Vec2(mCenter.x, mCenter.y - fH), fH, mDegrees, isDraw, color1, color2);
}

void ui_deco::addWall( bool isDraw
                       , int div
                       , POSITION_VECTOR * vecLeft
                       , POSITION_VECTOR * vecRight
                       , Color4F left1
                       , Color4F left2
                       , Color4F right1
                       , Color4F right2){
    
    const float hW = (mWallPostions.top.y - mCenter.y) / div;
    const float lenW = gui::inst()->getTanLen(hW/2, mDegrees);
    
    
    for(int n=0; n < div; n ++){
        Vec2 pos = Vec2(mCenter.x + lenW, mMainLayoput->getContentSize().height - (n * hW));
        Vec2 center = Vec2(pos.x - lenW, pos.y - hW / 2);
        Vec2 rTop = Vec2(pos.x - lenW, pos.y);
        Vec2 rBottom = Vec2(rTop.x, rTop.y - hW);
        Vec2 lTop = Vec2(center.x - lenW, center.y);
        Vec2 lBottom = Vec2(lTop.x, lTop.y - hW);
        
        //gui::inst()->drawRect(mMainLayoput, rTop, rBottom, lBottom, lTop, Color4F::ORANGE);
        gui::inst()->addWalls(true
                              , mMainLayoput
                              , Rect(Vec2(mWallPostions.left, mWallPostions.leftBottom), Size(mCenter.x - mWallPostions.left.x, mWallPostions.top.y))
                              , vecLeft
                              , Vec2(mCenter.x, mMainLayoput->getContentSize().height - (n * hW))
                              , hW, lenW
                              , isDraw
                              , (n % 2 == 0) ? left1 : left2
                              , (n % 2 == 0) ? left2 : left1
                              );
        
        gui::inst()->addWalls(false
                              , mMainLayoput
                              , Rect(Vec2(mCenter.x, 0), Size(mWallPostions.right.x - mCenter.x, mWallPostions.top.y))
                              , vecRight
                              , Vec2(mCenter.x, mMainLayoput->getContentSize().height - (n * hW))
                              , hW, lenW
                              , isDraw
                              , (n % 2 == 0) ? right1 : right2
                              , (n % 2 == 0) ? right2 : right1
                              );
    }
}

void ui_deco::drawGuidLine(){
    //guide line
    float xLen = gui::inst()->getTanLen(mH, mDegrees);
    Vec2 left = Vec2(mCenter.x - xLen, 0);
    Vec2 right = Vec2(mCenter.x + xLen, 0);
    Vec2 left2 = Vec2(mCenter.x - xLen, mH);
    Vec2 right2 = Vec2(mCenter.x + xLen, mH);
    
    auto draw = DrawNode::create();
    draw->setLineWidth(2);
    draw->drawLine(Vec2(mCenter.x, mMainLayoput->getContentSize().height), Vec2(mCenter.x, mH), Color4F::BLACK);
    draw->drawLine(Vec2(mCenter.x, mH), left, Color4F::BLACK);
    draw->drawLine(Vec2(mCenter.x, mH), right, Color4F::BLACK);
    
    draw->drawLine(Vec2(mCenter.x, mMainLayoput->getContentSize().height), left2, Color4F::BLACK);
    draw->drawLine(Vec2(mCenter.x, mMainLayoput->getContentSize().height), right2, Color4F::BLACK);
    
    mMainLayoput->addChild(draw);
    
    float margin = 5;
    gui::inst()->drawParallelogram(mMainLayoput
                                   , mWallPostions.rightBottom
                                   , Vec2(mCenter.x, -1 * mH)
                                   , Vec2(mWallPostions.rightBottom.x, mWallPostions.rightBottom.y - margin)
                                   , Vec2(mCenter.x, -1 * mH - margin)
                                   , Color4F::GRAY);
    
    gui::inst()->drawParallelogram(mMainLayoput
                                   , mWallPostions.leftBottom
                                   , Vec2(mCenter.x, -1 * mH)
                                   , Vec2(mWallPostions.leftBottom.x, mWallPostions.leftBottom.y - margin)
                                   , Vec2(mCenter.x, -1 * mH - margin)
                                   , Color4F::GRAY);
};
