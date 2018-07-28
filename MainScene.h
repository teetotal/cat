//
// Created by Jung, DaeCheon on 27/07/2018.
//

#ifndef PROJ_ANDROID_MAINSCENE_H
#define PROJ_ANDROID_MAINSCENE_H

#include "cocos2d.h"
#include "ui/ui.h"

USING_NS_CC;

enum SCENECODE{
    SCENECODE_MAIN = 0,
    SCENECODE_ACTION,
    SCENECODE_RACE,
    SCENECODE_FARMING,
    SCENECODE_BUY,
    SCENECODE_SELL,
    SCENECODE_RECHARGE,
    SCENECODE_PURCHASE,
    SCENECODE_COLLECTION,
    SCENECODE_ACHIEVEMENT,
    SCENECODE_NONE
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

    cocos2d::LayerColor * layer;
    cocos2d::LayerColor * layerGray;

    void callback2(Ref* pSender, SCENECODE type);

    virtual void onEnter();
    virtual void onEnterTransitionDidFinish();
    virtual void onExitTransitionDidStart();
    virtual void onExit();
};


#endif //PROJ_ANDROID_MAINSCENE_H