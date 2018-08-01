//
// Created by Jung, DaeCheon on 31/07/2018.
//

#include "ui_gacha.h"
void ui_gacha::test(){
    CCLOG("!!!!!CB TEST !!!!!");
    this->scheduleOnce(schedule_selector(ui_gacha::cb), 10);
}
void ui_gacha::cb(float f){
    CCLOG("!!!!!CB !!!!!");
}
void ui_gacha::initDetails(bool isAutoRelease
        , float moveLength
        , float rotateAmplitude
        , float readyEffectInterval
        , unsigned int shakeRepeat
        , float fadeoutTime
        , float fadeinTime
) {
    mIsAutoRelease = isAutoRelease;
    mMoveLength = moveLength;
    mRotateAmplitude = rotateAmplitude;
    mReadyEffectInterval  = readyEffectInterval;
    mShakeRepeat = shakeRepeat;
    mFadeoutTime = fadeoutTime;
    mFadeinTime = fadeinTime;
}

LayerColor * ui_gacha::createLayer(LayerColor * &pBG
        , Node * pParent
        , const string imgReady
        , const string particleReady
        , const string particleFinish
        , Size bgSize
        , const string imgBG
        , Color4B bgColor
) {

    mParent = pParent;
    mParticleFinish = particleFinish;

    mParitclePopup = gui::inst()->addPopup(mParitclePopupLayer, pParent, bgSize, imgBG, bgColor);
    pBG = mParitclePopupLayer;
    auto particle = ParticleSystemQuad::create(particleReady);
    //particle->setDuration(3);
    //particle->setStartRadius(10);
    //particle->setEndRadius(0);

    gui::inst()->getPoint(0,0, mPoint, ALIGNMENT_CENTER, bgSize, Size(1,1), Size::ZERO, Size::ZERO);
    particle->setPosition(mPoint);
    //particle->setAutoRemoveOnFinish(true);
    //particle->setonEnterTransitionDidFinishCallback(CC_CALLBACK_0(MainScene::callback0, this));
    //particle->setOnExitCallback(CC_CALLBACK_0(MainScene::callback0, this));
    /*
    particle->setOnExitCallback([this, popupBg](){
        CCLOG("setOnExitCallback");
        //mParitclePopupLayer->removeFromParent();
        this->removeChild(popupBg);
    });
     */

    auto img = Sprite::create(imgReady);
    img->setPosition(mPoint);

    //img->setTag(1);
    float t = mReadyEffectInterval;

    float length = mMoveLength;
    float amplitude = mRotateAmplitude;
    auto shakeAnimation = Sequence::create(
            MoveBy::create(t, Vec2(length / 2, 0)),

            RotateBy::create(t, amplitude / 2),
            RotateBy::create(t, amplitude * -1),

            MoveBy::create(t, Vec2(length * -1, 0)),

            RotateBy::create(t, amplitude),
            RotateBy::create(t, amplitude * -1),

            MoveBy::create(t, Vec2(length, 0)),

            RotateBy::create(t, amplitude),
            RotateBy::create(t, amplitude * -1),

            MoveBy::create(t, Vec2(length * -1, 0)),

            RotateBy::create(t, amplitude),
            RotateBy::create(t, amplitude * -1),

            MoveBy::create(t, Vec2(length / 2, 0)),
            RotateBy::create(t, amplitude / 2),

            ScaleBy::create(t, 1.5),
            ScaleTo::create(t, 1),
            nullptr
    );

    auto seq = Sequence::create(Repeat::create(shakeAnimation, mShakeRepeat)
            , FadeOut::create(mFadeoutTime)
            , nullptr);
    img->runAction(seq);

    mParitclePopup->addChild(img);
    mParitclePopup->addChild(particle);

    //auto delay = cocos2d::DelayTime::create(3);
    //this->runAction(Sequence::create(delay, nullptr));
    //CCLOG("finish delay");
    //this->removeChild(mParitclePopupLayer);

    particle->setAutoRemoveOnFinish(true);
    //this->scheduleOnce(schedule_selector(ui_gacha::callback), mReadyTime);
    particle->setOnExitCallback([this](){
        callback(false);
    });

    //this->removeChild(mParitclePopupLayer);

    return mParitclePopup;
}

void ui_gacha::callback(bool isRelease) {

    auto particle = ParticleSystemQuad::create(mParticleFinish);
    particle->setPosition(mPoint);
    particle->setAutoRemoveOnFinish(true);
    //particle->setScale(0.1);

    /*
    particle->setOnExitCallback([this, particle](){
        callback(true);
    });
     */

    auto sprite = Sprite::create("gem.jpg");
    sprite->setPosition(mPoint);
    sprite->setOpacity(0);
    sprite->runAction(FadeIn::create(mFadeinTime));
    //mParitclePopup->removeChildByTag(1);

    mParitclePopup->addChild(sprite);
    mParitclePopup->addChild(particle);

    //this->scheduleOnce(schedule_selector(ui_gacha::callbackFinish), 3);
}

void ui_gacha::callbackFinish(float f){
    //mParent->removeChild(mParitclePopupLayer);
};