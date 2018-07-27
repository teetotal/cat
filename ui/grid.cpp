//
// Created by daejung on 2018-07-24.
//

#include "grid.h"
#include "HelloWorldScene.h"
void grid::init(float originX, float originY, float width, float height, const char* font, int fontSize) {
    mOriginX =originX;
    mOriginY = originY;
    mWidth = width - GRID_MARGIN * 2;
    mHeight = height - GRID_MARGIN * 2;

    mGridWidth = (mWidth) / GRID_X;
    mGridHeight = (mHeight) / GRID_Y;

    mDefaultFont = font;
    mDefaultFontSize = fontSize;

    MenuItemFont::setFontName(font);
    MenuItemFont::setFontSize(fontSize);
}
bool grid::getPoint(int x, int y, float &pointX, float &pointY, ALIGNMENT align){

    pointX = (mGridWidth * x) + mOriginX + GRID_MARGIN;
    pointY = mOriginY + GRID_MARGIN+ mHeight -  (mGridHeight * y) ;

    switch(align){
        case ALIGNMENT_NONE:
            //pointX += mGridWidth/ 2;
            //pointY -= mGridHeight/2;
            break;
        case ALIGNMENT_CENTER:
            pointX += mGridWidth/ 2;
            pointY -= mGridHeight/2;
            break;
    }
    return true;
}

bool grid::drawGrid(Scene * p){
    for(int x=0; x<=GRID_X; x++){
        auto draw = DrawNode::create();
        float startX, startY, endX, endY;
        getPoint(x,0, startX, startY);
        getPoint(x,GRID_Y, endX, endY);
        draw->drawLine(Point(startX, startY), Point(endX, endY), Color4F::GRAY);
        p->addChild(draw);
    }

    for(int y=0; y<=GRID_Y; y++){
        auto draw = DrawNode::create();
        float startX, startY, endX, endY;
        getPoint(0,y, startX, startY);
        getPoint(GRID_X, y, endX, endY);
        draw->drawLine(Point(startX, startY), Point(endX, endY), Color4F::GRAY);
        p->addChild(draw);
    }

    return true;
}

bool grid::drawPoint(Scene *p) {
    for(int x =0; x < GRID_X; x ++){
        for(int y=0; y < GRID_Y; y++){
            std::string sz = std::to_string(x);
            sz += ",";
            sz += std::to_string(y);

            auto labelPoint = Label::createWithTTF(sz, mDefaultFont, 14);
            if(labelPoint == NULL)
                return false;
            labelPoint->setColor(Color3B::GRAY);
            float labelX, labelY;
            getPoint(x,y, labelX, labelY, ALIGNMENT_CENTER);
            labelPoint->setPosition(Vec2(labelX, labelY));
            //labelPoint->setAnchorPoint(Vec2(0,1));
            p->addChild(labelPoint, 1);
        }
    }
    return true;
}

Label * grid::addLabel(int x, int y, const string &text, Scene *p, int fontSize, ALIGNMENT align, const Color3B color){
    if(fontSize == 0)
        fontSize = mDefaultFontSize;

    float pointX, pointY;
    getPoint(x,y, pointX, pointY, align);
    auto label = Label::createWithTTF(text, mDefaultFont, fontSize);
    label->setPosition(Point(pointX, pointY));
    label->setColor(color);
    if(align == ALIGNMENT_NONE)
        label->setAnchorPoint(getNoneAnchorPoint());
    p->addChild(label);
    return label;
}

MenuItem *grid::addTextButton(int x, int y, const char * text, Scene *p, const ccMenuCallback &callback,
                              int fontSize, ALIGNMENT align, const Color3B color) {

    if(fontSize > 0)
        MenuItemFont::setFontSize(fontSize);

    auto pItem = MenuItemFont::create(text, callback);
    pItem->setColor(color);
    auto pMenu = Menu::create(pItem, NULL);
    float pointX, pointY;
    getPoint(x,y, pointX, pointY, align);
    pMenu->setPosition(Point(pointX, pointY));
    if(align == ALIGNMENT_NONE){
        pItem->setAnchorPoint(getNoneAnchorPoint());
        //pMenu->setAnchorPoint(Vec2(0,1.5));
    }


    p->addChild(pMenu);

    if(fontSize > 0)
        MenuItemFont::setFontSize(mDefaultFontSize);

    return pItem;
}