//
//  LotteryScene.hpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 27/11/2018.
//

#ifndef LotteryScene_h
#define LotteryScene_h

#include "MainScene.h"
#include "cocos2d.h"
#include "ui/ui.h"
#include "logics.h"

USING_NS_CC;

class LotteryScene : public Scene {
public:
    static Scene* createScene(int itemId);
    
    bool init(int itemId);
    
    static LotteryScene * create(int itemId)
    {
        LotteryScene *pRet = new(std::nothrow) LotteryScene();
        if (pRet && pRet->init(itemId))
        {
            pRet->autorelease();
            return pRet;
        }
        else
        {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    };
    
private:
    void gachaPoint();
    bool open();
    void callbackClose(Ref * pRef) {
        Director::getInstance()->popScene();
    };
    
    int mItemId;
    LayerColor * mInfoLayer;
};

#endif /* LotteryScene_hpp */
