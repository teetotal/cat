//
// Created by Jung, DaeCheon on 16/10/2018.
//

#include "ui_deco.h"
#include "../library/util.h"


ui_deco * ui_deco::hInst = NULL;

void ui_deco::init(Node * p, float degrees, bool isDebugModeBottom, bool isDebugModeWall){
    mWallColor1 = Color4F::WHITE;
    mWallColor2 = Color4F::WHITE;
    mBottomColor1 = Color4F::WHITE;
    mBottomColor2 = Color4F::WHITE;
    
    mDebugModeBottom = isDebugModeBottom;
    mDebugModeWall = isDebugModeWall;
    
    mParentLayoput = p;
    mMainLayoput = gui::inst()->createLayout(p->getContentSize());
    
    mParentLayoput->addChild(mMainLayoput);
    
    mLayout[LAYER_WALL] = gui::inst()->createLayout(mMainLayoput->getContentSize());
    mMainLayoput->addChild(mLayout[LAYER_WALL]);
    mLayout[LAYER_WALL_TEMP] = gui::inst()->createLayout(mMainLayoput->getContentSize());
    mMainLayoput->addChild(mLayout[LAYER_WALL_TEMP]);
    mLayout[LAYER_BOTTOM] = gui::inst()->createLayout(mMainLayoput->getContentSize());
    mMainLayoput->addChild(mLayout[LAYER_BOTTOM]);
    mLayout[LAYER_BOTTOM_TEMP] = gui::inst()->createLayout(mMainLayoput->getContentSize());
    mMainLayoput->addChild(mLayout[LAYER_BOTTOM_TEMP]);
    mLayout[LAYER_GUIDELINE] = gui::inst()->createLayout(mMainLayoput->getContentSize());
    mMainLayoput->addChild(mLayout[LAYER_GUIDELINE]);
    mLayout[LAYER_OBJECT] = gui::inst()->createLayout(mMainLayoput->getContentSize());
    mMainLayoput->addChild(mLayout[LAYER_OBJECT]);
    
   
    mDegrees = degrees;
    mH = mMainLayoput->getContentSize().height * 0.5;
    mCenter = Vec2(mMainLayoput->getContentSize().width / 2, mMainLayoput->getContentSize().height / 2);
    
    mWallPostions.top = Vec2(mCenter.x, mMainLayoput->getContentSize().height);
    mWallPostions.bottom = Vec2(mCenter.x, mH);
    mWallPostions.left = Vec2(0, mH);
    mWallPostions.leftBottom = Vec2(0, 0);
    mWallPostions.right = Vec2(mMainLayoput->getContentSize().width, mH);
    mWallPostions.rightBottom = Vec2(mMainLayoput->getContentSize().width, 0);
    
    mTouchedInfo.side = SIDE_MAX;
}

//바닥 좌표, 타일
void ui_deco::addBottom(int posDiv, int drawDiv, Color4F color1, Color4F color2){
    mBottomDivCnt = posDiv;
    mDrawBottomDivCnt = drawDiv;
    mBottomColor1 = color1;
    mBottomColor2 = color2;
    
    createBottom(false, mBottomDivCnt, mBottomVec);
    std::sort (mBottomVec.begin(), mBottomVec.end(), ui_deco::sortTouchVec);

    drawBottom(mDrawBottomDivCnt, mBottomColor1, mBottomColor2);
    mBottomGridSize = getBottomGridSize();

    if(mDebugModeBottom){
        for(int n=0; n< mBottomVec.size(); n++){
            auto posLabel = Label::create();
            posLabel->setSystemFontSize(8);
            posLabel->setString(to_string(n));
            posLabel->setPosition(mBottomVec[n]);
            mLayout[LAYER_BOTTOM]->addChild(posLabel);
            
        }
    }
}

void ui_deco::changeColorBottom(Color4F color1, Color4F color2){
    mLayout[LAYER_BOTTOM]->removeAllChildren();
    mBottomVec.clear();
    addBottom(mBottomDivCnt, mDrawBottomDivCnt, color1, color2);
}

void ui_deco::changeColorWall(Color4F color1, Color4F color2){
    mLayout[LAYER_WALL]->removeAllChildren();
    mRightVec.clear();
    mLeftVec.clear();
    addWall(mWallDivCnt, color1, color2);
}
//벽
void ui_deco::addWall(int div, Color4F color1, Color4F color2){
    mWallDivCnt = div;
    mWallColor1 = color1;
    mWallColor2 = color2;
    
    createWall(true, mWallDivCnt, &mLeftVec, &mRightVec, mWallColor1, mWallColor2);
    std::sort (mRightVec.begin(), mRightVec.end(), ui_deco::sortTouchVec);
    std::sort (mLeftVec.begin(), mLeftVec.end(), ui_deco::sortTouchVecLeft);
    
    mWallGridSize = getWallGridSize();

    for(int n=0; n< mRightVec.size(); n++){
        auto p = gui::inst()->addLabelAutoDimension(0, 0, "┕", mLayout[LAYER_WALL], 18, ALIGNMENT_CENTER, Color3B::GRAY);
        //p->setRotationSkewX(45);
        p->setRotationSkewY(mDegrees);
        TOUCHED_INFO info;
        info.side = SIDE_RIGHT;
        Size size = getGridSize(info);
        p->setPosition(Vec2(mRightVec[n].x - size.width / 4, mRightVec[n].y + size.height * 0.5));
        
    }

    for(int n=0; n< mLeftVec.size(); n++){
        auto p = gui::inst()->addLabelAutoDimension(0, 0, "┕", mLayout[LAYER_WALL], 18, ALIGNMENT_CENTER, Color3B::GRAY);
        p->setRotationSkewY(-1 * mDegrees);
        TOUCHED_INFO info;
        info.side = SIDE_LEFT;
        Size size = getGridSize(info);
        p->setPosition(Vec2(mLeftVec[n].x - size.width / 4, mLeftVec[n].y + size.height * 0.5));

    }
    
    if(mDebugModeWall){
        for(int n=0; n< mLeftVec.size(); n++){
            auto posLabel = Label::create();
            posLabel->setSystemFontSize(8);
            posLabel->setString(to_string(n));
            posLabel->setPosition(mLeftVec[n]);
            mLayout[LAYER_WALL]->addChild(posLabel);
        }
        for(int n=0; n< mRightVec.size(); n++){
            auto posLabel2 = Label::create();
            posLabel2->setSystemFontSize(8);
            posLabel2->setString(to_string(n));
            posLabel2->setPosition(mRightVec[n]);
            mLayout[LAYER_WALL]->addChild(posLabel2);
            
        }
    }
}


void ui_deco::createBottom(bool isDraw, int div, POSITION_VECTOR &vec, Color4F color1, Color4F color2) {
    const float fH = mMainLayoput->getContentSize().height / (float)div;
    Rect dimension = Rect(0, -1 * mH, mMainLayoput->getContentSize().width, mMainLayoput->getContentSize().height);
    gui::inst()->addTiles(mLayout[LAYER_BOTTOM], dimension, vec, Vec2(mCenter.x, mCenter.y - fH), fH, mDegrees, isDraw, color1, color2);
}

void ui_deco::createWall( bool isDraw
                       , int div
                       , POSITION_VECTOR * vecLeft
                       , POSITION_VECTOR * vecRight
                       , Color4F color1
                       , Color4F color2
                       ){
    
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
                              , mLayout[LAYER_WALL]
                              , Rect(Vec2(mWallPostions.left, mWallPostions.leftBottom), Size(mCenter.x - mWallPostions.left.x, mWallPostions.top.y))
                              , vecLeft
                              , Vec2(mCenter.x, mMainLayoput->getContentSize().height - (n * hW))
                              , hW, lenW
                              , isDraw
                              , color1
                              , color1
                              );
        
        gui::inst()->addWalls(false
                              , mLayout[LAYER_WALL]
                              , Rect(Vec2(mCenter.x, 0), Size(mWallPostions.right.x - mCenter.x, mWallPostions.top.y))
                              , vecRight
                              , Vec2(mCenter.x, mMainLayoput->getContentSize().height - (n * hW))
                              , hW, lenW
                              , isDraw
                              , color2
                              , color2
                              );
    }
}

void ui_deco::drawGuidLine(){
    //guide line
    const float margin = 15;
    float xLen = gui::inst()->getTanLen(mH, mDegrees);
    Vec2 left = Vec2(mCenter.x - xLen, 0);
    Vec2 right = Vec2(mCenter.x + xLen, 0);
    Vec2 left2 = Vec2(mCenter.x - xLen, mH);
    Vec2 right2 = Vec2(mCenter.x + xLen, mH);
    
    Color4F color = Color4F::GRAY;
    auto draw = DrawNode::create();
    draw->setLineWidth(3);
//    draw->drawLine(Vec2(mCenter.x, mMainLayoput->getContentSize().height), Vec2(mCenter.x, mH), color);
    draw->drawLine(Vec2(mCenter.x, mH), left, color);
    draw->drawLine(Vec2(mCenter.x, mH), right, color);
    
    draw->drawLine(Vec2(mCenter.x, mMainLayoput->getContentSize().height), left2, color);
    draw->drawLine(Vec2(mCenter.x, mMainLayoput->getContentSize().height), right2, color);
    
    draw->drawLine(Vec2(0, -1 * margin), Vec2(0, mH), color);
    draw->drawLine(Vec2(right.x, -1 * margin), Vec2(right.x, mH), color);
    
    mLayout[LAYER_GUIDELINE]->addChild(draw);
    
    
    gui::inst()->drawParallelogram(mLayout[LAYER_GUIDELINE]
                                   , mWallPostions.rightBottom
                                   , Vec2(mCenter.x, -1 * mH)
                                   , Vec2(mWallPostions.rightBottom.x, mWallPostions.rightBottom.y - margin)
                                   , Vec2(mCenter.x, -1 * mH - margin)
                                   , Color4F::GRAY);
    
    gui::inst()->drawParallelogram(mLayout[LAYER_GUIDELINE]
                                   , mWallPostions.leftBottom
                                   , Vec2(mCenter.x, -1 * mH)
                                   , Vec2(mWallPostions.leftBottom.x, mWallPostions.leftBottom.y - margin)
                                   , Vec2(mCenter.x, -1 * mH - margin)
                                   , Color4F::GRAY);
    
    /*
     gui::inst()->drawTriangle(mMainLayoput
     , Vec2(dimension.getMidX(), dimension.getMinY())
     , Vec2(dimension.getMaxX(), dimension.getMidY())
     , Vec2(dimension.getMaxX(), dimension.getMinY())
     , Color4F::RED);
     
     
     double _len_ = gui::inst()->getTanLen(h, degrees);
     
     gui::inst()->drawTriangle(mMainLayoput
     , Vec2(dimension.getMidX(), h)
     , Vec2(dimension.getMidX(), 0)
     , Vec2(dimension.getMidX() + _len_, 0)
     , Color4F::RED);
     */
}

Color4F ui_deco::getDarkColor(Color4F color){
    Color4F c;
    c.r = DARK_RATION * color.r;
    c.g = DARK_RATION * color.g;
    c.b = DARK_RATION * color.b;
    c.a = color.a;
    return c;
}

void ui_deco::addObject(OBJECT &obj, POSITION_VECTOR &posVec, vector<OBJECT> &vec){
    obj.sprite->setAnchorPoint(Vec2(1,0));
    obj.sprite->setPosition(posVec[obj.idx]);
    mLayout[LAYER_OBJECT]->addChild(obj.sprite, obj.idx);
    vec.push_back(obj);
}

void ui_deco::touchBegan(Vec2 pos){
    mTouchStart = pos;
    Vec2 nodePosition = mMainLayoput->convertToNodeSpace(mTouchStart);
    
    mTouchPointLabel = Label::create();
    mTouchPointLabel->setString("x");
    
    bool isTouched = false;
    //bottom
    for(int n=0; n < mBottomSpriteVec.size(); n++){
        if(mBottomSpriteVec[n].sprite->getBoundingBox().containsPoint(nodePosition)){
            mTouchedInfo.side = SIDE_BOTTOM;
            mTouchedInfo.idx = n;
            isTouched= true;
            break;
        }
    }
    
    //left
    if(!isTouched){
        for(int n=0; n < mLeftSpriteVec.size(); n++){
            if(mLeftSpriteVec[n].sprite->getBoundingBox().containsPoint(nodePosition)){
                mTouchedInfo.side = SIDE_LEFT;
                mTouchedInfo.idx = n;
                isTouched= true;
                break;
            }
        }
    }
    
    //right
    if(!isTouched){
        for(int n=0; n < mRightSpriteVec.size(); n++){
            if(mRightSpriteVec[n].sprite->getBoundingBox().containsPoint(nodePosition)){
                mTouchedInfo.side = SIDE_RIGHT;
                mTouchedInfo.idx = n;
                isTouched= true;
                break;
            }
        }
    }
    if(isTouched){
        mTouchPointLabel->setPosition(getSpriteVec(mTouchedInfo)->at(mTouchedInfo.idx).sprite->getPosition());
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
        if(p->isFlippedX()){
            p->setAnchorPoint(Vec2(0, 0));
        }else {
            p->setAnchorPoint(Vec2(1, 0));
        }
    }else{
        POSITION_VECTOR * vec = getPosVec(mTouchedInfo);
        if(vec == NULL)
            return;
        
        for(int n=0; n< vec->size(); n++){
            Rect rect = Rect(vec->at(n), getGridSize(mTouchedInfo));
            
            if(rect.containsPoint(p->getPosition())){
                p->setPosition(vec->at(n));
                p->setLocalZOrder(n);
                getSpriteVec(mTouchedInfo)->at(mTouchedInfo.idx).idx = n;
                break;
            }
        }
    }
    mTouchedInfoLast.copy(&mTouchedInfo);
    mTouchedInfo.side = SIDE_MAX;
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

string ui_deco::getJson(vector<OBJECT> *vec){
    rapidjson::Document d;
    d.SetArray();
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
    for (int n = 0; n < (int)vec->size(); n++) {
        OBJECT obj = vec->at(n);
        
        rapidjson::Value objValue;
        objValue.SetObject();
        objValue.AddMember("id", obj.id, allocator);
        objValue.AddMember("idx", obj.idx, allocator);
        
        d.PushBack(objValue, allocator);
    }

    rapidjson::StringBuffer bufferJson;
    bufferJson.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer(bufferJson);
    d.Accept(writer);
    
    string sz = bufferJson.GetString();
    
    return sz;
}

void ui_deco::addColor4FJson(rapidjson::Document &d, Color4F color){
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
    rapidjson::Value objValue;
    objValue.SetObject();
    objValue.AddMember("r", color.r, allocator);
    objValue.AddMember("g", color.g, allocator);
    objValue.AddMember("b", color.b, allocator);
    objValue.AddMember("a", color.a, allocator);
    
    d.PushBack(objValue, allocator);
}

string ui_deco::getColorJson(){
    rapidjson::Document d;
    d.SetArray();
    addColor4FJson(d, mWallColor1);
    addColor4FJson(d, mWallColor2);
    addColor4FJson(d, mBottomColor1);
    addColor4FJson(d, mBottomColor2);
    
    rapidjson::StringBuffer bufferJson;
    bufferJson.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer(bufferJson);
    d.Accept(writer);
    
    string sz = bufferJson.GetString();
    
    return sz;
}
