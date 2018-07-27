//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "ActionScene.h"


#include "SimpleAudioEngine.h"


Scene* ActionScene::createScene()
{
    return ActionScene::create();
}

bool ActionScene::init() {
    grid::inst()->addTextButton(0,6,"╈", this, CC_CALLBACK_1(ActionScene::callback2, this, SCENECODE_ACTION), 32);

    return true;
}

void ActionScene::callback2(Ref* pSender, SCENECODE type){
    auto pScene = MainScene::createScene();
    Director::getInstance()->replaceScene(pScene);
}