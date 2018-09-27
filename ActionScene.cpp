//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "ActionScene.h"
#include "AlertScene.h"
#include "SimpleAudioEngine.h"

#define RACE_UPDATE_INTERVAL 0.3
//#define RACE_MAX_TOUCH 200.f //초당 max 터치
#define RACE_DEFAULT_IMG "race/0.png"
#define RACE_GOAL_DISTANCE 2.5
#define RACE_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size(5, 10); auto nodeSize = Size(120, 50); auto gridSize = Size(3, 4);
#define POPUP_NODE_MARGIN  4

//#define RUNNER_WIDTH 80
Scene* ActionScene::createScene(int id)
{	
	return ActionScene::create(id);
}

bool ActionScene::init() {		
	mPlayCnt = 0;
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
	//names[idx++] = logics::hInst->getActor()->name;	
	names[idx++] = utf8_to_utf16(logics::hInst->getActor()->name);

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


	_race race = logics::hInst->getRace()->at(mRaceId);

	for (int n = 0; n < raceParticipantNum; n++) {
		if (race.mode == race_mode_1vs1 && n != 0)
			continue;
		mRunner[n] = createRunner(n);
	}
	//나는 무조건 있어야 하니까.
	mRunner[raceParticipantNum] = createRunner(raceParticipantNum);

	mFullLayer->runAction(Follow::create(mRunner[raceParticipantNum]
		, Rect(0, 0, Director::getInstance()->getVisibleSize().width * RACE_GOAL_DISTANCE, Director::getInstance()->getVisibleSize().height * 2)
	));

	mFullLayer->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(mFullLayer);	

	//layer
	Vec2 start = Director::getInstance()->getVisibleOrigin();
	Vec2 end = gui::inst()->getPointVec2(1, 0, ALIGNMENT_NONE);
	end.y = start.y + Director::getInstance()->getVisibleSize().height;

	auto layerItem = gui::inst()->createLayout(Size(end.x - start.x, end.y - start.y), "", true); 
	layerItem->setOpacity(128);
	layerItem->setPosition(start);
	this->addChild(layerItem);

	//boost label
	if (mRaceMode == race_mode_speed) {
		mBoostPercent = gui::inst()->addLabel(0, 0, " ", this);
		mBoostPercent->setColor(Color3B::GRAY);
	}

	//point
	mPoint = gui::inst()->addLabel(8, 0, COIN + to_string(logics::hInst->getActor()->point), this);

	//title	
	gui::inst()->addLabel(4, 0, getRomeNumber(race.level) + ". " + wstring_to_utf8(race.title), this, 12);

	//Race 초기 상태	
	errorCode err = logics::hInst->runRaceValidate(mRaceId);
	if (err != error_success)
		Director::getInstance()->popScene();
	
	err = logics::hInst->runRaceSetRunners(mRaceId);
	switch (logics::hInst->getRace()->at(mRaceId).mode) {
	case race_mode_item:
	case race_mode_1vs1:
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

bool ActionScene::initRace() {
	bool ret;
	//Race 초기 상태	
	errorCode err = logics::hInst->runRaceSetItems(mSelectItems);
	if (err != error_success && err != error_levelup) {
		//alert(wstring_to_utf8(logics::hInst->getErrorMessage(err)));
		Director::getInstance()->pushScene(AlertScene::createScene(err));
		return false;
	}
	mRaceCurrent = logics::hInst->getRaceResult();
	mRaceParticipants = logics::hInst->getNextRaceStatus(ret, -1);
	if (!ret) {
		Director::getInstance()->popScene();
		return false;
	}

	
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
		//string sz = getRomeNumber(item.grade) + "\n" + wstring_to_utf8(item.name);
		wstring szW = getSkillIconW(item.type) + to_wstring(item.grade);
		string sz = wstring_to_utf8(szW);
		mSkillItem[i] = gui::inst()->addTextButton(0, 3 + i, sz, this,
			CC_CALLBACK_1(ActionScene::invokeItem, this, i), 24, ALIGNMENT_NONE, Color3B::BLACK);
	}

	counting();
	return true;
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
	errorCode err;
	switch (type)
	{
	case SCENECODE_RACE_RUN:		
		if(initRace())
			this->removeChild(mPopupLayerBackground);
		updatePoint();
		break;
	case SCENECODE_CLOSEPOPUP: //cancel hp와 참가비 재입금
		logics::hInst->increaseHP(1);
		logics::hInst->increasePoint(logics::hInst->getRace()->at(mRaceId).fee);

		Director::getInstance()->popScene();
		break;
	case SCENECODE_RACE_FINISH:
		mPlayCnt++;

		if(logics::hInst->getRace()->at(mRaceId).mode == race_mode_speed)
			return Director::getInstance()->popScene();

		if(logics::hInst->runRaceValidate(mRaceId) != error_success)
			return Director::getInstance()->popScene();

		err = logics::hInst->runRaceSetRunners(mRaceId);
		switch (logics::hInst->getRace()->at(mRaceId).mode) {
		case race_mode_item:
		case race_mode_1vs1:
		case race_mode_friend_1:
			//경묘 선수 초기 셋팅 및 아이템 선택
			showItemSelect(err);
			break;
		default:
			break;
		}
		break;
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
	_race race = logics::hInst->getRace()->at(mRaceId);

	if (mInvokeItemQueue.size() > 0) {
		itemIdx = mInvokeItemQueue.front();
		mInvokeItemQueue.pop();
	}

	/*	
	RACE_MAX_TOUCH : 1 = x : RACE_UPDATE_INTERVAL
	RACE_MAX_TOUCH * RACE_UPDATE_INTERVAL = x
	*/
	
	float boost = 0.f;
	if (mRaceMode == race_mode_speed) {
		//boost = (float)mTouchCnt / (RACE_MAX_TOUCH * RACE_UPDATE_INTERVAL) * 100.f;
		float ratio = getTouchRatio(RACE_UPDATE_INTERVAL, mTouchCnt);
		boost = min(ratio * 100.f, 100.f);
		//CCLOG("boost %d", boost);
		//mRunnerLabel[raceParticipantNum]->setString(to_string(boost) + "%");
		//mRunnerLabel[raceParticipantNum]->setString(to_string(boost) + "," + to_string(mTouchCnt));
		mBoostPercent->setString(to_string(boost).substr(0, 4));
		mTouchCnt = 0;
	}
	

	mRaceParticipants = logics::hInst->getNextRaceStatus(ret, itemIdx, boost);
	if(!ret){
		unschedule(schedule_selector(ActionScene::timer));		
		for (int n = 0; n <= raceParticipantNum; n++) {
			if (mRunner[n]) {
				mRunner[n]->stopAllActions();
				mRunner[n]->runAction(getRunningAnimation());
				mRunner[n]->setPosition(Vec2(0, mRunner[n]->getPosition().y));
			}
		}
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

		if (race.mode == race_mode_1vs1 && n > 0 && n < raceParticipantNum)
			continue;
		//rank
		
		if (p.ratioLength >= 100.f) {			
			mRunner[n]->stopAllActions();
			mRunnerLabel[n]->setString(to_string(p.rank));
			continue;
		}
		else { //순위 전에는 사용한 아이템 
			mRunnerLabel[n]->setString(getSkillIcon(p.shootCurrentType));
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
	Size grid = Size(5, 8);
	mPopupLayer = gui::inst()->addPopup(mPopupLayerBackground, this, Size(300, 200), BG_RACE, Color4B::WHITE);

	//결과처리
	wstring sz;
	sz += L"순위: ";
	sz += to_wstring(mRaceCurrent->rank);
	
	int idx = 1;
	gui::inst()->addLabelAutoDimension(2, idx++, wstring_to_utf8(sz), mPopupLayer, 14, ALIGNMENT_CENTER, Color3B::BLACK
		, grid, Size::ZERO, Size::ZERO);

	sz = L"";
	int prize = logics::hInst->getRaceReward(mRaceCurrent->id, mRaceCurrent->rank - 1);
	if (mRaceCurrent->prize > 0) {
		sz += L"상금: $";		
		sz += to_wstring(prize);
	}

	if (mRaceCurrent->rewardItemQuantity > 0) {
		sz += L"\n상품: ";
		sz += logics::hInst->getItem(mRaceCurrent->rewardItemId).name;
		sz += L"x";
		sz += to_wstring(mRaceCurrent->rewardItemQuantity);
	}
	
	gui::inst()->addLabelAutoDimension(2, idx++, wstring_to_utf8(sz), mPopupLayer, 12, ALIGNMENT_CENTER, Color3B::BLACK
		, grid, Size::ZERO, Size::ZERO);
	//this->addChild(l);
	idx++;
	if (mRaceCurrent->rank == 1) {
		
		for (int n = 0; n < 5; n++) {
			auto star = gui::inst()->addSpriteAutoDimension(n, idx, "star.png", mPopupLayer, ALIGNMENT_CENTER, grid, Size::ZERO, Size::ZERO);
			if (logics::hInst->mRaceWin.winCnt == n + 1) {
				star->runAction(Sequence::create(ScaleTo::create(0.5, 1.5), ScaleTo::create(0, 1), NULL));
			}

			if(logics::hInst->mRaceWin.winCnt < n+1)
				star->setOpacity(128);
		}
		idx++;

		string szBonus;
		int nBonus = 0;
		for (int n = 1; n < 5; n++) {			
			szBonus = "+";
			szBonus += COIN + to_string(n * prize);

			auto bonus = gui::inst()->addLabelAutoDimension(n, idx, szBonus, mPopupLayer, 0
				, ALIGNMENT_CENTER, Color3B::BLACK, grid, Size::ZERO, Size::ZERO);

			if (logics::hInst->mRaceWin.winCnt == n + 1)
				nBonus = n * prize;

			if (logics::hInst->mRaceWin.winCnt < n + 1)
				bonus->setOpacity(128);
		}
		if(nBonus > 0)
			logics::hInst->increasePoint(nBonus);
		idx++;
	}

	gui::inst()->addTextButtonAutoDimension(2, idx, "OK", mPopupLayer
		, CC_CALLBACK_1(ActionScene::callback2, this, SCENECODE_RACE_FINISH), 24, ALIGNMENT_CENTER, Color3B::BLUE
		, grid, Size::ZERO, Size::ZERO);

	updatePoint();
}

void ActionScene::updateSelectItem() {
	for (int n = 0; n < raceItemSlot; n++) {
		if (mSelectItems.size() > n) {
			_item item = logics::hInst->getItem(mSelectItems[n].itemId);
			wstring sz = getSkillIconW(item.type) + to_wstring(item.grade);
			mSelectedItem[n]->setString(wstring_to_utf8(sz));
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
		/*
		if (!logics::hInst->getActor()->inven.checkItemQuantity(inventoryType_race, id, mSelectedItemQuantity[id] + 1))
		{
			//과다 입력
			return;
		}
		*/
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
	mSelectedItemQuantity.clear();
	mSelectItems.clear();
	for (int n = 0; n < raceItemSlot; n++) {
		if (mSkillItem[n] != NULL) {
			this->removeChild(mSkillItem[n]->getParent());
			mSkillItem[n] = NULL;
		}		
	}
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
		mSelectedItem[n] = gui::inst()->addTextButtonAutoDimension(1 + (3 * (n)), 5, "EMPTY", mPopupLayer
			, CC_CALLBACK_1(ActionScene::removeSelectItem, this, n)
			, 24, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
		);
	}
		
	if (err != error_success && err != error_levelup) {
		gui::inst()->addLabel(4, 3, wstring_to_utf8(logics::hInst->getErrorMessage(err)), this);
		return;
	}

	int newLine = 2;
	/*
	vector<intPair> vec;
	logics::hInst->getActor()->inven.getWarehouse(vec, (int)inventoryType_race);
	*/
	trade::tradeMap * m = logics::hInst->getTrade()->get();
	POPUP_LIST(mPopupLayer
		, gridSize
		, newLine
		, Vec2(0, 6)
		, Vec2(9, 1)
		, margin
		, POPUP_NODE_MARGIN
		, nodeSize
		, (trade::tradeMap::iterator it = m->begin(); it != m->end(); ++it)
		, if (logics::hInst->getItem(it->first).type <= itemType_race || logics::hInst->getItem(it->first).type >= itemType_adorn) continue;
		, MainScene::getItemImg(it->first)
		, CC_CALLBACK_1(ActionScene::selectItem, this, it->first)
		, getRomeNumber(logics::hInst->getItem(it->first).grade)
		, wstring_to_utf8(logics::hInst->getItem(it->first).name)
		, COIN + to_string(logics::hInst->getTrade()->getPriceBuy(it->first))
		, gui::inst()->EmptyString
		, gui::inst()->EmptyString
	)

		/*
	

	int cnt = vec.size();

	Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 6), Vec2(9, 1), size, margin);
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - POPUP_NODE_MARGIN;
	//nodeSize.width = sizeOfScrollView.width - POPUP_NODE_MARGIN;
	Size innerSize = Size(sizeOfScrollView.width , cnt * (sizeOfScrollView.height + POPUP_NODE_MARGIN));
	ScrollView * sv = gui::inst()->addScrollView(Vec2(0, 6), Vec2(9, 1), size, margin, "", innerSize);

	for (int n = 0; n < (int)vec.size(); n++) {
		int id = vec[n].key;
		string img = "items/" + to_string(id % 20) + ".png";
		Layout* l = gui::inst()->createLayout(nodeSize, "", true);
		l->setOpacity(192);
		_item item = logics::hInst->getItem(id);

		int heightIdx = 2;	
		
		gui::inst()->addLabelAutoDimension(0, heightIdx++, getSkillIcon(item.type), l, 20, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

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
	*/
	updatePoint();
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


void ActionScene::updatePoint() {
	string sz = COIN + to_string(logics::hInst->getActor()->point);
	if (sz.compare(mPoint->getString()) != 0) {
		mPoint->setString(sz);
		mPoint->runAction(gui::inst()->createActionFocus());
	}
}

void ActionScene::onEnter() {
	Scene::onEnter();
	updatePoint();
}
void ActionScene::onEnterTransitionDidFinish() {
	Scene::onEnterTransitionDidFinish();
}
void ActionScene::onExitTransitionDidStart() {
	Scene::onExitTransitionDidStart();
}
void ActionScene::onExit() {
	Scene::onExit();
}
