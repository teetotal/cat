//
//  ActionBall.hpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 10/01/2019.
//

#ifndef ActionBall_h
#define ActionBall_h

#include "library/PhysicsScene.h"

USING_NS_CC;

class ActionBall : public PhysicsScene {
public:
    static Scene* createScene();
    virtual bool init();
    
    CREATE_FUNC_PHYSICS(ActionBall);
private:
    bool onTouchBegan(Touch* touch, Event* event) ;
    bool onTouchEnded(Touch* touch, Event* event);
    void onTouchMoved(Touch *touch, Event *event);
    
    Node * mBall;
    Node * mLine;
    Vec2 mStartPos;
};

#endif /* ActionBall_hpp */
