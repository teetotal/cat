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
    //Director::getInstance()->setClearColor(Color4F::BLACK);

    gui::inst()->addBGScrolling("layers/sky.png", this, 3000);

    gui::inst()->addBGScrolling("layers/clouds_1.png", this, 25);
    gui::inst()->addBGScrolling("layers/rocks.png", this, 40);
    gui::inst()->addBGScrolling("layers/clouds_2.png", this, 20);

    gui::inst()->addBGScrolling("layers/plant.png", this, 30);
    gui::inst()->addBGScrolling("layers/ground_1.png", this, 20);

    auto zombie = gui::inst()->addSprite(0, 5, "zombie.png", this);

    float duration = 5;
    auto seq1 = RepeatForever::create(Sequence::create(
            MoveBy::create(duration, Vec2(Director::getInstance()->getVisibleSize().width /2 , 0))
            , MoveBy::create(duration, Vec2(Director::getInstance()->getVisibleSize().width * -0.5, 0))
            , NULL
    ));

    zombie->runAction(seq1);

    gui::inst()->addBGScrolling("layers/ground_2.png", this, 10);
    gui::inst()->addBGScrolling("layers/ground_3.png", this, 5);

    gui::inst()->addTextButton(0,6,"╈", this, CC_CALLBACK_1(ActionScene::callback2, this, SCENECODE_ACTION), 32, ALIGNMENT_CENTER, Color3B::WHITE);


    Vec2 center = Vec2(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
    //zombie->setPosition(center);

    //this->addChild(ParticleRain::create());

    Vec2 point;
    gui::inst()->getPoint(15, 3, point, ALIGNMENT_CENTER);

    this->runAction(Follow::create(zombie, Rect(0, 0, Director::getInstance()->getVisibleSize().width * 2, Director::getInstance()->getVisibleSize().height * 1)    ));
    //this->runAction(Follow::create(zombie));

    return true;
}

void ActionScene::callback2(Ref* pSender, SCENECODE type){
    //auto pScene = MainScene::createScene();
    //Director::getInstance()->replaceScene(pScene);
    Director::getInstance()->popScene();
}