//
// Created by Jung, DaeCheon on 31/07/2018.
//

#include "ui_cultivation.h"

//void ui_cultivation::addLevel(const string img, float maxLevel, const ccMenuCallback& callback, const string text) {
void ui_cultivation::addLevel(const string img, float maxLevel, const string text) {
    imgLevel p;
    p.img = img;
    p.maxLevel = maxLevel;
    p.text = text;
    //p.callback = callback;
    mVec.push_back(p);
};

Layout * ui_cultivation::init(int id
        , cultivationCB cb
        , float fontSize
        , const string decayImg
        , const string progressBarImg
        , Size size
        , Node * p
        , Vec2 position
        , bool isAutoDimension
        , Size dimension
        , Size grid
        , Size origin
        , Size margin) {

    if(mVec.size() <= 0)
        return NULL;

    mDecayImg = decayImg;
    mFontSize = fontSize;
    mId = id;

    mCurrentIdx = 0;
    mCurrentPercent = 0;

    auto * l = gui::inst()->createLayout(size);
    Vec2 point;
    gui::inst()->getPoint(position.x, position.y, point, ALIGNMENT_NONE, isAutoDimension ? p->getContentSize() : dimension, grid, origin, margin);
    l->setPosition(point);

    mSize = size;

    //mImg = gui::inst()->addTextButton(0, 1, mVec[mCurrentIdx].text, l, mVec[mCurrentIdx].callback, mFontSize,  ALIGNMENT_CENTER, Color3B::BLACK, l->getContentSize(), Size(1,3), Size::ZERO, Size::ZERO, mVec[mCurrentIdx].img);
    mImg = gui::inst()->addSprite(0, 0, mVec[mCurrentIdx].img, l, ALIGNMENT_CENTER, l->getContentSize(), Size(1,1), Size::ZERO, Size::ZERO);

    auto listener = EventListenerTouchOneByOne::create();
    //listener1->onTouchBegan;
    listener->onTouchBegan = [this, l, cb](Touch *touch,Event*event)->bool {
        if (l->getBoundingBox().containsPoint(touch->getLocation())){
            CCLOG("containsPoint x %f, y: %f"
            , touch->getLocation().x
            , touch->getLocation().y
            );
            cb(mId);
        }/*
        else{
            CCLOG("x %f, y: %f - %f, %f, %f, %f"
            , touch->getLocation().x
            , touch->getLocation().y
            , l->getBoundingBox().getMinX()
            , l->getBoundingBox().getMinY()
            , l->getBoundingBox().getMaxX()
            , l->getBoundingBox().getMaxY()
            );
        }
        */
        return true;
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, l);

    mProgressBar = gui::inst()->addProgressBar(0, 0, progressBarImg, l, mCurrentPercent, l->getContentSize(), Size(1, 1), Size::ZERO, Size::ZERO);

    mLabel = gui::inst()->addLabelAutoDimension(0, 0, mVec[mCurrentIdx].text, l, mFontSize, ALIGNMENT_CENTER, Color3B::GRAY, Size(1, 1), Size::ZERO, Size::ZERO);

    p->addChild(l);

    return l;
}

void ui_cultivation::update(float percent){
    if(mIsDecay)
        return;

    mCurrentPercent = percent;
    mProgressBar->setPercent(percent);
    for(int n=0; n < mVec.size(); n++){
        if(mVec[n].maxLevel >= percent){
            if(mCurrentIdx != n){
                mImg->setTexture(mVec[n].img);
                mLabel->setString(mVec[n].text);
                mCurrentIdx = n;
            }
            return;
        }
    }
}

void ui_cultivation::setDecay(bool b){
    mIsDecay = b;
    if(mIsDecay){
        mProgressBar->setPercent(0);
        mImg->setTexture(mDecayImg);
        mLabel->setString("");
    }
};
