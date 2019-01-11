//
//  ActionBall.cpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 10/01/2019.
//

#include "ActionBall.h"
#include "library/util.h"


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
    
    mLine = NULL;
    mScore = 0;
    
    TOUCH_INIT(ActionBall);
    PHYSICS_CONTACT(ActionBall);
   
    gui::inst()->addTextButton(0, 0, "CLOSE", this, [=](Ref* pSender){
        Director::getInstance()->popScene();
    });
    
    mScoreLabel = gui::inst()->addLabel(4, 0, " ", this);
    
    //ball
    mBall = gui::inst()->addSprite(0, 6, "circle.png", this);
    gui::inst()->setScale(mBall, 40);
    this->setPhysicsBodyCircle(mBall, material, true, 1, 0x1 << 0, 0x1 << 1 | 0x1 << 2, 0x1 << 1 | 0x1 << 2);
    
    //hurdle
    
    float radius = 20;
    
    for(int n=0; n<3; n++) {
        auto p = gui::inst()->addSprite(3 + (n * 2), 0, "circle.png", this);
        gui::inst()->setScale(p, radius);
        p->runAction(RepeatForever::create(Sequence::create(
                                                            MoveTo::create(((float)getRandValueMinMax(5, 15) / 10.f), Vec2(p->getPosition().x, gui::inst()->mOriginY + radius))
                                                            , MoveTo::create(((float)getRandValueMinMax(5, 15) / 10.f), Vec2(p->getPosition().x, gui::inst()->mOriginY + gui::inst()->mVisibleY - radius))
                                                            , NULL)
                                           )
                     );
        setPhysicsBodyCircle(p, material, false, 2, 0x1 << 1, 0x1 << 0, 0x1 << 0);
    }
    
    mCoin = gui::inst()->addSprite(8, 0, "coin.png", this);
    gui::inst()->setScale(mCoin, radius);
    mCoin->runAction(RepeatForever::create(Sequence::create(
                                                        MoveTo::create(1, Vec2(mCoin->getPosition().x, gui::inst()->mOriginY + radius))
                                                        , MoveTo::create(1, Vec2(mCoin->getPosition().x, gui::inst()->mOriginY + gui::inst()->mVisibleY - radius))
                                                        , NULL)
                                       )
                 );
    this->setPhysicsBodyCircle(mCoin, material, false, 4, 0x1 << 2, 0x1 << 0, 0x1 << 0);
    
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

    Vec2 start = gui::inst()->mOrigin;
    Vec2 end = Vec2(start.x - moved.x , start.y - moved.y);
    
    mLine = gui::inst()->drawLine(this, start, end);
}

bool ActionBall::onContactBegin(PhysicsContact &contact) {
//    int categoryA = contact.getShapeA()->getBody()->getCategoryBitmask();
//    int categoryB = contact.getShapeB()->getBody()->getCategoryBitmask();
//
//    int collisionA = contact.getShapeA()->getBody()->getCollisionBitmask();
//    int collisionB = contact.getShapeB()->getBody()->getCollisionBitmask();
//
//    int contactA = contact.getShapeA()->getBody()->getContactTestBitmask();
//    int contactB = contact.getShapeA()->getBody()->getContactTestBitmask();
    
    
    int other;
    if(isCollosion(contact, 1, other)) {
        CCLOG("Collision 1 with %d", other);
//        bitmask st = getBitmask(contact);
//        CCLOG("Category %d, %d", st.categoryA, st.categoryB);
//        CCLOG("Collision %d, %d", st.collisionA, st.collisionB);
//        CCLOG("Contact %d, %d", st.contactA, st.contactB);
    }
    
    if(isContact(contact, 1, other)) {
        CCLOG("Contact 1 with %d", other);
//        bitmask st = getBitmask(contact);
//        CCLOG("Category %d, %d", st.categoryA, st.categoryB);
//        CCLOG("Collision %d, %d", st.collisionA, st.collisionB);
//        CCLOG("Contact %d, %d", st.contactA, st.contactB);
        switch(other) {
            case 2:
                mScore = std::max(0, mScore -1);
                break;
            case 4:
                mScore ++;
                break;
            default:
                break;
        }
        
        mScoreLabel->setString(to_string(mScore));
    }
    
    return true;
}
