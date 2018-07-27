//
// Created by Jung, DaeCheon on 27/07/2018.
//

#ifndef PROJ_ANDROID_ACTIONSCENE_H
#define PROJ_ANDROID_ACTIONSCENE_H

#include "MainScene.h"
#include "cocos2d.h"
#include "ui/grid.h"

USING_NS_CC;

class ActionScene : public Scene {
public:
    static Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(ActionScene);

private:
    void callback2(Ref* pSender, SCENECODE type);
};


#endif //PROJ_ANDROID_ACTIONSCENE_H
