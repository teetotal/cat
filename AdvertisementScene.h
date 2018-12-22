//
//  AdvertisementScene.hpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 22/12/2018.
//

#ifndef AdvertisementScene_h
#define AdvertisementScene_h

#include "MainScene.h"
#include "cocos2d.h"
#include "ui/ui.h"
#include "logics.h"

USING_NS_CC;

class AdvertisementScene : public Scene {
public:
    static Scene* createScene(int itemId, int quantity) {
        return AdvertisementScene::create(itemId, quantity);
    };
    
    bool init(int itemId, int quantity);
    
    static AdvertisementScene * create(int itemId, int quantity)
    {
        AdvertisementScene *pRet = new(std::nothrow) AdvertisementScene();
        if (pRet && pRet->init(itemId, quantity))
        {
            pRet->autorelease();
        }
        else
        {
            delete pRet;
            pRet = nullptr;
        }
        return pRet;
    };
    
private:
    void callback(float f);
    Sprite * createDancer(float width, Vec2 pos);
     void closeScene() {
        Director::getInstance()->popScene();
    };
    
    int mItemId;
    int mQuantity;
};

#endif /* AdvertisementScene_h */
