//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"

#include "ActionScene.h"

using namespace cocos2d::ui;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    mGrid.init("fonts/Goyang.ttf", 14);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            CC_CALLBACK_1(MainScene::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /*

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");
    if (sprite == nullptr)
    {
        problemLoading("'HelloWorld.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
    */

    //Cat Main UI
    gui::inst()->drawGrid(this);
    //mGrid.drawPoint(this);
    //gui::inst()->addLabel(0,0,"대꼬 lv.18", this, 12, ALIGNMENT_NONE);
    gui::inst()->addLabel(0,0,"대꼬 lv.18", this, 12, ALIGNMENT_NONE);


    //mGrid.addLabel(0, 0, "abc", this, 12, ALIGNMENT_NONE);
    gui::inst()->addLabel(0,1,"[견습] 길거리의 낭만 고양이", this, 12, ALIGNMENT_NONE, Color3B::BLACK
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , "HelloWorld.png"
            , false);

    //mGrid.addTextButton(0,0,"대꼬 lv.18 \n[견습] 길거리의 낭만 고양이", this, CC_CALLBACK_1(HelloWorld::callback2, this, 0), 12, ALIGNMENT_NONE);

    gui::inst()->addTextButton(0,6,"╈", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACTION), 32);
    gui::inst()->addTextButton(1,6,"┲", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE), 32);
    gui::inst()->addTextButton(2,6,"╁", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_FARMING), 32);

    //mGrid.addTextButton(4,0,"EXP 1,024 / 2,048", this, CC_CALLBACK_1(HelloWorld::callback2, this, 5), 12, ALIGNMENT_CENTER, Color3B::GRAY);
    gui::inst()->addLabel(4,0,"EXP 1,024 / 2,048", this, 12, ALIGNMENT_CENTER, Color3B::GRAY
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , "banner.jpg"
            , true
    );

    gui::inst()->addTextButton(7,0,"$ 172,820 +", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_PURCHASE), 10, ALIGNMENT_CENTER, Color3B::GREEN);
    gui::inst()->addTextButton(8,0,"♥ 80/117 +", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RECHARGE), 10, ALIGNMENT_CENTER, Color3B::ORANGE);

    gui::inst()->addLabel(8,2,"체력: 10,234\n지력: 5,000\n매력: 82,340", this, 10);

    gui::inst()->addTextButton(0,3,"도감", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_COLLECTION), 14);
    gui::inst()->addTextButton(0,4,"업적", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACHIEVEMENT), 14);

    gui::inst()->addTextButton(6,6,"벼룩시장", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_SELL), 14);
    gui::inst()->addTextButton(7,6,"상점", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_BUY), 14);
    gui::inst()->addTextButton(8,6,"가방", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_INVENTORY), 14);


    gui::inst()->addTextButton(4,3,"╂", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE), 150);

    return true;
}


void MainScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void MainScene::callback2(cocos2d::Ref* pSender, SCENECODE type){
    CCLOG("Callback !!!!!!!! %d", type);

    auto size = Size(400,200);
    auto margin = Size(15, 15);
    Size nodeSize = Size(100, 100);
    Size gridSize = Size(1, 6);

    if(type == SCENECODE_ACTION){
        auto pScene = ActionScene::createScene();
        //Director::getInstance()->replaceScene(pScene);
        Director::getInstance()->pushScene(pScene);
    } else if(type == SCENECODE_RACE){
        this->removeChild(layerGray);
        //this->removeChild(layer);

    } else if(type == SCENECODE_INVENTORY){
        this->removeChild(layerGray);
        layer = gui::inst()->addPopup(layerGray, this, size, "bg.png", Color4B::WHITE);
        gui::inst()->addTextButton(8,0
                ,"Close"
                , layer
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE)
                , 14
                , ALIGNMENT_CENTER
                , Color3B::RED
                , size
                , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                , Size::ZERO
                , margin
        );

        //grid line
        //gui::inst()->drawGrid(layer, size, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);

        //tab
        gui::inst()->addTextButton(0,1
                ,"성장"
                , layer
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_1)
                , 14
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , size
                , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                , Size::ZERO
                , margin
        );

        gui::inst()->addTextButton(1,1
                ,"경묘"
                , layer
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_2)
                , 14
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , size
                , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                , Size::ZERO
                , margin
        );
        gui::inst()->addTextButton(2,1
                ,"농사"
                , layer
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_3)
                , 14
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , size
                , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                , Size::ZERO
                , margin
        );

        gui::inst()->addTextButton(3,1
                ,"체력"
                , layer
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_4)
                , 14
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , size
                , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                , Size::ZERO
                , margin
        );

        gui::inst()->addTextButton(4,1
                ,"꾸밈"
                , layer
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_5)
                , 14
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , size
                , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                , Size::ZERO
                , margin
        );

        gui::inst()->addTextButton(5,1
                ,"전체"
                , layer
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_INVENTORY)
                , 14
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , size
                , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                , Size::ZERO
                , margin
        );

        ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 2), size, margin);

        for(int n=0; n < 10; n++){

            Layout* l = gui::inst()->createLayout(nodeSize, "element.png", true, Color3B::WHITE);

            gui::inst()->addLabel(0,1
                    , "`"
                    , l
                    , 24
                    , ALIGNMENT_CENTER
                    , Color3B::BLACK
                    , nodeSize
                    , gridSize
                    , Size::ZERO
                    , Size::ZERO
            );

            gui::inst()->addTextButton(0,2
                    , "[초급] 정어리 낚시"
                    , l
                    , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                    , 12
                    , ALIGNMENT_CENTER
                    , Color3B::BLACK
                    , nodeSize
                    , gridSize
                    , Size::ZERO
                    , Size::ZERO
            );

            gui::inst()->addTextButton(0,3
                    , "$10 S0 I0 A0"
                    , l
                    , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                    , 12
                    , ALIGNMENT_CENTER
                    , Color3B::GRAY
                    , nodeSize
                    , gridSize
                    , Size::ZERO
                    , Size::ZERO
            );

            gui::inst()->addTextButton(0,4
                    , "$0 S10 I12 A0"
                    , l
                    , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                    , 12
                    , ALIGNMENT_CENTER
                    , Color3B::BLUE
                    , nodeSize
                    , gridSize
                    , Size::ZERO
                    , Size::ZERO
            );

            gui::inst()->addLayoutToScrollView(sv, l, 10);
        }

        layer->addChild(sv, 1, 123);

        /*
        gui::inst()->addLabel(4,5
                ,"┲"
                , layer
                , 50
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , size
                , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
                , Size(0,0)
                , Size(0,0)
        );
         */
    } else if(type == SCENECODE_POPUP_1) {
        ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 2), size, margin);

        for(int n=0; n < 10; n++){

            Layout* l = gui::inst()->createLayout(nodeSize, "element.png", true, Color3B::WHITE);

            gui::inst()->addLabel(0,1
                    , "╈"
                    , l
                    , 20
                    , ALIGNMENT_CENTER
                    , Color3B::BLACK
                    , nodeSize
                    , gridSize
                    , Size::ZERO
                    , Size::ZERO
            );

            gui::inst()->addTextButton(0,2
                    , "[초급] 정어리 낚시"
                    , l
                    , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                    , 12
                    , ALIGNMENT_CENTER
                    , Color3B::BLACK
                    , nodeSize
                    , gridSize
                    , Size::ZERO
                    , Size::ZERO
            );

            gui::inst()->addTextButton(0,3
                    , "$10 S0 I0 A0"
                    , l
                    , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                    , 12
                    , ALIGNMENT_CENTER
                    , Color3B::GRAY
                    , nodeSize
                    , gridSize
                    , Size::ZERO
                    , Size::ZERO
            );

            gui::inst()->addTextButton(0,4
                    , "$0 S10 I12 A0"
                    , l
                    , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                    , 12
                    , ALIGNMENT_CENTER
                    , Color3B::BLUE
                    , nodeSize
                    , gridSize
                    , Size::ZERO
                    , Size::ZERO
            );

            gui::inst()->addLayoutToScrollView(sv, l, 10);
        }
        layer->removeChildByTag(123, true);
        layer->addChild(sv);
    }
};


void MainScene::onEnter(){
    Scene::onEnter();
    CCLOG("onEnter!!!!!!!!!!!!!!!!!!!!!!!");
}
void MainScene::onEnterTransitionDidFinish(){
    Scene::onEnterTransitionDidFinish();
    CCLOG("onEnterTransitionDidFinish!!!!!!!!!!!!!!!!!!!!!!!");
}
void MainScene::onExitTransitionDidStart(){
    Scene::onExitTransitionDidStart();
    CCLOG("onExitTransitionDidStart\n");
}
void MainScene::onExit(){
    Scene::onExit();
    CCLOG("onExit\n");
}
