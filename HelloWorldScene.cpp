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
#include "MainScene.h"

USING_NS_CC;

#define EMPTY_PLANT_TAG -1
#define OPACITY_MAX 255
#define OPACITY_DIABLE 64
#define PLANT_COMPLETE_SEC 10

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
		
	const int max = 3;
	int id = 0;
	Color3B c[] = { Color3B(135, 118, 38), Color3B(123, 108, 5), Color3B(180, 164, 43), Color3B(72, 63, 4), Color3B(128, 104, 32) };
	
	//farm init or load
	if (logics::hInst->getFarm()->countField() == 0) {
		for (int x = 2; x < 7; x++) {
			for (int y = 2; y < 7; y++) {
				field * node = new field(x, y - 1);
				node->sprite = NULL;
				node->l = gui::inst()->createLayout(mGridSize, "", true, c[rand() % 5]);
				node->id = id;
				node->label = gui::inst()->addLabelAutoDimension(0, 2, "", node->l, 8, ALIGNMENT_NONE, Color3B::WHITE, Size(1, 3), Size::ZERO, Size::ZERO);
				//node->l->setTag(id);
				node->l->setPosition(gui::inst()->getPointVec2(x, y, ALIGNMENT_NONE));
				this->addChild(node->l);
				logics::hInst->getFarm()->addField(node);
				//mMap[id] = node;
				id++;
			}
		}
	}
	else {
		farming::fields * fields = logics::hInst->getFarm()->getFields();
		int nCnt = fields->size();
		for (int n = 0; n < nCnt; n++) {
			field * p = (field*)fields->at(n);

			p->sprite = NULL;
			p->l = gui::inst()->createLayout(mGridSize, "", true, c[rand() % 5]);
			string sz = p->level > 0 ? to_string(p->level) : "";
			p->label = gui::inst()->addLabelAutoDimension(0, 2, sz, p->l, 8, ALIGNMENT_NONE, Color3B::WHITE, Size(1, 3), Size::ZERO, Size::ZERO);
			p->l->setPosition(gui::inst()->getPointVec2(p->x, p->y + 1, ALIGNMENT_NONE));
			this->addChild(p->l);

			if (p->seedId != 0) {
				addSprite(p, p->seedId);

			}
		}
	}
	
	
	createSeedMenu();

	//gui::inst()->drawGrid(this);
	
	//캐릭터
	mCharacterInitPosition = Vec2(0, 6);
	mCharacter = MainScene::getIdle();
	mCharacter->setPosition(gui::inst()->getPointVec2(mCharacterInitPosition.x, mCharacterInitPosition.y));
	//mCharacter->setAnchorPoint(Vec2(0.5, 0));
	this->addChild(mCharacter, 99);

	this->schedule(schedule_selector(HelloWorld::updateFarming), 1.f);

    return true;
}


void HelloWorld::updateFarming(float f) {
	farming::fields * fields = logics::hInst->getFarm()->getFields();
	for (int n = 0; n < fields->size(); n++) {
		int cnt = 0;		
		field * p = (field*)fields->at(n);
		switch (fields->at(n)->status) {
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

	farming::fields * fields = logics::hInst->getFarm()->getFields();
	for (int n = 0; n < fields->size(); n++) {
	//for (plantMap::iterator it = mPlantMap.begin(); it != mPlantMap.end(); ++it) {
		field * p = (field*)fields->at(n);
		if (p->sprite != NULL && p->sprite->getBoundingBox().containsPoint(touch->getLocation())) {
			//CCLOG("%d", it->first);
			mCurrentNodeId = n;
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

void HelloWorld::swap(field* a, field * b) {
	field temp;
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
		return true;
	}

	if (mCurrentNodeId == -1)
		return true;

	clearOpacity();

	field * p = (field *)logics::hInst->getFarm()->getFields()->at(mCurrentNodeId);
	
	if (abs(mTouchDownPosition.x - touch->getLocation().x) < (mGridSize.width / 2) && abs(mTouchDownPosition.y - touch->getLocation().y) < (mGridSize.height / 2)) {
		p->sprite->setPosition(gui::inst()->getPointVec2(p->x, p->y, ALIGNMENT_CENTER));
		return true;
	}
		
	//swap
	int nFileds = logics::hInst->getFarm()->countField();
	for (int n = 0; n < nFileds; n++) {
		field * pField = (field *)logics::hInst->getFarm()->getFields()->at(n);
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

	//제 자리로
	p->sprite->setPosition(gui::inst()->getPointVec2(p->x, p->y, ALIGNMENT_CENTER));
	
	return true;
}

void HelloWorld::onTouchMoved(Touch *touch, Event *event) {
	//int cnt = 0;
	bool isRemove = false;
	int nFileds = 0;
	switch (mMode) {
	case Mode_Farming:
		mCharacter->setPosition(touch->getLocation());
		//harvest
		nFileds = logics::hInst->getFarm()->countField();
		for (int n = 0; n < nFileds; n++) {
			field * pField = (field *)logics::hInst->getFarm()->getFields()->at(n);
		
			if (pField->sprite != NULL && mCharacter->getBoundingBox().intersectsRect(pField->sprite->getBoundingBox())) {
				stopAction(pField->sprite);
								
				switch (pField->status) {
				case farming::farming_status_harvest:
					isRemove = true;					
				case farming::farming_status_grown:
					//수확한 만큼 인벤토리로
					if (pField->getGrownCnt() > 0) {
						int productId = 0;
						int earning = 0;
						logics::hInst->farmingHarvest(pField->id, productId, earning);						
						plantAnimation(pField, productId, earning);
					}
					break;
				case Plant_Status_Max:
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
			field * pField = (field *)logics::hInst->getFarm()->getFields()->at(mCurrentNodeId);
			pField->sprite->setPosition(touch->getLocation());
		}
			
		break;
	default:
		break;
	}
}

void HelloWorld::plantAnimation(field * node, int productId, int cnt) {
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

void HelloWorld::levelUp(field * p) {
	logics::hInst->getFarm()->levelup(p);
	p->label->setString(to_string(p->level));
	p->isHarvestAction = false;
	stopAction(p->sprite);
	p->sprite->runAction(Sequence::create(ScaleTo::create(0.1, 1.5), ScaleTo::create(0.1, 1), NULL));
}
void HelloWorld::clear(field * p) {
	p->label->setString("");
	p->isHarvestAction = false;
	this->removeChild(p->sprite);
	p->sprite = NULL;
	logics::hInst->getFarm()->clear(p);
}

void HelloWorld::setOpacity() {
	if (mCurrentNodeId == EMPTY_PLANT_TAG)
		return;

	field * p = (field *)logics::hInst->getFarm()->getFields()->at(mCurrentNodeId);
	//plant* p = mPlantMap[mCurrentNodeId];
	int nFileds = logics::hInst->getFarm()->countField();
	for (int n = 0; n < nFileds; n++) {
		field * pField = (field *)logics::hInst->getFarm()->getFields()->at(n);	
		
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
	int nFileds = logics::hInst->getFarm()->countField();
	for (int n = 0; n < nFileds; n++) {
		field * pField = (field *)logics::hInst->getFarm()->getFields()->at(n);	
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

void HelloWorld::addSprite(field * p, int seedId) {
	p->sprite = Sprite::create(MainScene::getItemImg(seedId));
	Vec2 position = gui::inst()->getPointVec2(p->x, p->y);
	p->sprite->setPosition(position);
	this->addChild(p->sprite, 1);
}

void HelloWorld::seedCallback(cocos2d::Ref * pSender, int seedIdx)
{
	seed * s = mSeedVector[seedIdx];
	s->itemQuantity--;	
	
	int nFileds = logics::hInst->getFarm()->countField();
	for (int n = 0; n < nFileds; n++) {
		field * p = (field *)logics::hInst->getFarm()->getFields()->at(n);
	
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
