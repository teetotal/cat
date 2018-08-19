//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "ActionScene.h"

using namespace cocos2d::ui;

#define INVENTORY_SIZE 	auto size = Size(400, 200); auto margin = Size(10, 10); auto nodeSize = Size(80, 50); auto gridSize = Size(1, 3);
#define BUY_SIZE 	auto size = Size(400, 200); auto margin = Size(10, 10); auto nodeSize = Size(150, 50); auto gridSize = Size(1, 3);


MainScene * MainScene::hInst = NULL;
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
    /*
    ssize_t pSize = 0;
    unsigned char* fileSrc = FileUtils::getInstance()->getFileData("res/meta.json", "r", &pSize);

    delete[] fileSrc;
    */

    hInst = this;
    mParitclePopup = NULL;
	if (!logics::hInst->init(MainScene::farmingCB, MainScene::tradeCB))
		return false;

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
    //gui::inst()->drawGrid(this);

    loadingBar = gui::inst()->addProgressBar(4, 0, "LoadingBarFile.png", this, 10);
    //loadingBar->setPercent(logics::hInst->getExpRatio());

    gui::inst()->addSpriteFixedSize(Size(110, 140), 4, 3, "25.png", this);


    string name = wstring_to_utf8(logics::hInst->getActor()->userName, true);
    name += " lv.";
    name += to_string(logics::hInst->getActor()->level);

    mName = gui::inst()->addLabel(0, 0, name, this, 12, ALIGNMENT_NONE);

    //mGrid.addLabel(0, 0, "abc", this, 12, ALIGNMENT_NONE);
    mJobTitle = gui::inst()->addLabel(4,1,wstring_to_utf8(logics::hInst->getActor()->jobTitle, true), this, 12);

    gui::inst()->addTextButton(0,6,"╈", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACTION), 32);
    gui::inst()->addTextButton(1,6,"┲", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE), 32);
    gui::inst()->addTextButton(2,6,"╁", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_FARMING), 32);

    /*
    string szExp;
    szExp += to_string(logics::hInst->getActor()->exp);
    szExp += " / ";
    szExp += to_string(logics::hInst->getMaxExp());
    mExp = gui::inst()->addLabel(4,0,szExp, this, 12, ALIGNMENT_CENTER);
    */
    mExp = gui::inst()->addLabel(4, 0, "", this, 12, ALIGNMENT_CENTER);

    /*
    string szPoint = "$ ";
    szPoint += to_string(logics::hInst->getActor()->point);
    szPoint += " +";

    mPoint = gui::inst()->addTextButton(7,0,szPoint, this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_PURCHASE), 10, ALIGNMENT_CENTER, Color3B::GREEN);
    */
    mPoint = gui::inst()->addTextButton(7, 0, "$", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_PURCHASE), 10, ALIGNMENT_CENTER, Color3B::GREEN);

    /*
    int hp = logics::hInst->getHP();
    int hpMax = logics::hInst->getMaxHP();

    string szHP = "♥ ";
    szHP += to_string(hp);
    szHP += "/";
    szHP += to_string(hpMax);
    szHP += " +";

    mHP = gui::inst()->addTextButton(8, 0, szHP , this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RECHARGE), 10, ALIGNMENT_CENTER, Color3B::ORANGE);
    */
    mHP = gui::inst()->addTextButton(8, 0, "♥", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RECHARGE), 10, ALIGNMENT_CENTER, Color3B::ORANGE);

    /*
    string properties;

    properties += "S: ";
    properties += to_string(logics::hInst->getActor()->property.strength);


    properties += "\nI: ";
    properties += to_string(logics::hInst->getActor()->property.intelligence);

    properties += "\nA: ";
    properties += to_string(logics::hInst->getActor()->property.appeal);

    mProperties = gui::inst()->addLabel(8, 2, properties, this, 10);
     */
    mProperties = gui::inst()->addLabel(8, 2, "", this, 10);

    gui::inst()->addTextButton(0,3,"도감", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_COLLECTION), 14, ALIGNMENT_NONE, Color3B::GRAY
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , Size(GRID_INVALID_VALUE,GRID_INVALID_VALUE)
            , "check.png"
            , false);
    gui::inst()->addTextButton(0,4,"업적", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACHIEVEMENT), 14);

    gui::inst()->addTextButton(6,6,"벼룩시장", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_SELL), 14);
    gui::inst()->addTextButton(7,6,"상점", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_BUY), 14);
	mInventory = gui::inst()->addTextButton(8,6,"가방", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_INVENTORY), 14);

    //gacha
    mParitclePopup = mGacha.createLayer(mParitclePopupLayer
            , this
            , "crystal_marvel.png"
            , "particles/particle_magic.plist"
            , "particles/particle_finally.plist");

    updateState(false);
	this->schedule(schedule_selector(MainScene::scheduleRecharge), 1); //HP recharge schedule

  	//cultivation test
    /*
    c1.addLevel("plant1.png", 10, "새싹");
    c1.addLevel("plant2.png", 90, "");
    c1.addLevel("plant3.png", 100, "다 큰 묘목");
    c1.init(123, cultivationCB, 10, "plant_decay.png", "timeBar.png", Size(25, 35), this, Vec2(3,3), false);

    c2.addLevel("plant1.png", 10, "새싹");
    c2.addLevel("plant2.png", 90, "");
    c2.addLevel("plant3.png", 100, "다 큰 묘목");
    c2.init(124, cultivationCB, 10, "plant_decay.png", "timeBar.png", Size(25, 35), this, Vec2(3,4), false);

    this->schedule(schedule_selector(MainScene::scheduleCB), .5f);
    */
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

void MainScene::alertCloseCallback(Ref* pSender){
    this->removeChild(mAlertLayer);
}

void MainScene::alert(const string msg){
    int fontSize = 12;
    mAlertLayer = gui::inst()->createLayout(Size(480, 100), "", true, Color3B::GRAY);
    Vec2 point;
    gui::inst()->getPoint(4, 3, point, ALIGNMENT_CENTER);
    mAlertLayer->setPosition(point);
    mAlertLayer->setAnchorPoint(Vec2(0.5, 0.5));
    gui::inst()->addLabelAutoDimension(0,1, msg, mAlertLayer, fontSize, ALIGNMENT_CENTER, Color3B::RED, Size(1,4), Size::ZERO, Size::ZERO);
    gui::inst()->addTextButtonAutoDimension(0, 2, "CLOSE", mAlertLayer
            , CC_CALLBACK_1(MainScene::alertCloseCallback, this)
            , fontSize
            , ALIGNMENT_CENTER
            , Color3B::BLACK
            , Size(1,4)
            , Size::ZERO
            , Size::ZERO
    );
    this->addChild(mAlertLayer);
}

void MainScene::updateState(bool isInventoryUpdated) {

    float raiseDuration = 0.3;
    float returnDuration = 0.1;
    float scale = 1.5f;

    string name = wstring_to_utf8(logics::hInst->getActor()->userName, true);
    name += " lv.";
    name += to_string(logics::hInst->getActor()->level);
    if(name.compare(mName->getString()) != 0 ){
        mName->runAction(Sequence::create(
                ScaleBy::create(raiseDuration, scale), ScaleTo::create(returnDuration, 1), NULL
        ));
    }
    mName->setString(name);

    mJobTitle->setString(wstring_to_utf8(logics::hInst->getActor()->jobTitle, true));

    string szExp;
    szExp += to_string(logics::hInst->getActor()->exp);
    szExp += " / ";
    szExp += to_string(logics::hInst->getMaxExp());
    if(szExp.compare(mExp->getString()) != 0 ){
        mExp->runAction(Sequence::create(
                ScaleBy::create(raiseDuration, scale), ScaleTo::create(returnDuration, 1), NULL
        ));
    }
    mExp->setString(szExp);
    loadingBar->setPercent(logics::hInst->getExpRatio());

    string properties;
    properties += "S: ";
    properties += to_string(logics::hInst->getActor()->property.strength);
    properties += "\nI: ";
    properties += to_string(logics::hInst->getActor()->property.intelligence);
    properties += "\nA: ";
    properties += to_string(logics::hInst->getActor()->property.appeal);
    if(properties.compare(mProperties->getString()) != 0 ){
        mProperties->runAction(Sequence::create(
                ScaleBy::create(raiseDuration, scale), ScaleTo::create(returnDuration, 1), NULL
        ));
    }
    mProperties->setString(properties);

    int hp = logics::hInst->getHP();
    int hpMax = logics::hInst->getMaxHP();

    string szHP = "♥ ";
    szHP += to_string(hp);
    szHP += "/";
    szHP += to_string(hpMax);
    szHP += " +";
    if(szHP.compare(mHP->getString()) != 0 ){
        mHP->runAction(Sequence::create(
                ScaleBy::create(raiseDuration, scale), ScaleTo::create(returnDuration, 1), NULL
        ));
    }
    mHP->setString(szHP);

    string szPoint = "$ ";
    szPoint += to_string(logics::hInst->getActor()->point);
    szPoint += " +";
    if(szPoint.compare(mPoint->getString()) != 0 ){
        mPoint->runAction(Sequence::create(
                ScaleBy::create(raiseDuration, scale), ScaleTo::create(returnDuration, 1), NULL
        ));
    }
    mPoint->setString(szPoint);

	if(isInventoryUpdated)
		mInventory->runAction(Sequence::create(
			ScaleBy::create(raiseDuration, scale), ScaleTo::create(returnDuration, 1), NULL
		));
}

void MainScene::callbackAction(Ref* pSender, int id){
    
    this->removeChild(layerGray);
    vector<_itemPair> rewards;
    _property property;
    int point;
    trainingType type;
    errorCode err = logics::hInst->runTraining(id, rewards, &property, point, type);
    wstring sz = logics::hInst->getErrorMessage(err);

	wstring szRewards = L"";
	for (int n = 0; n < rewards.size(); n++) {
		_item item = logics::hInst->getItem(rewards[n].itemId);
		szRewards += item.name + L"-" + to_wstring(rewards[n].val) + L" ";
	}

	bool isInventory = false;
	string szRewardsUTF8 = wstring_to_utf8(szRewards, true);
	if (szRewardsUTF8.size() > 1) {
		isInventory = true;
		particleSample(szRewardsUTF8);
	}

    switch(err){
        case error_success:
        case error_levelup:
            updateState(isInventory);
            break;
        default:
            alert(wstring_to_utf8(sz, true));
            break;
    }
}


void MainScene::callback2(cocos2d::Ref* pSender, SCENECODE type){
    CCLOG("Callback !!!!!!!! %d", type);

    switch(type){
        case SCENECODE_ACTION: //Action
            actionList();
            break;
		case SCENECODE_RECHARGE: //HP 충전
			showInventory(inventoryType_HP);
			break;
        case SCENECODE_SELL:
            //particleSample();
            break;
        case SCENECODE_BUY:
			showBuy();
            break;
        case SCENECODE_FARMING:
            Director::getInstance()->pushScene(ActionScene::createScene());
            break;
        case SCENECODE_RACE:
            this->removeChild(layerGray);
            break;
        case SCENECODE_INVENTORY: //inquiry inventory
			showInventory();
			//store2();
            break;
        case SCENECODE_POPUP_1:
            store();
            break;
        case SCENECODE_POPUP_2:
            this->removeChild(mParitclePopupLayer);
            break;
        case SCENECODE_COLLECTION:
            dailyReward();
            break;
        default:
            break;
    }

}

void MainScene::callback0(){
    CCLOG("callback0");

}

void MainScene::callback1(Ref* pSender){
    CCLOG("callback1");
}

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


void MainScene::store() {

    auto size = Size(400,200);
    auto margin = Size(15, 15);
    Size nodeSize = Size(100, 100);
    Size gridSize = Size(1, 6);

    ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 2), size, margin);

    for(int n=0; n < 10; n++){

        Layout* l = gui::inst()->createLayout(nodeSize, "element.png", true, Color3B::WHITE);

        gui::inst()->addLabelAutoDimension(0,1
                , "╈"
                , l
                , 20
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , gridSize
                , Size::ZERO
                , Size::ZERO
        );

        gui::inst()->addTextButtonAutoDimension(0,2
                , "[초급] 정어리 낚시"
                , l
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                , 12
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , gridSize
                , Size::ZERO
                , Size::ZERO
        );

        gui::inst()->addTextButtonAutoDimension(0,3
                , "$10 S0 I0 A0"
                , l
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                , 12
                , ALIGNMENT_CENTER
                , Color3B::GRAY
                , gridSize
                , Size::ZERO
                , Size::ZERO
        );

        gui::inst()->addTextButtonAutoDimension(0,4
                , "$0 S10 I12 A0"
                , l
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                , 12
                , ALIGNMENT_CENTER
                , Color3B::BLUE
                , gridSize
                , Size::ZERO
                , Size::ZERO
        );

        gui::inst()->addLayoutToScrollView(sv, l, 10);
    }
    layer->removeChildByTag(123, true);
    layer->addChild(sv);
}

void MainScene::dailyReward() {
    auto size = Size(400,200);
    auto margin = Size(10, 10);
    Size nodeSize = Size(50, 50);
    Size gridSize = Size(1, 3);

    this->removeChild(layerGray);
    layer = gui::inst()->addPopup(layerGray, this, size, "Main2.png", Color4B::WHITE);
    gui::inst()->addTextButtonAutoDimension(8,0
            ,"Close"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE)
            , 10
            , ALIGNMENT_CENTER
            , Color3B::RED
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );

    ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(8, 0), size, margin, "", Size(200, 280));

    for(int n=0; n < 20; n++){

        string img = "items/";
        img += to_string(n+1);
        img += ".png";
        Layout* l = gui::inst()->createLayout(nodeSize, "", true, Color3B::GRAY);

        gui::inst()->addLabelAutoDimension(0,0
                , to_string(n+1)
                , l
                , 10
                , ALIGNMENT_NONE
                , (n < 5) ? Color3B::WHITE : Color3B::BLACK
                , gridSize
                , Size::ZERO
                , Size(2,2)
        );

        auto sprite = gui::inst()->addSpriteAutoDimension(0, 1, img, l, ALIGNMENT_CENTER, gridSize, Size::ZERO, Size::ZERO);
        sprite->setContentSize(Size(20, 20));

        string text = "10개";
        if(n < 5) {
            sprite->setOpacity(80);
            text = "수령";
        }


        gui::inst()->addLabelAutoDimension(0,2
                , text
                , l
                , 10
                , ALIGNMENT_CENTER
                , (n < 5) ? Color3B::WHITE : Color3B::BLACK
                , gridSize
                , Size::ZERO
                , Size::ZERO
        );

        gui::inst()->addLayoutToScrollView(sv, l, 5, 4);
    }

    layer->addChild(sv, 1, 123);
}

void MainScene::showInventoryCategory(Ref* pSender, inventoryType code) {
	INVENTORY_SIZE;
	int nodeMargin = 5;
	int newLine = 3;

	vector<intPair> vec;
	logics::hInst->getActor()->inven.getWarehouse(vec, (int)code);
	Size innerSize = Size((nodeSize.width + nodeMargin) * newLine, ((vec.size() / newLine) + 1) * (nodeSize.height + nodeMargin));
	ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(8, 1), size, margin, "", innerSize);

	for (int n = 0; n < (int)vec.size(); n++) {
		string img = "items/";
		img += to_string(vec[n].key % 20);
		img += ".png";
		Layout* l = gui::inst()->createLayout(nodeSize, "", true, Color3B::GRAY);
		_item item = logics::hInst->getItem(vec[n].key);
		string name = wstring_to_utf8(item.name, true);

		gui::inst()->addLabelAutoDimension(0, 0, name, l, 9, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		auto sprite = gui::inst()->addSpriteAutoDimension(0, 1, img, l, ALIGNMENT_CENTER, gridSize, Size::ZERO, Size::ZERO);
		sprite->setContentSize(Size(20, 20));

		gui::inst()->addLabelAutoDimension(0, 2, "x " + to_string(vec[n].val), l, 10, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO		);

		gui::inst()->addLayoutToScrollView(sv, l, nodeMargin, newLine);
	}
	layer->removeChildByTag(CHILD_ID_INVENTORY, true);
	layer->addChild(sv, 1, CHILD_ID_INVENTORY);
}

void MainScene::showInventory(inventoryType type) {
#define __PARAMS(STR, ID) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showInventoryCategory, this, ID), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	
	INVENTORY_SIZE

	this->removeChild(layerGray);
	layer = gui::inst()->addPopup(layerGray, this, size, "Main2.png", Color4B::WHITE);

	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE)
		, 12, ALIGNMENT_CENTER, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);
	int nMenuIdx = 0;
	//tab
	gui::inst()->addTextButtonAutoDimension(__PARAMS("ALL", inventoryType_all));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Growth", inventoryType_growth));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Race", inventoryType_race));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Farming", inventoryType_farming));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("HP", inventoryType_HP));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Adore", inventoryType_adorn));
	
	showInventoryCategory(this, type);
	
}

void MainScene::showBuyCategory(Ref* pSender, inventoryType code) {
	BUY_SIZE;
	int nodeMargin = 5;
	int newLine = 2;

	trade::tradeMap * m = logics::hInst->getTrade()->get();
	Size innerSize = Size((nodeSize.width + nodeMargin) * newLine, ((m->size() / newLine) + 1) * (nodeSize.height + nodeMargin));
	ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(8, 1), size, margin, "", innerSize);

	for (trade::tradeMap::iterator it = m->begin(); it != m->end(); ++it) {
		int id = it->first;
		_item item = logics::hInst->getItem(id);
		if (item.type > itemType_max)
			continue;		
		
		string img = "items/";
		img += to_string(id % 20);
		img += ".png";
		Layout* l = gui::inst()->createLayout(nodeSize, "", true, Color3B::GRAY);
		
		string name = wstring_to_utf8(item.name, true);

		gui::inst()->addLabelAutoDimension(0, 0, name, l, 9, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		auto sprite = gui::inst()->addSpriteAutoDimension(0, 1, img, l, ALIGNMENT_CENTER, gridSize, Size::ZERO, Size::ZERO);
		sprite->setContentSize(Size(20, 20));

		gui::inst()->addLabelAutoDimension(0, 2, "$ " + to_string(logics::hInst->getTrade()->getPriceBuy(id)), l, 10, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		gui::inst()->addLayoutToScrollView(sv, l, nodeMargin, newLine);
	}

	layer->removeChildByTag(CHILD_ID_BUY, true);
	layer->addChild(sv, 1, CHILD_ID_BUY);
}

void MainScene::showBuy(inventoryType type) {
#define __PARAMS(STR, ID) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showBuyCategory, this, ID), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin

	BUY_SIZE;
	this->removeChild(layerGray);
	layer = gui::inst()->addPopup(layerGray, this, size, "background.png", Color4B::WHITE);

	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE)
		, 12, ALIGNMENT_CENTER, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);
	int nMenuIdx = 0;
	//tab
	gui::inst()->addTextButtonAutoDimension(__PARAMS("ALL", inventoryType_all));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Growth", inventoryType_growth));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Race", inventoryType_race));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Farming", inventoryType_farming));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("HP", inventoryType_HP));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Adore", inventoryType_adorn));

	showBuyCategory(this, type);
}

void MainScene::actionList() {
    Size size = Size(300,200);
    Size margin = Size(0, 0);
    Size innerMargin = Size(0, 5);
    Size nodeSize = Size(230, 90);
    Size gridSize = Size(3, 5);
    int layerMargin = 10;

    this->removeChild(layerGray);
    layer = gui::inst()->addPopup(layerGray, this, size, "bg.png", Color4B::WHITE);
    gui::inst()->addTextButtonAutoDimension(8,0
            ,"Close"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE)
            , 14
            , ALIGNMENT_CENTER
            , Color3B::RED
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );

    gui::inst()->addLabelAutoDimension(4,0
            ,"Action"
            , layer
            , 14
            , ALIGNMENT_CENTER
            , Color3B::BLACK
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );


    //grid line
    //gui::inst()->drawGrid(layer, size, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);


    __training * pTraining = logics::hInst->getActionList();
    ScrollView * sv = gui::inst()->addScrollView(Vec2(1, 7), Vec2(9, 1), size, margin, "", Size(nodeSize.width, (nodeSize.height) * pTraining->size()));

    for (__training::iterator it = pTraining->begin(); it != pTraining->end(); ++it) {
        if (!logics::hInst->isValidTraningTime(it->first))
            continue;

        wstring rewardItems;
        wstring costItems;

        for (int n = 0; n < maxTrainingItems; n++) {
            _itemPair* p = it->second.reward.items[n];
            if (p != NULL) {
                rewardItems += logics::hInst->getItem(p->itemId).name + L"(";
                rewardItems += to_wstring(p->val) + L") ";
            }
            p = it->second.cost.items[n];
            if (p != NULL) {
                costItems += logics::hInst->getItem(p->itemId).name + L"(";
                costItems += to_wstring(p->val) + L") ";
            }
        }

		string pay = "비용-";
        if(it->second.cost.point > 0){
            pay += "$ ";
            pay += to_string(it->second.cost.point);
			pay += " ";
        }

        if(it->second.cost.strength > 0){
            pay += "S: ";
            pay += to_string(it->second.cost.strength);
			pay += " ";
        }

        if(it->second.cost.intelligence > 0){
            pay += "I: ";
            pay += to_string(it->second.cost.intelligence);
			pay += " ";
        }

        if(it->second.cost.appeal > 0){
            pay += "A: ";
            pay += to_string(it->second.cost.appeal);
			pay += " ";
        }
		pay += wstring_to_utf8(costItems);
		
		string reward = "보상-";
        if(it->second.reward.point > 0){
            reward += "$ ";
            reward += to_string(it->second.reward.point);
			reward += " ";
        }

        if(it->second.reward.strength > 0){
            reward += "S: ";
            reward += to_string(it->second.reward.strength);
			reward += " ";
        }

        if(it->second.reward.intelligence > 0){
            reward += "I: ";
            reward += to_string(it->second.reward.intelligence);
			reward += " ";
        }

        if(it->second.reward.appeal > 0){
            reward += "A: ";
            reward += to_string(it->second.reward.appeal);
			reward += " ";
        }

		reward += wstring_to_utf8(rewardItems);

        //대충 정해 놓고 나중에
        int type = it->second.type;
        Color3B bgColor = Color3B(255- type * 5, 255- type * 10, 255);

        Layout* l = gui::inst()->createLayout(nodeSize, "", true, bgColor);

        gui::inst()->addLabelAutoDimension(0,2
                , "`"
                , l
                , 32
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , gridSize
                , Size::ZERO
                , innerMargin
        );

        gui::inst()->addTextButtonAutoDimension(1,0
                , wstring_to_utf8(it->second.name)
                , l
                , CC_CALLBACK_1(MainScene::callbackAction, this, it->first)
                , 12
                , ALIGNMENT_NONE
                , Color3B::BLACK
                , gridSize
                , Size::ZERO
                , innerMargin
        );

		gui::inst()->addTextButtonAutoDimension(1,2
                , pay
                , l
                , CC_CALLBACK_1(MainScene::callbackAction, this, it->first)
                , 12
                , ALIGNMENT_NONE
                , Color3B::BLACK
                , gridSize
                , Size::ZERO
                , innerMargin
        );

        gui::inst()->addTextButtonAutoDimension(1,3
                , reward
                , l
                , CC_CALLBACK_1(MainScene::callbackAction, this, it->first)
                , 12
                , ALIGNMENT_NONE
                , Color3B::BLACK
                , gridSize
                , Size::ZERO
                , innerMargin
        );


        gui::inst()->addLayoutToScrollView(sv, l, layerMargin, 1);
    }

    layer->addChild(sv, 1, 123);
}

void MainScene::store2() {
    auto size = Size(400,200);
    auto margin = Size(15, 15);
    Size nodeSize = Size(100, 100);
    Size gridSize = Size(1, 6);

    this->removeChild(layerGray);
    layer = gui::inst()->addPopup(layerGray, this, size, "bg.png", Color4B::WHITE);
    gui::inst()->addTextButtonAutoDimension(8,0
            ,"Close"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE)
            , 14
            , ALIGNMENT_CENTER
            , Color3B::RED
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );

    //grid line
    //gui::inst()->drawGrid(layer, size, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);

    //tab
    gui::inst()->addTextButtonAutoDimension(0,1
            ,"성장"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_1)
            , 14
            , ALIGNMENT_CENTER
            , Color3B::BLACK
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );

    gui::inst()->addTextButtonAutoDimension(1,1
            ,"경묘"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_2)
            , 14
            , ALIGNMENT_CENTER
            , Color3B::BLACK
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );
    gui::inst()->addTextButtonAutoDimension(2,1
            ,"농사"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_3)
            , 14
            , ALIGNMENT_CENTER
            , Color3B::BLACK
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );

    gui::inst()->addTextButtonAutoDimension(3,1
            ,"체력"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_4)
            , 14
            , ALIGNMENT_CENTER
            , Color3B::BLACK
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );

    gui::inst()->addTextButtonAutoDimension(4,1
            ,"꾸밈"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_5)
            , 14
            , ALIGNMENT_CENTER
            , Color3B::BLACK
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );

    gui::inst()->addTextButtonAutoDimension(5,1
            ,"전체"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_INVENTORY)
            , 14
            , ALIGNMENT_CENTER
            , Color3B::BLACK
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );

    ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 2), size, margin, "", Size(1050, 110));

    for(int n=0; n < 10; n++){

        Layout* l = gui::inst()->createLayout(nodeSize, "element.png", true, Color3B::WHITE);

        gui::inst()->addLabelAutoDimension(0,1
                , "`"
                , l
                , 24
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , gridSize
                , Size::ZERO
                , Size::ZERO
        );

        gui::inst()->addTextButtonAutoDimension(0,2
                , "[초급] 정어리 낚시"
                , l
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                , 12
                , ALIGNMENT_CENTER
                , Color3B::BLACK
                , gridSize
                , Size::ZERO
                , Size::ZERO
        );

        gui::inst()->addTextButtonAutoDimension(0,3
                , "$10 S0 I0 A0"
                , l
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                , 12
                , ALIGNMENT_CENTER
                , Color3B::GRAY
                , gridSize
                , Size::ZERO
                , Size::ZERO
        );

        gui::inst()->addTextButtonAutoDimension(0,4
                , "$0 S10 I12 A0"
                , l
                , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
                , 12
                , ALIGNMENT_CENTER
                , Color3B::BLUE
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
}

void MainScene::particleSample(const string sz){

    auto layer = LayerColor::create();
    layer->setContentSize(Size(300, 225));
    gui::inst()->addLabelAutoDimension(0, 0, sz, layer, 14, ALIGNMENT_CENTER, Color3B::GREEN, Size(1, 1), Size::ZERO, Size::ZERO);
    gui::inst()->addTextButtonAutoDimension(0,5, "CLOSE"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_POPUP_2)
            , 10
            , ALIGNMENT_CENTER
            , Color3B::WHITE
            , Size(1, 5)
    );
    mGacha.run("gem.jpg", layer);
}

void MainScene::scheduleCB(float f){
    float p =c1.getPercent() + 5;
    c1.update(p);

    if(p > 100){
        this->unschedule( schedule_selector(MainScene::scheduleCB));
        c1.setDecay(true);
    }


}

void MainScene::scheduleRecharge(float f) {
	if (logics::hInst->rechargeHP())
		updateState(false);
}