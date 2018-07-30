﻿//
// Created by daejung on 2018-07-24.
//

#include "ui.h"

gui * gui::hInstance = NULL;

void gui::init(const char* font, int fontSize, Color4F bgColor) {

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    mResolution = Director::getInstance()->getWinSizeInPixels();


    mOriginX =origin.x;
    mOriginY = origin.y;

    mVisibleX = visibleSize.width;
    mVisibleY = visibleSize.height;

    mWidth = visibleSize.width - GRID_MARGIN * 2;
    mHeight = visibleSize.height - GRID_MARGIN * 2;

    mDefaultFont = font;
    mDefaultFontSize = fontSize;

    MenuItemFont::setFontName(font);
    MenuItemFont::setFontSize(fontSize);

    Director::getInstance()->setClearColor(bgColor);
}

float gui::getRealPixel(float x){
    return mResolution.width * x / mVisibleX;
}

float gui::getSizeFromRealPixel(float x){

    return mVisibleX * x / mResolution.width;
}

bool gui::getPoint(int x, int y, Vec2 &point, ALIGNMENT align
        , Size dimension
        , Size grid
        , Size origin
        , Size margin){

    return getPoint(x, y, point.x, point.y, align, dimension, grid, origin, margin);

}

bool gui::getPoint(int x, int y, float &pointX, float &pointY, ALIGNMENT align
        , Size dimension
        , Size grid
        , Size origin
        , Size margin){

    int gridX = grid.width == GRID_INVALID_VALUE ? GRID_X : grid.width;
    int gridY = grid.height == GRID_INVALID_VALUE ? GRID_Y : grid.height;

    float originX = origin.width == GRID_INVALID_VALUE ? mOriginX : origin.width;
    float originY = origin.height == GRID_INVALID_VALUE ? mOriginY : origin.height;

    float marginX = margin.width == GRID_INVALID_VALUE ? GRID_MARGIN : margin.width;
    float marginY = margin.height == GRID_INVALID_VALUE ? GRID_MARGIN : margin.height;

    float dimensionX = dimension.width == GRID_INVALID_VALUE ? mWidth : dimension.width - marginX * 2;
    float dimensionY = dimension.height == GRID_INVALID_VALUE ? mHeight : dimension.height - marginY * 2;

    float gridWidth = dimensionX / gridX;
    float gridHeight = dimensionY / gridY;


    pointX = (gridWidth * x) + originX + marginX;
    pointY = originY + marginY + dimensionY -  (gridHeight * y) ;

    switch(align){
        case ALIGNMENT_NONE:
            //pointX += mGridWidth/ 2;
            //pointY -= mGridHeight/2;
            break;
        case ALIGNMENT_CENTER:
            pointX += gridWidth/ 2;
            pointY -= gridHeight/2;
            break;
    }
    return true;
}

void gui::getPoint(int x, int y
        , float &pointX_Center, float &pointY_Center
        , float &pointX_None, float &pointY_None
        , ALIGNMENT align
        , Size dimension
        , Size grid
        , Size origin
        , Size margin) {

    getPoint(x,y
            , pointX_Center, pointY_Center
            , ALIGNMENT_CENTER
            , dimension
            , grid
            , origin
            , margin
    );

    getPoint(x,y
            , pointX_None, pointY_None
            , ALIGNMENT_NONE
            , dimension
            , grid
            , origin
            , margin
    );
}

bool gui::drawGrid(Node * p
        , Size dimension
        , Size grid
        , Size origin
        , Size margin){
    int gridX = grid.width == GRID_INVALID_VALUE ? GRID_X : grid.width;
    int gridY = grid.height == GRID_INVALID_VALUE ? GRID_Y : grid.height;

    for(int x = 0; x <= gridX; x++){
        auto draw = DrawNode::create();
        float startX, startY, endX, endY;
        getPoint(x,0, startX, startY, ALIGNMENT_NONE, dimension
                , grid
                , origin
                , margin);
        getPoint(x, gridY, endX, endY, ALIGNMENT_NONE, dimension
                , grid
                , origin
                , margin);
        draw->drawLine(Point(startX, startY), Point(endX, endY), Color4F::GRAY);
        p->addChild(draw);
    }

    for(int y=0; y<=gridY; y++){
        auto draw = DrawNode::create();
        float startX, startY, endX, endY;
        getPoint(0,y, startX, startY, ALIGNMENT_NONE, dimension
                , grid
                , origin
                , margin);
        getPoint(gridX, y, endX, endY, ALIGNMENT_NONE, dimension
                , grid
                , origin
                , margin);
        draw->drawLine(Point(startX, startY), Point(endX, endY), Color4F::GRAY);
        p->addChild(draw);
    }

    return true;
}

bool gui::drawPoint(Node *p) {
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

Label * gui::addLabel(int x, int y, const string &text, Node *p, int fontSize, ALIGNMENT align, const Color3B color
        , Size dimension
        , Size grid
        , Size origin
        , Size margin
        , const string img
        , bool isBGImg
){
    if(fontSize == 0)
        fontSize = mDefaultFontSize;

    float pointX, pointY;
    float pointX_NONE, pointY_NONE;
    getPoint(x,y
            , pointX, pointY
            , pointX_NONE, pointY_NONE
            , ALIGNMENT_CENTER
            , dimension
            , grid
            , origin
            , margin
    );

    Label * label = Label::createWithTTF(text, mDefaultFont, fontSize);
    label->setColor(color);

    float pX;

    if(img.compare("") != 0){
        auto sprite = Sprite::create(img);
        if(isBGImg){

            sprite->setPosition(Point(
                    (align == ALIGNMENT_NONE) ? pointX_NONE + (sprite->getContentSize().width /2) : pointX
                    , pointY));
            label->setPosition(Point(
                    (align == ALIGNMENT_NONE) ? pointX_NONE + (label->getContentSize().width /2) : pointX
                    , pointY));

        }else{

            pX = (align == ALIGNMENT_NONE) ? pointX_NONE + (sprite->getContentSize().width /2) :
                 pointX_NONE + ((sprite->getContentSize().width + label->getContentSize().width) /2);

            sprite->setPosition(Point(pX, pointY));
            label->setPosition(Point(pX + (sprite->getContentSize().width /2) + (label->getContentSize().width / 2), pointY));
        }

        p->addChild(sprite);

    }else{

        pX = (align == ALIGNMENT_NONE) ? pointX_NONE + (label->getContentSize().width / 2) :
             pointX;

        label->setPosition(Point(pX, pointY));
    }

    p->addChild(label);

    return label;
}

MenuItem * gui::addTextButton(int x, int y, const char * text, Node *p, const ccMenuCallback &callback
        , int fontSize, ALIGNMENT align, const Color3B color
        , Size dimension
        , Size grid
        , Size origin
        , Size margin
        , const string img
        , bool isBGImg
) {

    if(fontSize > 0)
        MenuItemFont::setFontSize(fontSize);

    auto pItem = MenuItemFont::create(text, callback);
    pItem->setColor(color);
    auto pMenu = Menu::create(pItem, NULL);

    float pointX, pointY;
    float pointX_NONE, pointY_NONE;
    getPoint(x,y
            , pointX, pointY
            , pointX_NONE, pointY_NONE
            , ALIGNMENT_CENTER
            , dimension
            , grid
            , origin
            , margin
    );

    float pX;

    if(img.compare("") != 0){

        auto sprite = Sprite::create(img);
        if(isBGImg){

            sprite->setPosition(
                    Point((align == ALIGNMENT_NONE) ? pointX_NONE + (sprite->getContentSize().width /2) : pointX
                    , pointY));

            pMenu->setPosition(
                    Point((align == ALIGNMENT_NONE) ? pointX_NONE + (pItem->getContentSize().width /2) : pointX
                    , pointY));

        }else{

            pX = (align == ALIGNMENT_NONE) ? pointX_NONE + (sprite->getContentSize().width /2) :
                 pointX_NONE + ((sprite->getContentSize().width + pItem->getContentSize().width) /2);

            sprite->setPosition(Point(pX, pointY));
            pMenu->setPosition(Point(pX + (sprite->getContentSize().width /2) + (pItem->getContentSize().width / 2), pointY));
        }

        p->addChild(sprite);

    }else{

        pX = (align == ALIGNMENT_NONE) ? pointX_NONE + (pItem->getContentSize().width / 2) :
             pointX;

        pMenu->setPosition(Point(pX, pointY));
    }

    p->addChild(pMenu);

    if(fontSize > 0)
        MenuItemFont::setFontSize(mDefaultFontSize);

    return pItem;
}

LayerColor * gui::addPopup(LayerColor * &layerBG, Node * p, Size size, const string bgImg, Color4B bgColor){

    layerBG = LayerColor::create(Color4B::BLACK);
    layerBG->setContentSize(Size(mVisibleX, mVisibleY));

    layerBG->setPosition(Vec2(mOriginX, mOriginY));
    layerBG->setOpacity(DEFAULT_OPACITY);

    p->addChild(layerBG);


    LayerColor * layer = cocos2d::LayerColor::create(bgColor);
    layer->setContentSize(size);
    layer->setPosition(Vec2(mVisibleX / 2 - (size.width / 2),
                            (mVisibleY / 2) - (size.height / 2)
                       )
    );

    //drawGrid(layer, size, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, Size::ZERO);

    if(bgImg.compare("") != 0){
        auto sprite = Sprite::create(bgImg);
        sprite->setContentSize(size);
        sprite->setPosition(Vec2::ZERO);
        sprite->setAnchorPoint(Vec2::ZERO);
        layer->addChild(sprite);
    }

    layerBG->addChild(layer);


    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch *touch,Event*event)->bool {
        CCLOG("x %f, y: %f"
        , touch->getLocation().x
        , touch->getLocation().y
        );
        return true;
    };

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listener, layerBG);

    return layer;
}

Layout * gui::createLayout(Size size, const string bgImg, bool hasBGColor, Color3B bgColor){
    Layout* l = Layout::create();
    if(hasBGColor){
        l->setBackGroundColor(bgColor);
        l->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    }

    if(bgImg.compare("") != 0)
        l->setBackGroundImage(bgImg);

    l->setContentSize(size);

    return l;
}

void gui::addLayoutToScrollView(ScrollView * p, Layout * e, float margin, ScrollView::Direction d){
    ssize_t n = p->getChildrenCount();
    if(d == ScrollView::Direction::HORIZONTAL){
        e->setPosition(Vec2(
                n * (e->getContentSize().width + margin) //+ margin
                , (p->getContentSize().height / 2) - (e->getContentSize().height / 2)
                       )
        );
        p->setInnerContainerSize( Size((n+1) * (e->getContentSize().width + margin) /*+ margin*/, e->getContentSize().height + margin) );
    }
    p->addChild(e);
}

ScrollView * gui::addScrollView(Vec2 p1, Vec2 p2, Size size, Size margin, const string bgImg ){

    ScrollView * sv = ScrollView::create();
    //sv->setBackGroundColor(Color3B::GRAY);
    //sv->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    if(bgImg.compare("") != 0)
        sv->setBackGroundImage(bgImg);

    float svX1, svY1, svX2, svY2;
    getPoint(p1.x, p1.y, svX1, svY1, ALIGNMENT_NONE, size, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);
    getPoint(p2.x, p2.y, svX2, svY2, ALIGNMENT_NONE, size, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);

    sv->setPosition(Vec2(std::min(svX1, svX2), std::min(svY1, svY2)));


    sv->setContentSize(Size(svX2 - svX1, std::max(svY1, svY2) - std::min(svY1, svY2)));

    ScrollView::Direction d = ScrollView::Direction::BOTH;

    float gapX = std::max(svX1, svX2) - std::min(svX1, svX2);
    float gapY = std::max(svY1, svY2) - std::min(svY1, svY2);

    if(gapX < gapY)
        d = ScrollView::Direction::VERTICAL;
    else if(gapX > gapY)
        d = ScrollView::Direction::HORIZONTAL;

    sv->setDirection(d);
    return sv;
}

LoadingBar * gui::addProgressBar(int x, int y, const string img, Node * p, float defaultVal){
    Vec2 point;
    getPoint(x, y, point, ALIGNMENT_CENTER);
    LoadingBar * loadingBar = LoadingBar::create(img);
    loadingBar->setDirection(LoadingBar::Direction::LEFT);
    loadingBar->setPosition(point);
    loadingBar->setPercent(defaultVal);
    p->addChild(loadingBar);

    return loadingBar;
}