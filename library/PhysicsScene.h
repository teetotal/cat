//
//  PhysicsScene.hpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 10/01/2019.
//

#ifndef PhysicsScene_hpp
#define PhysicsScene_hpp

#include "cocos2d.h"
#include "../ui/ui.h"

USING_NS_CC;

#define CREATE_FUNC_PHYSICS(__TYPE__) \
static __TYPE__* create() \
{ \
__TYPE__ *pRet = new(std::nothrow) __TYPE__(); \
if (pRet && pRet->initWithPhysics() && pRet->init()) \
{ \
pRet->autorelease(); \
return pRet; \
} \
else \
{ \
delete pRet; \
pRet = nullptr; \
return nullptr; \
} \
}

#define PHYSICS_CONTACT(__TYPE__) \
auto listenerContact = EventListenerPhysicsContact::create(); \
listenerContact->onContactBegin = CC_CALLBACK_1(__TYPE__::onContactBegin, this); \
_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerContact, this);

class PhysicsScene : public Scene {
public:
    
protected:
    void initPhysicsBody(PhysicsMaterial material, bool isDebugDrawMask = false, float speed = 2.f);
    void setDebugDrawMask();
    void setAutoStepToggle();
    void setStep(float step);
    void setGravity(const Vec2& gravity);
    Node * setPhysicsBodyCircle(Node * p, PhysicsMaterial material, bool dynamic = true, int tag = 0, int bitmaskCategory=0xFFFFFFFF, int bitmaskCollision=0xFFFFFFFF, int bitmaskContact=0);
    
    struct bitmask {
        int categoryA;
        int categoryB;
        
        int collisionA;
        int collisionB;
        
        int contactA;
        int contactB;
    };
    
    bitmask getBitmask(PhysicsContact &contact);
    bool isCollosion(PhysicsContact &contact, int tag, int &otherTag);
    bool isContact(PhysicsContact &contact, int tag, int &otherTag);
    Vec2 getTag(PhysicsContact &contact); //x가 A, Y가 B
};

#endif /* PhysicsScene_hpp */
