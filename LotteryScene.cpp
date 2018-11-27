//
//  LotteryScene.cpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 27/11/2018.
//

#include "LotteryScene.h"
#include "ui_gacha.h"

#define PARTICLE_FINAL "particles/particle_finally.plist"
#define PARTICLE_MAGIC "particles/particle_magic.plist"

Scene* LotteryScene::createScene(int itemId)
{
    return LotteryScene::create(itemId);
}

bool LotteryScene::init(int itemId)
{
    mItemId = itemId;
    gui::inst()->addTextButton(4, 3, "Run", this, [=](Ref* pRef){ gachaPoint(getRandValue(1000)); });
    return true;
}

void LotteryScene::gachaPoint(unsigned int point){
    
    logics::hInst->increasePoint(point);
    
    auto bg = LayerColor::create(Color4B::BLACK);
    bg->setContentSize(Size(640,320));
    this->addChild(bg);
    
    string sz = COIN + to_string(point);
    auto layer = LayerColor::create();
    layer->setContentSize(Size(300, 225));
    gui::inst()->addLabelAutoDimension(0, 0, sz, layer, 32, ALIGNMENT_CENTER, Color3B::GREEN, Size(1, 1), Size::ZERO, Size::ZERO);
    gui::inst()->addTextButtonAutoDimension(0,5, "CLOSE"
                                            , layer
                                            , CC_CALLBACK_1(LotteryScene::callbackClose, this)
                                            , 10
                                            , ALIGNMENT_CENTER
                                            , Color3B::WHITE
                                            , Size(1, 5)
                                            );
    ui_gacha gacha;
    gacha.run(this, "coin.png", layer, PARTICLE_MAGIC, PARTICLE_FINAL);
}

