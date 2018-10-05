//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "ActionScene.h"
#include "AlertScene.h"
#include "FarmingScene.h"
#include "ActionBasic.h"

using namespace cocos2d::ui;

#define PARTICLE_FINAL "particles/particle_finally.plist"
#define PARTICLE_MAGIC "particles/particle_magic.plist"

#define INVENTORY_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(120, 65); auto gridSize = Size(3, 4);
#define BUY_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(120, 60); auto gridSize = Size(3, 4);
#define ACHIEVEMENT_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(178, 70); auto gridSize = Size(3, 5);
#define RACE_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(178, 90); auto gridSize = Size(3, 6);
#define ACTION_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(178, 70); auto gridSize = Size(3, 4);

#define nodeMargin 4

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
	mLastUpdateQuest = 0;
	mQuantityItemId = 0;
	layerGray = NULL;
	layer = NULL;
	mAlertLayer = NULL;
    hInst = this;    
	if (!logics::hInst->init(MainScene::farmingCB, MainScene::tradeCB, MainScene::achievementCB, false)) {
		CCLOG("logics Init Failure !!!!!!!!!!!!!!");
		return false;
	}
		

    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Color3B fontColor = Color3B::BLACK;
	//BG
	
	auto bg = Sprite::create(BG_HOME);
	bg->setContentSize(Director::getInstance()->getVisibleSize());
	bg->setAnchorPoint(Vec2(0, 0));
	//bg->setOpacity(50);
	bg->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(bg);
	
    mGrid.init("fonts/Goyang.ttf", 14);

	//farming init
	if (!initFarm()) {
		CCLOG("Init Farm Failure !!!!!!!!!!!!!!");
		return false;
	}

    //Cat Main UI
    //gui::inst()->drawGrid(this);

    loadingBar = gui::inst()->addProgressBar(4, 0, LOADINGBAR_IMG, this, 150, 10);
    
	//Character
	auto pCharacter = getIdle();
	pCharacter->setPosition(gui::inst()->getPointVec2(7, 4));
    gui::inst()->setScale(pCharacter, 50);
	this->addChild(pCharacter);


    //string name = wstring_to_utf8(logics::hInst->getActor()->name, true);
	string name = logics::hInst->getActor()->name;
    name += " lv.";
    name += to_string(logics::hInst->getActor()->level);

    mName = gui::inst()->addLabel(0, 0, name, this, 12, ALIGNMENT_NONE, fontColor);

    //mGrid.addLabel(0, 0, "abc", this, 12, ALIGNMENT_NONE);
    //mJobTitle = gui::inst()->addLabel(4,0,wstring_to_utf8(logics::hInst->getActor()->jobTitle, true), this, 12, ALIGNMENT_CENTER, fontColor);
	mJobTitle = gui::inst()->addLabel(4, 0, logics::hInst->getActor()->jobTitle, this, 12, ALIGNMENT_CENTER, fontColor);

	mJobTitle->setPosition(Vec2(mJobTitle->getPosition().x, mJobTitle->getPosition().y + 15));

	gui::inst()->addTextButton(0, 6, "Action", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACTION), 0, ALIGNMENT_CENTER, fontColor);
	gui::inst()->addTextButton(1, 6, "Race", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE), 0, ALIGNMENT_CENTER, fontColor);
    mFarming = gui::inst()->addTextButton(2,6, "Farm", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_FARMING), 0, ALIGNMENT_CENTER, fontColor);
	//gui::inst()->addTextButton(3, 6, wstring_to_utf8(L"임시"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_TEMP), 32, ALIGNMENT_CENTER, fontColor);


    mExp = gui::inst()->addLabel(4, 0, "", this, 12, ALIGNMENT_CENTER);	    
    mHP = gui::inst()->addTextButton(7, 0, "♥", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RECHARGE), 0, ALIGNMENT_CENTER, Color3B::ORANGE);
	mPoint = gui::inst()->addTextButton(8, 0, COIN, this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_PURCHASE));

    mProperties = gui::inst()->addLabel(8, 2, "", this, 12, ALIGNMENT_CENTER, fontColor);
	   
	mAchievement = gui::inst()->addTextButton(0, 2, wstring_to_utf8(L"업적"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACHIEVEMENT), 0, ALIGNMENT_CENTER, fontColor);

	gui::inst()->addTextButton(8, 4, wstring_to_utf8(L"도감"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_COLLECTION), 0, ALIGNMENT_CENTER, fontColor);

    mSell = gui::inst()->addTextButton(6, 6, wstring_to_utf8(L"벼룩시장"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_SELL), 0, ALIGNMENT_CENTER, fontColor);
    mBuy = gui::inst()->addTextButton(7, 6, wstring_to_utf8(L"상점"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_BUY), 0, ALIGNMENT_CENTER, fontColor);
	mInventory = gui::inst()->addTextButton(8, 6, wstring_to_utf8(L"가방"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_INVENTORY), 0, ALIGNMENT_CENTER, fontColor);

	//auto mail = gui::inst()->addLabel(4, 5, "message...", this, 10);
	//EaseBackOut::create

	//quest 표시
	updateQuests();
    //gacha
	/*
    mParitclePopup = mGacha.createLayer(mParitclePopupLayer
            , this
            , "crystal_marvel.png"
            , "particles/particle_magic.plist"
            , PARTICLE_FINAL);
	*/
	mLevel = logics::hInst->getActor()->level;

    updateState(false);
	this->schedule(schedule_selector(MainScene::scheduleRecharge), 1); //HP recharge schedule
	//퀘스트 타이머. 퀘스트 정산이 1초마다 되기 때문에 싱크가 잘 안맞아서 어쩔 수 없다.
	this->schedule([=](float delta) {		
		this->updateQuests();
		mLevel = logics::hInst->getActor()->level;
	}, 0.5, "questTimer");

	CCLOG("Init Done !!!!!!!!!!!!!!");
    return true;
}

bool MainScene::initFarm() {
	Vec2 a1 = gui::inst()->getPointVec2(0, 0, ALIGNMENT_NONE);
	Vec2 a2 = gui::inst()->getPointVec2(1, 1, ALIGNMENT_NONE);
	Size gridSize = Size(a2.x - a1.x, a1.y - a2.y);
	int id = 0;
	//farm init or load	
	sqlite3_stmt * stmt = Sql::inst()->select("select * from farm");
	if (stmt == NULL)
		return false;

	int result = 0;
	while (true)
	{
		result = sqlite3_step(stmt);

		if (result == SQLITE_ROW)
		{
			int idx = 0;
			int id = sqlite3_column_int(stmt, idx++);
			int x = sqlite3_column_int(stmt, idx++);
			int y = sqlite3_column_int(stmt, idx++);
			int seedId = sqlite3_column_int(stmt, idx++);
			//farming::farming_status status = (farming::farming_status)sqlite3_column_int(stmt, idx++);
			time_t timePlant = sqlite3_column_int64(stmt, idx++);
			int cntCare = sqlite3_column_int(stmt, idx++);
			time_t timeLastGrow = sqlite3_column_int64(stmt, idx++);
			int boost = sqlite3_column_int(stmt, idx++);
			int level = sqlite3_column_int(stmt, idx++);
			int accumulation = sqlite3_column_int(stmt, idx++);

			field * p = new field();
			p->id = id;
			p->x = x;
			p->y = y;
			p->seedId = seedId;
			p->status = farming::farming_status_max;
			p->timePlant = timePlant;
			p->cntCare = cntCare;
			p->timeLastGrow = timeLastGrow;
			p->boost = boost;
			p->level = level;
			p->accumulation = accumulation;

			logics::hInst->farmingAddField((farming::field *)p);
		}
		else
			break;
	}

	if (logics::hInst->getFarm()->countField() == 0) {
		for (int x = 2; x < 7; x++) {
			for (int y = 2; y < 6; y++) {
				field * node = new field(x, y - 1);
				//node->sprite = NULL;
				//node->l = gui::inst()->createLayout(gridSize, "", true, c[rand() % 5]);
				node->id = id;
				//node->label = gui::inst()->addLabelAutoDimension(0, 2, "", node->l, 8, ALIGNMENT_NONE, Color3B::WHITE, Size(1, 3), Size::ZERO, Size::ZERO);
				//node->l->setPosition(gui::inst()->getPointVec2(x, y, ALIGNMENT_NONE));
				logics::hInst->farmingAddField(node);
				id++;
			}
		}
	}
	return true;
}

//get Idle
RepeatForever * MainScene::getIdleAnimation(int id) {
	string prefix = "action/" + to_string(id) + "/";
	auto animation = Animation::create();
	animation->setDelayPerUnit(0.05);

	string path;
	for (int n = 0; n < 37; n++) {
		path = prefix + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}
	return RepeatForever::create(Animate::create(animation));
}
Sprite * MainScene::getIdle(int id) {
	string prefix = "action/" + to_string(id) + "/";
	auto p = Sprite::create(prefix + "0.png");
	
	p->runAction(getIdleAnimation(id));

	return p;
};


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

LayerColor * MainScene::createMessagePopup(LayerColor* &layerBG, Node * parent, const string title, const string msg, bool enableParticle) {

	if (layerBG != NULL)
		parent->removeChild(layerBG);

	int fontSize = 12;
	Size grid = Size(1, 5);
	LayerColor * layer = gui::inst()->addPopup(layerBG, parent, Size(240, 150)
			, "bg_temp.png"
			, Color4B::WHITE);
	layerBG->setLocalZOrder(ZORDER_POPUP);
	
	gui::inst()->addLabelAutoDimension(0, 0, title, layer, 16, ALIGNMENT_CENTER, Color3B::BLACK, grid, Size::ZERO, Size::ZERO);
	gui::inst()->addLabelAutoDimension(0, 2, msg, layer, fontSize, ALIGNMENT_CENTER, Color3B::BLACK, grid, Size::ZERO, Size::ZERO);
	
	//particle
	if (enableParticle) {		
		Vec2 pos;
		pos.x = Director::getInstance()->getVisibleSize().width / 2;
		pos.y = Director::getInstance()->getVisibleSize().height / 2;

		auto particle = ParticleSystemQuad::create(PARTICLE_FINAL);
		particle->setPosition(pos);
		particle->setAutoRemoveOnFinish(true);
		layerBG->addChild(particle);
	}

	return layer;
}

void MainScene::showResult(const string msg, bool enableParticle) {

	layer = createMessagePopup(layerGray, this, wstring_to_utf8(L"결과"), msg, enableParticle);
	gui::inst()->addTextButtonAutoDimension(0, 4, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 12
		, ALIGNMENT_CENTER
		, Color3B::BLUE
		, Size(1, 5)
		, Size::ZERO
		, Size::ZERO
	);	
}
void MainScene::alert(errorCode err) {
	Director::getInstance()->pushScene(AlertScene::createScene(err));
}

void MainScene::updateState(bool isInventoryUpdated) {
    //string name = wstring_to_utf8(logics::hInst->getActor()->name, true);
	string name = logics::hInst->getActor()->name;
    name += " lv.";
    name += to_string(logics::hInst->getActor()->level);
    if(name.compare(mName->getString()) != 0 ){
        mName->runAction(gui::inst()->createActionFocus());
		//level up effect
		Vec2 center = gui::inst()->getCenter();
		this->addChild(gui::inst()->createParticle(PARTICLE_FINAL, 4, 3), 100);
		const float duration = 1;
		const float scale = 3;
		auto label = gui::inst()->addLabel(4, 3
			, wstring_to_utf8(logics::hInst->getL10N("LEVEL_UP", logics::hInst->getActor()->level))
                                           , this, 0, ALIGNMENT_CENTER, Color3B::ORANGE);
        label->runAction(Sequence::create(
				EaseIn::create(ScaleBy::create(duration, scale), 0.4)
				, EaseOut::create(ScaleBy::create(duration, 1 / scale), 0.4)
				, RemoveSelf::create()
				, NULL));
        label->setLocalZOrder(100);
	}

    mName->setString(name);

    //mJobTitle->setString(wstring_to_utf8(logics::hInst->getActor()->jobTitle, true));
	mJobTitle->setString(logics::hInst->getActor()->jobTitle);

    string szExp;
    szExp += to_string(logics::hInst->getActor()->exp);
    szExp += " / ";
    szExp += to_string(logics::hInst->getMaxExp());
    if(szExp.compare(mExp->getString()) != 0 ){
        mExp->runAction(gui::inst()->createActionFocus());
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
        mProperties->runAction(gui::inst()->createActionFocus());
    }
    mProperties->setString(properties);

    int hp = logics::hInst->getHP();
    int hpMax = logics::hInst->getMaxHP();

    wstring szwHP = L"♥ ";
    szwHP += to_wstring(hp) + L"/" + to_wstring(hpMax);
	
	string szHP = wstring_to_utf8(szwHP);

    if(szHP.compare(mHP->getString()) != 0 ){
		
        mHP->runAction(gui::inst()->createActionFocus());
		
		//mHP->runAction(RepeatForever::create(Sequence::create(Blink::create(1, 1), NULL)));

    }
    mHP->setString(szHP);

    string szPoint = COIN;
    szPoint += to_string(logics::hInst->getActor()->point);
    //szPoint += " +";
    if(szPoint.compare(mPoint->getString()) != 0 ){
        mPoint->runAction(gui::inst()->createActionFocus());
    }
    mPoint->setString(szPoint);

	if(isInventoryUpdated)
		mInventory->runAction(gui::inst()->createActionFocus());
}
void MainScene::callbackActionAnimation(int id, int maxTimes) {
	closePopup();

	vector<_itemPair> rewards;
	_property property;
	int point;
	trainingType type;
		
	//float ratioTouch = min(1.f, 1.f - ((float)mActionCnt / (float)maxTimes));
	float ratioTouch = min(1.f, ((float)mActionTouchCnt / (float)maxTimes));
	//말도 안되게 빠른 경우 음수가 발생할 수도 있다.
	if (ratioTouch < 0.f)
		ratioTouch = 1.f;

	errorCode err = logics::hInst->runTraining(id, rewards, &property, point, type, ratioTouch);
	wstring sz = logics::hInst->getErrorMessage(err);

	wstring szRewards = L"";
	for (int n = 0; n < rewards.size(); n++) {
		_item item = logics::hInst->getItem(rewards[n].itemId);
		szRewards += item.name + L"x" + to_wstring(rewards[n].val) + L" ";
	}

	bool isInventory = false;

	string szResult = "Bonus: " + to_string((int)(ratioTouch * 100.f)) + "% \n";
	if (point > 0)	szResult = COIN + to_string(point);
	if (property.strength > 0) szResult += " S:" + to_string(property.strength);
	if (property.intelligence > 0) szResult += " I:" + to_string(property.intelligence);
	if (property.appeal > 0) szResult += " A:" + to_string(property.appeal);
	//szResult += " (" + to_string(ratioTouch * 100) + "%)";

	string szRewardsUTF8 = wstring_to_utf8(szRewards, true);
	if (szRewardsUTF8.size() > 1) {
		isInventory = true;	
		szResult += "\n" + szRewardsUTF8;
	}	
	closePopup();
	switch (err) {
	case error_success:
	case error_levelup:
		showResult(szResult, isInventory);
		updateState(isInventory);
		break;
	default:
		alert(err);
		break;
	}
		
}

void MainScene::callbackAction(Ref* pSender, int id){
	if (id == -1)
		return;    

	errorCode err = logics::hInst->isValidTraining(id);
	if (err != error_success) {
		alert(err);
		return;
	}

	ActionBasic * p = ActionBasic::create();
	if (!p->runAction(id))
		return alert(error_invalid_id);
	Director::getInstance()->pushScene(p);
	return;
}

void MainScene::runRace(Ref* pSender, int raceId) {
	errorCode err = logics::hInst->runRaceValidate(raceId);
	if (err != error_success) {
		alert(err);
		return;
	}

	Director::getInstance()->pushScene(ActionScene::createScene(raceId));
	
}

void MainScene::callback2(cocos2d::Ref* pSender, SCENECODE type){
	mCurrentScene = type;

   	switch (type) {
		/*
	case SCENECODE_TEMP:
		Director::getInstance()->pushScene(HelloWorld::createScene());
		break;
		*/
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
		mSell->stopAllActions();
		mSell->setScale(1);
		showInventory(inventoryType_all, true);
		break;
	case SCENECODE_ACHIEVEMENT: // 업적
		mAchievement->stopAllActions();
		mAchievement->setScale(1);
		showAchievement();
		break;
	case SCENECODE_BUY: //구매
		mBuy->stopAllActions();
		mBuy->setScale(1);
		showBuy();
		break;
	case SCENECODE_FARMING: //farming		
		mFarming->stopAllActions();
		mFarming->setScale(1);
		Director::getInstance()->pushScene(FarmingScene::createScene());
		break;
	case SCENECODE_RACE://RACE
		showRace();
		//runRace();
		break;
	case SCENECODE_INVENTORY: //inquiry inventory
		showInventory();
		//store2();
		break;
	case SCENECODE_POPUP_1:
		break;
	case SCENECODE_POPUP_2:
		//this->removeChild(mParitclePopupLayer);
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
//action touch 클릭 횟수
void MainScene::callback1(Ref* pSender){
	mActionTouchCnt++;
    CCLOG("callback1");
}

void MainScene::onEnter(){
    Scene::onEnter();
    CCLOG("onEnter!!!!!!!!!!!!!!!!!!!!!!!");
	
}
void MainScene::onEnterTransitionDidFinish(){
    Scene::onEnterTransitionDidFinish();
    CCLOG("onEnterTransitionDidFinish!!!!!!!!!!!!!!!!!!!!!!!");
	updateState(false);
	mCurrentScene = SCENECODE_MAIN;
}
void MainScene::onExitTransitionDidStart(){
    Scene::onExitTransitionDidStart();
    CCLOG("onExitTransitionDidStart\n");
}
void MainScene::onExit(){
    Scene::onExit();
    CCLOG("onExit\n");
}

void MainScene::dailyReward() {
    auto size = Size(400,200);
    auto margin = Size(10, 10);
    Size nodeSize = Size(50, 50);
    Size gridSize = Size(1, 3);

    this->removeChild(layerGray);
    layer = gui::inst()->addPopup(layerGray, this, size, "bg_inventory.png", Color4B::WHITE);
	layerGray->setLocalZOrder(ZORDER_POPUP);
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

    ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(8, 0), size, Size(-1, -1), Size(-1, -1), margin, "", Size(200, 280));

    for(int n=0; n < 20; n++){

		string img = getItemImg(n);/*"items/";
        img += to_string(n+1);
        img += ".png";
		*/
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
		alert(err);
	}
	else {
		closePopup();
		updateState(true);
	}	
}

void MainScene::sellItem(Ref* pSender) {
	errorCode err = logics::hInst->runTrade(false, mQuantityItemId, mQuantity);
	if (err != error_success && err != error_levelup) {
		alert(err);
	}
	else {
		updateState(false);
		closePopup();
	}
}

void MainScene::showInventoryCategory(Ref* pSender, inventoryType code, bool isSell) {
	INVENTORY_SIZE;
	
	vector<intPair> vec;
	logics::hInst->getActor()->inven.getWarehouse(vec, (int)code);

	int newLine = 2;
	if (isSell)
		gridSize.height++;

	Vec2 rect1, rect2;
	rect1 = Vec2(0, 7);

	if (isSell) {
		rect2 = Vec2(7, 1);
		POPUP_LIST(layer
			, gridSize
			, newLine
			, rect1
			, rect2
			, margin
			, nodeMargin
			, nodeSize
			, (int n = 0; n < (int)vec.size(); n++)
			,
			, getItemImg(vec[n].key)
			, CC_CALLBACK_1(MainScene::selectCallback, this, logics::hInst->getItem(vec[n].key).id)
			, "Lv." + to_string(logics::hInst->getItem(vec[n].key).grade)
			, wstring_to_utf8(logics::hInst->getItem(vec[n].key).name)
			, "x " + to_string(vec[n].val)
			, COIN + to_string(logics::hInst->getTrade()->getPriceSell(vec[n].key))
			, gui::inst()->EmptyString
		)
	}
	else {
		rect2 = Vec2(9, 1);
		POPUP_LIST(layer
			, gridSize
			, newLine
			, rect1
			, rect2
			, margin
			, nodeMargin
			, nodeSize
			, (int n = 0; n < (int)vec.size(); n++)
			, 
			, getItemImg(vec[n].key)
			, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_NONE)
			, "Lv." + to_string(logics::hInst->getItem(vec[n].key).grade)
			, wstring_to_utf8(logics::hInst->getItem(vec[n].key).name)
			, "x " + to_string(vec[n].val)
			, gui::inst()->EmptyString
			, gui::inst()->EmptyString
		)
	}

	return;
}

void MainScene::showInventory(inventoryType type, bool isSell) {
#define __PARAMS(STR, ID) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showInventoryCategory, this, ID, isSell), 14, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	
	INVENTORY_SIZE;
	closePopup();
	//this->removeChild(layerGray);
	layer = gui::inst()->addPopup(layerGray, this, size
		, isSell ? BG_SELL : BG_INVENTORY
		, Color4B::WHITE);
	layerGray->setLocalZOrder(ZORDER_POPUP);
	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 12, ALIGNMENT_CENTER, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);
	int nMenuIdx = 0;
	//tab
	gui::inst()->addTextButtonAutoDimension(__PARAMS("ALL", inventoryType_all));
	//gui::inst()->addTextButtonAutoDimension(__PARAMS("Race", inventoryType_race));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Grow", inventoryType_growth));
	//gui::inst()->addTextButtonAutoDimension(__PARAMS("Farm", inventoryType_farming));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("HP", inventoryType_HP));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Beauty", inventoryType_adorn));
	
	if (isSell) {
		mIsSell = true;
		mQuantityItemId = -1;
		auto time = gui::inst()->addLabelAutoDimension(nMenuIdx, 0, getTradeRemainTime(), layer, 8, ALIGNMENT_NONE, Color3B::GRAY
			, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);
		time->setAnchorPoint(Vec2(0, 0));
		time->setPosition(Vec2(margin.width, 0));
		
		//수량 추가
		gui::inst()->addQuantityLayer(layer, layer->getContentSize(), margin, mQuantityImg, mQuantityTitle, mQuantityLabel, mQuantityPrice
			, wstring_to_utf8(L"판매")
			, CC_CALLBACK_1(MainScene::quantityCallback, this, -1)
			, CC_CALLBACK_1(MainScene::quantityCallback, this, 1)
			, CC_CALLBACK_1(MainScene::sellItem, this)
		);
	}
	
	showInventoryCategory(this, type, isSell);
}

void MainScene::quantityCallback(Ref* pSender, int value) {
	if (mQuantity <= 1 && value < 0)
		return;

	mQuantity += value;
	mQuantityLabel->setString(to_string(mQuantity));
	_item item = logics::hInst->getItem(mQuantityItemId);	

	//판매 수량 체크
	if (mIsSell && !logics::hInst->getActor()->inven.checkItemQuantity(logics::hInst->getInventoryType(item.id), item.id, mQuantity)) {
		mQuantityPrice->setString(wstring_to_utf8(logics::hInst->getErrorMessage(error_invalid_quantity)));
		mQuantityPrice->setScale(0.5);
		return;
	}

	int price = mIsSell ? logics::hInst->getTrade()->getPriceSell(mQuantityItemId) : logics::hInst->getTrade()->getPriceBuy(mQuantityItemId);
	price = price * mQuantity;
	
	//구매시 보유 금액 초과
	if (!mIsSell && price > logics::hInst->getActor()->point) {
		mQuantityPrice->setString(wstring_to_utf8(logics::hInst->getErrorMessage(error_not_enough_point)));
		mQuantityPrice->setScale(0.5);
		return;
	}

	mQuantityPrice->setString(COIN + to_string(price));
	mQuantityPrice->setScale(1);
	
}

void MainScene::buyCallback(Ref* pSender) {
	if (mQuantityItemId == 0)
		return;
	
	errorCode err = logics::hInst->runTrade(true, mQuantityItemId, mQuantity);
	if (err != error_success) {
		alert(err);
	} else {
		closePopup();
		updateState(true);
	}
}

void MainScene::selectCallback(Ref* pSender, int id) {
	/*
	//quantity modal
	LayerColor * bg, *l2;
	l2 = gui::inst()->createModalLayer(bg, Size(100, 100));
	gui::inst()->addLabelAutoDimension(0, 0, "modal", l2, 0, ALIGNMENT_CENTER, Color3B::BLACK, Size(1, 1), Size::ZERO, Size::ZERO);
	this->addChild(bg);
	return;
	*/
	mQuantity = 1;
	mQuantityItemId = id;
	string szImg = getItemImg(id);/*"items/";
	szImg += to_string(id % 20) + ".png";
	*/
	_item item = logics::hInst->getItem(mQuantityItemId);
	mQuantityImg->setTexture(szImg);
	mQuantityImg->setContentSize(Size(20, 20));
	
	mQuantityTitle->setString(wstring_to_utf8(item.name));
	if (item.name.size() > 14) {
		mQuantityTitle->setScale(0.6);
	}
	else if (item.name.size() > 10) {
		mQuantityTitle->setScale(0.8);
	}
	else {
		mQuantityTitle->setScale(1);
	}

	quantityCallback(this, 0);
}

void MainScene::showBuyCategory(Ref* pSender, inventoryType type) {
	BUY_SIZE;
	//int nodeMargin = 2;
	int newLine = 2;
	
	trade::tradeMap * m = logics::hInst->getTrade()->get();
	
	POPUP_LIST(layer
		, gridSize
		, newLine
		, Vec2(0, 7)
		, Vec2(7, 1)
		, margin
		, nodeMargin
		, nodeSize
		, (trade::tradeMap::iterator it = m->begin(); it != m->end(); ++it)
		, if ((type != inventoryType_all && logics::hInst->getInventoryType(it->first) != type) || logics::hInst->getItem(it->first).type > itemType_max) continue;
		, getItemImg(it->first)
		, CC_CALLBACK_1(MainScene::selectCallback, this, logics::hInst->getItem(it->first).id)
		, "Lv." + to_string(logics::hInst->getItem(it->first).grade)
		, wstring_to_utf8(logics::hInst->getItem(it->first).name)
		, COIN + to_string(logics::hInst->getTrade()->getPriceBuy(it->first))
		, gui::inst()->EmptyString
		, gui::inst()->EmptyString
	)

	return;
}

void MainScene::showBuy(inventoryType type) {
#define __PARAMS_BUY(STR, ID) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showBuyCategory, this, ID), 14, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin

	BUY_SIZE;
	mIsSell = false;
	mQuantityItemId = -1;
	closePopup();
	layer = gui::inst()->addPopup(layerGray, this, size, BG_BUY, Color4B::WHITE);
	layerGray->setLocalZOrder(ZORDER_POPUP);
	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 12, ALIGNMENT_CENTER, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);
	int nMenuIdx = 0;
	//tab
	gui::inst()->addTextButtonAutoDimension(__PARAMS_BUY("ALL", inventoryType_all));
	//gui::inst()->addTextButtonAutoDimension(__PARAMS_BUY("Race", inventoryType_race));
	gui::inst()->addTextButtonAutoDimension(__PARAMS_BUY("Grow", inventoryType_growth));
	//gui::inst()->addTextButtonAutoDimension(__PARAMS_BUY("Farm", inventoryType_farming));
	gui::inst()->addTextButtonAutoDimension(__PARAMS_BUY("HP", inventoryType_HP));
	gui::inst()->addTextButtonAutoDimension(__PARAMS_BUY("Beauty", inventoryType_adorn));

	
	gui::inst()->addQuantityLayer(layer, layer->getContentSize(), margin, mQuantityImg, mQuantityTitle, mQuantityLabel, mQuantityPrice
		, wstring_to_utf8(L"구매")
		, CC_CALLBACK_1(MainScene::quantityCallback, this, -1)
		, CC_CALLBACK_1(MainScene::quantityCallback, this, 1)
		, CC_CALLBACK_1(MainScene::buyCallback, this)
	);
	auto time = gui::inst()->addLabelAutoDimension(nMenuIdx, 0, getTradeRemainTime(), layer, 8, ALIGNMENT_NONE, Color3B::GRAY
		, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);
	time->setAnchorPoint(Vec2(0, 0));
	time->setPosition(Vec2(margin.width, 0));
	
	showBuyCategory(this, type);
}

void MainScene::achievementCallback(Ref* pSender, Quest::_quest * p){
	/*
	logics::hInst->addInventory(p->rewardId, p->rewardValue);
	logics::hInst->getQuests()->rewardReceive(p);
	*/
	logics::hInst->achieveReward(p);
	updateState(true);
	callback2(this, SCENECODE_CLOSEPOPUP);
}

void MainScene::showAchievementCategory(Ref* pSender) {
	ACHIEVEMENT_SIZE;
	int newLine = 2;

	int cnt = logics::hInst->getQuests()->getQuests()->size();

	Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 7), Vec2(9, 1), layer->getContentSize(), Size(-1, -1), Size::ZERO, margin);
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - nodeMargin;	
	Size innerSize = Size(sizeOfScrollView.width, ((cnt / newLine) + 1) * (nodeSize.height + nodeMargin));
    ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 1), layer->getContentSize(), Size(-1, -1), Size::ZERO, margin, "", innerSize);
	
	int counting = 0;
	for (int n = 0; n < logics::hInst->getQuests()->getQuests()->size(); n++) {
		Quest::_quest * p = logics::hInst->getQuests()->getQuests()->at(n);
		if (p->isReceived)
			continue;

		Layout* l = gui::inst()->createLayout(nodeSize, "", true, Color3B::WHITE);
		
		string state = "";
		if (p->isFinished)
			state = "O";
		else if (p->isReceived)
			state = "V";
		else
			counting++;
				
		if (p->isFinished) {
			noticeEffect(
				gui::inst()->addTextButtonAutoDimension(0, 2, state, l
					, CC_CALLBACK_1(MainScene::achievementCallback, this, p), 24, ALIGNMENT_CENTER
					, Color3B::BLUE, gridSize, Size::ZERO, Size::ZERO)
			);
		}			

		int heightIdx = 1;

		gui::inst()->addLabelAutoDimension(1, heightIdx++, wstring_to_utf8(p->title), l, 12, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		string score = to_string(p->accumulation) + "/" + to_string(p->value);
		gui::inst()->addLabelAutoDimension(1, heightIdx++, score, l, 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		string reward = wstring_to_utf8(logics::hInst->getItem(p->rewardId).name + L" x" + to_wstring(p->rewardValue));
		gui::inst()->addLabelAutoDimension(1, heightIdx++, reward, l, 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		gui::inst()->addLayoutToScrollView(sv, l, nodeMargin, newLine);

		if (counting >= questCnt) break;
	}

	layer->removeChildByTag(CHILD_ID_ACHIEVEMENT, true);
	layer->addChild(sv, 1, CHILD_ID_ACHIEVEMENT);

}
void MainScene::showAchievement() {
#define __PARAMS_ACHIEVEMENT(STR, ISDAILY) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showAchievementCategory, this, ISDAILY), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	ACHIEVEMENT_SIZE;
	//this->removeChild(layerGray);
	closePopup();
	layer = gui::inst()->addPopup(layerGray, this, size, BG_ACHIEVEMENT, Color4B::WHITE);

	gui::inst()->addTextButtonAutoDimension(0, 0, wstring_to_utf8(L"전체수령"), layer
		, CC_CALLBACK_1(MainScene::recieveAllAchievement, this)
		, 0, ALIGNMENT_NONE, Color3B::BLUE, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 12, ALIGNMENT_NONE, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

	showAchievementCategory(this);
}

void MainScene::showCollection() {
#define __PARAMS_COLLECTION(STR, ISDAILY) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showAchievementCategory, this, ISDAILY), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	ACHIEVEMENT_SIZE;
	//this->removeChild(layerGray);
	closePopup();
	layer = gui::inst()->addPopup(layerGray, this, size, BG_COLLECTION, Color4B::WHITE);
	layerGray->setLocalZOrder(ZORDER_POPUP);
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

    Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 7), Vec2(9, 1), layer->getContentSize(), Size(-1, -1), Size::ZERO, margin);
    nodeSize.width = (sizeOfScrollView.width / (float)newLine) - nodeMargin;
	Size innerSize = Size(sizeOfScrollView.width, ((cnt / newLine) + 1) * (nodeSize.height + nodeMargin));
	ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 1), layer->getContentSize(), Size(-1, -1), Size::ZERO, margin, "", innerSize);

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


void MainScene::showActionCategory(Ref* pSender, int type) {
	ACTION_SIZE;
	//int nodeMargin = 2;
	int newLine = 2;
	__training * pTraining = logics::hInst->getActionList();

	POPUP_LIST(layer
		, gridSize
		, newLine
		, Vec2(0, 7)
		, Vec2(9, 1)
		, margin
		, nodeMargin
		, nodeSize
		, (__training::iterator it = pTraining->begin(); it != pTraining->end(); ++it)
		, if (logics::hInst->isValidTraining(it->first) == error_not_enough_level) continue; switch (type) { case 0: break; case 1: if (it->second.reward.strength == 0) continue; break; case 2: if (it->second.reward.intelligence == 0) continue; break; case 3: if (it->second.reward.appeal == 0) continue; break; default: break; }
		, getItemImg(it->first)
		, CC_CALLBACK_1(MainScene::callbackAction, this, it->first)
		, "Lv." + to_string(it->second.level)
		, wstring_to_utf8(it->second.name)
		, COIN + to_string(it->second.cost.point)
		, gui::inst()->EmptyString
		, gui::inst()->EmptyString
		)
}


void MainScene::actionList() {
#define __PARAMS_ACTION(STR, ID) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showActionCategory, this, ID), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	
	ACTION_SIZE;	
	int newLine = 2;   

	closePopup();
    layer = gui::inst()->addPopup(layerGray, this, size, BG_ACTION, Color4B::WHITE);
	layerGray->setLocalZOrder(ZORDER_POPUP);
    gui::inst()->addTextButtonAutoDimension(8,0
            ,"CLOSE"
            , layer
            , CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
            , 12
            , ALIGNMENT_CENTER
            , Color3B::RED
            , Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size::ZERO
            , margin
			//, "close.png"
    );

	int nMenuIdx = 0;

	gui::inst()->addTextButtonAutoDimension(__PARAMS_ACTION("ALL", 0));
	gui::inst()->addTextButtonAutoDimension(__PARAMS_ACTION(wstring_to_utf8(L"체력"), 1));
	gui::inst()->addTextButtonAutoDimension(__PARAMS_ACTION(wstring_to_utf8(L"지력"), 2));
	gui::inst()->addTextButtonAutoDimension(__PARAMS_ACTION(wstring_to_utf8(L"매력"), 3));
	
    showActionCategory(this, 0);
//    gui::inst()->drawGrid(layer, layer->getContentSize(), Size(-1, -1), Size::ZERO, Size::ZERO);
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

void MainScene::scheduleRecharge(float f) {
	if (logics::hInst->rechargeHP())
		updateState(false);
}


void MainScene::showRaceCategory(Ref* pSender, race_mode mode) {
	RACE_SIZE;

	int newLine = 1;

	int cnt = logics::hInst->getRaceModeCnt(mode);

    Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 7), Vec2(9, 1), layer->getContentSize(), Size(-1, -1), Size::ZERO, margin);
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - nodeMargin;
	Size innerSize = Size(sizeOfScrollView.width, ((cnt / newLine) + 1) * (nodeSize.height + nodeMargin));
	ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 7), Vec2(9, 1), layer->getContentSize(), Size(-1, -1), Size::ZERO, margin, "", innerSize);

	for (raceMeta::iterator it = logics::hInst->getRace()->begin(); it != logics::hInst->getRace()->end(); ++it) {
		int id = it->first;
		_race race = it->second;

		if (race.mode != mode)
			continue;

		string szIcon;
		switch (race.mode) {
		case race_mode_item:
			szIcon = wstring_to_utf8(L"ITEM");
			break;
		case race_mode_speed:
			szIcon = wstring_to_utf8(L"SPEED");
			break;
		case race_mode_1vs1:
			szIcon = wstring_to_utf8(L"1vs1");
			break;
		case race_mode_friend_1:
			szIcon = wstring_to_utf8(L"Friend");
			break;
		default:
			break;
		}

		bool isEnable = true;
		Color3B fontColor = Color3B::BLACK;
		//속성 부족 체크
		if (race.min * 0.8 > logics::hInst->getActor()->property.total()) {
			isEnable = false;
			fontColor = Color3B::GRAY;
		}

		//속성이 과한지 체크
		if (race.max < logics::hInst->getActor()->property.total())
			continue;

		Layout* l = gui::inst()->createLayout(nodeSize, "", true, Color3B::WHITE);
		int heightIdx = 1;

		auto btn1 = gui::inst()->addTextButtonAutoDimension(0, 1
			, getRomeNumber(race.level), l, CC_CALLBACK_1(MainScene::runRace, this, id), 12, ALIGNMENT_CENTER, fontColor, gridSize, Size::ZERO, Size::ZERO);
		if (!isEnable)
			btn1->setEnabled(false);

		auto btn2 = gui::inst()->addTextButtonAutoDimension(0, 3
			, szIcon, l, CC_CALLBACK_1(MainScene::runRace, this, id), 28, ALIGNMENT_CENTER, fontColor, gridSize, Size::ZERO, Size::ZERO);
		if (!isEnable)
			btn2->setEnabled(false);

		auto btn3 = gui::inst()->addTextButtonAutoDimension(1, heightIdx++
			, wstring_to_utf8(race.title), l, CC_CALLBACK_1(MainScene::runRace, this, id), 12, ALIGNMENT_NONE, fontColor, gridSize, Size::ZERO, Size::ZERO);
		if (!isEnable)
			btn3->setEnabled(false);

		auto btn4 = gui::inst()->addTextButtonAutoDimension(1, heightIdx++
			, COIN + to_string(race.fee), l, CC_CALLBACK_1(MainScene::runRace, this, id), 12, ALIGNMENT_NONE, fontColor, gridSize, Size::ZERO, Size::ZERO);
		if (!isEnable)
			btn4->setEnabled(false);

		for (int m = 0; m < (int)it->second.rewards.size(); m++) {
			/*
			printf("%d등 상금: %d (%ls 외 %d)\n"
			, m + 1
			, race.rewards[m].prize
			, mItems[it->second.rewards[m].items[0].itemId].name.c_str()
			, (int)it->second.rewards[m].items.size() - 1
			);
			*/
			string rewardPrefix;
			switch (m) {
			case 0:
				rewardPrefix = "1st: ";
				rewardPrefix += COIN;
				break;
			case 1:
				rewardPrefix = "2nd: ";
				rewardPrefix += COIN;
				break;
			default:
				rewardPrefix = to_string(m + 1) + "th: ";
				rewardPrefix += COIN;
				break;
			}
			auto btn = gui::inst()->addTextButtonAutoDimension(1
				, heightIdx++
				, rewardPrefix + to_string(logics::hInst->getRaceReward(id, m))
				, l
				, CC_CALLBACK_1(MainScene::runRace, this, id), 12, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
			if (!isEnable)
				btn->setEnabled(false);
		}

		if (!isEnable) {
			gui::inst()->addLabelAutoDimension(1, heightIdx++
				, wstring_to_utf8(logics::hInst->getErrorMessage(error_not_enough_property)), l, 12, ALIGNMENT_NONE, Color3B::RED, gridSize, Size::ZERO, Size::ZERO);
		}

		gui::inst()->addLayoutToScrollView(sv, l, nodeMargin, newLine);

	}

	layer->removeChildByTag(CHILD_ID_RACE, true);
	layer->addChild(sv, 1, CHILD_ID_RACE);
}

void MainScene::showRace() {
	RACE_SIZE;
	//this->removeChild(layerGray);
	closePopup();
	layer = gui::inst()->addPopup(layerGray, this, size, BG_RACE, Color4B::WHITE);
	layerGray->setLocalZOrder(ZORDER_POPUP);

	gui::inst()->addTextButtonAutoDimension(0, 0, "ITEM", layer
		, CC_CALLBACK_1(MainScene::showRaceCategory, this, race_mode_item)
		, 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

	gui::inst()->addTextButtonAutoDimension(1, 0, "SPEED", layer
		, CC_CALLBACK_1(MainScene::showRaceCategory, this, race_mode_speed)
		, 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

	gui::inst()->addTextButtonAutoDimension(2, 0, "1 vs 1", layer
		, CC_CALLBACK_1(MainScene::showRaceCategory, this, race_mode_1vs1)
		, 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

	gui::inst()->addTextButtonAutoDimension(3, 0, "FRIEND", layer
		, CC_CALLBACK_1(MainScene::showRaceCategory, this, race_mode_friend_1)
		, 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 12, ALIGNMENT_CENTER, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

    showRaceCategory(this, race_mode_item);
	
}

void MainScene::closePopup() {
	if (layerGray != NULL) {
		layerGray->removeChild(layer);
		this->removeChild(layerGray);

		delete layer;
		delete layerGray;
	}

	layer = NULL;
	layerGray = NULL;
	mCurrentScene = SCENECODE_MAIN;
}

void MainScene::updateQuests() {
	if (mLastUpdateQuest == logics::hInst->getQuests()->mLastUpdated)
		return;

	mLastUpdateQuest = logics::hInst->getQuests()->mLastUpdated;

	int cnt = 0;
	Color3B fontColor;
	
	for (int n = 0; n < mQuestButtons.size(); n++) {
//		mQuestButtons[n]->removeAllChildren();
		this->removeChild(mQuestButtons[n]);
	}
	mQuestButtons.clear();
	
	for (int n = 0; n < logics::hInst->getQuests()->getQuests()->size(); n++) {
		Quest::_quest * p = logics::hInst->getQuests()->getQuests()->at(n);
		if (p->isFinished || p->isReceived)
			continue;
			
		//wstring sz = p->title + L" " + to_wstring(p->accumulation) + L"/" + to_wstring(p->value);
        string sz = " - " + wstring_to_utf8(p->title);
        sz += " " + to_string(p->accumulation) + "/" + to_string(p->value);
		//if (p->accumulation >= p->value) sz = L"COMPLETE";
		Menu * pMenu = NULL;
		auto q = gui::inst()->addTextButtonRaw(pMenu, 0, 3, sz, this
			, CC_CALLBACK_1(MainScene::callback2, this, getSceneCodeFromQuestCategory(p->category)), 10, ALIGNMENT_NONE);
		q->setPosition(q->getPosition().x, q->getPosition().y - (cnt * 15));
		q->setLocalZOrder(ZORDER_QUEST);
		mQuestButtons.push_back(pMenu);

		cnt++;

		if (cnt >= questCnt)
			return;
	}
}

string MainScene::getItemImg(int id) {
	_item item = logics::hInst->getItem(id);
	if (item.type == itemType_farming) {
		return "fruit/" + to_string(id - itemType_farming) + ".png";
	}
	if (item.type == itemType_hp_meal) {
		return "fruit/" + to_string(id - itemType_hp) + ".png";
	}

	return "items/" + to_string(id % 20) + ".png";
}

SCENECODE MainScene::getSceneCodeFromQuestCategory(int category) {
	SCENECODE code = SCENECODE_NONE;
	switch ((achievement_category)category) {
	case achievement_category_training:
	case achievement_category_property:	//능력치
		code = SCENECODE_ACTION;
		break;
	case achievement_category_trade_buy:
		code = SCENECODE_BUY;
		break;
	case achievement_category_trade_sell:
		code = SCENECODE_SELL;
		break;
	case achievement_category_recharge: //체력 충전
		code = SCENECODE_RECHARGE;
		break;
	case achievement_category_farming:   // 농사 전체
		code = SCENECODE_FARMING;
		break;
	case achievement_category_race: //경묘 전체
	case achievement_category_race_use_item: // 경묘 아이템 사용
	case achievement_category_race_use_item_type: // 경묘 아이템 사용 타입
	case achievement_category_race_item: // 경묘 아이템 모드
	case achievement_category_race_speed: // 경묘 스피드 모드
	case achievement_category_race_1vs1: //경묘 1:1
	case achievement_category_race_friend_1: // 경묘 friend 모드
		code = SCENECODE_RACE;
		break;
	default:
		break;
	}
	return code;
}
