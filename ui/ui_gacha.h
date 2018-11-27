//
// Created by Jung, DaeCheon on 31/07/2018.
//

#ifndef PROJ_ANDROID_UI_GACHA_H
#define PROJ_ANDROID_UI_GACHA_H

#include "ui.h"

class ui_gacha{
public:
    ui_gacha() {
        initDetails();
    };
    virtual ~ui_gacha(){
    };

    void initDetails(bool isAutoRelease = true
            , float moveLength = 5
            , float rotateAmplitude = 10
            , float readyEffectInterval = 0.05
            , unsigned int shakeRepeat = 4
            , float fadeoutTime = 1
            , float fadeinTime = 3
    );

    void run(Node * parent, const string img, LayerColor * contentsLayer, const string particleReady, const string particle);
private:
   
    Vec2 mPoint;

    float mMoveLength
    , mRotateAmplitude
    , mReadyEffectInterval
    , mFadeoutTime
    , mFadeinTime;

    bool mIsAutoRelease;

    unsigned int mShakeRepeat;
};


#endif //PROJ_ANDROID_UI_GACHA_H
