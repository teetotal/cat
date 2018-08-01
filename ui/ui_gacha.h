//
// Created by Jung, DaeCheon on 31/07/2018.
//

#ifndef PROJ_ANDROID_UI_GACHA_H
#define PROJ_ANDROID_UI_GACHA_H

#include "ui.h"

class ui_gacha : public Scene{
public:
    static Scene* createScene();

    virtual bool init(){
        initDetails();
        return true;
    };

    CREATE_FUNC(ui_gacha);


    virtual ~ui_gacha(){
        CCLOG("gacha released");
    };

    void initDetails(bool isAutoRelease = true
            , float moveLength = 5
            , float rotateAmplitude = 10
            , float readyEffectInterval = 0.05
            , unsigned int shakeRepeat = 4
            , float fadeoutTime = 1
            , float fadeinTime = 1
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

    void test();
    void cb(float f);

private:
    void callback(bool isRelease);
    void callbackFinish(float f);

    LayerColor * mParitclePopupLayer, * mParitclePopup;
    Node * mParent;
    string mParticleFinish;
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
