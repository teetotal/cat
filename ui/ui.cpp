//
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
        p->addChild(draw, 1000);
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
        p->addChild(draw, 1000);
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
Label * gui::addLabel(int x
        , int y
        , const string &text
        , Scene *p
        , int fontSize
        , ALIGNMENT align
        , const Color3B color
        , const string img
        , bool isBGImg
){
 return  addLabel(p, x, y, text, fontSize, align, color
         , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
         , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
         , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
         , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
         , img
         , isBGImg
    );
}

Label * gui::addLabelAutoDimension(int x
        , int y
        , const string text
        , Node *p
        , int fontSize
        , ALIGNMENT align
        , const Color3B color
        , Size grid
        , Size origin
        , Size margin
        , const string img
        , bool isBGImg
){
    return  addLabel(p, x, y, text, fontSize, align, color
            , p->getContentSize()
            , grid
            , origin
            , margin
            , img
            , isBGImg
    );
}

Label * gui::createLabel(int x, int y, const string text,int fontSize, ALIGNMENT align, const Color3B color
                         , Size dimension
                         , Size grid
                         , Size origin
                         , Size margin
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
    switch (align) {
        case ALIGNMENT_NONE:
            pX = pointX_NONE;
            label->setAnchorPoint(Vec2(0.f, 0.5f));
            break;
        case ALIGNMENT_CENTER:
        default:
            pX = pointX;
            break;
    }
    label->setPosition(Point(pX, pointY));
    
    return label;
}

Label * gui::addLabel(Node *p, int x, int y, const string text, int fontSize, ALIGNMENT align, const Color3B color
        , Size dimension
        , Size grid
        , Size origin
        , Size margin
        , const string img
        , bool isBGImg
){

    Label * label = createLabel(x, y, text, fontSize, align, color
                                , dimension
                                , grid
                                , origin
                                , margin
                                );
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
    }

    p->addChild(label);

    return label;
}
MenuItemLabel * gui::addTextButtonAutoDimension(int x, int y, const string text, Node *p, const ccMenuCallback &callback
        , int fontSize, ALIGNMENT align, const Color3B color
        , Size grid
        , Size origin
        , Size margin
        , const string img
        , bool isBGImg
){
    return addTextButton(x, y, text, p, callback, fontSize, align, color, p->getContentSize(), grid, origin, margin, img, isBGImg);
}
MenuItemLabel * gui::addTextButtonRaw(Menu* &pMenu, int x, int y, const string text, Node *p, const ccMenuCallback &callback
        , int fontSize, ALIGNMENT align, const Color3B color
        , Size dimension
        , Size grid
        , Size origin
        , Size margin
        , const string img
        , bool isBGImg
) {

    //if(fontSize > 0) MenuItemFont::setFontSize(fontSize);

    //MenuItemLabel::create(<#Node *label#>, <#const ccMenuCallback &callback#>) 요걸로 교체
    /*
    auto pItem = MenuItemFont::create(text, callback);
    pItem->setColor(color);	
    
     */
    if(fontSize == 0)
        fontSize = mDefaultFontSize;
    
    auto label = Label::createWithTTF(text, mDefaultFont, fontSize);
    label->setColor(color);
    auto pItem = MenuItemLabel::create(label, callback);
    
    pMenu = Menu::create(pItem, NULL);
    
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
		switch (align) {
		case ALIGNMENT_NONE:
			pX = pointX_NONE;
			pItem->setAnchorPoint(Vec2(0.f, 0.5f));
			break;
		case ALIGNMENT_CENTER:
			pX = pointX;
		}
        //pX = (align == ALIGNMENT_NONE) ? pointX_NONE + (pItem->getContentSize().width / 2) : pointX;

        pMenu->setPosition(Point(pX, pointY));
    }

    p->addChild(pMenu);

    //if(fontSize > 0) MenuItemFont::setFontSize(mDefaultFontSize);

    return pItem;
}


LayerColor *
gui::createModalLayer(LayerColor * &layerBG, Size size, const string bgImg, Color4B bgColor) {
	mModalTouchCnt = 0;
    //layerBG = LayerColor::create(Color4B::BLACK);
    layerBG = new (std::nothrow) LayerColor();
    layerBG->initWithColor(Color4B::BLACK);

    layerBG->setContentSize(Size(mVisibleX, mVisibleY));

    layerBG->setPosition(Vec2(mOriginX, mOriginY));
    layerBG->setOpacity(DEFAULT_OPACITY);

    //LayerColor * layer = cocos2d::LayerColor::create(bgColor);
    LayerColor * layer = new (std::nothrow) LayerColor();
    layer->initWithColor(bgColor);

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
    listener->onTouchBegan = [this](Touch *touch,Event*event)->bool {
        CCLOG("x %f, y: %f"
        , touch->getLocation().x
        , touch->getLocation().y
        );
		mModalTouchCnt++;
        return true;
    };

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listener, layerBG);

    return layer;
}

LayerColor * gui::addPopup(LayerColor * &layerBG, Node * p, Size size, const string bgImg, Color4B bgColor){

    Vec2 start = getPointVec2(0, 6);
    Vec2 end = getPointVec2(8, 0);
    Size sizeLayer = Size(end.x - start.x, end.y - start.y);
    LayerColor * layer = createModalLayer(layerBG, sizeLayer, bgImg, bgColor);
    p->addChild(layerBG);
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

void gui::addLayoutToScrollView(ScrollView * p, Layout * e, float margin, int newlineInterval){
    ssize_t nCount = p->getChildrenCount();
    float x, y;

    if(newlineInterval == 0) {
        x = nCount * (e->getContentSize().width + margin)  + (margin / 2);
		y = p->getInnerContainerSize().height - e->getContentSize().height; // +margin;
    } else {
        x = (nCount % newlineInterval) * (e->getContentSize().width + margin) + (margin / 2);
        y = (p->getInnerContainerSize().height) - ((nCount / newlineInterval) + 1) * (e->getContentSize().height + margin);

    }
    e->setPosition(Vec2(x, y));
    p->addChild(e);
}

Size gui::getScrollViewSize(Vec2 p1, Vec2 p2, Size size, Size grid, Size origin, Size margin) {
//    if (size.width <= 0) size = Director::getInstance()->getVisibleSize();

	float svX1, svY1, svX2, svY2;
	getPoint(p1.x, p1.y, svX1, svY1, ALIGNMENT_NONE, size, grid, origin, margin);
	getPoint(p2.x, p2.y, svX2, svY2, ALIGNMENT_NONE, size, grid, origin, margin);

	return Size(svX2 - svX1, std::max(svY1, svY2) - std::min(svY1, svY2));
}

ScrollView * gui::addScrollView(Vec2 p1, Vec2 p2, Size size, Size grid, Size origin, Size margin, const string bgImg, Size innerSize, Node * parent){
	
//    if (size.width <= 0) size = Director::getInstance()->getVisibleSize();

    ScrollView * sv = ScrollView::create();
//    sv->setBackGroundColor(Color3B::GRAY);
//    sv->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    if(bgImg.compare("") != 0)
        sv->setBackGroundImage(bgImg);

    float svX1, svY1, svX2, svY2;
    getPoint(p1.x, p1.y, svX1, svY1, ALIGNMENT_NONE, size, grid, origin, margin);
    getPoint(p2.x, p2.y, svX2, svY2, ALIGNMENT_NONE, size, grid, origin, margin);

    sv->setPosition(Vec2(std::min(svX1, svX2), std::min(svY1, svY2)));
    sv->setContentSize(Size(svX2 - svX1, std::max(svY1, svY2) - std::min(svY1, svY2)));
    
    if(innerSize.width > 0 && innerSize.height > 0)
        sv->setInnerContainerSize(innerSize);

    ScrollView::Direction d = ScrollView::Direction::NONE;

    if(sv->getContentSize().width < sv->getInnerContainerSize().width
       && sv->getContentSize().height < sv->getInnerContainerSize().height)
        d = ScrollView::Direction::BOTH;
    else if(sv->getContentSize().width < sv->getInnerContainerSize().width)
        d = ScrollView::Direction::HORIZONTAL;
    else if(sv->getContentSize().height < sv->getInnerContainerSize().height)
        d = ScrollView::Direction::VERTICAL;
    /*
    float gapX = std::max(svX1, svX2) - std::min(svX1, svX2);
    float gapY = std::max(svY1, svY2) - std::min(svY1, svY2);

    if(gapX < gapY)
        d = ScrollView::Direction::VERTICAL;
    else if(gapX > gapY)
        d = ScrollView::Direction::HORIZONTAL;

     */
    sv->setDirection(d);
	if (parent != NULL)
		parent->addChild(sv);

    return sv;
}

LoadingBar * gui::addProgressBar(int x, int y, const string img, Node * p, float width, float defaultVal, Size dimension, Size grid, Size origin, Size margin
	, LoadingBar::Direction direction)
{
	
    Vec2 point;
    getPoint(x, y, point, ALIGNMENT_CENTER, dimension, grid, origin, margin);
    LoadingBar * loadingBar = LoadingBar::create(img);
    loadingBar->setDirection(direction);
    loadingBar->setPosition(point);
    loadingBar->setPercent(defaultVal);
    setScale(loadingBar, width);
    
	//progress backgroung image
	auto bgProgress = Sprite::create(img);
	bgProgress->setOpacity(50);
	bgProgress->setPosition(point);
    setScale(bgProgress, width);
	
	p->addChild(bgProgress);
	p->addChild(loadingBar);

    return loadingBar;
}

Sprite *
gui::addSpriteAutoDimension(int x, int y, const string img, Node *p, ALIGNMENT align, Size grid,
                            Size origin, Size margin) {
    return addSprite(x, y, img, p, align, p->getContentSize(), grid, origin, margin);
}

Sprite *
gui::addSpriteFixedSize(const Size &spriteSize, int x, int y, const string img, Node *p, ALIGNMENT align, Size dimension, Size grid, Size origin,
               Size margin){
    Sprite * sprite = addSprite(x, y, img, p, align, dimension, grid, origin, margin);
    sprite->setContentSize(spriteSize);
    return sprite;
}
Sprite *
gui::addSprite(int x, int y, const string img, Node *p, ALIGNMENT align, Size dimension, Size grid, Size origin,
               Size margin) {

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

    Sprite * sprite = Sprite::create(img);
    sprite->setAnchorPoint(Vec2(0.5, 0.5));

    float pX;

    pX = (align == ALIGNMENT_NONE) ? pointX_NONE + (sprite->getContentSize().width / 2) :
         pointX;

    sprite->setPosition(Point(pX, pointY));

    p->addChild(sprite);
    return sprite;
}

void gui::addBGScrolling(const string img, Node * p, float duration){
    auto sprite1 = Sprite::create(img);
    auto sprite2 = Sprite::create(img);
    auto sprite3 = Sprite::create(img); // 카메라 전환시 빈공간 메꿔주기 위해 추가

    Vec2 point = Vec2(mVisibleX / 2 + mOriginX, mVisibleY / 2 + mOriginY);
    sprite1->setContentSize(Size(mVisibleX, mVisibleY));
    sprite1->setAnchorPoint(Vec2(0.5, 0.5));
    sprite1->setPosition(point);

    Vec2 point2 = Vec2(point.x + sprite1->getContentSize().width , point.y);
    sprite2->setContentSize(Size(mVisibleX, mVisibleY));
    sprite2->setAnchorPoint(Vec2(0.5, 0.5));
    sprite2->setPosition(point2);


    Vec2 point3 = Vec2(point.x + sprite1->getContentSize().width *2, point.y);
    sprite3->setContentSize(Size(mVisibleX, mVisibleY));
    sprite3->setAnchorPoint(Vec2(0.5, 0.5));
    sprite3->setPosition(point3);


    Vec2 finishPoint = Vec2(point.x * -1, point.y);
	if (duration > 0) {
		auto seq1 = RepeatForever::create(Sequence::create(
			MoveTo::create(duration, finishPoint)
			, Place::create(point)
			, NULL
		));

		auto seq2 = RepeatForever::create(Sequence::create(
			MoveTo::create(duration, point)
			, Place::create(point2)
			, NULL
		));

		auto seq3 = RepeatForever::create(Sequence::create(
			MoveTo::create(duration, point2)
			, Place::create(point3)
			, NULL
		));

		sprite1->runAction(seq1);
		sprite2->runAction(seq2);
		sprite3->runAction(seq3);
	}
    p->addChild(sprite1);
    p->addChild(sprite2);
    p->addChild(sprite3);
}

void gui::addQuantityLayer(Node * p, Size size, Size margin
	, Sprite* &sprite, Label * &label, Label * &labelQuantity, Label* &labelPrice
	, const string szSubmit
	, const ccMenuCallback& callbackDec
	, const ccMenuCallback& callbackInc
	, const ccMenuCallback& callbackSubmit
) {
	//quantity
	Vec2 s, e;
	gui::inst()->getPoint(7, 7, s, ALIGNMENT_NONE, size, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);
	gui::inst()->getPoint(9, 1, e, ALIGNMENT_NONE, size, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);

	Size sizeQunatity = Size(e.x - s.x, e.y - s.y);
	auto layerQunatity = gui::inst()->createLayout(sizeQunatity, "", true);
	layerQunatity->setOpacity(192);

	Size gridQuantity = Size(5, 6);
	sprite = gui::inst()->addSpriteAutoDimension(2, 1, "blank.png", layerQunatity, ALIGNMENT_CENTER, gridQuantity, Size::ZERO, Size::ZERO);
	sprite->setContentSize(Size(20, 20));

	label = gui::inst()->addLabelAutoDimension(1, 2, " ", layerQunatity, 10, ALIGNMENT_NONE, Color3B::BLACK, gridQuantity, Size::ZERO, Size::ZERO);
	gui::inst()->addTextButtonAutoDimension(1, 3, "-", layerQunatity, callbackDec, 20, ALIGNMENT_CENTER, Color3B::BLACK, gridQuantity, Size::ZERO, Size::ZERO);
	labelQuantity = gui::inst()->addLabelAutoDimension(2, 3, "  ", layerQunatity, 14, ALIGNMENT_CENTER, Color3B::BLACK, gridQuantity, Size::ZERO, Size::ZERO);
	gui::inst()->addTextButtonAutoDimension(3, 3, "+", layerQunatity, callbackInc, 20, ALIGNMENT_CENTER, Color3B::BLACK, gridQuantity, Size::ZERO, Size::ZERO);
	labelPrice = gui::inst()->addLabelAutoDimension(2, 4, "           ", layerQunatity, 14, ALIGNMENT_CENTER, Color3B::BLACK, gridQuantity, Size::ZERO, Size::ZERO);
	gui::inst()->addTextButtonAutoDimension(2, 5, szSubmit, layerQunatity, callbackSubmit, 14, ALIGNMENT_CENTER, Color3B::BLUE, gridQuantity, Size::ZERO, Size::ZERO);

	layerQunatity->setAnchorPoint(Vec2(0, 0));
	layerQunatity->setPosition(s);

	p->addChild(layerQunatity);
}

MenuItemImage * gui::addSpriteButtonRaw(
	Menu* &pMenu
	, int x
	, int y
	, const string normalImg
	, const string selectImg
	, Node *p
	, const ccMenuCallback &callback
	, ALIGNMENT align
	, Size dimension
	, Size grid
	, Size origin
	, Size margin
) {
	auto pItem = MenuItemImage::create(normalImg, selectImg, callback);
	pMenu = Menu::create(pItem, NULL);

	float pointX, pointY;
	float pointX_NONE, pointY_NONE;
	getPoint(x, y
		, pointX, pointY
		, pointX_NONE, pointY_NONE
		, ALIGNMENT_CENTER
		, dimension
		, grid
		, origin
		, margin
	);

	float pX;
	switch (align) {
	case ALIGNMENT_NONE:
		pX = pointX_NONE;
		pItem->setAnchorPoint(Vec2(0.f, 0.5f));
		break;
	case ALIGNMENT_CENTER:
		pX = pointX;
	}
	//pX = (align == ALIGNMENT_NONE) ? pointX_NONE + (pItem->getContentSize().width / 2) : pointX;

	pMenu->setPosition(Point(pX, pointY));
	p->addChild(pMenu);
	
	return pItem;
}
