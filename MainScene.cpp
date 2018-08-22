//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "ActionScene.h"

using namespace cocos2d::ui;

#define PARTICLE_FINAL "particles/particle_finally.plist"
#define DEFAULT_LAYER_SIZE Size(385, 200)

#define INVENTORY_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size(5, 10); auto nodeSize = Size(120, 70); auto gridSize = Size(3, 5);
#define BUY_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size(5, 10); auto nodeSize = Size(120, 70); auto gridSize = Size(3, 5);
#define ACHIEVEMENT_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size(10, 10); auto nodeSize = Size(178, 70); auto gridSize = Size(3, 5);
#define ACTION_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size(10, 10); auto nodeSize = Size(178, 70); auto gridSize = Size(3, 5);

int nodeMargin = 4;

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
	mAlertLayer = NULL;
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

    gui::inst()->addSpriteFixedSize(Size(110, 80), 4, 3, "21.png", this);


    string name = wstring_to_utf8(logics::hInst->getActor()->name, true);
    name += " lv.";
    name += to_string(logics::hInst->getActor()->level);

    mName = gui::inst()->addLabel(0, 0, name, this, 12, ALIGNMENT_NONE);

    //mGrid.addLabel(0, 0, "abc", this, 12, ALIGNMENT_NONE);
    mJobTitle = gui::inst()->addLabel(4,1,wstring_to_utf8(logics::hInst->getActor()->jobTitle, true), this, 12);

    gui::inst()->addTextButton(0,6, wstring_to_utf8(L"╈"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACTION), 32);
    gui::inst()->addTextButton(1,6, wstring_to_utf8(L"┲"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE), 32);
    gui::inst()->addTextButton(2,6, wstring_to_utf8(L"╁"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_FARMING), 32);


    mExp = gui::inst()->addLabel(4, 0, "", this, 12, ALIGNMENT_CENTER);
    mPoint = gui::inst()->addTextButton(7, 0, "$", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_PURCHASE), 12, ALIGNMENT_CENTER, Color3B::GREEN);
    mHP = gui::inst()->addTextButton(8, 0, "♥", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RECHARGE), 12, ALIGNMENT_CENTER, Color3B::ORANGE);
    mProperties = gui::inst()->addLabel(8, 2, "", this, 12);
	   
    gui::inst()->addTextButton(0, 2, wstring_to_utf8(L"├"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACHIEVEMENT), 32);

	gui::inst()->addTextButton(0, 4, wstring_to_utf8(L"도감"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_COLLECTION), 16);

    gui::inst()->addTextButton(6, 6, wstring_to_utf8(L"┞"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_SELL), 32);
    gui::inst()->addTextButton(7, 6, wstring_to_utf8(L"╅"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_BUY), 32);
	mInventory = gui::inst()->addTextButton(8, 6, wstring_to_utf8(L"╆"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_INVENTORY), 32);

    //gacha
    mParitclePopup = mGacha.createLayer(mParitclePopupLayer
            , this
            , "crystal_marvel.png"
            , "particles/particle_magic.plist"
            , PARTICLE_FINAL);

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
	mAlertLayer = NULL;
}

void MainScene::showResult(const string msg, bool enableParticle) {

	if (mAlertLayer != NULL)
		this->removeChild(mAlertLayer);

	int fontSize = 12;
	mAlertLayer = gui::inst()->createLayout(DEFAULT_LAYER_SIZE, "bg_temp.png");
	Vec2 point;
	gui::inst()->getPoint(4, 3, point, ALIGNMENT_CENTER);
	mAlertLayer->setPosition(point);
	mAlertLayer->setAnchorPoint(Vec2(0.5, 0.5));	
	gui::inst()->addLabelAutoDimension(0, 0, "RESULT", mAlertLayer, 16, ALIGNMENT_CENTER, Color3B::BLACK, Size(1, 5), Size::ZERO, Size::ZERO);
	gui::inst()->addLabelAutoDimension(0, 1, msg, mAlertLayer, fontSize, ALIGNMENT_CENTER, Color3B::BLACK, Size(1, 5), Size::ZERO, Size::ZERO);
	gui::inst()->addTextButtonAutoDimension(0, 3, "CLOSE", mAlertLayer
		, CC_CALLBACK_1(MainScene::alertCloseCallback, this)
		, fontSize
		, ALIGNMENT_CENTER
		, Color3B::BLUE
		, Size(1, 5)
		, Size::ZERO
		, Size::ZERO
	);
	this->addChild(mAlertLayer);

	//particle
	if (enableParticle) {
		auto particle = ParticleSystemQuad::create(PARTICLE_FINAL);
		particle->setPosition(point);		
		particle->setAutoRemoveOnFinish(true);
		this->addChild(particle);
	}
	
}

void MainScene::alert(const string msg){

	if(mAlertLayer != NULL)
		this->removeChild(mAlertLayer);

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

    string name = wstring_to_utf8(logics::hInst->getActor()->name, true);
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

    wstring szwHP = L"♥ ";
    szwHP += to_wstring(hp) + L"/" + to_wstring(hpMax) + L" +";
	
	string szHP = wstring_to_utf8(szwHP);

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
void MainScene::callbackActionAnimation(Ref* pSender, int id) {

	this->removeChild(layerGray);

	vector<_itemPair> rewards;
	_property property;
	int point;
	trainingType type;
	float max = 40.f;
	float ratioTouch = gui::inst()->mModalTouchCnt / max;
	errorCode err = logics::hInst->runTraining(id, rewards, &property, point, type, ratioTouch);
	wstring sz = logics::hInst->getErrorMessage(err);

	wstring szRewards = L"";
	for (int n = 0; n < rewards.size(); n++) {
		_item item = logics::hInst->getItem(rewards[n].itemId);
		szRewards += item.name + L"x" + to_wstring(rewards[n].val) + L" ";
	}

	bool isInventory = false;

	string szResult;
	if (point > 0)	szResult = "$ " + to_string(point);
	if (property.strength > 0) szResult += " S:" + to_string(property.strength);
	if (property.intelligence > 0) szResult += " I:" + to_string(property.intelligence);
	if (property.appeal > 0) szResult += " A:" + to_string(property.appeal);
	//szResult += " (" + to_string(ratioTouch * 100) + "%)";

	string szRewardsUTF8 = wstring_to_utf8(szRewards, true);
	if (szRewardsUTF8.size() > 1) {
		isInventory = true;	
		szResult += "\n" + szRewardsUTF8;
	}	

	switch (err) {
	case error_success:
	case error_levelup:
		showResult(szResult, isInventory);
		updateState(isInventory);
		break;
	default:
		alert(wstring_to_utf8(sz, true));
		break;
	}
		
}

void MainScene::callbackAction(Ref* pSender, int id){
	if (id == -1)
		return;    
	this->removeChild(layerGray);
	errorCode err = logics::hInst->isValidTraining(id);
	if (err != error_success) {
		alert(wstring_to_utf8(logics::hInst->getErrorMessage(err), true));
		return;
	}

	auto size = Size(300, 200);
	layer = gui::inst()->addPopup(layerGray, this, size);
	int fontSize = 12;
	auto l = gui::inst()->createLayout(size, "", true, Color3B::WHITE);
	l->setPosition(Vec2(layerGray->getContentSize().width / 2, layerGray->getContentSize().height / 2));
	l->setAnchorPoint(Vec2(0.5, 0.5));
	
	_training t = logics::hInst->getActionList()->at(id);
	int type = t.type;

	/*
	wstring rewardItems;
	int rewardItemCnt = 0;
	for (int n = 0; n < maxTrainingItems; n++) {
		_itemPair* p = t.reward.items[n];
		if (p != NULL) {
			if (n == 0)
				rewardItems = logics::hInst->getItem(p->itemId).name;
			rewardItemCnt++;
		}
        else
            break;
	}
	if (rewardItemCnt > 1)
		rewardItems += L" 외 " + to_wstring(rewardItemCnt - 1);
	*/

	string pay = " ";
	if (t.cost.point > 0) pay += "$ " + to_string(t.cost.point) + " ";
	if (t.cost.strength > 0) pay += "S: " + to_string(t.cost.strength) + " ";
	if (t.cost.intelligence > 0) pay += "I: " + to_string(t.cost.intelligence) + " ";
	if (t.cost.appeal > 0) pay += "A: " + to_string(t.cost.appeal) + " ";

	string reward;
	if (t.reward.point > 0)         reward += "$ " + to_string(t.reward.point) + " ";
	if (t.reward.strength > 0)      reward += "S: " + to_string(t.reward.strength) + " ";
	if (t.reward.intelligence > 0)  reward += "I: " + to_string(t.reward.intelligence) + " ";
	if (t.reward.appeal > 0)        reward += "A: " + to_string(t.reward.appeal) + " ";
	
	gui::inst()->addLabelAutoDimension(2, 1, wstring_to_utf8(t.name), l, 14, ALIGNMENT_NONE);

	int idx = 2;
	if (pay.size() > 1)
		gui::inst()->addLabelAutoDimension(2, idx++, "- " + pay, l, 12, ALIGNMENT_NONE, Color3B::RED);
	if(reward.size() > 1)
		gui::inst()->addLabelAutoDimension(2, idx++, "+ " + reward, l, 12, ALIGNMENT_NONE);

	/*
	string szRewardItem = wstring_to_utf8(rewardItems);
	if(szRewardItem.size() > 1)
		gui::inst()->addLabelAutoDimension(idx++, 2, szRewardItem, l, 10, ALIGNMENT_NONE);
	*/

    string path = "action/" + to_string(type) + "/0.png";
	auto pMan = Sprite::create(path);
	pMan->setPosition(Point(l->getContentSize().width / 2, l->getContentSize().height / 3));
	l->addChild(pMan);

	auto animation = Animation::create();
	animation->setDelayPerUnit(0.15f);
	for (int n = 0; n <= 5; n++) {
        path = "action/" + to_string(type) + "/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}
		
	auto cb = CallFuncN::create(CC_CALLBACK_1(MainScene::callbackActionAnimation, this, id));
	auto animate = Sequence::create(Repeat::create(Animate::create(animation), 4), cb, NULL);
	
	pMan->runAction(animate);
	
	layerGray->addChild(l);

	return;	
}

void MainScene::runRace() {
	
	if (logics::hInst->getActor()->property.total() < 30) {
		alert(wstring_to_utf8(logics::hInst->getErrorMessage(error_not_enough_property)));
		return;
	}

	itemsVector v;
	int raceId = 1;
	errorCode err = logics::hInst->runRace(raceId, v);
	if (err != error_success && err != error_levelup) {
		alert(wstring_to_utf8(logics::hInst->getErrorMessage(err)));
		return;
	}

	Director::getInstance()->pushScene(ActionScene::createScene());
	
}

void MainScene::callback2(cocos2d::Ref* pSender, SCENECODE type){
    CCLOG("Callback !!!!!!!! %d", type);

	switch (type) {
	case SCENECODE_CLOSEPOPUP:
		closePopup();
		break;
	case SCENECODE_ACTION: //Action
		actionList();
		break;
	case SCENECODE_RECHARGE: //HP 충전
		showInventory(inventoryType_HP);
		break;
	case SCENECODE_SELL: //아이템 판매
		showInventory(inventoryType_all, true);
		break;
	case SCENECODE_ACHIEVEMENT:
		showAchievement();
		break;
	case SCENECODE_BUY:
		showBuy();
		break;
	case SCENECODE_FARMING: 		
		break;
	case SCENECODE_RACE://RACE
		runRace();
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
		//dailyReward();
		showCollection();
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
	updateState(false);
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
    layer = gui::inst()->addPopup(layerGray, this, size, "bg_inventory.png", Color4B::WHITE);
    gui::inst()->addTextButtonAutoDimension(8,0
            ,"Close"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
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

void MainScene::invokeItem(Ref* pSender, int id) {
	errorCode err = logics::hInst->runRecharge(id, 1);
	if (err != error_success && err != error_levelup) {
		wstring sz = logics::hInst->getErrorMessage(err);
		alert(wstring_to_utf8(sz, true));
	}
	else {
		closePopup();
		updateState(true);
	}	
}

void MainScene::sellItem(Ref* pSender, inventoryType code, int id) {
	errorCode err = logics::hInst->runTrade(false, id, 1);
	if (err != error_success && err != error_levelup) {
		wstring sz = logics::hInst->getErrorMessage(err);
		alert(wstring_to_utf8(sz, true));
	}
	else {
		//closePopup();
		updateState(true);
		showInventoryCategory(pSender, code, true);
	}
}

void MainScene::showInventoryCategory(Ref* pSender, inventoryType code, bool isSell) {
	INVENTORY_SIZE;
	//int nodeMargin = 2;
	int newLine = 3;
	if (isSell)
		gridSize.height++;

	vector<intPair> vec;
	logics::hInst->getActor()->inven.getWarehouse(vec, (int)code);
	Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 7), Vec2(9, 1), size, margin);
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - nodeMargin;
	Size innerSize = Size(sizeOfScrollView.width, ((vec.size() / newLine) + 1) * (nodeSize.height + nodeMargin));
	ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 1), size, margin, "", innerSize);

	for (int n = 0; n < (int)vec.size(); n++) {
		int id = vec[n].key;
		string img = "items/" + to_string(id % 20) + ".png";
		Layout* l = gui::inst()->createLayout(nodeSize, "", true);
		_item item = logics::hInst->getItem(id);
		
		int heightIdx = 2;
		//item image
		auto sprite = gui::inst()->addSpriteAutoDimension(0, heightIdx++, img, l, ALIGNMENT_CENTER, gridSize, Size::ZERO, Size::ZERO);
		sprite->setContentSize(Size(20, 20));
		gui::inst()->addLabelAutoDimension(0, heightIdx++, to_string(item.type), l, 9, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		heightIdx = 1;
		//item name
		string name = wstring_to_utf8(item.name);

		//판매
		if (isSell) {
			int price = logics::hInst->getTrade()->getPriceSell(item.id);

			gui::inst()->addTextButtonAutoDimension(1, heightIdx++, "Lv." + to_string(item.grade), l
				, CC_CALLBACK_1(MainScene::sellItem, this, code, item.id), 9, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
			gui::inst()->addTextButtonAutoDimension(1, heightIdx++, name, l
				, CC_CALLBACK_1(MainScene::sellItem, this, code, item.id), 9, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
			//item quantity
			gui::inst()->addTextButtonAutoDimension(1, heightIdx++, "x " + to_string(vec[n].val), l
				, CC_CALLBACK_1(MainScene::sellItem, this, code, item.id), 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
			gui::inst()->addTextButtonAutoDimension(1, heightIdx++, "Sell $" + to_string(price), l
				, CC_CALLBACK_1(MainScene::sellItem, this, code, item.id), 10, ALIGNMENT_NONE, Color3B::BLUE, gridSize, Size::ZERO, Size::ZERO);
		}
		else {
			if (item.type == itemType_hp_meal) {
				gui::inst()->addTextButtonAutoDimension(1, heightIdx++, "Lv." + to_string(item.grade), l
					, CC_CALLBACK_1(MainScene::invokeItem, this, id), 9, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
				gui::inst()->addTextButtonAutoDimension(1, heightIdx++, name, l
					, CC_CALLBACK_1(MainScene::invokeItem, this, id), 9, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
			}
			else {
				gui::inst()->addLabelAutoDimension(1, heightIdx++, "Lv." + to_string(item.grade), l, 9, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
				gui::inst()->addLabelAutoDimension(1, heightIdx++, name, l, 9, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
			}

			//item quantity
			gui::inst()->addLabelAutoDimension(1, heightIdx++, "x " + to_string(vec[n].val), l, 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
		}

		gui::inst()->addLayoutToScrollView(sv, l, nodeMargin, newLine);
	}
	layer->removeChildByTag(CHILD_ID_INVENTORY, true);
	layer->addChild(sv, 1, CHILD_ID_INVENTORY);
}

void MainScene::showInventory(inventoryType type, bool isSell) {
#define __PARAMS(STR, ID) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showInventoryCategory, this, ID, isSell), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	
	INVENTORY_SIZE

	this->removeChild(layerGray);
	layer = gui::inst()->addPopup(layerGray, this, size
		, isSell ? "bg_sell.png" : "bg_inventory.png"
		, Color4B::WHITE);

	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
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
	
	showInventoryCategory(this, type, isSell);
}

void MainScene::buyCallback(Ref* pSender, int id) {
	errorCode err = logics::hInst->runTrade(true, id, 1);
	if (err != error_success && err != error_levelup) {
		wstring sz = logics::hInst->getErrorMessage(err);
		alert(wstring_to_utf8(sz, true));
	}
	else {
		closePopup();
		updateState(true);
	}
}

void MainScene::showBuyCategory(Ref* pSender, inventoryType code) {
	BUY_SIZE;
	//int nodeMargin = 2;
	int newLine = 3;

	trade::tradeMap * m = logics::hInst->getTrade()->get();

	Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 7), Vec2(9, 1), size, margin);
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - nodeMargin;
	Size innerSize = Size(sizeOfScrollView.width, ((m->size() / newLine) + 1) * (nodeSize.height + nodeMargin));

	ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 1), size, margin, "", innerSize);
	
	for (trade::tradeMap::iterator it = m->begin(); it != m->end(); ++it) {
		int id = it->first;
		_item item = logics::hInst->getItem(id);
		if (item.type > itemType_max)
			continue;		
		
		string img = "items/";
		img += to_string(id % 20);
		img += ".png";
		Layout* l = gui::inst()->createLayout(nodeSize, "", true, Color3B::WHITE);
		
		string name = wstring_to_utf8(item.name, false);
		int heightIdx = 1;

		auto sprite = gui::inst()->addSpriteAutoDimension(0, 2, img, l, ALIGNMENT_CENTER, gridSize, Size::ZERO, Size::ZERO);
		sprite->setContentSize(Size(20, 20));
		gui::inst()->addTextButtonAutoDimension(0, 3, to_string(item.type), l
			, CC_CALLBACK_1(MainScene::buyCallback, this, id), 9, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		gui::inst()->addTextButtonAutoDimension(1, heightIdx++, "Lv." + to_string(item.grade), l
			, CC_CALLBACK_1(MainScene::buyCallback, this, id), 9, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
		gui::inst()->addTextButtonAutoDimension(1, heightIdx++, name, l
			, CC_CALLBACK_1(MainScene::buyCallback, this, id), 9, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
		gui::inst()->addTextButtonAutoDimension(1, heightIdx++, "$ " + to_string(logics::hInst->getTrade()->getPriceBuy(id)), l
			, CC_CALLBACK_1(MainScene::buyCallback, this, id), 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		gui::inst()->addLayoutToScrollView(sv, l, nodeMargin, newLine);
	}

	layer->removeChildByTag(CHILD_ID_BUY, true);
	layer->addChild(sv, 1, CHILD_ID_BUY);
}

void MainScene::showBuy(inventoryType type) {
#define __PARAMS(STR, ID) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showBuyCategory, this, ID), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin

	BUY_SIZE;
	this->removeChild(layerGray);
	layer = gui::inst()->addPopup(layerGray, this, size, "bg_buy.png", Color4B::WHITE);

	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
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
void MainScene::showAchievementCategory(Ref* pSender, bool isDaily) {
	ACHIEVEMENT_SIZE;
	//int nodeMargin = 2;
	int newLine = 2;

	int cnt = logics::hInst->getAchievementSize(isDaily);

	Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 7), Vec2(9, 1), size, margin);
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - nodeMargin;	
	Size innerSize = Size(sizeOfScrollView.width, ((cnt / newLine) + 1) * (nodeSize.height + nodeMargin));
	ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 1), size, margin, "", innerSize);
	
	for (int n = 0; n < cnt; n++) {
		achievement::detail p;
		logics::hInst->getAchievementDetail(isDaily, n, p);
		Layout* l = gui::inst()->createLayout(nodeSize, "", true, Color3B::WHITE);
		
		string state = "";
		if (p.isFinished)
			state = "O";

		if (p.isReceived)
			state = "V";

		gui::inst()->addLabelAutoDimension(0, 2
			, state, l, 24, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		int heightIdx = 1;

		gui::inst()->addLabelAutoDimension(1, heightIdx++
			, wstring_to_utf8(p.title), l, 12, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		string score = to_string(p.accumulation) + "/" + to_string(p.goal);
		gui::inst()->addLabelAutoDimension(1, heightIdx++
			, score, l, 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		string reward = wstring_to_utf8(logics::hInst->getItem(p.rewardId).name + L" x" + to_wstring(p.rewardVal));
		gui::inst()->addLabelAutoDimension(1, heightIdx++
			, reward, l, 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		gui::inst()->addLayoutToScrollView(sv, l, nodeMargin, newLine);
	}

	layer->removeChildByTag(CHILD_ID_ACHIEVEMENT, true);
	layer->addChild(sv, 1, CHILD_ID_ACHIEVEMENT);

}
void MainScene::showAchievement() {
#define __PARAMS_ACHIEVEMENT(STR, ISDAILY) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showAchievementCategory, this, ISDAILY), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	ACHIEVEMENT_SIZE;
	this->removeChild(layerGray);
	layer = gui::inst()->addPopup(layerGray, this, size, "bg_achievement.png", Color4B::WHITE);

	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 12, ALIGNMENT_CENTER, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);
	int nMenuIdx = 0;
	//tab
	gui::inst()->addTextButtonAutoDimension(__PARAMS_ACHIEVEMENT("Daily", true));
	gui::inst()->addTextButtonAutoDimension(__PARAMS_ACHIEVEMENT("Totally", false));

	showAchievementCategory(this, true);
}

void MainScene::showCollection() {
#define __PARAMS_COLLECTION(STR, ISDAILY) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showAchievementCategory, this, ISDAILY), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	ACHIEVEMENT_SIZE;
	this->removeChild(layerGray);
	layer = gui::inst()->addPopup(layerGray, this, size, "bg_achievement.png", Color4B::WHITE);

	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 12, ALIGNMENT_CENTER, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);
	int nMenuIdx = 0;	
	int newLine = 2;

	int cnt = 0;
	for (__items::iterator it = logics::hInst->getItems()->find(collectionStartId); it != logics::hInst->getItems()->end(); ++it) {		
		if (it->second.type == itemType_collection) {
			cnt++;			
		}
	}

	Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 7), Vec2(9, 1), size, margin);
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - nodeMargin;
	Size innerSize = Size(sizeOfScrollView.width, ((cnt / newLine) + 1) * (nodeSize.height + nodeMargin));
	ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 1), size, margin, "", innerSize);

	for (__items::iterator it = logics::hInst->getItems()->find(collectionStartId); it != logics::hInst->getItems()->end(); ++it) {
		if (it->second.type == itemType_collection) {
			bool has = false;
			if (logics::hInst->getActor()->collection.find(it->first) != logics::hInst->getActor()->collection.end())
				has = true;

			_item item = it->second;
			
			Layout* l = gui::inst()->createLayout(nodeSize, "", true, Color3B::WHITE);

			string state = " ";
			if (has)
				state = "O";

			gui::inst()->addLabelAutoDimension(0, 2
				, state, l, 24, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

			int heightIdx = 1;

			gui::inst()->addLabelAutoDimension(1, heightIdx++
				, "Lv." + to_string(item.grade), l, 12, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

			gui::inst()->addLabelAutoDimension(1, heightIdx++
				, wstring_to_utf8(item.name), l, 12, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
			
			gui::inst()->addLayoutToScrollView(sv, l, nodeMargin, newLine);
		}
	}

	layer->removeChildByTag(CHILD_ID_COLLECTION, true);
	layer->addChild(sv, 1, CHILD_ID_COLLECTION);
}

void MainScene::actionList() {
	ACTION_SIZE;	
	int newLine = 2;   

    this->removeChild(layerGray);
    layer = gui::inst()->addPopup(layerGray, this, size, "bg_action.png", Color4B::WHITE);
    gui::inst()->addTextButtonAutoDimension(8,0
            ,"CLOSE"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
            , 14
            , ALIGNMENT_CENTER
            , Color3B::RED
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
    );

    gui::inst()->addLabelAutoDimension(4,0
            ,"ACTION"
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
	
	Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 7), Vec2(9, 1), size, margin);
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - nodeMargin;
	Size innerSize = Size(sizeOfScrollView.width, ((pTraining->size() / newLine) + 1) * (nodeSize.height + nodeMargin));
    ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 1), size, margin, "", innerSize);

    for (__training::iterator it = pTraining->begin(); it != pTraining->end(); ++it) {        
		int id = it->first;

		if (!logics::hInst->isValidTraningTime(id))
            continue;

        wstring rewardItems;
        wstring costItems;		
		int rewardItemCnt = 0;		

        for (int n = 0; n < maxTrainingItems; n++) {
            _itemPair* p = it->second.reward.items[n];
            if (p != NULL) {				
				if(n == 0)
					rewardItems = logics::hInst->getItem(p->itemId).name;
				rewardItemCnt++;
                
            }
            p = it->second.cost.items[n];
            if (p != NULL) {				
				costItems += logics::hInst->getItem(p->itemId).name + L"x" + to_wstring(p->val);				
            }
        }		
		if (rewardItemCnt > 1)
			rewardItems += L" 외 " + to_wstring(rewardItemCnt - 1);
		
		/*
		string pay = " ";
		if (it->second.cost.point > 0) pay += "$ " + to_string(it->second.cost.point) + " ";		
		if (it->second.cost.strength > 0) pay += "S: " + to_string(it->second.cost.strength) + " ";
		if (it->second.cost.intelligence > 0) pay += "I: " + to_string(it->second.cost.intelligence) + " ";
		if (it->second.cost.appeal > 0) pay += "A: " + to_string(it->second.cost.appeal) + " ";
		//pay += wstring_to_utf8(costItems);
		*/
		/*
		string reward;
        if(it->second.reward.point > 0)         reward += "$ " + to_string(it->second.reward.point) + " ";
        if(it->second.reward.strength > 0)      reward += "S: " + to_string(it->second.reward.strength) + " ";
        if(it->second.reward.intelligence > 0)  reward += "I: "+ to_string(it->second.reward.intelligence) + " ";
        if(it->second.reward.appeal > 0)        reward += "A: " + to_string(it->second.reward.appeal) + " ";
		reward += wstring_to_utf8(rewardItems);
		*/

        //대충 정해 놓고 나중에
        int type = it->second.type;
		int level = it->second.level;
		const wchar_t c[] = {L'`' , L'─', L'┌', L'┐', L'┘', L'└', L'├', L'┬'};
        Color3B bgColor = Color3B(255 - type * 5, 255 - level * 10, 255);

        Layout* l = gui::inst()->createLayout(nodeSize, "", true, bgColor);

		wstring szC = L" ";
		szC[0] = c[type];

        gui::inst()->addLabelAutoDimension(0,2, wstring_to_utf8(szC), l, 24, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		//Error처리
		Color3B fontColor = Color3B::BLACK;
		Color3B fontColorReward = Color3B::BLUE;
		errorCode err = logics::hInst->isValidTraining(id);
		if (err != error_success) {			
			id = -1;
			gui::inst()->addLabelAutoDimension(1, 4
				, wstring_to_utf8(logics::hInst->getErrorMessage(err), true)
				, l				
				, 8
				, ALIGNMENT_NONE
				, Color3B::RED
				, gridSize
				, Size::ZERO
				, Size::ZERO
			);
			fontColor = Color3B::GRAY;
			fontColorReward = fontColor;
		}
			
		gui::inst()->addLabelAutoDimension(1, 1, "Lv. " + to_string(level), l, 8, ALIGNMENT_NONE, fontColor, gridSize, Size::ZERO, Size::ZERO);
        gui::inst()->addTextButtonAutoDimension(1,2, wstring_to_utf8(it->second.name), l, CC_CALLBACK_1(MainScene::callbackAction, this, id), 12, ALIGNMENT_NONE, fontColor, gridSize, Size::ZERO, Size::ZERO);
		gui::inst()->addTextButtonAutoDimension(1,3, wstring_to_utf8(costItems), l, CC_CALLBACK_1(MainScene::callbackAction, this, id), 9, ALIGNMENT_NONE, fontColor, gridSize, Size::ZERO, Size::ZERO);
        //gui::inst()->addTextButtonAutoDimension(1,3, pay, l, CC_CALLBACK_1(MainScene::callbackAction, this, id), 9, ALIGNMENT_NONE, fontColor, gridSize, Size::ZERO, Size::ZERO);
        
		gui::inst()->addLayoutToScrollView(sv, l, nodeMargin, newLine);
    }

    layer->addChild(sv, 1, 123);
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