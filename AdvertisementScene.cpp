//
//  AdvertisementScene.cpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 22/12/2018.
//

#include "AdvertisementScene.h"
#include <map>
#include <iterator>

Sprite * AdvertisementScene::createDancer(float width, Vec2 pos) {
    auto p = gui::inst()->addSprite(0, 0, "action/2/0.png", this);
    p->setPosition(pos);
    
    auto animation = Animation::create();
    animation->setDelayPerUnit(0.12);
    
    string path;
    for (int n = 0; n <= 5; n++) {
        path = "action/2/" + to_string(n) + ".png";
        animation->addSpriteFrameWithFile(path);
    }
    
    p->runAction(RepeatForever::create(Animate::create(animation)));
    gui::inst()->setScale(p, width);
    
    return p;
}

void AdvertisementScene::callback(float f) {
    logics::hInst->addInventory(mItemId, mQuantity);
    closeScene();
}

bool AdvertisementScene::init(int itemId, int quantity) {
    mItemId = itemId;
    mQuantity = quantity;
    
    _item item = logics::hInst->getItem(itemId);
    
    gui::inst()->addBG("bg_temp.png", this);
    
    gui::inst()->addLabel(0, 0, "Advertisement", this, 0, ALIGNMENT_NONE, Color3B::GRAY);
    
    __items::iterator it = logics::hInst->getItems()->begin();
    std::advance(it, rand() % logics::hInst->getItems()->size());
    
    wstring sz = L"겁나 좋아!! " + it->second.name;
    auto label = gui::inst()->addLabel(4, 1, wstring_to_utf8(sz), this);
    label->runAction(Blink::create(item.grade, item.grade * 2));
    
    createDancer(gui::inst()->mVisibleY / 3, gui::inst()->getPointVec2(4, 4));
    
    this->scheduleOnce(schedule_selector(AdvertisementScene::callback), item.grade);
    return true;
}
