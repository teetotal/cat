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

class PhysicsScene : public Scene {
public:
    
protected:
    void initPhysicsBody(PhysicsMaterial material, bool isDebugDrawMask = false, float speed = 2.f);
    void setDebugDrawMask();
    void setAutoStepToggle();
    void setStep(float step);
    void setGravity(const Vec2& gravity);
    Node * setPhysicsBodyCircle(Node * p, PhysicsMaterial material, bool dynamic = true);
};

#endif /* PhysicsScene_hpp */
