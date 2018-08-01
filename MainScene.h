//
// Created by Jung, DaeCheon on 27/07/2018.
//

#ifndef PROJ_ANDROID_MAINSCENE_H
#define PROJ_ANDROID_MAINSCENE_H

#include "cocos2d.h"
#include "ui/ui.h"
#include "ui/ui_gacha.h"
USING_NS_CC;

enum SCENECODE{
    SCENECODE_MAIN = 0,
    SCENECODE_ACTION,
    SCENECODE_RACE,
    SCENECODE_FARMING,
    SCENECODE_BUY,
    SCENECODE_SELL,
    SCENECODE_INVENTORY,
    SCENECODE_RECHARGE,
    SCENECODE_PURCHASE,
    SCENECODE_COLLECTION,
    SCENECODE_ACHIEVEMENT,
    SCENECODE_NONE,
    SCENECODE_POPUP_1,
    SCENECODE_POPUP_2,
    SCENECODE_POPUP_3,
    SCENECODE_POPUP_4,
    SCENECODE_POPUP_5,

};

class MainScene : public Scene{
public:
    static Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

private:
    gui mGrid;
    ui_gacha  mGacha;
    cocos2d::LayerColor * layer;
    cocos2d::LayerColor * layerGray;
    cocos2d::LayerColor * mParitclePopupLayer, * mParitclePopup;
    bool mIsParticleFinished;
    LoadingBar * loadingBar;

    static void paricleCB(){
        //this->removeChild(mParitclePopupLayer);
    };
    void scheduleCB(float f);
    void callback0();
    void callback1(Ref* pSender);
    void callback2(Ref* pSender, SCENECODE type);

    virtual void onEnter();
    virtual void onEnterTransitionDidFinish();
    virtual void onExitTransitionDidStart();
    virtual void onExit();

    void store();
    void store2();
    void particleSample();
};


#endif //PROJ_ANDROID_MAINSCENE_H
