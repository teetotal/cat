/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{	
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);	
	listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);

	
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//gui::inst()->init();
	//srand(time(NULL));
	Vec2 a1 = gui::inst()->getPointVec2(0, 0, ALIGNMENT_NONE);
	Vec2 a2 = gui::inst()->getPointVec2(1, 1, ALIGNMENT_NONE);
	mGridSize.width = a2.x - a1.x;
	mGridSize.height = a1.y - a2.y;

	auto bg = Sprite::create(BG_FARM);
	bg->setContentSize(Director::getInstance()->getVisibleSize());
	bg->setAnchorPoint(Vec2(0, 0));
	bg->setOpacity(192);
	bg->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(bg);

	gui::inst()->addTextButton(0, 0, "BACK", this, CC_CALLBACK_1(HelloWorld::closeCallback, this), 0, ALIGNMENT_CENTER, Color3B::RED);
		
	int n = 0;
	farming::field * f = NULL;
	Color3B c[] = { Color3B(135, 118, 38), Color3B(123, 108, 5), Color3B(180, 164, 43), Color3B(72, 63, 4), Color3B(128, 104, 32) };


	while (logics::hInst->getFarm()->getField(n++, f)) {
		MainScene::field * p = (MainScene::field*)f;
		p->l = gui::inst()->createLayout(mGridSize, "", true, c[rand() % 5]);
		string sz = p->level > 0 ? to_string(p->level) : "";
		p->label = gui::inst()->addLabelAutoDimension(0, 2, sz, p->l, 8, ALIGNMENT_NONE, Color3B::WHITE, Size(1, 3), Size::ZERO, Size::ZERO);
		p->l->setPosition(gui::inst()->getPointVec2(p->x, p->y + 1, ALIGNMENT_NONE));
		p->isHarvestAction = false;

		if (p->seedId != 0) {
			if (p->seedId > 0 && p->seedId < 400)
				CCLOG("init error. id = %d, seedId = %d", p->id, p->seedId);

			p->sprite = Sprite::create(MainScene::getItemImg(p->seedId));
			Vec2 position = gui::inst()->getPointVec2(p->x, p->y);
			p->sprite->setPosition(position);
		}
		else {
			p->sprite = NULL;
		}

		this->addChild(p->l);
		if (p->sprite)
			this->addChild(p->sprite);
	}
	
	//seed Menu
	createSeedMenu();

	//gui::inst()->drawGrid(this);
	
	//quest	
	for (int n = 0; n < QUEST_CNT; n++) {
		mQuestLayer[n] = gui::inst()->createLayout(mGridSize, "", true);
		mQuestLayer[n]->setOpacity(128);
		mQuestLayer[n]->setPosition(gui::inst()->getPointVec2(n + 2, 1, ALIGNMENT_NONE));

		this->addChild(mQuestLayer[n]);
	}
	setQuest();
	
	mCharacterInitPosition = Vec2(0, 6);
	mCharacter = MainScene::getIdle();
	mCharacter->setPosition(gui::inst()->getPointVec2(mCharacterInitPosition.x, mCharacterInitPosition.y));
	//mCharacter->setAnchorPoint(Vec2(0.5, 0));
	this->addChild(mCharacter, 99);

	this->schedule(schedule_selector(HelloWorld::updateFarming), 1.f);

    return true;
}

void HelloWorld::setQuest() {
	//init
	for (int n = 0; n < QUEST_CNT; n++) 
		mQuestLayer[n]->removeAllChildren();

	//quest
	farming::questVector * vec = logics::hInst->farmingGetQuest(QUEST_CNT);
	Size imgSize = Size(mGridSize.height / 3, mGridSize.height / 3);
	for (int n = 0; n < vec->size(); n++) {
		Size gridSize = Size(3, 3);
		string sz = "";
		bool isComplete = true;

		int money = 0;
		for (int i = 0; i < sizeof(vec->at(n).items) / sizeof(vec->at(n).items[0]); i++) {
			if (vec->at(n).items[i].itemId != -1) {

				money += logics::hInst->farmingQuestReward(n);

				gui::inst()->addSpriteAutoDimension(0, i, MainScene::getItemImg(vec->at(n).items[i].itemId), mQuestLayer[n], ALIGNMENT_CENTER, gridSize, Size::ZERO, Size::ZERO)->setContentSize(imgSize);

				int quantity = logics::hInst->getActor()->inven.getItemQuantuty(logics::hInst->getInventoryType(vec->at(n).items[i].itemId), vec->at(n).items[i].itemId);
				if (quantity >= vec->at(n).items[i].quantity) {
					gui::inst()->addSpriteAutoDimension(1, i, "check.png", mQuestLayer[n], ALIGNMENT_CENTER, gridSize, Size::ZERO, Size::ZERO)->setContentSize(imgSize);
				}
				else {
					gui::inst()->addLabelAutoDimension(1, i, to_string(quantity), mQuestLayer[n], 12, ALIGNMENT_CENTER, Color3B::BLUE, gridSize, Size::ZERO, Size::ZERO);
					gui::inst()->addLabelAutoDimension(2, i, "/ " + to_string(vec->at(n).items[i].quantity), mQuestLayer[n], 12, ALIGNMENT_CENTER, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
					isComplete = false;
				}
			}
		}

		if (isComplete) {
			mQuestLayer[n]->removeAllChildren();
			gui::inst()->addTextButtonAutoDimension(0, 0, "Done", mQuestLayer[n]
				, CC_CALLBACK_1(HelloWorld::questCallback, this, n), 0, ALIGNMENT_CENTER, Color3B::BLUE, Size(1, 2), Size::ZERO, Size::ZERO);

			gui::inst()->addTextButtonAutoDimension(0, 1, "$" + to_string(money), mQuestLayer[n]
				, CC_CALLBACK_1(HelloWorld::questCallback, this, n), 0, ALIGNMENT_CENTER, Color3B::BLUE, Size(1, 2), Size::ZERO, Size::ZERO);
		}
	}
}

void HelloWorld::questCallback(cocos2d::Ref* pSender, int idx) {
	//done
	if (!logics::hInst->farmingQuestDone(idx)) {
		CCLOG("Failure. questCallback");
	}
	setQuest();
}

void HelloWorld::updateFarming(float fTimer) {
	int n = 0;
	farming::field * f;
	while (logics::hInst->getFarm()->getField(n++, f)) {
		MainScene::field * p = (MainScene::field*)f;

        if(p->seedId > 0 && p->seedId < 400)
            CCLOG("id = %d, seedId = %d", p->id, p->seedId);

		switch (p->status) {
		case farming::farming_status_decay:
			p->sprite->setColor(Color3B::RED);
			break;
		case farming::farming_status_grown:
			this->addChild(createClinkParticle(gui::inst()->getPointVec2(p->x, p->y, ALIGNMENT_CENTER)), 100);
			break;
		case farming::farming_status_harvest:
			if (p->isHarvestAction == false) {
				//it->second->sprite->runAction(RepeatForever::create(TintTo::create(1, Color3B::RED)));
				p->sprite->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.2, 1.1), ScaleTo::create(0.4, 1.f), NULL)));
				p->isHarvestAction = true;
			}
			break;
		default:
			break;
		}
	}
}

bool HelloWorld::onTouchBegan(Touch* touch, Event* event) {
	mTouchDownPosition = touch->getLocation();
	//찾기
	if (mCharacter->getBoundingBox().containsPoint(touch->getLocation())) {
		mMode = Mode_Farming;
		stopAction(mCharacter);
		mCharacter->runAction(getFarmingAnimation());
		return true;
	}

	int n = 0;
	farming::field * f;
	while (logics::hInst->getFarm()->getField(n++, f)) {
		MainScene::field * p = (MainScene::field*)f;
		if (p->sprite != NULL && p->sprite->getBoundingBox().containsPoint(touch->getLocation())) {
			mCurrentNodeId = p->id;
			p->sprite->setAnchorPoint(Vec2(0.5, 0.5));
			setOpacity();
			mMode = Mode_Seed;
			return true;
		}
	}
	mMode = Mode_Max;
	mCurrentNodeId = -1;
	
	return true;
}

void HelloWorld::swap(MainScene::field* a, MainScene::field * b) {
	MainScene::field temp;
	::memcpy(&temp, a, sizeof(temp));

	//a->l = b->l;
	a->sprite = b->sprite;
	//a->label = b->label;

	//b->l = temp.l;
	b->sprite = temp.sprite;
	//b->label = temp.label;

	logics::hInst->getFarm()->swap(a, b);
	
	if(a->sprite)
		a->sprite->setPosition(gui::inst()->getPointVec2(a->x, a->y, ALIGNMENT_CENTER));
	if(b->sprite)
		b->sprite->setPosition(gui::inst()->getPointVec2(b->x, b->y, ALIGNMENT_CENTER));

	string sz = a->level > 0 ? to_string(a->level) : "";
	a->label->setString(sz);

	sz = b->level > 0 ? to_string(b->level) : "";
	b->label->setString(sz);
}

bool HelloWorld::onTouchEnded(Touch* touch, Event* event) {

	if (mMode == Mode_Farming) {
		stopAction(mCharacter);
		mCharacter->runAction(MainScene::getIdleAnimation());
		mCharacter->setPosition(gui::inst()->getPointVec2(mCharacterInitPosition.x, mCharacterInitPosition.y));
		setQuest();
		return true;
	}

	if (mCurrentNodeId == -1)
		return true;

	clearOpacity();

	farming::field * f;
	logics::hInst->getFarm()->getField(mCurrentNodeId, f);
	MainScene::field * p = (MainScene::field*)f;
	if (abs(mTouchDownPosition.x - touch->getLocation().x) < (mGridSize.width / 2) && abs(mTouchDownPosition.y - touch->getLocation().y) < (mGridSize.height / 2)) {
		p->sprite->setPosition(gui::inst()->getPointVec2(p->x, p->y, ALIGNMENT_CENTER));
		return true;
	}
		
	//swap
	int n = 0;	
	while (logics::hInst->getFarm()->getField(n++, f)) {
		MainScene::field * pField = (MainScene::field*)f;
		if (pField->id == p->id)
			continue;
		
		if (pField->l->getBoundingBox().intersectsRect(p->sprite->getBoundingBox())) {
			if (p->seedId == pField->seedId && p->level == pField->level) {
				//merge
				levelUp(pField);				
				//current clear				
				clear(p);
				return true;
			} 			
			//swap
			swap(p, pField);

			return true;
		}
	}

	p->sprite->setPosition(gui::inst()->getPointVec2(p->x, p->y, ALIGNMENT_CENTER));
	
	return true;
}

void HelloWorld::onTouchMoved(Touch *touch, Event *event) {
	//int cnt = 0;
	bool isRemove = false;
	int nFileds = 0;
	int productId = 0;
	int earning = 0;
	errorCode err;
	int n = 0;
	farming::field * f;

	switch (mMode) {
	case Mode_Farming:
		mCharacter->setPosition(touch->getLocation());
		//harvest		
		while (logics::hInst->getFarm()->getField(n++, f)) {
			MainScene::field * pField = (MainScene::field*)f;

			if (pField->seedId > 0 && pField->seedId < 400)
				CCLOG("id = %d, seedId = %d", pField->id, pField->seedId);

			if (pField->sprite != NULL && mCharacter->getBoundingBox().intersectsRect(pField->sprite->getBoundingBox())) {
				stopAction(pField->sprite);
				
				switch (pField->status) {
				case farming::farming_status_decay:
				case farming::farming_status_harvest:
					isRemove = true;					
				case farming::farming_status_grown:											
					err = logics::hInst->farmingHarvest(pField->id, productId, earning);
					if (err != error_success) {
						CCLOG("errorCode = %d, seedId = %d,  %s", err, pField->seedId, wstring_to_utf8(logics::hInst->getErrorMessage(err)).c_str());
						return;
					}
					if(earning > 0)
						plantAnimation(pField, productId, earning);
					
					break;

                    default:
					break;
				}

				
				if(isRemove)
					clear(pField);
				break;
			}
		}
		break;
	case Mode_Seed:
		if (mCurrentNodeId != -1) {			
			logics::hInst->getFarm()->getField(mCurrentNodeId, f);
			MainScene::field * pField = (MainScene::field *)f;
			pField->sprite->setPosition(touch->getLocation());
		}
			
		break;
	default:
		break;
	}
}

void HelloWorld::plantAnimation(MainScene::field * node, int productId, int cnt) {
	for (int n = 0; n < cnt; n++) {
		auto sprite1 = Sprite::create(MainScene::getItemImg(productId));
		sprite1->setPosition(gui::inst()->getPointVec2(node->x, node->y));
		sprite1->setContentSize(Size(50, 50));

		this->addChild(sprite1);
		auto delay1 = DelayTime::create(n * 0.05);
		auto delay2 = delay1->clone();
		auto seq1 = Sequence::create(delay1, MoveTo::create(0.5, Vec2(0, 0)), RemoveSelf::create(), NULL);
		auto seq2 = Sequence::create(delay2, ScaleTo::create(0.2, 1.5), ScaleTo::create(0.3, 0.5), NULL);
		sprite1->runAction(Spawn::create(seq1, seq2, NULL));
	}
}

void HelloWorld::levelUp(MainScene::field * p) {
	logics::hInst->getFarm()->levelup(p->id);
	p->label->setString(to_string(p->level));
	p->isHarvestAction = false;
	stopAction(p->sprite);
	p->sprite->runAction(Sequence::create(ScaleTo::create(0.1, 1.5), ScaleTo::create(0.1, 1), NULL));
}
void HelloWorld::clear(MainScene::field * p) {
	p->label->setString("");
	p->isHarvestAction = false;
	this->removeChild(p->sprite);
	p->sprite = NULL;
	logics::hInst->getFarm()->clear(p->id);
}

void HelloWorld::setOpacity() {
	if (mCurrentNodeId == EMPTY_PLANT_TAG)
		return;

	farming::field * p;
	logics::hInst->getFarm()->getField(mCurrentNodeId, p);

	int n = 0;
	farming::field *f;
	while (logics::hInst->getFarm()->getField(n++, f)) {
		MainScene::field * pField = (MainScene::field*)f;
		
		if (pField->id == mCurrentNodeId || pField->sprite == NULL)
			continue;
		if (pField->level == p->level && pField->seedId == p->seedId) {

		}
		else {
			pField->sprite->setOpacity(OPACITY_DIABLE);
		}
	}
}
void HelloWorld::clearOpacity() {
	int n = 0;
	farming::field *f;
	while (logics::hInst->getFarm()->getField(n++, f)) {
		MainScene::field * pField = (MainScene::field*)f;

		if(pField->sprite)
			pField->sprite->setOpacity(OPACITY_MAX);
	}
}

void HelloWorld::createSeedMenu()
{
	Vec2 start, end;
	start = Vec2(8, 6);
	end = Vec2(start.x + 1, 0);

	vector<intPair> v;
	logics::hInst->getActor()->inven.getWarehouse(v, inventoryType_farming);
	int cnt = v.size();

	Size sizeOfScrollView = gui::inst()->getScrollViewSize(start, end, Size::ZERO, Size::ZERO);
	mScrollView = gui::inst()->addScrollView(start, end, Size::ZERO, Size::ZERO, "", Size(sizeOfScrollView.width, 30 * cnt), this);
	
	
	for (int n = 0; n < cnt; n++) {
		seed* s = new seed;
		s->itemId = v[n].key;
		s->itemQuantity = v[n].val;
		mSeedVector.push_back(s);
	}

	addSeedMenu();
}

void HelloWorld::addSeedMenu() {
	mScrollView->removeAllChildren();
	mScrollView->setInnerContainerSize(Size(mScrollView->getContentSize().width, 30 * mSeedVector.size()));

	for (int n = 0; n < mSeedVector.size(); n++) {
		seed * s = mSeedVector[n];
		s->layout = gui::inst()->createLayout(Size(30, 30));
		gui::inst()->addSpriteAutoDimension(0, 0, MainScene::getItemImg(s->itemId), s->layout, ALIGNMENT_CENTER, Size(1, 1), Size::ZERO, Size::ZERO);
		s->label = gui::inst()->addTextButtonAutoDimension(0, 0, "x" + to_string(s->itemQuantity), s->layout
			, CC_CALLBACK_1(HelloWorld::seedCallback, this, n)
			, 0, ALIGNMENT_CENTER, Color3B::BLACK, Size(1, 1), Size::ZERO, Size::ZERO);

		gui::inst()->addLayoutToScrollView(mScrollView, s->layout, 1, 1);
	}	
}

void HelloWorld::addSprite(MainScene::field * p, int seedId) {
	p->sprite = Sprite::create(MainScene::getItemImg(seedId));
	Vec2 position = gui::inst()->getPointVec2(p->x, p->y);
	p->sprite->setPosition(position);
	this->addChild(p->sprite, 1);
}

void HelloWorld::seedCallback(cocos2d::Ref * pSender, int seedIdx)
{
	seed * s = mSeedVector[seedIdx];
	s->itemQuantity--;	
	
	int n = 0;
	farming::field *f;
	while (logics::hInst->getFarm()->getField(n++, f)) {
		MainScene::field * p = (MainScene::field*)f;
	
		if (p->seedId != 0)
			continue;
		logics::hInst->farmingPlant(p->id, s->itemId);
		
		addSprite(p, s->itemId);
		/*
		p->sprite = Sprite::create(MainScene::getItemImg(s->itemId));
		Vec2 position = gui::inst()->getPointVec2(p->x, p->y);
		p->sprite->setPosition(position);
		this->addChild(p->sprite, 1);
		*/
		p->label->setString(to_string(p->level));

		break;
	}

	if (s->itemQuantity <= 0) {
		//seed menu 갱신
		mScrollView->removeChild(s->layout);
		mSeedVector.erase(mSeedVector.begin() + seedIdx);
		delete s;
		addSeedMenu();
	}
	else {
		s->label->setString("x" + to_string(s->itemQuantity));
	}
}
/*
int HelloWorld::getPlantId() {
	int max = mMap.size();
	for (int n = 0; n < max; n++) {
		if (mPlantMap.find(n) == mPlantMap.end())
			return n;
	}
	return EMPTY_PLANT_TAG;
}
*/
RepeatForever * HelloWorld::getFarmingAnimation() {

	auto animation = Animation::create();
	animation->setDelayPerUnit(0.1);

	string path;
	for (int n = 0; n < 8; n++) {
		path = "action/201/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}

	return RepeatForever::create(Animate::create(animation));
}

void HelloWorld::menuCloseCallback(Ref* pSender)
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
