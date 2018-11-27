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
    mPoint = gui::inst()->getCenter();
}

void ui_gacha::run(Node * parent, const string img, LayerColor * contentsLayer, const string particleReady, const string particleFinal){

    auto readyImg = Sprite::create(img);
    gui::inst()->setScale(readyImg, 100);
    readyImg->setPosition(mPoint);

    auto particle = ParticleSystemQuad::create(particleReady);
    particle->setPosition(mPoint);
    parent->addChild(particle);
    
    parent->addChild(readyImg);

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
                                           
                                           ScaleBy::create(t * 1.5, 1.5),
                                           ScaleBy::create(t * 1.5, 1/ 1.5),
                                           nullptr
                                           );
    
    auto seq = Sequence::create(Repeat::create(shakeAnimation, mShakeRepeat)
                            , FadeOut::create(mFadeoutTime)
                            , CallFunc::create([=]()
    {
        if(particleFinal.compare("") != 0){
            auto p = ParticleSystemQuad::create(particleFinal);
            p->setAnchorPoint(Vec2(0.5, 0.5));
            p->setPosition(gui::inst()->getCenter());
            parent->addChild(p);
        }
        contentsLayer->setVisible(true);
        
    })
                            , nullptr);
    
    readyImg->runAction(seq);

    contentsLayer->setVisible(false);
    contentsLayer->setPosition(Vec2(mPoint.x - contentsLayer->getContentSize().width / 2, mPoint.y - contentsLayer->getContentSize().height / 2));
    parent->addChild(contentsLayer);
}
