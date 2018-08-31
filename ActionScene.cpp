﻿//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "ActionScene.h"
#include "SimpleAudioEngine.h"

#define RACE_UPDATE_INTERVAL 0.3
#define RACE_MAX_TOUCH 200.f //초당 max 터치
#define RACE_DEFAULT_IMG "race/0.png"
#define RACE_GOAL_DISTANCE 2.5
#define RACE_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size(5, 10); auto nodeSize = Size(120, 50); auto gridSize = Size(3, 5);
#define POPUP_NODE_MARGIN  4

//#define RUNNER_WIDTH 80
Scene* ActionScene::createScene(int id)
{	
	return ActionScene::create(id);
}

bool ActionScene::init() {		

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(ActionScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(ActionScene::onTouchEnded, this);
	/*
	listener->onTouchMoved = CC_CALLBACK_2(ActionScene::onTouchMoved, this);
	
	*/	
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//초기화	
	mPopupLayer = NULL;
	mPopupLayerBackground = NULL;

	for (int n = 0; n < raceItemSlot; n++) {
		mSelectedItem[n] = NULL;
	}

	int idx = 0;
	names[idx++] = L"꼴등이";
	txtColors[idx] = Color3B::YELLOW;
	names[idx++] = L"시그";
	txtColors[idx] = Color3B::GRAY;
	names[idx++] = L"김밥이";
	txtColors[idx] = Color3B::MAGENTA;
	names[idx++] = L"인절미";
	txtColors[idx] = Color3B::ORANGE;
	names[idx++] = logics::hInst->getActor()->name;
	txtColors[idx] = Color3B::WHITE;

	mFullLayer = gui::inst()->createLayout(Size(Director::getInstance()->getVisibleSize().width * RACE_GOAL_DISTANCE, Director::getInstance()->getVisibleSize().height));

	gui::inst()->addBGScrolling("layers/race/1.png", mFullLayer, 0);
	gui::inst()->addBGScrolling("layers/race/2.png", mFullLayer, 0);
	gui::inst()->addBGScrolling("layers/race/3.png", mFullLayer, 0);
	//gui::inst()->addBGScrolling("layers/race/4.png", this, 3000);
	//gui::inst()->addBGScrolling("layers/race/5.png", this, 3000);
	
	auto finishLine = gui::inst()->addSprite(0, 0, "finish.png", mFullLayer);

	mGoalLength = Director::getInstance()->getVisibleSize().width * RACE_GOAL_DISTANCE - finishLine->getContentSize().width;
	
	finishLine->setPosition(Vec2(mGoalLength, 120));

	//구간 표시
	auto label0 = gui::inst()->addLabelAutoDimension(0, 2, "75%", mFullLayer, 0, ALIGNMENT_NONE, Color3B::GRAY);
	label0->setPosition(Vec2(mGoalLength / 4, label0->getPosition().y));
	auto label1 = gui::inst()->addLabelAutoDimension(0, 2, "50%", mFullLayer, 0, ALIGNMENT_NONE, Color3B::GRAY);
	label1->setPosition(Vec2(mGoalLength/2, label1->getPosition().y));
	auto label2 = gui::inst()->addLabelAutoDimension(0, 2, "25%", mFullLayer, 0, ALIGNMENT_NONE, Color3B::GRAY);
	label2->setPosition(Vec2(mGoalLength * 3 / 4, label2->getPosition().y));

	for (int n = 0; n <= raceParticipantNum; n++) {
		mRunner[n] = createRunner(n);
	}

	mFullLayer->runAction(Follow::create(mRunner[raceParticipantNum]
		, Rect(0, 0, Director::getInstance()->getVisibleSize().width * RACE_GOAL_DISTANCE, Director::getInstance()->getVisibleSize().height * 2)
	));

	mFullLayer->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(mFullLayer);	

	//Race 초기 상태	
	errorCode err = logics::hInst->runRaceSetRunners(mRaceId);
	switch (logics::hInst->getRace()->at(mRaceId).mode) {
	case race_mode_item:
	case race_mode_friend_1:
		//경묘 선수 초기 셋팅 및 아이템 선택
		showItemSelect(err);
		break;
	case race_mode_speed:
		initRace();
		break;
	default:
		break;
	}
    return true;
}

void ActionScene::initRace() {
	bool ret;
	//Race 초기 상태	
	errorCode err = logics::hInst->runRaceSetItems(mSelectItems);
	if (err != error_success && err != error_levelup) {
		//alert(wstring_to_utf8(logics::hInst->getErrorMessage(err)));
		return;
	}
	mRaceCurrent = logics::hInst->getRaceResult();
	mRaceParticipants = logics::hInst->getNextRaceStatus(ret, -1);
	if (!ret) {
		Director::getInstance()->popScene();
		return;
	}

	//title
	_race race = logics::hInst->getRace()->at(mRaceCurrent->id);
	gui::inst()->addLabel(4, 0, getRomeNumber(race.level) + ". " + wstring_to_utf8(race.title), this, 12);

	//rank
	/*
	for (int n = 0; n <= raceParticipantNum; n++) {
		mRankLabel[n] = gui::inst()->addLabel(8, 0, to_string(n+1) + "0" + wstring_to_utf8(names[n]), this, 8, ALIGNMENT_NONE);
		mRankLabel[n]->setColor(txtColors[n]);
		mRankLabel[n]->setPosition(Vec2(mRankLabel[n]->getPosition().x, mRankLabel[n]->getPosition().y - n * 10));
	}
	*/
		
	//아이템 설정
	for (int i = 0; i < mSelectItems.size(); i++) {
		_item item = logics::hInst->getItem(mSelectItems[i].itemId);
		string sz = getRomeNumber(item.grade) + "\n" + wstring_to_utf8(item.name);
		mSkillItem[i] = gui::inst()->addTextButton(0, 3 + i, sz, this,
			CC_CALLBACK_1(ActionScene::invokeItem, this, i), 12, ALIGNMENT_NONE, Color3B::BLACK);
	}

	counting();
}

void ActionScene::counting() {
	mTouchCnt = 0;
	mCount = 3;
	mCounting = gui::inst()->addLabel(4, 3, "Ready", this);
	mCounting->runAction(Sequence::create(ScaleTo::create(0.5, 4), ScaleTo::create(0.5, 1), NULL));
	this->schedule(schedule_selector(ActionScene::counter), 1);
}

void ActionScene::counter(float f) {		
	string sz = to_string(mCount);
	if (mCount == 0) {
		sz = "Go!";
		mCounting->setColor(Color3B::RED);
		mCounting->runAction(EaseIn::create(ScaleTo::create(0.2, 4), 3.f));
	}
	else {
		mCounting->setColor(Color3B::BLUE);
		mCounting->runAction(Sequence::create(EaseIn::create(ScaleTo::create(0.3, 4), 3.f), EaseIn::create(ScaleTo::create(0.3, 1), 3.f), NULL));
	}
	mCounting->setString(sz);
	
	if (mCount < 0) {
		this->removeChild(mCounting);
		unschedule(schedule_selector(ActionScene::counter));
		this->schedule(schedule_selector(ActionScene::timer), RACE_UPDATE_INTERVAL);
	}	
	mCount--;
}

void ActionScene::invokeItem(Ref* pSender, int idx) {
	//mSkillItem[idx]->setString("");
	mSkillItem[idx]->setEnabled(false);
	//아이템 사용
	if(mSelectItems.size() > idx)
		mInvokeItemQueue.push(idx);	
}

void ActionScene::callback2(Ref* pSender, SCENECODE type){

	switch (type)
	{
	case SCENECODE_RACE_RUN:
		this->removeChild(mPopupLayerBackground);				
		initRace();
		break;
	case SCENECODE_CLOSEPOPUP:
		Director::getInstance()->popScene();
	default:		
		break;
	}   
}

RepeatForever * ActionScene::getRunningAnimation(bool isSpeedUp) {
	auto animation = Animation::create();
	if (isSpeedUp) {
		animation->setDelayPerUnit(0.015);
	}
	else {
		animation->setDelayPerUnit(0.03 + (getRandValue(9) * 0.001));
	}

	string path;
	for (int n = 0; n <= 8; n++) {
		path = "race/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}

	return RepeatForever::create(Animate::create(animation));
}

Sprite* ActionScene::createRunner(int idx) {
	mSufferState[idx] = SUFFER_STATE_NONE;
		
	auto p = gui::inst()->addSprite(0, 7, RACE_DEFAULT_IMG, mFullLayer);
	p->setPosition(Vec2(0
		, p->getPosition().y + 
		p->getContentSize().height / 2 + 
		((raceParticipantNum - idx) * (p->getContentSize().height / 2))
	));
	//gui::inst()->setScale(p, RUNNER_WIDTH);
    p->runAction(getRunningAnimation());

    Color3B color = txtColors[idx];
	auto label = gui::inst()->addLabelAutoDimension(9, 0, wstring_to_utf8(names[idx]), p, 10, ALIGNMENT_CENTER, color);
	label->setPosition(p->getContentSize().width / 2, p->getContentSize().height - 10);

	mRunnerLabel[idx] = gui::inst()->addLabelAutoDimension(9, 0, " ", p, 10, ALIGNMENT_CENTER, color);
	mRunnerLabel[idx]->setPosition(p->getContentSize().width / 2, p->getContentSize().height);

	return p;
}

void ActionScene::timer(float f) {	
	bool ret;
	int itemIdx = -1;

	if (mInvokeItemQueue.size() > 0) {
		itemIdx = mInvokeItemQueue.front();
		mInvokeItemQueue.pop();
	}

	/*	
	RACE_MAX_TOUCH : 1 = x : RACE_UPDATE_INTERVAL
	RACE_MAX_TOUCH * RACE_UPDATE_INTERVAL = x
	*/
	
	int boost = 0;
	if (mRaceMode == race_mode_speed) {
		//boost = (float)mTouchCnt / (RACE_MAX_TOUCH * RACE_UPDATE_INTERVAL) * 100.f;
		float ratio = getTouchRatio(RACE_UPDATE_INTERVAL, mTouchCnt);
		boost = (int)min(ratio * 100.f, 100.f);
		CCLOG("boost %d", boost);
		//mRunnerLabel[raceParticipantNum]->setString(to_string(boost) + "," + to_string(mTouchCnt));
		mTouchCnt = 0;
	}
	

	mRaceParticipants = logics::hInst->getNextRaceStatus(ret, itemIdx, boost);
	if(!ret){
		unschedule(schedule_selector(ActionScene::timer));		
		mRunner[raceParticipantNum]->stopAllActions();
		mRunner[raceParticipantNum]->runAction(getRunningAnimation());
		mRunner[raceParticipantNum]->setPosition(Vec2(100, mRunner[raceParticipantNum]->getPosition().y));
		/*
		for (int n = 0; n < mRaceParticipants->size(); n++) {
			_raceParticipant p = mRaceParticipants->at(n);
			CCLOG("%d-%d", n, p.shootItemCount);
		}
		*/
		result();
		return;
	}
	
	for (int n = 0; n <= raceParticipantNum; n++) {
		_raceParticipant p = mRaceParticipants->at(n);
		//rank
		//mRankLabel[n]->setString(to_string(p.rank));

		if (p.ratioLength >= 100.f) {			
			mRunner[n]->stopAllActions();
			mRunnerLabel[n]->setString(to_string(p.rank));
			continue;
		}

		if (p.currentSuffer != itemType_max) {
			switch (p.currentSuffer)
			{
			case itemType_race_speedUp:
				if (mSufferState[n] != SUFFER_STATE_SPEEDUP) {
					mRunner[n]->stopAllActions();
					mRunner[n]->runAction(getRunningAnimation(true));
					mSufferState[n] = SUFFER_STATE_SPEEDUP;
				}
				break;
			case itemType_race_shield:
				if (mSufferState[n] != SUFFER_STATE_SHIELD) {
					mRunner[n]->stopAllActions();
					auto animation = Animation::create();
					animation->setDelayPerUnit(0.1);
					for(int n=0; n < 8; n++)
						animation->addSpriteFrameWithFile("action/98/"+ to_string(n) +".png");
					mRunner[n]->runAction(RepeatForever::create(Animate::create(animation)));
					mSufferState[n] = SUFFER_STATE_SHIELD;
				}
				break;
			default:				
				if (mSufferState[n] != SUFFER_STATE_ATTACK) {
					mRunner[n]->stopAllActions();
					auto animation = Animation::create();
					animation->setDelayPerUnit(0.1);
					for (int n = 0; n < 8; n++)
						animation->addSpriteFrameWithFile("action/99/"+ to_string(n) +".png");							
					mRunner[n]->runAction(RepeatForever::create(Animate::create(animation)));
					mSufferState[n] = SUFFER_STATE_ATTACK;
				}				
				break;
			}
		}
		else {
			if (mSufferState[n] != SUFFER_STATE_NONE) {
				mRunner[n]->stopAllActions();
				mRunner[n]->runAction(getRunningAnimation());				
				mSufferState[n] = SUFFER_STATE_NONE;
			}
		}	
		float x = mRaceParticipants->at(n).ratioLength / 100 * mGoalLength;
		Vec2 position = mRunner[n]->getPosition();
		position.x = x;
		mRunner[n]->runAction(MoveTo::create(0.3, position));
		
		//mRunnerLabel[n]->setString(to_string(p.currentLength) + "-" + to_string(p.sufferItems.size()) + "," + to_string(p.currentSuffer));
	}
}

void ActionScene::result() {
	
	//결과처리
	wstring sz;
	sz += L"순위: ";
	sz += to_wstring(mRaceCurrent->rank);
	if (mRaceCurrent->prize > 0) {
		sz += L"\n상금: $";		
		sz += to_wstring(logics::hInst->getRaceReward(mRaceCurrent->id, mRaceCurrent->rank - 1));
	}

	if (mRaceCurrent->rewardItemQuantity > 0) {
		sz += L"\n상품: ";
		sz += logics::hInst->getItem(mRaceCurrent->rewardItemId).name;
		sz += L"x";
		sz += to_wstring(mRaceCurrent->rewardItemQuantity);
	}
	
	auto l = gui::inst()->createLayout(Size(250, 150), "", true);
	Vec2 point;
	gui::inst()->getPoint(4, 3, point, ALIGNMENT_CENTER);
	l->setPosition(point);
	l->setAnchorPoint(Vec2(0.5, 0.5));

	gui::inst()->addLabelAutoDimension(0, 1, wstring_to_utf8(sz), l, 14, ALIGNMENT_CENTER, Color3B::BLACK, Size(1, 5), Size::ZERO, Size::ZERO);
	this->addChild(l);
	gui::inst()->addTextButtonAutoDimension(0, 3, "OK", l
		, CC_CALLBACK_1(ActionScene::callback2, this, SCENECODE_CLOSEPOPUP), 24, ALIGNMENT_CENTER, Color3B::BLUE, Size(1, 5), Size::ZERO, Size::ZERO);
}

void ActionScene::updateSelectItem() {
	for (int n = 0; n < raceItemSlot; n++) {
		if (mSelectItems.size() > n) {
			_item item = logics::hInst->getItem(mSelectItems[n].itemId);
			mSelectedItem[n]->setString(wstring_to_utf8(item.name));
		}
		else {
			mSelectedItem[n]->setString("EMPTY");
		}
	}
}

void ActionScene::removeSelectItem(Ref* pSender, int idx) {
	if (mSelectItems.size() > idx) {
		mSelectedItemQuantity[mSelectItems[idx].itemId]--; //맵에서 제거
		mSelectItems.erase(mSelectItems.begin() + idx);
		updateSelectItem();
	}
}

void ActionScene::selectItem(Ref* pSender, int id) {
	if (mSelectItems.size() >= raceItemSlot)
		return;

	if (mSelectedItemQuantity.find(id) == mSelectedItemQuantity.end() || mSelectedItemQuantity[id] == 0) {
		mSelectedItemQuantity[id] = 1;
	}
	else {
		if (!logics::hInst->getActor()->inven.checkItemQuantity(inventoryType_race, id, mSelectedItemQuantity[id] + 1))
		{
			//과다 입력
			return;
		}

		mSelectedItemQuantity[id]++;
	}

	_itemPair p;
	p.itemId = id;
	p.val = 1;
	mSelectItems.push_back(p);

	updateSelectItem();
}

void ActionScene::showItemSelect(errorCode err) {
	RACE_SIZE;

	this->removeChild(mPopupLayerBackground);
	mPopupLayer = gui::inst()->addPopup(mPopupLayerBackground, this, size, BG_RACE, Color4B::WHITE);

	//Go!
	gui::inst()->addTextButtonAutoDimension(3, 6, "GO!", mPopupLayer
		, CC_CALLBACK_1(ActionScene::callback2, this, SCENECODE_RACE_RUN)
		, 16, ALIGNMENT_CENTER, Color3B::YELLOW, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);
	//CANCEL!
	gui::inst()->addTextButtonAutoDimension(5, 6, "CANCEL", mPopupLayer
		, CC_CALLBACK_1(ActionScene::callback2, this, SCENECODE_CLOSEPOPUP)
		, 16, ALIGNMENT_CENTER, Color3B::ORANGE, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);

	//Selected Item
	for (int n = 0; n < raceItemSlot; n++) {
		mSelectedItem[n] = gui::inst()->addTextButtonAutoDimension(3 + (2 * (n)), 5, "EMPTY", mPopupLayer
			, CC_CALLBACK_1(ActionScene::removeSelectItem, this, n)
			, 12, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
		);
	}
		
	if (err != error_success && err != error_levelup) {
		gui::inst()->addLabel(4, 3, wstring_to_utf8(logics::hInst->getErrorMessage(err)), this);
		return;
	}

	//선수 정보
	raceParticipants* p = logics::hInst->getRaceRunners();
	for (int n = 0; n < p->size(); n++) {
		gui::inst()->addLabelAutoDimension(0, n +1, wstring_to_utf8(names[n]), mPopupLayer, 10, ALIGNMENT_NONE, txtColors[n]);
		string pro = "S: " + to_string(p->at(n).strength) + " ";
		pro += "I: " + to_string(p->at(n).intelligence) + " ";
		pro += "A: " + to_string(p->at(n).appeal);

		gui::inst()->addLabelAutoDimension(1, n + 1, pro, mPopupLayer, 10, ALIGNMENT_NONE, txtColors[n]);
	}

	gui::inst()->addLabelAutoDimension(0, p->size() + 1, wstring_to_utf8(logics::hInst->getActor()->name), mPopupLayer, 10, ALIGNMENT_NONE, txtColors[p->size()]);
	string pro = "S: " + to_string(logics::hInst->getActor()->property.strength) + " ";
	pro += "I: " + to_string(logics::hInst->getActor()->property.intelligence) + " ";
	pro += "A: " + to_string(logics::hInst->getActor()->property.appeal);

	gui::inst()->addLabelAutoDimension(1, p->size() + 1, pro, mPopupLayer, 10, ALIGNMENT_NONE, txtColors[p->size()]);
		
	int nMenuIdx = 0;
	int newLine = 2;

	vector<intPair> vec;
	logics::hInst->getActor()->inven.getWarehouse(vec, (int)inventoryType_race);

	int cnt = vec.size();

	Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(3, 5), Vec2(9, 0), size, margin);
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - POPUP_NODE_MARGIN;
	//nodeSize.width = sizeOfScrollView.width - POPUP_NODE_MARGIN;
	Size innerSize = Size(sizeOfScrollView.width , cnt * (sizeOfScrollView.height + POPUP_NODE_MARGIN));
	ScrollView * sv = gui::inst()->addScrollView(Vec2(3, 5), Vec2(9, 0), size, margin, "", innerSize);

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
		
		gui::inst()->addTextButtonAutoDimension(1, heightIdx++, getRomeNumber(item.grade), l
			, CC_CALLBACK_1(ActionScene::selectItem, this, id), 10, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
		gui::inst()->addTextButtonAutoDimension(1, heightIdx++, name, l
			, CC_CALLBACK_1(ActionScene::selectItem, this, id), 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
		//item quantity
		gui::inst()->addTextButtonAutoDimension(1, heightIdx++, "x " + to_string(vec[n].val), l
			, CC_CALLBACK_1(ActionScene::selectItem, this, id), 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		gui::inst()->addLayoutToScrollView(sv, l, POPUP_NODE_MARGIN, newLine);

	}

	mPopupLayer->addChild(sv, 1, CHILD_ID_RACE);
}

bool ActionScene::onTouchEnded(Touch* touch, Event* event)
{
	/*
		auto touchPoint = touch->getLocation();
		log("onTouchBegan id = %d, x = %f, y = %f", touch->getID(), touchPoint.x, touchPoint.y);
		bool bTouch = pMan->getBoundingBox().containsPoint(touchPoint);
		if (bTouch)
		{
		log("Sprite clicked...");
		}
	*/
	if(mRaceMode == race_mode_speed)
		mTouchCnt++;

	return true;
}



