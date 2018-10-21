//
// Created by Jung, DaeChun on 21/10/2018.
//

#include "DecoScene.h"

Scene* DecoScene::createScene()
{
    return DecoScene::create();
}

bool DecoScene::init() {
    //touch
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(DecoScene::onTouchesBegan, this);
    listener->onTouchesEnded = CC_CALLBACK_2(DecoScene::onTouchesEnded, this);
    listener->onTouchesMoved = CC_CALLBACK_2(DecoScene::onTouchesMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    const float degrees = 27.f;
    const float layerWidth = gui::inst()->getTanLen(visibleSize.height / 2, degrees) * 2;
    mMainLayoput = gui::inst()->createLayout(Size(layerWidth, visibleSize.height));

    mDeco.clone(mMainLayoput, ui_deco::inst());
    
    Vec2 posMainLayer = gui::inst()->getCenter();
    posMainLayer.y  += mMainLayoput->getContentSize().height /3;
    mMainLayoput->setPosition(posMainLayer);
    mMainLayoput->setAnchorPoint(Vec2(0.5, 0.5));
    this->addChild(mMainLayoput);
    
    gui::inst()->addTextButton(0, 0, "CLOSE"
                                            , this
                                            , CC_CALLBACK_1(DecoScene::closeCallback, this)
                                            , 0
                                            , ALIGNMENT_CENTER
                                            , Color3B::BLUE
                                            );
    
    return true;
}

void DecoScene::closeCallback(Ref * pSender) {
    Director::getInstance()->popScene();
}

void DecoScene::onTouchesBegan(const std::vector<Touch*>& touches, Event *event)
{
    Touch* touch;
    Vec2 touchPoint;
    for (int index = 0; index < touches.size(); index++)
    {
        touch = touches[index];
        touchPoint = touch->getLocation();
        int touchIndex = touch->getID();
        if(mTouchVec.size() < touchIndex + 1){
            mTouchVec.resize(touchIndex + 1);
        }
        mTouchVec[touchIndex] = touch;
    }
    
    if(getValidTouchCnt() == 1 && mTouchVec[0]){
        
        mTouchStart = mTouchVec[0]->getLocation();
        ui_deco::inst()->touchBegan(mTouchVec[0]->getLocation());
    }
    
    mTouchGap = -1;
    return;
}

void DecoScene::onTouchesEnded(const std::vector<Touch*>& touches, Event *event)
{
    if(ui_deco::inst()->mTouchedInfo.side != ui_deco::TOUCHED_SIDE_MAX){
        ui_deco::inst()->touchEnded(mTouchVec[0]->getLocation());
    }

    Touch* touch;
    Vec2 touchPoint;
    for (int index = 0; index < touches.size(); index++)
    {
        touch = touches[index];
        touchPoint = touch->getLocation();
        int touchIndex = touch->getID();

        if(mTouchVec.size() > touchIndex)
            mTouchVec[touchIndex] = NULL;
    }
}
void DecoScene::onTouchesMoved(const std::vector<Touch*>& touches, Event *event)
{
    Touch* touch;
    Vec2 touchPoint;
    for (int index = 0; index < touches.size(); index++)
    {
        touch = touches[index];
        touchPoint = touch->getLocation();
        int touchIndex = touch->getID();
        if(mTouchVec.size() < touchIndex + 1){
            mTouchVec.resize(touchIndex + 1);
        }
        mTouchVec[touchIndex] = touch;
    }

    if(getValidTouchCnt() == 1 && mTouchVec[0]){

        if(mTouchStart.x == mTouchVec[0]->getLocation().x && mTouchStart.y == mTouchVec[0]->getLocation().y)
            return;

        Vec2 move = Vec2(mTouchStart.x - mTouchVec[0]->getLocation().x, mTouchStart.y - mTouchVec[0]->getLocation().y);

        if(ui_deco::inst()->mTouchedInfo.side != ui_deco::TOUCHED_SIDE_MAX){
            ui_deco::inst()->touchMoved(mTouchVec[0]->getLocation());
        }else{
            Vec2 current = mMainLayoput->getPosition();
            Vec2 movedPoint = Vec2(current.x - move.x, current.y - move.y);

            mMainLayoput->setPosition(movedPoint);
        }
        mTouchStart = mTouchVec[0]->getLocation();

    }
    else if(getValidTouchCnt() == 2 && mTouchVec[0] && mTouchVec[1]){
        float gap = abs(mTouchVec[0]->getLocation().y - mTouchVec[1]->getLocation().y);

        if(mTouchGap == -1){
            mTouchGap = gap;
            return;
        } else if(mTouchGap == gap){
            return;
        }
        //mTouchGap : scale = gap :x
        //x = scale * gap / mTouchGap
        float currentScale = mMainLayoput->getScale();
        float ratio = (currentScale * gap / mTouchGap);
        float max = 1.5f;
        float min = 0.7f;
        if(ratio > max)
            ratio = max;
        else if(ratio < min)
            ratio = min;
        else if(ratio < 1.1f && ratio > 0.9f)
            ratio = 1;

        mMainLayoput->setScale(ratio);
    }
}
