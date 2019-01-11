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

Node * PhysicsScene::setPhysicsBodyCircle(Node * p, PhysicsMaterial material, bool dynamic, int tag, int bitmaskCategory, int bitmaskCollision, int bitmaskContact) {
    float width = p->getContentSize().width / 2;
    auto body = PhysicsBody::createCircle(width, material);
    body->setDynamic(dynamic);
    body->setCategoryBitmask(bitmaskCategory);
    body->setCollisionBitmask(bitmaskCollision);
    body->setContactTestBitmask(bitmaskContact);
    body->setTag(tag);
    p->setPhysicsBody(body);
    return p;
}

PhysicsScene::bitmask PhysicsScene::getBitmask(PhysicsContact &contact) {
    PhysicsScene::bitmask st;
    st.categoryA = contact.getShapeA()->getBody()->getCategoryBitmask();
    st.categoryB = contact.getShapeB()->getBody()->getCategoryBitmask();
    
    st.collisionA = contact.getShapeA()->getBody()->getCollisionBitmask();
    st.collisionB = contact.getShapeB()->getBody()->getCollisionBitmask();
    
    st.contactA = contact.getShapeA()->getBody()->getContactTestBitmask();
    st.contactB = contact.getShapeA()->getBody()->getContactTestBitmask();
    
    return st;
}

bool PhysicsScene::isCollosion(PhysicsContact &contact, int tag, int &otherTag) {
    Vec2 v = getTag(contact);
    if(v.x != tag && v.y != tag)
        return false;
    
    PhysicsScene::bitmask st = getBitmask(contact);
     //A의 CollisionBitmask 와 바디 B의 CategoryBitmask 의 & 연산의 결과가 0
    bool is = ((st.collisionA & st.categoryB) != 0) ? true : false;
    if(is) {
        otherTag = v.x == tag ? v.y : v.x;
    }
    return is;
}
bool PhysicsScene::isContact(PhysicsContact &contact, int tag, int &otherTag) {
    Vec2 v = getTag(contact);
    if(v.x != tag && v.y != tag)
        return false;
    
    PhysicsScene::bitmask st = getBitmask(contact);
    //A의 CategoryBitmask 와 바디 B의 ContactTestBitmask 의 & 연산의 결과가 0이 아니면 됩니다.
    bool is = ((st.categoryA & st.contactB) != 0 || (st.categoryB & st.contactA) != 0) ? true : false;
    if(is) {
        otherTag = v.x == tag ? v.y : v.x;
    }
    return is;
}

Vec2 PhysicsScene::getTag(PhysicsContact &contact) {
    return Vec2(contact.getShapeA()->getBody()->getTag(), contact.getShapeB()->getBody()->getTag());
}
