//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "ActionScene.h"
#include "AlertScene.h"
#include "FarmingScene.h"
#include "DecoScene.h"
#include "ActionBasic.h"
#include "LotteryScene.h"
#include "AdvertisementScene.h"
#include "ui/ui_color.h"

using namespace cocos2d::ui;

#define PARTICLE_FINAL "particles/particle_finally.plist"
#define PARTICLE_MAGIC "particles/particle_magic.plist"

#define INVENTORY_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(120, 65); auto gridSize = Size(3, 4);
#define BUY_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(120, 60); auto gridSize = Size(3, 4);
#define ACHIEVEMENT_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(178, 70); auto gridSize = Size(3, 5);
#define RACE_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(178, 90); auto gridSize = Size(3, 5);
#define ACTION_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size::ZERO; auto nodeSize = Size(178, 70); auto gridSize = Size(3, 4);

#define TAG_DECO_MENU 1000

#define nodeMargin 8

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
	
	//logics
	if (!logics::hInst->init(MainScene::farmingCB, MainScene::tradeCB, MainScene::achievementCB, false)) {
		CCLOG("logics Init Failure !!!!!!!!!!!!!!");
		return false;
	}
    
    mLastUpdatePlayBonus = getNow();
		
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

//    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //touch
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(MainScene::onTouchesBegan, this);
    listener->onTouchesCancelled = CC_CALLBACK_2(MainScene::onTouchesCancelled, this);
    listener->onTouchesEnded = CC_CALLBACK_2(MainScene::onTouchesEnded, this);
    listener->onTouchesMoved = CC_CALLBACK_2(MainScene::onTouchesMoved, this);
//    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
	Color3B fontColor = Color3B::BLACK;
	//BG
	
	auto bg = Sprite::create("bg_temp.png");
	bg->setContentSize(Director::getInstance()->getVisibleSize());
	bg->setAnchorPoint(Vec2(0, 0));
	//bg->setOpacity(50);
	bg->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(bg);
	/**/
    mGrid.init("fonts/Goyang.ttf", 14, Color4F::WHITE);

    //Deco init
    if (!initDeco()) {
        CCLOG("Init Deco Failure !!!!!!!!!!!!!!");
        return false;
    }
    
	//farming init
	if (!initFarm()) {
		CCLOG("Init Farm Failure !!!!!!!!!!!!!!");
		return false;
	}
    
    //Cat Main UI
    
    loadingBar = gui::inst()->addProgressBar(4, 0, LOADINGBAR_IMG, this, 150, 10);
    
    //string name = wstring_to_utf8(logics::hInst->getActor()->name, true);
	string name = logics::hInst->getActor()->name;
    name += " lv.";
    name += to_string(logics::hInst->getActor()->level);

    mName = gui::inst()->addLabel(0, 0, name, this, 12, ALIGNMENT_NONE, fontColor);

    //mJobTitle = gui::inst()->addLabel(4,0,wstring_to_utf8(logics::hInst->getActor()->jobTitle, true), this, 12, ALIGNMENT_CENTER, fontColor);
	mJobTitle = gui::inst()->addLabel(4, 0, logics::hInst->getActor()->jobTitle, this, 12, ALIGNMENT_CENTER, fontColor);

	mJobTitle->setPosition(Vec2(mJobTitle->getPosition().x, mJobTitle->getPosition().y + 15));

//    gui::inst()->addTextButton(0, 5, "꾸미기", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_DECORATION), 0, ALIGNMENT_CENTER, fontColor);
    gui::inst()->addTextButton(0, 6, logics::hInst->getL10N("MAINMENU_ACTION"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACTION), 0, ALIGNMENT_CENTER, fontColor);
	gui::inst()->addTextButton(1, 6, logics::hInst->getL10N("MAINMENU_RACE"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RACE), 0, ALIGNMENT_CENTER, fontColor);
    mFarming = gui::inst()->addTextButton(2,6, logics::hInst->getL10N("MAINMENU_FARM"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_FARMING), 0, ALIGNMENT_CENTER, fontColor);
	

    mExp = gui::inst()->addLabel(4, 0, "", this, 12, ALIGNMENT_CENTER);	    
    mHP = gui::inst()->addTextButton(7, 0, "♥", this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_RECHARGE), 0, ALIGNMENT_CENTER, Color3B::ORANGE);
	mPoint = gui::inst()->addTextButton(8, 0, COIN, this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_PURCHASE), 0, ALIGNMENT_CENTER, Color3B::GRAY);

    mProperties = gui::inst()->addLabel(8, 2, "", this, 12, ALIGNMENT_CENTER, fontColor);
	   
	//mAchievement = gui::inst()->addTextButton(0, 2, wstring_to_utf8(L"업적"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_ACHIEVEMENT), 0, ALIGNMENT_CENTER, fontColor);
//    gui::inst()->addTextButton(8, 4, wstring_to_utf8(L"도감"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_COLLECTION), 0, ALIGNMENT_CENTER, fontColor);
    
    mSell = gui::inst()->addTextButton(6, 6, logics::hInst->getL10N("MAINMENU_SELL"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_SELL), 0, ALIGNMENT_CENTER, fontColor);
    mInventory = gui::inst()->addTextButton(7, 6, logics::hInst->getL10N("MAINMENU_INVENTORY"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_INVENTORY), 0, ALIGNMENT_CENTER, fontColor);
    mBuy = gui::inst()->addTextButton(8, 6, logics::hInst->getL10N("MAINMENU_BUY"), this, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_BUY), 0, ALIGNMENT_CENTER, fontColor);
    

	//quest 표시
	updateQuests();
    
	// mLevel = logics::hInst->getActor()->level;

    updateState(false);
	this->schedule(schedule_selector(MainScene::scheduleRecharge), 1); //HP recharge schedule
    /*
	//퀘스트 타이머. 퀘스트 정산이 1초마다 되기 때문에 싱크가 잘 안맞아서 어쩔 수 없다.
	this->schedule([=](float delta) {		
		this->updateQuests();
		mLevel = logics::hInst->getActor()->level;
	}, 0.5, "questTimer");
     */

//    CCLOG("Init Done !!!!!!!!!!!!!!");
   
    return true;
}
int MainScene::initDecoObject(const char * sz, ui_deco::SIDE side){
    int ret = 0;
    if(sz){
        rapidjson::Document d;
        d.Parse(sz);
        if (d.HasParseError())
            return -1;
        
        for (rapidjson::SizeType i = 0; i < d.Size(); i++)
        {
            int id = d[rapidjson::SizeType(i)]["id"].GetInt();
            int idx = d[rapidjson::SizeType(i)]["idx"].GetInt();
            bool isFlipped = d[rapidjson::SizeType(i)]["flipped"].GetBool();
            
            auto sprite = (id != -1) ? Sprite::create(getItemImg(id)) : getIdle();
            sprite->setFlippedX(isFlipped);
            if(isFlipped)
                sprite->setAnchorPoint(Vec2(0, 0));
            else
                sprite->setAnchorPoint(Vec2(1, 0));
            
            if (side == ui_deco::SIDE_BOTTOM) {
                if(id != -1)
                    sprite->setScale(mInteriorScale);
                else
                    gui::inst()->setScale(sprite, mTouchGrid.width * 2);
            }
            
            ui_deco::OBJECT obj(id, sprite, side, idx);
            ui_deco::inst()->addObject(obj);
            ret ++;
        }
    }
    
    return ret;
}

bool MainScene::initDeco() {
    const float degrees = 27.f;
    const float layerWidth = gui::inst()->getTanLen(Director::getInstance()->getVisibleSize().height / 2, degrees) * 2;
    mMainLayoput = gui::inst()->createLayout(Size(layerWidth, Director::getInstance()->getVisibleSize().height), "", false, Color3B::GRAY);
    const float _div = 40;
    ui_deco::inst()->init(mMainLayoput, degrees, false, false);
   
    Color4F colors[4] = {
        ui_color::inst()->getColor4F(0, 0)
        , ui_color::inst()->getColor4F(1, 0)
        , ui_color::inst()->getColor4F(2, 0)
        , ui_color::inst()->getColor4F(3, 0)
    };
    
    sqlite3_stmt * stmt = Sql::inst()->select("SELECT colors, wallLeft, wallRight, bottom, wallParttern FROM actor WHERE idx = 1");
    if (stmt == NULL)
        return false;
    
    int result = sqlite3_step(stmt);
    if (result == SQLITE_ROW){
        int idx = 0;
        const char* sz = (const char*)sqlite3_column_text(stmt, idx++); //colors
        if(sz){
            rapidjson::Document d;
            d.Parse(sz);
            if (d.HasParseError())
                return false;
            
            for (rapidjson::SizeType i = 0; i < d.Size(); i++)
            {
                if(d[rapidjson::SizeType(i)].HasMember("r")){
                    colors[i].r = d[rapidjson::SizeType(i)]["r"].GetFloat();
                    colors[i].g = d[rapidjson::SizeType(i)]["g"].GetFloat();
                    colors[i].b = d[rapidjson::SizeType(i)]["b"].GetFloat();
                    colors[i].a = d[rapidjson::SizeType(i)]["a"].GetFloat();
                }
            }
        }
        
        ui_deco::inst()->addWall(_div/ 8, colors[0], colors[1]);
        ui_deco::inst()->addBottom(_div, _div / 5, colors[2], colors[3]);
        ui_deco::inst()->drawGuideLine();
        mTouchGrid = ui_deco::inst()->getBottomGridSize(); //Size(gui::inst()->getTanLen(fH, degrees) * 2, fH * 2);
        mInteriorScale = mTouchGrid.width / Sprite::create("home/00.png")->getContentSize().width;
        
        
        sz = (const char*)sqlite3_column_text(stmt, idx++); //wallLeft
        initDecoObject(sz, ui_deco::SIDE_LEFT);
        sz = (const char*)sqlite3_column_text(stmt, idx++); //wallRight
        initDecoObject(sz, ui_deco::SIDE_RIGHT);
        sz = (const char*)sqlite3_column_text(stmt, idx++); //bottom
        if(initDecoObject(sz, ui_deco::SIDE_BOTTOM) <= 0){
            auto sprite = getIdle();
            sprite->setAnchorPoint(Vec2(1, 0));
            gui::inst()->setScale(sprite, mTouchGrid.width * 2);
            
            ui_deco::OBJECT obj(-1, sprite, ui_deco::SIDE_BOTTOM, ui_deco::inst()->getDefaultBottomIdx());
            ui_deco::inst()->addObject(obj);
        }
        
        sz = (const char*)sqlite3_column_text(stmt, idx++); //wallParttern
        if(sz != NULL && strlen(sz) > 1)
            ui_deco::inst()->addWallParttern(sz);
        
    }
    //-------------------------------------------------
    //    gui::inst()->drawGrid(mMainLayoput, mMainLayoput->getContentSize(), mTouchGrid, Size::ZERO, Size::ZERO);
    
    Vec2 posMainLayer = gui::inst()->getCenter();
    posMainLayer.y  += mMainLayoput->getContentSize().height /3;
    mMainLayoput->setPosition(posMainLayer);
    mMainLayoput->setAnchorPoint(Vec2(0.5, 0.5));
    this->addChild(mMainLayoput);
    //---------------------------------------------------------------------------
    return true;
}

bool MainScene::initFarm() {
	Vec2 a1 = gui::inst()->getPointVec2(0, 0, ALIGNMENT_NONE);
	Vec2 a2 = gui::inst()->getPointVec2(1, 1, ALIGNMENT_NONE);
//    Size gridSize = Size(a2.x - a1.x, a1.y - a2.y);
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
    properties += logics::hInst->getL10N("PROPERTY_S") + " ";
    properties += to_string(logics::hInst->getActor()->property.strength);
    properties += "\n" + logics::hInst->getL10N("PROPERTY_I") + " ";
    properties += to_string(logics::hInst->getActor()->property.intelligence);
    properties += "\n" + logics::hInst->getL10N("PROPERTY_A") + " ";
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
    //szPoint += to_string(logics::hInst->getActor()->point);
    szPoint += numberFormat(logics::hInst->getActor()->point);
    //szPoint += " +";
    if(szPoint.compare(mPoint->getString()) != 0 ){
        mPoint->runAction(gui::inst()->createActionFocus());
    }
    mPoint->setString(szPoint);

	if(isInventoryUpdated)
		mInventory->runAction(gui::inst()->createActionFocus());
}
/*
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
*/

void MainScene::callbackAction(Ref* pSender, int id){
	if (id == -1)
		return;
    
    closePopup();

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
    case SCENECODE_DECORATION:
        Director::getInstance()->pushScene(DecoScene::createScene());
        break;
	default:
		break;
	}

}

/*
//action touch 클릭 횟수
void MainScene::callback1(Ref* pSender){
	mActionTouchCnt++;
    CCLOG("callback1");
}
 */

void MainScene::onEnter(){
    Scene::onEnter();
    CCLOG("onEnter!!!!!!!!!!!!!!!!!!!!!!!");
	
}
void MainScene::onEnterTransitionDidFinish(){
    Scene::onEnterTransitionDidFinish();
    CCLOG("onEnterTransitionDidFinish!!!!!!!!!!!!!!!!!!!!!!!");
	updateState(false);
    updateQuests();
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
            , getItemColor(logics::hInst->getItem(vec[n].key).id)
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
			, CC_CALLBACK_1(MainScene::applyInventory, this, vec[n].key)
			, "Lv." + to_string(logics::hInst->getItem(vec[n].key).grade)
			, wstring_to_utf8(logics::hInst->getItem(vec[n].key).name) + " x " + to_string(vec[n].val)
			, "Apply"
			, gui::inst()->EmptyString
			, gui::inst()->EmptyString
            , getItemColor(logics::hInst->getItem(vec[n].key).id)
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
//        , isSell ? BG_SELL : BG_INVENTORY
                                  , "", isSell ? ui_color::inst()->getColor4B(0, 0): ui_color::inst()->getColor4B(1, 0));
	layerGray->setLocalZOrder(ZORDER_POPUP);
	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 12, ALIGNMENT_CENTER, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);
	int nMenuIdx = 0;
    //tab
    //scrollview 가 먼저 눌리는건 Menu의 localZorder값이 낮아서 그럼. 보튼 생성 함수를 고쳐야 함.
    auto pMenu = Menu::create();
    pMenu->addChild(MenuItemFont::create("ALL", CC_CALLBACK_1(MainScene::showInventoryCategory, this, inventoryType_all, isSell)));
    pMenu->addChild(MenuItemFont::create("Lottery", CC_CALLBACK_1(MainScene::showInventoryCategory, this, inventoryType_lottery, isSell)));
    pMenu->addChild(MenuItemFont::create("Interior", CC_CALLBACK_1(MainScene::showInventoryCategory, this, inventoryType_interior, isSell)));
    pMenu->addChild(MenuItemFont::create("Exterior", CC_CALLBACK_1(MainScene::showInventoryCategory, this, inventoryType_exterior, isSell)));
    pMenu->addChild(MenuItemFont::create("Wall", CC_CALLBACK_1(MainScene::showInventoryCategory, this, inventoryType_wall, isSell)));
    pMenu->addChild(MenuItemFont::create("Pattern", CC_CALLBACK_1(MainScene::showInventoryCategory, this, inventoryType_wall_pattern, isSell)));
    pMenu->addChild(MenuItemFont::create("Bottom", CC_CALLBACK_1(MainScene::showInventoryCategory, this, inventoryType_bottom, isSell)));
    //    pMenu->addChild(MenuItemFont::create("Beauty", CC_CALLBACK_1(MainScene::showInventoryCategory, this, inventoryType_adorn, isSell)));
    pMenu->addChild(MenuItemFont::create("HP", CC_CALLBACK_1(MainScene::showInventoryCategory, this, inventoryType_HP, isSell)));
    
    pMenu->alignItemsHorizontally();
    Vec2 pos = gui::inst()->getPointVec2(4, 0, ALIGNMENT_CENTER, layer->getContentSize(), Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);
    pMenu->setPosition(pos);
    pMenu->setLocalZOrder(100);
    layer->addChild(pMenu);
    /*
	//tab
	gui::inst()->addTextButtonAutoDimension(__PARAMS("ALL", inventoryType_all));
	//gui::inst()->addTextButtonAutoDimension(__PARAMS("Race", inventoryType_race));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Grow", inventoryType_growth));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Wall", inventoryType_wall));
    gui::inst()->addTextButtonAutoDimension(__PARAMS("Bottom", inventoryType_bottom));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Interior", inventoryType_interior));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Exterior", inventoryType_exterior));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("Beauty", inventoryType_adorn));
	gui::inst()->addTextButtonAutoDimension(__PARAMS("HP", inventoryType_HP));
	*/
	if (isSell) {
		mIsSell = true;
		mQuantityItemId = -1;
		auto time = gui::inst()->addLabelAutoDimension(nMenuIdx, 0, getTradeRemainTime(), layer, 8, ALIGNMENT_NONE, Color3B::GRAY
			, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);
		time->setAnchorPoint(Vec2(0, 0));
		time->setPosition(Vec2(margin.width, 0));
		
		//수량 추가
		mQuantityLayout = gui::inst()->addQuantityLayer(layer, layer->getContentSize(), margin, mQuantityImg, mQuantityTitle, mQuantityLabel, mQuantityPrice
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
        logics::hInst->saveActor();
		closePopup();
		updateState(true);
	}
}

void MainScene::selectCallback(Ref* pSender, int id) {
    _item item = logics::hInst->getItem(id);
    
    //광고 상품
    if(item.isAdvertisementPayment) {
        closePopup();
        Director::getInstance()->pushScene(AdvertisementScene::createScene(item.id, 1));
        return;
    }
    
    //인게임캐시 상품
	mQuantity = 1;
    mQuantityItemId = id;
    
    
    mQuantityLayout->removeChildByTag(CHILD_ID_QUANTITY_COLOR);
    mQuantityImg->setTexture("blank.png");
    LayerColor * colorLayer = getItemColor(id);
    if(colorLayer){
        colorLayer->setContentSize(Size(20, 20));
        Vec2 pos = mQuantityImg->getPosition();
        pos.x -= colorLayer->getContentSize().width / 2;
        pos.y -= colorLayer->getContentSize().height / 2;
        colorLayer->setPosition(pos);
        colorLayer->setTag(CHILD_ID_QUANTITY_COLOR);
        
        mQuantityLayout->addChild(colorLayer);
        
    }else{
        mQuantityImg->setTexture(getItemImg(id));
        gui::inst()->setScale(mQuantityImg, 20);
    }
	
	mQuantityTitle->setString(wstring_to_utf8(item.name));
	if (mQuantityTitle->getStringLength() > 14) {
		mQuantityTitle->setScale(0.6);
	}
	else if (mQuantityTitle->getStringLength() > 10) {
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
        , logics::hInst->getItem(it->first).isAdvertisementPayment ? logics::hInst->getL10N("ADVERTISEMENT_WATCH") : COIN + numberFormat(logics::hInst->getTrade()->getPriceBuy(it->first))
		, gui::inst()->EmptyString
		, gui::inst()->EmptyString
        , getItemColor(logics::hInst->getItem(it->first).id)
	)

	return;
}

void MainScene::showBuy(inventoryType type) {
#define __PARAMS_BUY(STR, ID) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showBuyCategory, this, ID), 14, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin

	BUY_SIZE;
    
    if(type == inventoryType_all)
        type = inventoryType_lottery;
    
	mIsSell = false;
	mQuantityItemId = -1;
	closePopup();
	layer = gui::inst()->addPopup(layerGray, this, size, "", ui_color::inst()->getColor4B(2, 0));
	layerGray->setLocalZOrder(ZORDER_POPUP);
    showBuyCategory(this, type);
    
	gui::inst()->addTextButtonAutoDimension(8, 0, "CLOSE", layer
		, CC_CALLBACK_1(MainScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 12, ALIGNMENT_CENTER, Color3B::RED, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);
	int nMenuIdx = 0;
	//tab
    //scrollview 가 먼저 눌리는건 Menu의 localZorder값이 낮아서 그럼. 보튼 생성 함수를 고쳐야 함.
    auto pMenu = Menu::create();
    pMenu->addChild(MenuItemFont::create("Lottery", CC_CALLBACK_1(MainScene::showBuyCategory, this, inventoryType_lottery)));
    pMenu->addChild(MenuItemFont::create("Interior", CC_CALLBACK_1(MainScene::showBuyCategory, this, inventoryType_interior)));
    pMenu->addChild(MenuItemFont::create("Exterior", CC_CALLBACK_1(MainScene::showBuyCategory, this, inventoryType_exterior)));
    pMenu->addChild(MenuItemFont::create("Wall", CC_CALLBACK_1(MainScene::showBuyCategory, this, inventoryType_wall)));
    pMenu->addChild(MenuItemFont::create("Pattern", CC_CALLBACK_1(MainScene::showBuyCategory, this, inventoryType_wall_pattern)));
    pMenu->addChild(MenuItemFont::create("Bottom", CC_CALLBACK_1(MainScene::showBuyCategory, this, inventoryType_bottom)));
//    pMenu->addChild(MenuItemFont::create("Beauty", CC_CALLBACK_1(MainScene::showBuyCategory, this, inventoryType_adorn)));
    pMenu->addChild(MenuItemFont::create("HP", CC_CALLBACK_1(MainScene::showBuyCategory, this, inventoryType_HP)));
    
    pMenu->alignItemsHorizontally();
    Vec2 pos = gui::inst()->getPointVec2(4, 0, ALIGNMENT_CENTER, layer->getContentSize(), Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);
    pMenu->setPosition(pos);
    pMenu->setLocalZOrder(100);
    layer->addChild(pMenu);
    
	mQuantityLayout = gui::inst()->addQuantityLayer(layer, layer->getContentSize(), margin, mQuantityImg, mQuantityTitle, mQuantityLabel, mQuantityPrice
		, wstring_to_utf8(L"구매")
		, CC_CALLBACK_1(MainScene::quantityCallback, this, -1)
		, CC_CALLBACK_1(MainScene::quantityCallback, this, 1)
		, CC_CALLBACK_1(MainScene::buyCallback, this)
	);
	auto time = gui::inst()->addLabelAutoDimension(nMenuIdx, 0, getTradeRemainTime(), layer, 8, ALIGNMENT_NONE, Color3B::GRAY
		, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);
	time->setAnchorPoint(Vec2(0, 0));
	time->setPosition(Vec2(margin.width, 0));
	
	
}

void MainScene::achievementCallback(Ref* pSender, Quest::_quest * p){
	/*
	logics::hInst->addInventory(p->rewardId, p->rewardValue);
	logics::hInst->getQuests()->rewardReceive(p);
	*/
	logics::hInst->achieveReward(p);
	updateState(true);
    updateQuests();
}

void MainScene::showAchievementCategory(Ref* pSender) {
	ACHIEVEMENT_SIZE;
	int newLine = 2;

	int cnt = (int)logics::hInst->getQuests()->getQuests()->size();

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
	layer = gui::inst()->addPopup(layerGray, this, size, "", ui_color::inst()->getColor4B(3, 0));

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
	layer = gui::inst()->addPopup(layerGray, this, size, "", ui_color::inst()->getColor4B(4, 0));
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
               , if (logics::hInst->isValidTraining(it->first) == error_not_enough_level) continue; switch (type) { case 0: break; case 1: if (it->second.reward.strength == 0) continue; break; case 2: if (it->second.reward.intelligence == 0) continue; break; case 3: if (it->second.reward.appeal == 0) continue; break; default: break; } fontColor0 = Color3B::MAGENTA;
		, getItemImg(it->first)
		, CC_CALLBACK_1(MainScene::callbackAction, this, it->first)
        , gui::inst()->getStar((unsigned int)((float)logics::hInst->getHighScore(it->first) / (float)logics::hInst->getMaxScore(it->first) * 100.f))
		, wstring_to_utf8(it->second.name)
		, COIN + to_string(it->second.cost.point)
		, gui::inst()->EmptyString
		, gui::inst()->EmptyString
        , NULL
		)
}


void MainScene::actionList() {
#define __PARAMS_ACTION(STR, ID) nMenuIdx++, 0, STR, layer, CC_CALLBACK_1(MainScene::showActionCategory, this, ID), 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	
	ACTION_SIZE;	
	int newLine = 2;   

	closePopup();
    layer = gui::inst()->addPopup(layerGray, this, size, "", ui_color::inst()->getColor4B(5, 0));
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

void MainScene::scheduleRecharge(float f) {
	if (logics::hInst->rechargeHP())
		updateState(false);
    
    //복권 지급. 나중에 config로 뺴야햄.
    int diff = (int)(getNow() - mLastUpdatePlayBonus);
    if(diff > 25 * 60) {
        logics::hInst->addInventory(1, 1);
        auto mail = gui::inst()->addLabel(4, 5, logics::hInst->getL10N("MESSAGE_PLAY_BONUS"), this, 0, ALIGNMENT_CENTER, Color3B::ORANGE);
        mail->runAction(
                        Sequence::create(
                        FadeIn::create(1)
                        , FadeOut::create(2)
                        , NULL)
                        );
        updateState(true);
        mLastUpdatePlayBonus = getNow();
    }
    
}


void MainScene::showRaceCategory(Ref* pSender, race_mode mode) {
	RACE_SIZE;
	int newLine = 1;
    string reward_1st = gui::inst()->EmptyString;
    string reward_2nd = gui::inst()->EmptyString;
    string szNotEnoughProperty = gui::inst()->EmptyString;
    string img = "cat-hand1.png";
    
    POPUP_LIST(layer
               , gridSize
               , newLine
               , Vec2(0, 7)
               , Vec2(9, 1)
               , margin
               , nodeMargin
               , nodeSize
               , (raceMeta::iterator it = logics::hInst->getRace()->begin(); it != logics::hInst->getRace()->end(); ++it)
               ,
               if (it->second.mode != mode) continue;
               if (it->second.max < logics::hInst->getActor()->property.total()) continue;
               for (int m = 0; m < (int)it->second.rewards.size(); m++) {
                   switch (m) {
                       case 0:
                           reward_1st = "1st: ";
                           reward_1st += COIN + to_string(logics::hInst->getRaceReward(it->first, m));
                           break;
                       case 1:
                           reward_2nd = "2nd: ";
                           reward_2nd += COIN + to_string(logics::hInst->getRaceReward(it->first, m));
                           break;
                       default:
                           break;
                   }
               }
               
               if (it->second.min * 0.8 > logics::hInst->getActor()->property.total()) {
                   isEnable = false;
                   fontColor4 = Color3B::RED;
                   szNotEnoughProperty = wstring_to_utf8(logics::hInst->getErrorMessage(error_not_enough_property));
               } else{
                   isEnable = true;
                   szNotEnoughProperty = gui::inst()->EmptyString;
               }
               
               , img
               , CC_CALLBACK_1(MainScene::runRace, this, it->first)
               , "Lv." + to_string(it->second.level)
               , wstring_to_utf8(it->second.title)
               , reward_1st
               , reward_2nd
               , szNotEnoughProperty
               , NULL
            )
    
    
    return;
/*
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
 */
}

void MainScene::showRace() {
	RACE_SIZE;
	//this->removeChild(layerGray);
	closePopup();
	layer = gui::inst()->addPopup(layerGray, this, size, "", ui_color::inst()->getColor4B(6, 0));
	layerGray->setLocalZOrder(ZORDER_POPUP);

	gui::inst()->addTextButtonAutoDimension(0, 0, "ITEM", layer
		, CC_CALLBACK_1(MainScene::showRaceCategory, this, race_mode_item)
		, 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

	gui::inst()->addTextButtonAutoDimension(1, 0, "SPEED", layer
		, CC_CALLBACK_1(MainScene::showRaceCategory, this, race_mode_speed)
		, 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

	gui::inst()->addTextButtonAutoDimension(2, 0, "1 on 1", layer
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
    
    updateQuests();
}

void MainScene::updateQuests() {
    /*
	if (mLastUpdateQuest == logics::hInst->getQuests()->mLastUpdated)
		return;

	mLastUpdateQuest = logics::hInst->getQuests()->mLastUpdated;
     */
	int cnt = 0;
	Color3B fontColor;
	
	for (int n = 0; n < mQuestButtons.size(); n++) {
//		mQuestButtons[n]->removeAllChildren();
		this->removeChild(mQuestButtons[n]);
	}
	mQuestButtons.clear();
	
	for (int n = 0; n < logics::hInst->getQuests()->getQuests()->size(); n++) {
		Quest::_quest * p = logics::hInst->getQuests()->getQuests()->at(n);
		if (p->isReceived)
			continue;
        
        string sz = " - " + wstring_to_utf8(p->title);
        
        Menu * pMenu = NULL;
        MenuItemLabel * menuItem;
        
        if(p->isFinished){
            sz += " Done";
            menuItem = gui::inst()->addTextButtonRaw(pMenu, 0, 2, sz, this
                                                     , CC_CALLBACK_1(MainScene::achievementCallback, this, p), 12, ALIGNMENT_NONE, Color3B::BLUE);
            
        } else {
            sz += " " + to_string(p->accumulation) + "/" + to_string(p->value);
            menuItem = gui::inst()->addTextButtonRaw(pMenu, 0, 2, sz, this
                                                     , CC_CALLBACK_1(MainScene::callback2, this, getSceneCodeFromQuestCategory(p->category)), 12, ALIGNMENT_NONE);
        }
		
		
		menuItem->setPosition(menuItem->getPosition().x, menuItem->getPosition().y - (cnt * 15));
		menuItem->setLocalZOrder(ZORDER_QUEST);
		mQuestButtons.push_back(pMenu);

		cnt++;

		if (cnt >= questCnt)
			return;
	}
}

string MainScene::getItemImg(int id) {
	_item item = logics::hInst->getItem(id);
    switch(item.type){
        case itemType_farming:
            return "fruit/" + to_string(id - itemType_farming) + ".png";
        case itemType_hp_meal:
            return "fruit/" + to_string(id - itemType_hp) + ".png";
        case itemType_interior:
            return "home/0" + to_string(id - 2000) + ".png";
		case itemType_exterior:
            return "home/exterior/0" + to_string(id - 3000) + ".png";
        case itemType_wall_pattern:
            return "parttern/" + to_string(id - 4000) + ".png";
        default:
            return "items/" + to_string(id % 20) + ".png";
    }
}

LayerColor * MainScene::getItemColor(int id) {
    _item item = logics::hInst->getItem(id);
    
   if(item.type == itemType_wall || item.type == itemType_bottom) {
        LayerColor * layer = LayerColor::create();
        ui_color::COLOR_RGB color = ui_color::inst()->getColor(item.value);
        layer->setContentSize(Size(20, 20));
        if(color.R[1] == -1){
            auto l1 = LayerColor::create(Color4B(ui_color::inst()->getColor3B(item.value, 0)));
            l1->setContentSize(Size(layer->getContentSize().width, layer->getContentSize().height));
            
            layer->addChild(l1);
        }else{
            auto l1 = LayerColor::create(Color4B(ui_color::inst()->getColor3B(item.value, 0)));
            l1->setContentSize(Size(layer->getContentSize().width / 2, layer->getContentSize().height));
            
            layer->addChild(l1);
            
            auto l2 = LayerColor::create(Color4B(ui_color::inst()->getColor3B(item.value, 1)));
            l2->setContentSize(Size(layer->getContentSize().width / 2, layer->getContentSize().height));
            l2->setPosition(Vec2(layer->getContentSize().width / 2, 0));
            layer->addChild(l2);
        }
        
        return layer;
    }
    
    return NULL;
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

void MainScene::applyInventory(Ref* pSender, int itemId){
    closePopup();
    _item item = logics::hInst->getItem(itemId);
    bool isPop = true;
    if(item.type == itemType_wall){
        ui_color::COLOR_RGB color = ui_color::inst()->getColor(item.value);
        if(color.R[1] == -1){
            ui_deco::inst()->changeColorWall(ui_color::inst()->getColor4F(item.value, 0));
        }else{
            ui_deco::inst()->changeColorWall(ui_color::inst()->getColor4F(item.value, 0), ui_color::inst()->getColor4F(item.value, 1));
        }
    } else if(item.type == itemType_bottom){
        ui_color::COLOR_RGB color = ui_color::inst()->getColor(item.value);
        if(color.R[1] == -1){
            ui_deco::inst()->changeColorBottom(ui_color::inst()->getColor4F(item.value, 0));
        }else{
            ui_deco::inst()->changeColorBottom(ui_color::inst()->getColor4F(item.value, 0), ui_color::inst()->getColor4F(item.value, 1));
        }
    } else if(item.type == itemType_interior){
        auto img = Sprite::create(getItemImg(item.id));
        img->setScale(mInteriorScale);
        ui_deco::OBJECT obj(item.id, img, ui_deco::SIDE_BOTTOM, ui_deco::inst()->getDefaultBottomIdx());
        ui_deco::inst()->addObject(obj);
    } else if(item.type == itemType_exterior){
        auto img = Sprite::create(getItemImg(item.id));
        ui_deco::OBJECT obj(item.id, img, ui_deco::SIDE_LEFT, ui_deco::inst()->getDefaultLeftIdx());
        ui_deco::inst()->addObject(obj);
    } else if(item.type == itemType_wall_pattern){
        ui_deco::inst()->addWallParttern(getItemImg(item.id));
    } else if(item.category == itemCategory_lottery) {
        Director::getInstance()->pushScene(LotteryScene::createScene(item.id));
        isPop = false;
    }
    else {
        isPop = false;
    }
    
    if(isPop){
        inventoryType category = logics::hInst->getInventoryType(itemId);
        logics::hInst->getActor()->inven.popItem(category, itemId, 1);
        logics::hInst->saveActor();
    }
}

void MainScene::onTouchesBegan(const std::vector<Touch*>& touches, Event *event)
{
    Touch* touch;
    Vec2 touchPoint;
    for (int index = 0; index < touches.size(); index++)
    {
        touch = touches[index];
        touchPoint = touch->getLocation();
        int touchIndex = touch->getID();
        if(mTouchVec.size() < touchIndex + 1){
            mTouchVec.resize(touchIndex + 1);
        }
        mTouchVec[touchIndex] = touch;
    }

    if(getValidTouchCnt() == 1 && mTouchVec[0]){
        
        mTouchStart = mTouchVec[0]->getLocation();
        ui_deco::inst()->touchBegan(mTouchVec[0]->getLocation());
    }
    
    removeDecoMenu();
    
    mTouchGap = -1;
    return;
}
void MainScene::onTouchesCancelled(const std::vector<Touch*>& touches, Event *event)
{
    Touch* touch;
    Vec2 touchPoint;
    for (int index = 0; index < touches.size(); index++)
    {
        touch = touches[index];
        touchPoint = touch->getLocation();
        int touchIndex = touch->getID();
        
        if(mTouchVec.size() > touchIndex)
            mTouchVec[touchIndex] = NULL;
    }
}
void MainScene::onTouchesEnded(const std::vector<Touch*>& touches, Event *event)
{
    if(ui_deco::inst()->mTouchedInfo.side != ui_deco::SIDE_MAX
       && ui_deco::inst()->touchEnded(mTouchVec[0]->getLocation())
       && ui_deco::inst()->getLastObjectItemId() > 0
       ){
        createDecoMenu(mTouchVec[0]->getLocation());
    }
    
    Touch* touch;
    Vec2 touchPoint;
    for (int index = 0; index < touches.size(); index++)
    {
        touch = touches[index];
        touchPoint = touch->getLocation();
        int touchIndex = touch->getID();
        
        if(mTouchVec.size() > touchIndex)
            mTouchVec[touchIndex] = NULL;
    }
    
//    Vec2 locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
//    Size s = event->getCurrentTarget()->getContentSize();
//    Rect rect = Rect(0,0, s.width, s.height);
//
//    if (rect.containsPoint(locationInNode)) {
//        printf("listener1");
//        return;
//    }
}
void MainScene::onTouchesMoved(const std::vector<Touch*>& touches, Event *event)
{
    Touch* touch;
    Vec2 touchPoint;
    for (int index = 0; index < touches.size(); index++)
    {
        touch = touches[index];
        touchPoint = touch->getLocation();
        int touchIndex = touch->getID();
        if(mTouchVec.size() < touchIndex + 1){
            mTouchVec.resize(touchIndex + 1);
        }
        mTouchVec[touchIndex] = touch;
    }
    
    if(getValidTouchCnt() == 1 && mTouchVec[0]){
       
        if(mTouchStart.x == mTouchVec[0]->getLocation().x && mTouchStart.y == mTouchVec[0]->getLocation().y)
            return;
        
        Vec2 move = Vec2(mTouchStart.x - mTouchVec[0]->getLocation().x, mTouchStart.y - mTouchVec[0]->getLocation().y);
        
        if(ui_deco::inst()->mTouchedInfo.side != ui_deco::SIDE_MAX){
            ui_deco::inst()->touchMoved(mTouchVec[0]->getLocation());
        }else{
            Vec2 current = mMainLayoput->getPosition();
            Vec2 movedPoint = Vec2(current.x - move.x, current.y - move.y);
            
            mMainLayoput->setPosition(movedPoint);
        }
        mTouchStart = mTouchVec[0]->getLocation();
        
    }
    else if(getValidTouchCnt() == 2 && mTouchVec[0] && mTouchVec[1]){
        float gap = abs(mTouchVec[0]->getLocation().y - mTouchVec[1]->getLocation().y);
        
        if(mTouchGap == -1){
            mTouchGap = gap;
            return;
        } else if(mTouchGap == gap){
            return;
        }
        //mTouchGap : scale = gap :x
        //x = scale * gap / mTouchGap
        float currentScale = mMainLayoput->getScale();
        float ratio = (currentScale * gap / mTouchGap);
        float max = 1.5f;
        float min = 0.7f;
        if(ratio > max)
            ratio = max;
        else if(ratio < min)
            ratio = min;
        else if(ratio < 1.1f && ratio > 0.9f)
            ratio = 1;
       
        mMainLayoput->setScale(ratio);
    }
}

void MainScene::backToInventory(Ref* pSender){
    //to inven
    int itemId = ui_deco::inst()->getLastObjectItemId();
    if(itemId > 0){
        logics::hInst->addInventory(itemId, 1);
        ui_deco::inst()->removeLastObject();
        removeDecoMenu();
        updateState(true);
    }
    
}

void MainScene::flip(Ref* pSender){
    ui_deco::inst()->setFlipLastObject();
    removeDecoMenu();
}

void MainScene::removeDecoMenu() {
    if(this->getChildByTag(TAG_DECO_MENU))
        this->removeChildByTag(TAG_DECO_MENU);
}

void MainScene::createDecoMenu(Vec2 pos) {
    auto pMenu = Menu::create();
    auto item1 = MenuItemFont::create("가방", CC_CALLBACK_1(MainScene::backToInventory, this));
    auto item2 = MenuItemFont::create("회전", CC_CALLBACK_1(MainScene::flip, this));
    
    item1->setColor(Color3B::MAGENTA);
    item2->setColor(Color3B::MAGENTA);
    
    pMenu->addChild(item1);
    switch(ui_deco::inst()->getLastObjectSide()){
        case ui_deco::SIDE_BOTTOM:
            pMenu->addChild(item2);
            break;
        default:
            break;
    }
    
    pMenu->alignItemsVertically();
    pMenu->setPosition(pos);
    pMenu->setTag(TAG_DECO_MENU);
    this->addChild(pMenu);
}
