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
    {128, 500}
    , {64, 10}
    , {32, 5}
    , {16, 3}
    , {8, 2}
};

Scene* LotteryScene::createScene(int itemId)
{
    return LotteryScene::create(itemId);
}

bool LotteryScene::init(int itemId)
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    mItemId = itemId;
    _item item = logics::hInst->getItem(mItemId);
    
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    gui::inst()->addBG("bg_temp.png", this);
    
    mInfoLayer = LayerColor::create();
    mInfoLayer->setContentSize(Director::getInstance()->getVisibleSize());
    this->addChild(mInfoLayer);
    
    gui::inst()->addLabel(4, 0, wstring_to_utf8(item.name), mInfoLayer, 24);
    gui::inst()->addLabel(4, 1, "- Prizes -", mInfoLayer);
    string szPrize = "";
    for(int n=0; n < MAXRANK; n++){
        szPrize = szPrize + getRankString(n+1) + ": ";
        szPrize = szPrize + COIN + numberFormat((int)(logics::hInst->getTrade()->getPriceBuy(mItemId) * _prize1[n][1]));
        szPrize = szPrize + "\n";
    }
    
    int remain = logics::hInst->getActor()->inven.getItemQuantuty(logics::hInst->getInventoryType(mItemId), mItemId);
    
    gui::inst()->addLabel(4, 3, szPrize, mInfoLayer, 0, ALIGNMENT_NONE);
    
    gui::inst()->addTextButton(3, 5, "OPEN One", mInfoLayer, [=](Ref* pRef){ gachaPoint(); }, 20, ALIGNMENT_CENTER, Color3B::WHITE); //->runAction(RepeatForever::create(Blink::create(1, 2)));
    gui::inst()->addTextButton(5, 5, "OPEN ALL(" + to_string(remain) + ")", mInfoLayer, [=](Ref* pRef){ gachaPoint(true); }, 20, ALIGNMENT_CENTER, Color3B::ORANGE);
    gui::inst()->addTextButton(8, 0, "CLOSE", mInfoLayer, [=](Ref* pRef){ callbackClose(this); }, 0, ALIGNMENT_CENTER, Color3B::BLUE);
    return true;
}

bool LotteryScene::open(int cnt) {
    if(mInfoLayer)
        this->removeChild(mInfoLayer);
    if(mResultLayer)
        this->removeChild(mResultLayer);
    
    bool ret = false;
    inventoryType category = logics::hInst->getInventoryType(mItemId);
    ret = logics::hInst->getActor()->inven.popItem(category, mItemId, cnt);
    logics::hInst->saveActor();
    
    return ret;
}

void LotteryScene::gachaPoint(bool isAll){
    int loopCnt = 1;
    if(isAll)
        loopCnt = logics::hInst->getActor()->inven.getItemQuantuty(logics::hInst->getInventoryType(mItemId), mItemId);
    
    if(!open(loopCnt)){
        callbackClose(this);
        return;
    }
    
    unsigned int point = 0;
    time_t t = getNow();
    int number;
    
    int arrRank[MAXRANK] = {0,};
    
    for(int i=0; i< loopCnt; i++) {
        for(int n=0; n < MAXRANK; n++) {
            int probability = (int)_prize1[n][0];
            number = (int)t % probability;
            int winningNumber = getRandValue(probability);
            
            if(winningNumber == number){
                point += logics::hInst->getTrade()->getPriceBuy(mItemId) * _prize1[n][1];
                arrRank[n]++;
                break;
            }
        }
    }
    
    
    logics::hInst->increasePoint(point);
    
    auto bg = LayerColor::create(Color4B::BLACK);
    bg->setContentSize(Director::getInstance()->getVisibleSize());
    bg->setPosition(Director::getInstance()->getVisibleOrigin());
    this->addChild(bg);
    
    Size grid = Size(5,5);
    string szRank;
    for(int n=0; n < MAXRANK; n++) {
        if(arrRank[n] > 0) {
            szRank += to_string(n+1) + wstring_to_utf8(L" 등 당첨 x ") + to_string(arrRank[n]) + "\n";
        }
    }
    string sz = point > 0 ? COIN + numberFormat(point) : "T.T";
    mResultLayer = LayerColor::create();
    mResultLayer->setContentSize(Size(300, 225));
    gui::inst()->addLabelAutoDimension(2, 1, szRank, mResultLayer, 20, ALIGNMENT_CENTER, Color3B::WHITE, grid, Size::ZERO, Size::ZERO);
    gui::inst()->addLabelAutoDimension(2, 2, sz, mResultLayer, 24, ALIGNMENT_CENTER, Color3B::GREEN, grid, Size::ZERO, Size::ZERO);
    
    int x = 2;
    if(logics::hInst->getActor()->inven.checkItemQuantity(logics::hInst->getInventoryType(mItemId), mItemId, 1)){
        x++;
        int remain = logics::hInst->getActor()->inven.getItemQuantuty(logics::hInst->getInventoryType(mItemId), mItemId);
        gui::inst()->addTextButtonAutoDimension(1,5, "1 MORE TRY"
                                                , mResultLayer
                                                , [=](Ref * pRef) { gachaPoint(); }
                                                , 20
                                                , ALIGNMENT_CENTER
                                                , Color3B::WHITE
                                                , grid
                                                );
        
        gui::inst()->addTextButtonAutoDimension(3,5, "Open All(" + to_string(remain) + ")"
                                                , mResultLayer
                                                , [=](Ref * pRef) { gachaPoint(true); }
                                                , 20
                                                , ALIGNMENT_CENTER
                                                , Color3B::ORANGE
                                                , grid
                                                );
    }
    
    gui::inst()->addTextButtonAutoDimension(5,0, "CLOSE"
                                            , mResultLayer
                                            , CC_CALLBACK_1(LotteryScene::callbackClose, this)
                                            , 0
                                            , ALIGNMENT_CENTER
                                            , Color3B::BLUE
                                            , grid
                                            );
    ui_gacha gacha;
    gacha.run(this, "coin.png", mResultLayer, PARTICLE_MAGIC, (point > 0) ? PARTICLE_FINAL : "");
}

