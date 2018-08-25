//
// Created by Jung, DaeChun on 24/08/2018.
//

#include "FarmingScene.h"
#include "SimpleAudioEngine.h"
#define FARM_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size(5, 10); auto nodeSize = Size(120, 70); auto gridSize = Size(3, 5);
#define POPUP_NODE_MARGIN  4
#define FARM_CREATE_CB CallFuncN * cb = CallFuncN::create(CC_CALLBACK_1(FarmingScene::callbackActionAnimation, hInst))
FarmingScene * FarmingScene::hInst = NULL;
Vec2 _characterInitPosition = Vec2(4, 5);

Scene* FarmingScene::createScene()
{
	return FarmingScene::create();
}

bool FarmingScene::init() 
{	
	mSleepCnt = 0;
	hInst = this;
	
	auto bg = Sprite::create("bg_farming.jpg");
	bg->setContentSize(Director::getInstance()->getVisibleSize());
	bg->setAnchorPoint(Vec2(0, 0));
	//bg->setOpacity(200);
	bg->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(bg);
	gui::inst()->addTextButton(8, 0, "BACK", this, CC_CALLBACK_1(FarmingScene::callback, this, SCENECODE_CLOSESCENE), 0, ALIGNMENT_CENTER, Color3B::RED);

	//field 기본 3개
	int cntFarm = logics::hInst->getFarm()->countField();
	if (cntFarm == 0) {
		for (int n = 0; n < 3; n++) {
			logics::hInst->getFarm()->addField(3 + n, 4);
		}
		cntFarm = logics::hInst->getFarm()->countField();
	}
		
	for (int n = 0; n < cntFarm; n++) {
		farming::field * pField = logics::hInst->getFarm()->getFields()->at(n);

		mCultivation[n].addLevel("plant0.png", 0, wstring_to_utf8(L"심기"));
		mCultivation[n].addLevel("plant0.png", 10, "");
		mCultivation[n].addLevel("plant1.png", 40, "");
		mCultivation[n].addLevel("plant2.png", 70, "");
		mCultivation[n].addLevel("plant3.png", 100, "");
		mCultivation[n].init(n, cultivationCB, 10, "plant_decay.png", "timeBar.png", this, Vec2(pField->x, pField->y), false);
	}
	
	//gui::inst()->drawGrid(this);

	//캐릭터
	mCharacter = MainScene::getIdle();
	mCharacter->setPosition(gui::inst()->getPointVec2(_characterInitPosition.x, _characterInitPosition.y));
	mCharacter->setAnchorPoint(Vec2(0.5 , 0));
	this->addChild(mCharacter);
		
	mLabel = gui::inst()->addLabel(4, 2, " ", this, 9);
	//schedule
	updateFarming(0.f);//작물 키우다가 잠깐 나갔다 왔을 떄 화면 이상한거 막기 위해
	this->schedule(schedule_selector(FarmingScene::updateFarming), 0.3f);
	
	return true;
}

void FarmingScene::updateFarming(float f) {
	int cnt = logics::hInst->getFarm()->countField();
	if (mSleepCnt > 0) {
		mSleepCnt--;
		return;
	}

	if (mHarvestQ.size() > 0) {
		string sz = "";
		while (mHarvestQ.size() > 0)
		{
			intPair p = mHarvestQ.front();
			sz += wstring_to_utf8(logics::hInst->getItem(p.key).name + L" x" + to_wstring(p.val) + L"\n");
			mHarvestQ.pop();
		}
		messagePopup(sz);
	}

	for (int n = 0; n < cnt; n++)
	{
		farming::field * pField = logics::hInst->getFarm()->getFields()->at(n);
		wstring szComment = L" ";
		switch (pField->status) {
		case farming::farming_status_max:
			if (mCultivation[n].getPercent() != 0)
				mCultivation[n].update(0, "");
			continue;
			break;
		case farming::farming_status_sprout://새싹
		case farming::farming_status_good:	 //건강
			break;
		case farming::farming_status_grown:	//다 자람
			szComment = L"다 자람";
			break;
		case farming::farming_status_week: //돌봄 필요
			szComment = L"돌봄 필요!";
			break;
		case farming::farming_status_decay:	 //썩음
			szComment = L"썩음";
			break;
		}
		mCultivation[n].update(pField->percent, wstring_to_utf8(szComment));

	}
}

void FarmingScene::callbackSeed(Ref* pSender, int idx, int itemId) {
	errorCode err = logics::hInst->farmingPlant(idx, itemId);
	if (err != error_success) {
		messagePopup(wstring_to_utf8(logics::hInst->getErrorMessage(err)));
		return;
	}
	callback(this, SCENECODE_CLOSEPOPUP);
}
void FarmingScene::callbackActionAnimation(Ref* pSender) {
	mCharacter->stopAllActions();
	mCharacter->runAction(MainScene::getIdleAnimation());
	mCharacter->setPosition(gui::inst()->getPointVec2(_characterInitPosition.x, _characterInitPosition.y));
	mLabel->setString(" ");

}

void FarmingScene::callback(Ref* pSender, SCENECODE type) {

	switch (type)
	{
	case SCENECODE_CLOSESCENE:
		unschedule("schedule");
		Director::getInstance()->popScene();
		break;
	case SCENECODE_CLOSEPOPUP:
		closePopup();
		break;
	default:
		break;
	}
}

void FarmingScene::cultivationCB(int idx) {
	
	FARM_CREATE_CB; //쓸데없는 객체 생성을 없애고 싶지만 로직을 바꿔야 해서 걍 냅둔다.
	farming::field* pField = logics::hInst->getFarm()->getFields()->at(idx);
	intPair earn;
	earn.key = 0;
	errorCode err = error_success;
	switch (pField->status) {
	case farming::farming_status_max:
		hInst->showItemSelect(idx);
		break;
	case farming::farming_status_decay:	 //썩음
	case farming::farming_status_grown:	//다 자람
		err = logics::hInst->farmingHarvest(idx, earn.key, earn.val);
		//초기화
		hInst->mCharacter->stopAllActions();		
		hInst->mCharacter->runAction(hInst->getFarmingAnimation(cb));
		hInst->mCharacter->setPosition(gui::inst()->getPointVec2(pField->x, pField->y));
		//hInst->mCultivation[idx].update(0, "");
		hInst->mSleepCnt = 5;
		
		if (err != error_success && err != error_levelup) {
			hInst->messagePopup(wstring_to_utf8(logics::hInst->getErrorMessage(err)));
		}
		else if (earn.key != 0) {
			hInst->mHarvestQ.push(earn);
		}
		break;
	case farming::farming_status_week: //돌봄 필요
		
		err = logics::hInst->farmingCare(idx);
		hInst->mCharacter->stopAllActions();
		hInst->mCharacter->setPosition(gui::inst()->getPointVec2(pField->x, pField->y));
		if (err == error_farming_gluttony) {
			hInst->mCharacter->runAction(hInst->getFarmingMistakeAnimation(cb));
			//에러 메세지
			hInst->mLabel->setString(wstring_to_utf8(logics::hInst->getErrorMessage(err)));
		}
		else {
			hInst->mCharacter->runAction(hInst->getFarmingAnimation(cb));
		}
		break;		
	default:
		break;
	}
	
};
void FarmingScene::messagePopup(string msg) {
	closePopup();
	mPopupLayer = MainScene::createMessagePopup(mPopupLayerBackground, this, wstring_to_utf8(L"짝짝짝! 농작물을 수확 했어요~"), msg, false);
	gui::inst()->addTextButtonAutoDimension(0, 4, "CLOSE", mPopupLayer
		, CC_CALLBACK_1(FarmingScene::callback, this, SCENECODE_CLOSEPOPUP)
		, 12
		, ALIGNMENT_CENTER
		, Color3B::BLUE
		, Size(1, 5)
		, Size::ZERO
		, Size::ZERO
	);
}
void FarmingScene::showItemSelect(int idx) {
	FARM_SIZE;
	closePopup();
	mPopupLayer = gui::inst()->addPopup(mPopupLayerBackground, this, size, "bg_race.png", Color4B::WHITE);

	gui::inst()->addTextButtonAutoDimension(7, 0, "CANCEL", mPopupLayer
		, CC_CALLBACK_1(FarmingScene::callback, this, SCENECODE_CLOSEPOPUP)
		, 12, ALIGNMENT_CENTER, Color3B::ORANGE, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin
	);


	int nMenuIdx = 0;
	int newLine = 2;

	vector<intPair> vec;
	logics::hInst->getActor()->inven.getWarehouse(vec, (int)inventoryType_farming);

	int cnt = vec.size();

	Size sizeOfScrollView = gui::inst()->getScrollViewSize(Vec2(0, 7), Vec2(9, 1), size, margin);
	//nodeSize.width = (sizeOfScrollView.width / (float)newLine) - POPUP_NODE_MARGIN;
	nodeSize.width = (sizeOfScrollView.width / (float)newLine) - POPUP_NODE_MARGIN;
	Size innerSize = Size(sizeOfScrollView.width, ((vec.size() / newLine) + 1) * (nodeSize.height + POPUP_NODE_MARGIN));

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

		gui::inst()->addTextButtonAutoDimension(1, heightIdx++, "Lv." + to_string(item.grade), l
			, CC_CALLBACK_1(FarmingScene::callbackSeed, this, idx, id), 12, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
		gui::inst()->addTextButtonAutoDimension(1, heightIdx++, name, l
			, CC_CALLBACK_1(FarmingScene::callbackSeed, this, idx, id), 12, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
		//item quantity
		gui::inst()->addTextButtonAutoDimension(1, heightIdx++, "x " + to_string(vec[n].val), l
			, CC_CALLBACK_1(FarmingScene::callbackSeed, this, idx, id), 12, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);

		gui::inst()->addLayoutToScrollView(sv, l, POPUP_NODE_MARGIN, newLine);

	}

	mPopupLayer->addChild(sv, 1, CHILD_ID_RACE);
}


Sequence * FarmingScene::getFarmingAnimation(CallFuncN * cb) {
	
	auto animation = Animation::create();
	animation->setDelayPerUnit(0.1);
	
	string path;
	for (int n = 0; n < 8; n++) {
		path = "action/201/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}

	return Sequence::create(Repeat::create(Animate::create(animation), 2), cb, NULL);
}

Sequence * FarmingScene::getFarmingMistakeAnimation(CallFuncN * cb) {
	auto animation = Animation::create();
	animation->setDelayPerUnit(0.1);

	string path;
	for (int n = 0; n < 18; n++) {
		path = "action/200/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}

	return Sequence::create(Animate::create(animation), cb, NULL);
}
