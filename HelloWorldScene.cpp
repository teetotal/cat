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

	/*
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

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
	*/
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
	/*
    auto label = Label::createWithTTF("ACT Nimble", "fonts/Marker Felt.ttf", 24);
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

	auto textField = ui::TextField::create("ID", "fonts/Marker Felt.ttf", 30);
	textField->setColor(Color3B::WHITE);
	textField->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	textField->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		//std::cout << "editing a TextField" << std::endl;
	});

	this->addChild(textField);

	auto textFieldPW = ui::TextField::create("Password", "fonts/Marker Felt.ttf", 30);
	textFieldPW->setColor(Color3B::WHITE);
	textFieldPW->setPasswordEnabled(true);
	textFieldPW->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 30));
	textFieldPW->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		//std::cout << "editing a TextField" << std::endl;
	});

	this->addChild(textFieldPW);

	gui::inst()->addLabel(4, 4, "Login", this, 30);
	*/
	const int max = 3;
	int id = 0;
	Color3B c[] = { Color3B(135, 118, 38), Color3B(123, 108, 5), Color3B(180, 164, 43), Color3B(72, 63, 4), Color3B(128, 104, 32) };
	
	for (int x = 2; x < 7; x++) {
		for (int y = 2; y < 7; y++) {
			field * node = new field();
			node->l = gui::inst()->createLayout(mGridSize, "", true, c[rand() % 5]);
			node->tag = id;						
			node->plantTag = EMPTY_PLANT_TAG;
			//gui::inst()->drawGrid(this);
			node->label = gui::inst()->addLabelAutoDimension(0, 2, "", node->l, 8, ALIGNMENT_NONE, Color3B::WHITE, Size(1, 3), Size::ZERO, Size::ZERO);
			node->l->setTag(id);
			node->position = gui::inst()->getPointVec2(x, y, ALIGNMENT_NONE);
			node->l->setPosition(node->position);
			//l->setOpacity(128);
			this->addChild(node->l);
			
			mMap[id] = node;
			id++;
		}		
	}
	
	createSeedMenu();
	
	//캐릭터
	mCharacterInitPosition = Vec2(0, 6);
	mCharacter = MainScene::getIdle();
	mCharacter->setPosition(gui::inst()->getPointVec2(mCharacterInitPosition.x, mCharacterInitPosition.y));
	//mCharacter->setAnchorPoint(Vec2(0.5, 0));
	this->addChild(mCharacter, 99);

	this->schedule(schedule_selector(HelloWorld::updateFarming), 1.f);

    return true;
}

HelloWorld::Plant_Status HelloWorld::getStatus(plant * p, int &cnt) {
	int t = getNow() - p->start; //심어서 부터 지금까지 걸린 시간.
	int total = 1 << (p->level - 1);

	int cntCrop = min(t / PLANT_COMPLETE_SEC, total); // 수확물
	if (cntCrop + p->accumulation >= total) {
		cnt = total - p->accumulation;
		return Plant_Status_Harvest;
	}
	else if (cntCrop > p->accumulation) {
		cnt = cntCrop - p->accumulation;
		return Plant_Status_Crop;
	}
	cnt = 0;
	return Plant_Status_Max;
}

void HelloWorld::updateFarming(float f) {
	for (plantMap::iterator it = mPlantMap.begin(); it != mPlantMap.end(); ++it) {
		int cnt = 0;
		switch (getStatus(it->second, cnt)) {
		case Plant_Status_Crop:
			this->addChild(createClinkParticle(it->second->position), 100);
			break;
		case Plant_Status_Harvest:
			if (it->second->isHarvestAction == false) {
				//it->second->sprite->runAction(RepeatForever::create(TintTo::create(1, Color3B::RED)));
				it->second->sprite->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.2, 1.1), ScaleTo::create(0.4, 1.f), NULL)));

				it->second->isHarvestAction = true;
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

	for (plantMap::iterator it = mPlantMap.begin(); it != mPlantMap.end(); ++it) {
		if (it->second->sprite->getBoundingBox().containsPoint(touch->getLocation())) {
			//CCLOG("%d", it->first);
			mCurrentNodeId = it->first;
			mPlantMap[mCurrentNodeId]->sprite->setAnchorPoint(Vec2(0.5, 0.5));
			setOpacity();
			mMode = Mode_Seed;
			return true;
		}
	}
	mMode = Mode_Max;
	mCurrentNodeId = -1;
	
	return true;
}

void HelloWorld::swap(plant* a, plant * b) {
	int tempFieldTag = a->fieldTag;
	Vec2 tempPosition = a->position;
	
	a->fieldTag = b->fieldTag;
	a->position = b->position;
	b->fieldTag = tempFieldTag;
	b->position = tempPosition;

	mMap[a->fieldTag]->label->setString(to_string(a->level));
	mMap[a->fieldTag]->plantTag = a->tag;
	mMap[b->fieldTag]->label->setString(to_string(b->level));
	mMap[b->fieldTag]->plantTag = b->tag;

	a->sprite->setPosition(a->position);
	b->sprite->setPosition(b->position);
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

	plant * p =mPlantMap[mCurrentNodeId];
	
	if (abs(mTouchDownPosition.x - touch->getLocation().x) < (mGridSize.width / 2) && abs(mTouchDownPosition.y - touch->getLocation().y) < (mGridSize.height / 2)) {
		p->sprite->setPosition(p->position);
		return true;
	}
		
	//swap
	for (plantMap::iterator it = mPlantMap.begin(); it != mPlantMap.end(); ++it) {
		if (it->first == mCurrentNodeId)
			continue;

		if (p->sprite->getBoundingBox().intersectsRect(it->second->sprite->getBoundingBox())) {
			if (p->type == it->second->type && p->level == it->second->level) {
				//merge
				levelUp(it->first);
				//current clear
				clear(mCurrentNodeId);
				//target levelup
				return true;
			} 
			//swap
			swap(p, it->second);

			return true;
		}
	}
	//빈 layer
	for (fieldMap::iterator it = mMap.begin(); it != mMap.end(); ++it) {
		if (it->second->l->getBoundingBox().intersectsRect(p->sprite->getBoundingBox())) {			
			if (it->second->plantTag != EMPTY_PLANT_TAG) 
				break;

			it->second->label->setString(to_string(p->level));
			mMap[p->fieldTag]->label->setString("");
			mMap[p->fieldTag]->plantTag = EMPTY_PLANT_TAG;
			it->second->plantTag = p->tag;
			p->fieldTag = it->first;
			
			Vec2 position = mMap[p->fieldTag]->position;
			Size size = mMap[p->fieldTag]->l->getContentSize();
			position.x += size.width / 2;
			position.y += size.height / 2;

			p->position = position;
			p->sprite->setPosition(position);

			return true;
		}
	}

	//제 자리로
	mPlantMap[mCurrentNodeId]->sprite->setPosition(mPlantMap[mCurrentNodeId]->position);
	
	return true;
}
void HelloWorld::onTouchMoved(Touch *touch, Event *event) {
	int cnt = 0;

	switch (mMode) {
	case Mode_Farming:
		mCharacter->setPosition(touch->getLocation());
		//harvest
		for (plantMap::iterator it = mPlantMap.begin(); it != mPlantMap.end(); ++it) {
			if (mCharacter->getBoundingBox().intersectsRect(it->second->sprite->getBoundingBox())) {
				stopAction(it->second->sprite);

				switch (getStatus(it->second, cnt)) {
				case Plant_Status_Crop:
					plantAnimation(it->second, cnt);
					it->second->accumulation += cnt;
					break;
				case Plant_Status_Harvest:
					plantAnimation(it->second, cnt);
					it->second->accumulation += cnt;
					clear(it->first);
					break;
				case Plant_Status_Max:
					break;
				}
				break;
			}
		}
		break;
	case Mode_Seed:
		if (mCurrentNodeId != -1)
			mPlantMap[mCurrentNodeId]->sprite->setPosition(touch->getLocation());
		break;
	default:
		break;
	}
}

void HelloWorld::plantAnimation(plant * node, int cnt) {
	for (int n = 0; n < cnt; n++) {
		auto sprite1 = Sprite::create(MainScene::getItemImg(node->type));
		sprite1->setPosition(Vec2(node->position));
		sprite1->setContentSize(Size(50, 50));

		this->addChild(sprite1);
		auto delay1 = DelayTime::create(n * 0.05);
		auto delay2 = delay1->clone();
		auto seq1 = Sequence::create(delay1, MoveTo::create(0.5, Vec2(0, 0)), RemoveSelf::create(), NULL);
		auto seq2 = Sequence::create(delay2, ScaleTo::create(0.2, 1.5), ScaleTo::create(0.3, 0.5), NULL);
		sprite1->runAction(Spawn::create(seq1, seq2, NULL));
	}
}

void HelloWorld::levelUp(int tag) {	
	plant * node = mPlantMap[tag];
	node->level++;
	mMap[node->fieldTag]->label->setString(to_string(node->level));
	mMap[node->fieldTag]->plantTag = node->tag;
	node->isHarvestAction = false;
	stopAction(node->sprite);
}
void HelloWorld::clear(int tag) {
	
	mMap[mPlantMap[tag]->fieldTag]->label->setString("");
	mMap[mPlantMap[tag]->fieldTag]->plantTag = EMPTY_PLANT_TAG;
	mPlantMap[tag]->level = 0;
	mPlantMap[tag]->type = 0;
	this->removeChild(mPlantMap[tag]->sprite);
	mPlantMap.erase(tag);
		
}

void HelloWorld::setOpacity() {
	if (mCurrentNodeId == EMPTY_PLANT_TAG)
		return;

	plant* p = mPlantMap[mCurrentNodeId];
	for (plantMap::iterator it = mPlantMap.begin(); it != mPlantMap.end(); ++it) {
		if (it->first == mCurrentNodeId)
			continue;
		if (it->second->level == p->level && it->second->type == p->type) {

		}
		else {
			it->second->sprite->setOpacity(OPACITY_DIABLE);
		}
	}
}
void HelloWorld::clearOpacity() {
	for (plantMap::iterator it = mPlantMap.begin(); it != mPlantMap.end(); ++it) {
		it->second->sprite->setOpacity(OPACITY_MAX);
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

void HelloWorld::seedCallback(cocos2d::Ref * pSender, int seedIdx)
{
	seed * s = mSeedVector[seedIdx];
	s->itemQuantity--;
	logics::hInst->addInventory(s->itemId, -1);	
	
	for (fieldMap::iterator it = mMap.begin(); it != mMap.end(); ++it) {
		field * p = it->second;
		if (p->plantTag != EMPTY_PLANT_TAG)
			continue;

		plant * o = new plant;
		o->fieldTag = p->tag;
		o->tag = getPlantId();
		p->plantTag = o->tag;
		o->level = 1;
		o->type = s->itemId;
		o->sprite = Sprite::create(MainScene::getItemImg(s->itemId));

		Vec2 position = p->l->getPosition();
		Size size = p->l->getContentSize();
		position.x += size.width / 2;
		position.y += size.height / 2;

		o->sprite->setPosition(position);
		o->position = position;
		//o->sprite->setContentSize(Size(mGridSize.height / 2, mGridSize.height / 2));
		this->addChild(o->sprite, 1);

		mPlantMap[o->tag] = o;

		p->label->setString("1");
		
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

int HelloWorld::getPlantId() {
	int max = mMap.size();
	for (int n = 0; n < max; n++) {
		if (mPlantMap.find(n) == mPlantMap.end())
			return n;
	}
	return EMPTY_PLANT_TAG;
}

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
