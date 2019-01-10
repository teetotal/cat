//
//  PhysicsScene.cpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 10/01/2019.
//

#include "PhysicsScene.h"

void PhysicsScene::initPhysicsBody(PhysicsMaterial material, bool isDebugDrawMask, float speed) {
    auto edgeNode = Node::create();
    edgeNode->setPosition(gui::inst()->getCenter());
    edgeNode->setPhysicsBody(PhysicsBody::createEdgeBox(Director::getInstance()->getVisibleSize(), material));
    
    this->addChild(edgeNode);
    
    this->getPhysicsWorld()->setSpeed(speed);
    if(isDebugDrawMask)
        setDebugDrawMask();
}

void PhysicsScene::setDebugDrawMask() {
    this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
}

void PhysicsScene::setAutoStepToggle() {
    this->getPhysicsWorld()->setAutoStep(!this->getPhysicsWorld()->isAutoStep());
}

void PhysicsScene::setGravity(const Vec2& gravity) {
    this->getPhysicsWorld()->setGravity(gravity);
}

void PhysicsScene::setStep(float step) {
    if(this->getPhysicsWorld()->isAutoStep())
        this->getPhysicsWorld()->step(step);
}

Node * PhysicsScene::setPhysicsBodyCircle(Node * p, PhysicsMaterial material, bool dynamic) {
    float width = p->getContentSize().width / 2;
    auto body = PhysicsBody::createCircle(width, material);
    body->setDynamic(dynamic);
    p->setPhysicsBody(body);
    return p;
}

