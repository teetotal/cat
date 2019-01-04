//
//  ui_bgText.cpp
//  cat-mobile
//
//  Created by Jung, DaeCheon on 04/01/2019.
//

#include "ui_bgText.h"

void ui_bgText::drawBG(Node * pParent) {
    if(mBGLayerColor)
        mParent->removeChild(mBGLayerColor);
    mParent = pParent;
    
    mBGLayerColor = LayerColor::create(Color4B(mBGColor));
    mBGLayerColor->setOpacity(mOpacity);
    setBGSize();
//    mBGLayerColor->setContentSize(size);
//    mBGLayerColor->setPosition(pos);
    mParent->addChild(mBGLayerColor);
}

Node * ui_bgText::getNode() {
    if(mIsButton) {
        return mMenuItemLabel;
    }
    
    return NULL;
}

Vec2 ui_bgText::getPosition() {
    Node * p = getNode();
    if(p)
        return p->getPosition();
    
    return Vec2::ZERO;
}

string ui_bgText::getString() const{
    if(mIsButton) {
        return mMenuItemLabel->getString();
    }
    return "";
}

void ui_bgText::setString(const string& sz) {
    if(mIsButton) {
        mMenuItemLabel->setString(sz);
    }
    
    setBGSize();
}

void ui_bgText::setBGSize() {
    if(mIsButton) {
        
        mBGLayerColor->setContentSize(mMenuItemLabel->getContentSize());
        mBGLayerColor->setPosition(Vec2(mMenu->getPosition().x - mMenuItemLabel->getContentSize().width / 2, mMenu->getPosition().y - mMenuItemLabel->getContentSize().height / 2));
    }
}
