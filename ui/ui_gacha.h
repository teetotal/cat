//
// Created by Jung, DaeCheon on 31/07/2018.
//

#ifndef PROJ_ANDROID_UI_GACHA_H
#define PROJ_ANDROID_UI_GACHA_H

#include "ui.h"

class ui_gacha{
public:
    ui_gacha() : mParitclePopupLayer(NULL), mParitclePopup(NULL), mReadyImg(NULL) {
        initDetails();
    };
    virtual ~ui_gacha(){
        CCLOG("gacha released");
        if(mParitclePopupLayer)
            delete mParitclePopupLayer;
        if(mParitclePopup)
            delete mParitclePopup;
        if(mReadyImg)
            delete mReadyImg;
    };

    void initDetails(bool isAutoRelease = true
            , float moveLength = 5
            , float rotateAmplitude = 10
            , float readyEffectInterval = 0.05
            , unsigned int shakeRepeat = 4
            , float fadeoutTime = 1
            , float fadeinTime = 3
    );

    LayerColor * createLayer(LayerColor * &pBG
            , Node * pParent
            , const string imgReady
            , const string particleReady
            , const string particleFinish
            , Size bgSize = Size(300, 225)
            , const string imgBG = ""
            , Color4B bgColor = Color4B::BLACK
    );

    void run(const string img, LayerColor * contentsLayer);
private:
    void callback(const string img, LayerColor * contentsLayer);
    Sequence * getSequence();

    LayerColor * mParitclePopupLayer, * mParitclePopup; //memory
    Node * mParent;
    //Sequence * mShakeSeq;
    Sprite * mReadyImg; //memory
    string mParticleFinish;
    string mParticleReady;
    Vec2 mPoint;

    gui mGUI;
    float mMoveLength
    , mRotateAmplitude
    , mReadyEffectInterval
    , mFadeoutTime
    , mFadeinTime;

    bool mIsAutoRelease;

    unsigned int mShakeRepeat;
};


#endif //PROJ_ANDROID_UI_GACHA_H
