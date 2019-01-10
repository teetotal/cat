//
//  ActionBall.cpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 10/01/2019.
//

#include "ActionBall.h"



USING_NS_CC;

Scene* ActionBall::createScene()
{
    return ActionBall::create();
}

bool ActionBall::init() {
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    PhysicsMaterial material = PhysicsMaterial(1,0.8,1);
    // 2. physics body init
    initPhysicsBody(material);
//    setDebugDrawMask();
    
    TOUCH_INIT(ActionBall);
    mBall = gui::inst()->addSprite(1, 6, "circle.png", this);
    gui::inst()->setScale(mBall, 40);
    mLine = NULL;
    mBall = this->setPhysicsBodyCircle(mBall, material);
    
    
    setPhysicsBodyCircle(gui::inst()->addSprite(4, 3, "circle.png", this), material, false);
    
    gui::inst()->addTextButton(0, 0, "CLOSE", this, [=](Ref* pSender){
        Director::getInstance()->popScene();
    });
    return true;
}

bool ActionBall::onTouchBegan(Touch* touch, Event* event)  {
    mStartPos = touch->getLocation();
    return true;
}
bool ActionBall::onTouchEnded(Touch* touch, Event* event) {
    if(mLine)
        this->removeChild(mLine);
    
    Vec2 moved = Vec2(mStartPos.x - touch->getLocation().x, mStartPos.y - touch->getLocation().y);

    mBall->getPhysicsBody()->setVelocity(moved);
    return true;
}
void ActionBall::onTouchMoved(Touch *touch, Event *event) {
    if(mLine)
        this->removeChild(mLine);
    
    Vec2 moved = Vec2(touch->getLocation().x - mStartPos.x, touch->getLocation().y - mStartPos.y);

    Vec2 start = gui::inst()->getCenter();
    Vec2 end = Vec2(start.x - moved.x , start.y - moved.y);
    
    mLine = gui::inst()->drawLine(this, start, end);
}
