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

#define MAXRANK 5
//확률, 배율
float _prize1[MAXRANK][2] = {
    {10000, 10000}
    , {1000, 1000}
    , {12, 5}
    , {6, 2}
    , {2, 1.5}
};

Scene* LotteryScene::createScene(int itemId)
{
    return LotteryScene::create(itemId);
}

bool LotteryScene::init(int itemId)
{
    mItemId = itemId;
    _item item = logics::hInst->getItem(mItemId);
    
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto bg = Sprite::create("bg_temp.png");
    bg->setContentSize(Director::getInstance()->getVisibleSize());
    bg->setAnchorPoint(Vec2(0, 0));
    bg->setPosition(Director::getInstance()->getVisibleOrigin());
    this->addChild(bg);
    
    gui::inst()->addLabel(4, 0, wstring_to_utf8(item.name), this, 24);
    gui::inst()->addLabel(4, 1, "- Prizes -", this);
    string szPrize = "";
    for(int n=0; n < MAXRANK; n++){
        szPrize = szPrize + to_string(n+1) + ": ";
        szPrize = szPrize + COIN + to_string((int)(logics::hInst->getTrade()->getPriceBuy(mItemId) * _prize1[n][1]));
        szPrize = szPrize + "\n";
    }
    
    gui::inst()->addLabel(4, 3, szPrize, this, 0, ALIGNMENT_NONE);
    
    gui::inst()->addTextButton(4, 5, "OPEN", this, [=](Ref* pRef){ gachaPoint(); }, 32, ALIGNMENT_CENTER, Color3B::BLUE); //->runAction(RepeatForever::create(Blink::create(1, 2)));
    gui::inst()->addTextButton(8, 0, "CLOSE", this, [=](Ref* pRef){ callbackClose(this); }, 0, ALIGNMENT_CENTER, Color3B::RED);
    return true;
}

bool LotteryScene::open() {
    bool ret = false;
    inventoryType category = logics::hInst->getInventoryType(mItemId);
    ret = logics::hInst->getActor()->inven.popItem(category, mItemId, 1);
    logics::hInst->saveActor();
    
    return ret;
}

void LotteryScene::gachaPoint(){
    if(!open())
        return;
    
    unsigned int point = 0;
    time_t t = getNow();
    int number;
    int rank = -1;
    for(int n=0; n < MAXRANK; n++) {
        int probability = (int)_prize1[n][0];
        number = (int)t % probability;
        int winningNumber = getRandValue(probability);
        
        if(winningNumber == number){
            point = logics::hInst->getTrade()->getPriceBuy(mItemId) * _prize1[n][1];
            rank = n+1;
            break;
        }
    }
    
    
    logics::hInst->increasePoint(point);
    
    auto bg = LayerColor::create(Color4B::BLACK);
    bg->setContentSize(Size(640,320));
    this->addChild(bg);
    
    string szRank = rank == -1 ? " " : to_string(rank) + wstring_to_utf8(L"등 당첨");
    string sz = point > 0 ? COIN + to_string(point) : "T.T";
    auto layer = LayerColor::create();
    layer->setContentSize(Size(300, 225));
    gui::inst()->addLabelAutoDimension(0, 1, szRank, layer, 32, ALIGNMENT_CENTER, Color3B::WHITE, Size(1, 5), Size::ZERO, Size::ZERO);
    gui::inst()->addLabelAutoDimension(0, 2, sz, layer, 28, ALIGNMENT_CENTER, Color3B::GREEN, Size(1, 5), Size::ZERO, Size::ZERO);
    gui::inst()->addTextButtonAutoDimension(0,5, "CLOSE"
                                            , layer
                                            , CC_CALLBACK_1(LotteryScene::callbackClose, this)
                                            , 0
                                            , ALIGNMENT_CENTER
                                            , Color3B::WHITE
                                            , Size(1, 5)
                                            );
    ui_gacha gacha;
    gacha.run(this, "coin.png", layer, PARTICLE_MAGIC, (point > 0) ? PARTICLE_FINAL : "");
}

