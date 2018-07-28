//
// Created by daejung on 2018-07-24.
//

#include "ui.h"

gui * gui::hInstance = NULL;

void gui::init(const char* font, int fontSize, Color4F bgColor) {

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

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

    float dimensionX = dimension.width == GRID_INVALID_VALUE ? mWidth : dimension.width;
    float dimensionY = dimension.height == GRID_INVALID_VALUE ? mHeight : dimension.height;

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
){
    if(fontSize == 0)
        fontSize = mDefaultFontSize;

    float pointX, pointY;
    getPoint(x,y
            , pointX, pointY
            , align
            , dimension
            , grid
            , origin
            , margin
    );
    auto label = Label::createWithTTF(text, mDefaultFont, fontSize);
    label->setPosition(Point(pointX, pointY));
    label->setColor(color);
    if(align == ALIGNMENT_NONE)
        label->setAnchorPoint(getNoneAnchorPoint());
    p->addChild(label);
    return label;
}

MenuItem * gui::addTextButton(int x, int y, const char * text, Node *p, const ccMenuCallback &callback,
                              int fontSize, ALIGNMENT align, const Color3B color
        , Size dimension
        , Size grid
        , Size origin
        , Size margin
) {

    if(fontSize > 0)
        MenuItemFont::setFontSize(fontSize);

    auto pItem = MenuItemFont::create(text, callback);
    pItem->setColor(color);
    auto pMenu = Menu::create(pItem, NULL);
    float pointX, pointY;
    getPoint(x,y
            , pointX, pointY
            , align
            , dimension
            , grid
            , origin
            , margin);
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
        return true;
    };

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listener, layerBG);

    return layer;
}