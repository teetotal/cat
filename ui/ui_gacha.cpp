//
// Created by Jung, DaeCheon on 31/07/2018.
//

#include "ui_gacha.h"

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
    mParticleReady = particleReady;
    mParticleFinish = particleFinish;

    mParitclePopup = gui::inst()->createModalLayer(mParitclePopupLayer, bgSize, imgBG, bgColor);
    pBG = mParitclePopupLayer;

    gui::inst()->getPoint(0,0, mPoint, ALIGNMENT_CENTER, bgSize, Size(1,1), Size::ZERO, Size::ZERO);

    mReadyImg = new (std::nothrow) Sprite();
    mReadyImg->initWithFile(imgReady);
    mReadyImg->setPosition(mPoint);

    return mParitclePopup;
}

void ui_gacha::run(const string img, LayerColor * contentsLayer){

    mParitclePopup->removeAllChildren();
    mReadyImg->setRotation(0);
    mReadyImg->setScale(1);

    auto particle = ParticleSystemQuad::create(mParticleReady);
    particle->setPosition(mPoint);

    mParitclePopup->addChild(particle);

    particle->setAutoRemoveOnFinish(true);
    particle->setOnExitCallback([this, img, contentsLayer](){
        callback(img, contentsLayer);
    });
    mParitclePopup->addChild(mReadyImg);

    mReadyImg->setOpacity(255);
    mReadyImg->runAction(getSequence());

    mParent->addChild(mParitclePopupLayer);

    contentsLayer->setVisible(false);
    contentsLayer->setPosition(Vec2(mPoint.x - contentsLayer->getContentSize().width / 2, mPoint.y - contentsLayer->getContentSize().height / 2));
    mParitclePopup->addChild(contentsLayer);
}

void ui_gacha::callback(const string img, LayerColor * contentsLayer) {

    auto particle = ParticleSystemQuad::create(mParticleFinish);
    particle->setPosition(mPoint);
    particle->setAutoRemoveOnFinish(true);
    particle->setOnExitCallback([this, contentsLayer](){
        contentsLayer->setVisible(true);
    });

    auto sprite = Sprite::create(img);
    sprite->setPosition(mPoint);
    sprite->setOpacity(0);

    auto seq = Sequence::create(
            FadeIn::create(mFadeinTime)
            , Hide::create()
            , nullptr);

    sprite->runAction(seq);


    mParitclePopup->addChild(sprite);
    mParitclePopup->addChild(particle);
}

Sequence * ui_gacha::getSequence(){

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

    return Sequence::create(Repeat::create(shakeAnimation, mShakeRepeat)
            , FadeOut::create(mFadeoutTime)
            , nullptr);
}