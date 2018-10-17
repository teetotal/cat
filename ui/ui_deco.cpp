//
// Created by Jung, DaeCheon on 16/10/2018.
//

#include "ui_deco.h"
#include "../library/util.h"

void ui_deco::init(Node * p, float degrees, bool isDebugModeBottom, bool isDebugModeWall){
    mDebugModeBottom = isDebugModeBottom;
    mDebugModeWall = isDebugModeWall;
    
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
    
    mTouchedInfo.side = TOUCHED_SIDE_MAX;
    
}
//바닥 좌표, 타일
void ui_deco::addBottom(int posDiv, int drawDiv, Color4F color1, Color4F color2){
    mBottomDivCnt = posDiv;
    createBottom(false, posDiv, mBottomVec);
    std::sort (mBottomVec.begin(), mBottomVec.end(), ui_deco::sortTouchVec);

    drawBottom(drawDiv, color1, color2);
    mBottomGridSize = getBottomGridSize();

    if(mDebugModeBottom){
        for(int n=0; n< mBottomVec.size(); n++){
            auto posLabel = Label::create();
            posLabel->setSystemFontSize(8);
            posLabel->setString(to_string(n));
            posLabel->setPosition(mBottomVec[n]);
            mMainLayoput->addChild(posLabel);
            
        }
    }
}
//벽
void ui_deco::addWall(int div, Color4F color1, Color4F color2, Color4F color3, Color4F color4){
    createWall(true, div, &mLeftVec, &mRightVec, color1, color2, color3, color4);
    std::sort (mRightVec.begin(), mRightVec.end(), ui_deco::sortTouchVec);
    std::sort (mLeftVec.begin(), mLeftVec.end(), ui_deco::sortTouchVecLeft);
    mWallDivCnt = div;
    mWallGridSize = getWallGridSize();
//
//    for(int n=0; n< mRightVec.size(); n++){
//        auto p = gui::inst()->addLabelAutoDimension(0, 0, "┕", mMainLayoput, 18, ALIGNMENT_CENTER, Color3B::ORANGE);
//        p->setRotation(mDegrees);
//        TOUCHED_INFO info;
//        info.side = TOUCHED_SIDE_RIGHT;
//        Size size = getGridSize(info);
//        p->setPosition(Vec2(mRightVec[n].x - size.width / 4, mRightVec[n].y + size.height * 0.5));
//        
//    }
//
//    for(int n=0; n< mLeftVec.size(); n++){
//        auto p = gui::inst()->addLabelAutoDimension(0, 0, "T", mMainLayoput, 18, ALIGNMENT_CENTER, Color3B::ORANGE);
//        p->setRotation(-1 * mDegrees);
//        TOUCHED_INFO info;
//        info.side = TOUCHED_SIDE_LEFT;
//        Size size = getGridSize(info);
//        p->setPosition(Vec2(mLeftVec[n].x - size.width / 4, mLeftVec[n].y + size.height * 0.5));
//
//    }
    
    if(mDebugModeWall){
        for(int n=0; n< mLeftVec.size(); n++){
            auto posLabel = Label::create();
            posLabel->setSystemFontSize(8);
            posLabel->setString(to_string(n));
            posLabel->setPosition(mLeftVec[n]);
            mMainLayoput->addChild(posLabel);
        }
        for(int n=0; n< mRightVec.size(); n++){
            auto posLabel2 = Label::create();
            posLabel2->setSystemFontSize(8);
            posLabel2->setString(to_string(n));
            posLabel2->setPosition(mRightVec[n]);
            mMainLayoput->addChild(posLabel2);
            
        }
    }
}


void ui_deco::createBottom(bool isDraw, int div, POSITION_VECTOR &vec, Color4F color1, Color4F color2) {
    const float fH = mMainLayoput->getContentSize().height / (float)div;
    Rect dimension = Rect(0, -1 * mH, mMainLayoput->getContentSize().width, mMainLayoput->getContentSize().height);
    gui::inst()->addTiles(mMainLayoput, dimension, vec, Vec2(mCenter.x, mCenter.y - fH), fH, mDegrees, isDraw, color1, color2);
}

void ui_deco::createWall( bool isDraw
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
}

Color4F ui_deco::getDarkColor(Color4F color){
    Color4F c;
    c.r = DARK_RATION * color.r;
    c.g = DARK_RATION * color.g;
    c.b = DARK_RATION * color.b;
    c.a = color.a;
    return c;
}

void ui_deco::addObject(Sprite * sprite, POSITION_VECTOR &posVec, vector<Sprite*> &vec, int idx){
    sprite->setAnchorPoint(Vec2(1,0));
    sprite->setPosition(posVec[idx]);
    mMainLayoput->addChild(sprite, idx);
    vec.push_back(sprite);
}

void ui_deco::touchBegan(Vec2 pos){
    mTouchStart = pos;
    Vec2 nodePosition = mMainLayoput->convertToNodeSpace(mTouchStart);
    
    mTouchPointLabel = Label::create();
    mTouchPointLabel->setString("x");
    
    bool isTouched = false;
    //bottom
    for(int n=0; n < mBottomSpriteVec.size(); n++){
        if(mBottomSpriteVec[n]->getBoundingBox().containsPoint(nodePosition)){
            mTouchedInfo.side = TOUCHED_SIDE_BOTTOM;
            mTouchedInfo.idx = n;
            isTouched= true;
            break;
        }
    }
    
    //left
    if(!isTouched){
        for(int n=0; n < mLeftSpriteVec.size(); n++){
            if(mLeftSpriteVec[n]->getBoundingBox().containsPoint(nodePosition)){
                mTouchedInfo.side = TOUCHED_SIDE_LEFT;
                mTouchedInfo.idx = n;
                isTouched= true;
                break;
            }
        }
    }
    
    //right
    if(!isTouched){
        for(int n=0; n < mRightSpriteVec.size(); n++){
            if(mRightSpriteVec[n]->getBoundingBox().containsPoint(nodePosition)){
                mTouchedInfo.side = TOUCHED_SIDE_RIGHT;
                mTouchedInfo.idx = n;
                isTouched= true;
                break;
            }
        }
    }
    if(isTouched){
        mTouchPointLabel->setPosition(getSpriteVec(mTouchedInfo)->at(mTouchedInfo.idx)->getPosition());
        mMainLayoput->addChild(mTouchPointLabel);
        mTouchedInfo.firstTouchTime = getNow();
    }
    
}
void ui_deco::touchEnded(Vec2 pos){
    Sprite * p = getSprite(mTouchedInfo);
    if(p == NULL)
        return;
    
    mMainLayoput->removeChild(mTouchPointLabel);
    
    //flip
    if(mTouchedInfoLast.side == mTouchedInfo.side && mTouchedInfoLast.idx == mTouchedInfo.idx && (getNow() - mTouchedInfoLast.firstTouchTime) < 1){
        bool currentFlipped = p->isFlippedX();
        p->setFlippedX(currentFlipped ? false : true);
    }else{
        POSITION_VECTOR * vec = getPosVec(mTouchedInfo);
        if(vec == NULL)
            return;
        
        for(int n=0; n< vec->size(); n++){
            Rect rect = Rect(vec->at(n), getGridSize(mTouchedInfo));
            
            if(rect.containsPoint(p->getPosition())){
                p->setPosition(vec->at(n));
                p->setLocalZOrder(n);
                break;
            }
        }
    }
    mTouchedInfoLast.copy(&mTouchedInfo);
    mTouchedInfo.side = TOUCHED_SIDE_MAX;
}
void ui_deco::touchMoved(Vec2 pos){
    if(mTouchStart.x == pos.x && mTouchStart.y == pos.y)
        return;
    
    Sprite * p = getSprite(mTouchedInfo);
    if(p == NULL)
        return;
    
    Vec2 move = Vec2(mTouchStart.x - pos.x, mTouchStart.y - pos.y);
    
    Vec2 current = p->getPosition();
    Vec2 movedPoint = Vec2(current.x - move.x, current.y - move.y);
    
    p->setPosition(movedPoint);
    
    POSITION_VECTOR * vec = getPosVec(mTouchedInfo);
    if(vec == NULL)
        return;
    
    bool isFind=false;
    for(int n=0; n< vec->size(); n++){
        Rect rect = Rect(vec->at(n), getGridSize(mTouchedInfo));
        
        if(rect.containsPoint(p->getPosition())){
            mTouchPointLabel->setPosition(vec->at(n));
            
            //p->setPosition(vec->at(n));
            //p->setLocalZOrder(n);
            isFind = true;
            break;
        }
    }

    if(!isFind){
        p->setPosition(current);
    }
    
    mTouchStart = pos;
}
