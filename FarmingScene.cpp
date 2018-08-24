//
// Created by Jung, DaeChun on 24/08/2018.
//

#include "FarmingScene.h"
#include "SimpleAudioEngine.h"
#define FARM_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size(5, 10); auto nodeSize = Size(120, 70); auto gridSize = Size(3, 5);
#define POPUP_NODE_MARGIN  4
FarmingScene * FarmingScene::hInst = NULL;

Scene* FarmingScene::createScene()
{
	return FarmingScene::create();
}

bool FarmingScene::init() 
{	
	hInst = this;
	/*
	mFullLayer = gui::inst()->createLayout(Size(Director::getInstance()->getVisibleSize().width, Director::getInstance()->getVisibleSize().height));
	gui::inst()->addBGScrolling("bg_farming.png", mFullLayer, 0);
	mFullLayer->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(mFullLayer);
	*/
	auto bg = Sprite::create("bg_farming.jpg");
	bg->setContentSize(Director::getInstance()->getVisibleSize());
	bg->setAnchorPoint(Vec2(0, 0));
	//bg->setOpacity(200);
	bg->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(bg);
	gui::inst()->addTextButton(0, 0, "CLOSE", this, CC_CALLBACK_1(FarmingScene::callback, this, SCENECODE_CLOSESCENE), 0, ALIGNMENT_CENTER, Color3B::RED);

	auto pCharacter = MainScene::getIdle();
	pCharacter->setPosition(gui::inst()->getPointVec2(4, 3));
	this->addChild(pCharacter);

	int cntFarm = logics::hInst->getFarm()->countField();
	if (cntFarm == 0) {
		logics::hInst->getFarm()->addField(3, 5);
		cntFarm = logics::hInst->getFarm()->countField();
	}
		
	for (int n = 0; n < cntFarm; n++) {
		farming::field * pField = logics::hInst->getFarm()->getFields()->at(n);

		mCultivation[n].addLevel("star.png", 0, wstring_to_utf8(L"½É±â"));
		mCultivation[n].addLevel("plant0.png", 10, "");
		mCultivation[n].addLevel("plant1.png", 40, "");
		mCultivation[n].addLevel("plant2.png", 70, "");
		mCultivation[n].addLevel("plant3.png", 100, "");
		mCultivation[n].init(n, cultivationCB, 10, "plant_decay.png", "timeBar.png", this, Vec2(pField->x, pField->y), false);
		/*
		switch (pField->status) {
		case farming::farming_status_max:
			mSeedButton[n] = gui::inst()->addTextButton(pField->x, pField->y - 1, "EMPTY", this
				, CC_CALLBACK_1(FarmingScene::callbackSeed, this, n), 12, ALIGNMENT_CENTER, Color3B::BLACK
				, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
				, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
				, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
				, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
				//, "star.png"
			);			
			break;
		case farming::farming_status_sprout://»õ½Ï
			break;
		case farming::farming_status_week: //µ¹º½ ÇÊ¿ä
		case farming::farming_status_good:	 //°Ç°­
		case farming::farming_status_decay:	 //½âÀ½
		case farming::farming_status_grown:	//´Ù ÀÚ¶÷
			break;
		}
		*/
	}
	
	//gui::inst()->drawGrid(this);

	mLabel = gui::inst()->addLabel(4, 0, "IDX", this);
	
	this->schedule([=](float delta) {
		int cnt = logics::hInst->getFarm()->countField();

		for (int n = 0; n < cnt; n++)
		{
			farming::field * pField = logics::hInst->getFarm()->getFields()->at(n);
			wstring szComment = L" ";
			switch (pField->status) {
			case farming::farming_status_max:
				continue;
				break;
			case farming::farming_status_sprout://»õ½Ï
			case farming::farming_status_good:	 //°Ç°­
				break;
			case farming::farming_status_grown:	//´Ù ÀÚ¶÷
				szComment = L"´Ù ÀÚ¶÷";
				break;
			case farming::farming_status_week: //µ¹º½ ÇÊ¿ä
				szComment = L"µ¹º½ ÇÊ¿ä!";
				break;
			case farming::farming_status_decay:	 //½âÀ½
				szComment = L"½âÀ½";
				break;
			}	
			mCultivation[n].update(pField->percent, wstring_to_utf8(szComment));

		}
	}, 0.5f, "schedule");
	
	return true;
}

void FarmingScene::callbackSeed(Ref* pSender, int idx, int itemId) {
	errorCode err = logics::hInst->farmingPlant(idx, itemId);
	if (err != error_success) {
		mLabel->setString(wstring_to_utf8(logics::hInst->getErrorMessage(err)));
		return;
	}
	callback(this, SCENECODE_CLOSEPOPUP);
}

void FarmingScene::callback(Ref* pSender, SCENECODE type) {

	switch (type)
	{
	case SCENECODE_CLOSESCENE:
		unschedule("schedule");
		Director::getInstance()->popScene();
		break;
	case SCENECODE_CLOSEPOPUP:
		this->removeChild(mPopupLayerBackground);
		break;
	default:
		break;
	}
}

void FarmingScene::cultivationCB(int idx) {
	//hInst->cultivationCBInner(id);
	hInst->mLabel->setString(to_string(idx));
	farming::field* pField = logics::hInst->getFarm()->getFields()->at(idx);
	int productId = 0;
	int earning = 0;
	errorCode err = error_success;
	switch (pField->status) {
	case farming::farming_status_max:
		hInst->showItemSelect(idx);
		break;
	case farming::farming_status_decay:	 //½âÀ½
	case farming::farming_status_grown:	//´Ù ÀÚ¶÷
		err = logics::hInst->farmingHarvest(idx, productId, earning);
		//ÃÊ±âÈ­
		hInst->mCultivation[idx].update(0, "");
		break;
	case farming::farming_status_week: //µ¹º½ ÇÊ¿ä
		err = logics::hInst->farmingCare(idx);
		break;		
	default:
		break;
	}
	if (err != error_success && err != error_levelup) {
		hInst->mLabel->setString(wstring_to_utf8(logics::hInst->getErrorMessage(err)));
	}
	else if(productId != 0){
		string sz = wstring_to_utf8(logics::hInst->getItem(productId).name + L" x" + to_wstring(earning));
		hInst->mLabel->setString(sz);
	}
};

void FarmingScene::showItemSelect(int idx) {
	FARM_SIZE;
	this->removeChild(mPopupLayerBackground);
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