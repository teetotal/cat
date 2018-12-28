#include "FarmingScene.h"
#include "AlertScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* FarmingScene::createScene()
{
    return FarmingScene::create();
}

// on "init" you need to initialize your instance
bool FarmingScene::init()
{	
	//init variable
	mPopupBackground = NULL;
	mPopupLayer = NULL;
    mSelectedField = NULL;
    mSelectedFieldLayer = NULL;
    mCurrentNodeId = -1;
	mThiefCnt = 0;
	mMode = Mode_Max;
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(FarmingScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(FarmingScene::onTouchEnded, this);	
	listener->onTouchMoved = CC_CALLBACK_2(FarmingScene::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
	Vec2 a1 = gui::inst()->getPointVec2(0, 0, ALIGNMENT_NONE);
	Vec2 a2 = gui::inst()->getPointVec2(1, 1, ALIGNMENT_NONE);
	mGridSize.width = a2.x - a1.x;
	mGridSize.height = a1.y - a2.y;

    
//    auto bg = gui::inst()->addBG("layers/dark_forest/sky.png", this);
//    bg->setLocalZOrder(-2);
//    bg->setOpacity(192);
	gui::inst()->addTextButton(0, 0, "BACK", this, CC_CALLBACK_1(FarmingScene::closeCallback, this), 0, ALIGNMENT_CENTER, Color3B::RED);
    gui::inst()->addTextButton(8, 6, "Extend", this, [=](Ref * pSender) {
        this->removeChild(mMainLayoput);
        logics::hInst->getActor()->farmExtendCnt = min(logics::hInst->getActor()->farmExtendCnt + 1, 6);
        initDeco();
    }, 0, ALIGNMENT_CENTER, Color3B::RED);
	mPoint = gui::inst()->addLabel(8, 0, COIN + to_string(logics::hInst->getActor()->point), this);
		
	//seed Menu
	createSeedMenu();

	//grid
	//gui::inst()->drawGrid(this);
	
    gui::inst()->setScale(gui::inst()->addSprite(2, 0, "owl.png", this), 50);
	//quest	
	for (int n = 0; n < QUEST_CNT; n++) {
		mQuestLayer[n] = gui::inst()->createLayout(mGridSize, "", true);
		mQuestLayer[n]->setOpacity(128);
		mQuestLayer[n]->setPosition(gui::inst()->getPointVec2(n + 3, 1, ALIGNMENT_NONE));

		this->addChild(mQuestLayer[n]);
	}
	setQuest();
	
	mCharacterInitPosition = Vec2(0, 6);
	mCharacter = MainScene::getIdle();
	mCharacter->setPosition(gui::inst()->getPointVec2(mCharacterInitPosition.x, mCharacterInitPosition.y));
	//mCharacter->setAnchorPoint(Vec2(0.5, 0));
    //gui::inst()->setScale(mCharacter, 50);
    onActionFinished();
	this->addChild(mCharacter, 99);
    
    initDeco();
//    mMainLayoput->addChild(ParticleRain::create());

	updateFarming(0);
	this->schedule(schedule_selector(FarmingScene::updateFarming), 1.f);

    return true;
}

void FarmingScene::initDeco() {
    const float degrees = 27.f;
    const float _div = logics::hInst->getActor()->farmExtendCnt * 6;
    
//    const float layerWidth = gui::inst()->getTanLen(Director::getInstance()->getVisibleSize().height / 2, degrees) * 2;
//    mMainLayoput = gui::inst()->createLayout(Size(layerWidth, Director::getInstance()->getVisibleSize().height), "", false, Color3B::GRAY);

    float mainLayerHeight = (Director::getInstance()->getVisibleSize().height / 2) * logics::hInst->getActor()->farmExtendCnt;
    const float layerWidth = gui::inst()->getTanLen(mainLayerHeight / 2, degrees) * 2;
    mMainLayoput = gui::inst()->createLayout(Size(layerWidth, mainLayerHeight), "", false, Color3B::GRAY);
    //wall bg
    mMainLayoput->addChild(gui::inst()->getfittedSprite("layers/dark_forest/sky.png", mMainLayoput));
    
//    auto bgLeft = Sprite::create("layers/dark_forest/sky.png");
//    bgLeft->setContentSize(Size(mMainLayoput->getContentSize().width / 2 + 13, mMainLayoput->getContentSize().height));
//    bgLeft->setRotationSkewY(-1*degrees);
//    bgLeft->setAnchorPoint(Vec2::ZERO);
//    mMainLayoput->addChild(bgLeft);
    auto bgRight = Sprite::create("layers/dark_forest/clouds_2.png");
    bgRight->setContentSize(Size(mMainLayoput->getContentSize().width / 2 + 13, mMainLayoput->getContentSize().height));
    bgRight->setRotationSkewY(degrees);
    bgRight->setPosition(Vec2(mMainLayoput->getContentSize().width, 0));
    bgRight->setAnchorPoint(Vec2(1,0));
    mMainLayoput->addChild(bgRight);
    
//    mMainLayoput->addChild(gui::inst()->getfittedSprite("layers/dark_forest/clouds_2.png", mMainLayoput));
//    mMainLayoput->addChild(gui::inst()->getfittedSprite("layers/dark_forest/rocks.png", mMainLayoput));
//    mMainLayoput->addChild(gui::inst()->getfittedSprite("layers/dark_forest/ground_1.png", mMainLayoput));
//    mMainLayoput->addChild(gui::inst()->getfittedSprite("layers/dark_forest/ground_2.png", mMainLayoput));
//    mMainLayoput->addChild(gui::inst()->getfittedSprite("layers/dark_forest/ground_3.png", mMainLayoput));
    mUIDeco.finalize();
    mUIDeco.init(mMainLayoput, degrees, false, false);
    
//    mUIDeco.addWall(_div/ 8, Color4F::WHITE, Color4F::BLACK);
    mUIDeco.addBottom(_div, _div, Color4F(Color3B(135, 118, 38)), Color4F(Color3B(123, 108, 5)));
//    mUIDeco.addBottom(_div, _div, Color4F::GRAY, Color4F::YELLOW);
    mUIDeco.drawGuideLine(true, false);
    
    Vec2 posMainLayer = gui::inst()->getCenter();
    posMainLayer.y  += mMainLayoput->getContentSize().height / 2;
    mMainLayoput->setAnchorPoint(Vec2(0.5, 0.5));
    mMainLayoput->setPosition(posMainLayer);
    
//    mMainLayoput->setScale(2);
    
    this->addChild(mMainLayoput, -1);
    
    
    for (int n = (int)logics::hInst->getFarm()->getFields()->size(); n < mUIDeco.getBottomVecSize(); n++) {
        MainScene::field * node = new MainScene::field();
        node->id = n;
        logics::hInst->farmingAddField(node);
    }
    
    int n = 0;
    farming::field * f = NULL;
    MainScene::field * p = NULL;
    Color3B c[] = { Color3B(135, 118, 38), Color3B(123, 108, 5), Color3B(180, 164, 43), Color3B(72, 63, 4), Color3B(128, 104, 32) };
    
    while (logics::hInst->getFarm()->getField(n, f)) {
        p = (MainScene::field*)f;
        p->l = gui::inst()->createLayout(mUIDeco.getBottomGridSize(), "", false, c[n % 5]);
        string sz = p->level > 0 ? to_string(p->level) : "";
        
         //        auto label = Label::create();
        //        label->setString(to_string(p->id));
        //        label->setPosition(mUIDeco.getBottomPos(p->id, true));
        //        label->setColor(Color3B::BLACK);
        //        mMainLayoput->addChild(label);
        p->l->setPosition(mUIDeco.getBottomPos(p->id, false));
        p->isHarvestAction = false;
        
        if (p->seedId != 0) {
            if (p->seedId > 0 && p->seedId < 400)
                CCLOG("init error. id = %d, seedId = %d", p->id, p->seedId);
            
            p->sprite = Sprite::create(MainScene::getItemImg(p->seedId));
            Vec2 position = getSpritePos(p);
            p->sprite->setPosition(position);
            p->sprite->setScale(getScaleRatio(p));
            //label
            if(!logics::hInst->getFarm()->getSeed(p->seedId)->isDeco)
                setLabel(sz, p);
        }
        else {
            p->sprite = NULL;
            //bg
//            string szImg = (n % 3 == 0) ? "field.png" : "rock.png";
//            Sprite * img = gui::inst()->getfittedSprite(szImg, p->l);
//            img->setScale(0.5);
//            img->setFlippedX(n % 2 == 0 ? true : false);
//            p->l->addChild(img);
        }
        
        //this->addChild(p->l, 0);
        mMainLayoput->addChild(p->l);
        if (p->sprite)
            mMainLayoput->addChild(p->sprite, 1);
        n++;
    }
}

void FarmingScene::updatePoint() {	
	string sz = COIN + numberFormat(logics::hInst->getActor()->point);
	if (sz.compare(mPoint->getString()) != 0) {
		mPoint->setString(sz);
		mPoint->runAction(gui::inst()->createActionFocus());
	}
	
}

void FarmingScene::setQuest() {
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

		int money = logics::hInst->farmingQuestReward(n);

		for (int i = 0; i < sizeof(vec->at(n).items) / sizeof(vec->at(n).items[0]); i++) {
			if (vec->at(n).items[i].itemId != -1) {


				gui::inst()->addSpriteAutoDimension(0, i, MainScene::getItemImg(vec->at(n).items[i].itemId), mQuestLayer[n], ALIGNMENT_CENTER, gridSize, Size::ZERO, Size::ZERO)->setContentSize(imgSize);

				int quantity = logics::hInst->getActor()->inven.getItemQuantuty(logics::hInst->getInventoryType(vec->at(n).items[i].itemId), vec->at(n).items[i].itemId);
				if (quantity >= vec->at(n).items[i].quantity) {
					gui::inst()->addSpriteAutoDimension(1, i, "check.png", mQuestLayer[n], ALIGNMENT_CENTER, gridSize, Size::ZERO, Size::ZERO)->setContentSize(imgSize);
				}
				else {
					gui::inst()->addLabelAutoDimension(1, i, to_string(quantity), mQuestLayer[n], 12, ALIGNMENT_CENTER, Color3B::BLUE, gridSize, Size::ZERO, Size::ZERO);
					gui::inst()->addLabelAutoDimension(2, i, "/" + to_string(vec->at(n).items[i].quantity), mQuestLayer[n], 10, ALIGNMENT_NONE, Color3B::BLACK, gridSize, Size::ZERO, Size::ZERO);
					isComplete = false;
				}
			}
		}

		if (isComplete && money > 0) {
			
			mQuestLayer[n]->removeAllChildren();
			gui::inst()->addTextButtonAutoDimension(0, 0, "Done", mQuestLayer[n]
				, CC_CALLBACK_1(FarmingScene::questCallback, this, n), 0, ALIGNMENT_CENTER, Color3B::BLUE, Size(1, 2), Size::ZERO, Size::ZERO);

			gui::inst()->addTextButtonAutoDimension(0, 1, "$" + to_string(money), mQuestLayer[n]
				, CC_CALLBACK_1(FarmingScene::questCallback, this, n), 0, ALIGNMENT_CENTER, Color3B::BLUE, Size(1, 2), Size::ZERO, Size::ZERO)
				->runAction(gui::inst()->createActionBlink());
		}
	}
}

void FarmingScene::questCallback(cocos2d::Ref* pSender, int idx) {
	//done
	Vec2 from = mQuestLayer[idx]->getPosition();
	from.x += mQuestLayer[idx]->getContentSize().width / 2;
	from.y += mQuestLayer[idx]->getContentSize().height / 2;

	gui::inst()->actionHarvest(this
		, "coin.png"
		, 8
		, from
		, mPoint->getPosition()
		, Size(20, 20)
	);

	if (!logics::hInst->farmingQuestDone(idx)) {
		CCLOG("Failure. questCallback");
	}
	setQuest();
	updatePoint();
}

void FarmingScene::updateFarming(float fTimer) {
	int n = 0;
	farming::field * f;
//    MainScene::field * pThiefField = NULL;
	while (logics::hInst->getFarm()->getField(n, f)) {
		MainScene::field * p = (MainScene::field*)f;

        if(p->seedId > 0 && p->seedId < 400)
            CCLOG("id = %d, seedId = %d", p->id, p->seedId);

		switch (p->status) {
		case farming::farming_status_decay:
			p->sprite->setColor(Color3B::RED);
			break;
		case farming::farming_status_grown:
//            this->addChild(createClinkParticle(gui::inst()->getPointVec2(p->x, p->y, ALIGNMENT_CENTER)), 100);
            mMainLayoput->addChild(createClinkParticle(getSpritePos(p)));
			break;
		case farming::farming_status_harvest:
			if (p->isHarvestAction == false) {
				//float ratio = mGridSize.height / p->sprite->getContentSize().height;
                addSprite(p, p->seedId);
                setLabel(to_string(p->level), p);
                float ratio = p->sprite->getScale();
				p->sprite->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.2, ratio * 1.1), ScaleTo::create(0.4, ratio), NULL)));
				p->isHarvestAction = true;

                /*
				//수확 상태에 수확할게 2개 이상이고 체력만 높으면 훔쳐 먹는다.
				if (mMode == Mode_Max
					&& mThiefCnt < 1
					&& p->getGrownCnt(logics::hInst->getFarm()->getSeed(p->seedId)->timeGrow) > 1
					&& getRandValue(4) == 0
					&& logics::hInst->getItem(logics::hInst->getFarm()->getSeed(p->seedId)->farmProductId).grade >= logics::hInst->getActor()->level
					//&& logics::hInst->getActor()->property.strength > logics::hInst->getActor()->property.appeal
					//&& logics::hInst->getActor()->property.strength > logics::hInst->getActor()->property.intelligence
					) {					
					mThiefCnt++;
					pThiefField = p;
					//p->accumulation++;
					//1개 빼고 다 훔쳐먹음
					pThiefField->accumulation = pThiefField->getGrownCnt(logics::hInst->getFarm()->getSeed(pThiefField->seedId)->timeGrow) - 1;
				}
                 */
			}
			break;
        case farming::farming_status_deco:
                if (p->isHarvestAction == false) {
                    addSprite(p, p->seedId);
                    p->isHarvestAction = true;
                }
            break;
		default:
			break;
		}
        n++;
	}	

	//도둑질
    /*
	if (pThiefField) {
        stopActionCharacter();
		mCharacter->runAction(getThiefAnimate());
//        mCharacter->setPosition(gui::inst()->getPointVec2(pThiefField->x, pThiefField->y, ALIGNMENT_CENTER));
        mCharacter->setPosition(getSpritePos(pThiefField));
	}
     */
}
void FarmingScene::selectField(MainScene::field * p) {
    mSelectedField = p;
    if(mSelectedFieldLayer)
        mMainLayoput->removeChild(mSelectedFieldLayer);
    
    if(p) {
        mScrollView->setVisible(true);
        //Create select Layer
        Color4F color = Color4F::WHITE;
        color.a = 0.5;
        mSelectedFieldLayer = gui::inst()->drawDiamond(mMainLayoput, getSpritePos(p), p->l->getContentSize(), color);
    } else {
        mScrollView->setVisible(false);
        
    }
}

bool FarmingScene::resetMode(bool isClearSelectedLayer) {
    if(isClearSelectedLayer)
        selectField(NULL);
    mMode = Mode_Max;
    return true;
}

bool FarmingScene::onTouchBegan(Touch* touch, Event* event) {
	mTouchDownPosition = touch->getLocation();
    //찾기
	if (mCharacter->getBoundingBox().containsPoint(touch->getLocation())) {
		mMode = Mode_Farming;
        stopActionCharacter();
		mCharacter->runAction(getFarmingAnimation());
		return true;
	}

	int n = 0;
	farming::field * f;
	while (logics::hInst->getFarm()->getField(n, f)) {
		MainScene::field * p = (MainScene::field*)f;
        
        if (gui::inst()->getResizedRect(p->l->getBoundingBox(), 0.5).containsPoint(mMainLayoput->convertToNodeSpace(touch->getLocation()))) {
            if(p->sprite != NULL) {
                mCurrentNodeId = n;
                p->sprite->setAnchorPoint(Vec2(0.5, 0.5));
                p->sprite->setLocalZOrder(2);
                setOpacity();
                mMode = Mode_Seed;
            } else {
                mCurrentNodeId = -1;
//                  for debug
//                Rect rect = gui::inst()->getResizedRect(p->l->getBoundingBox(), 0.5);
//                auto draw = DrawNode::create();
//                draw->drawRect(rect.origin, Vec2(rect.getMaxX(), rect.getMaxY()), Color4F::RED);
//                mMainLayoput->addChild(draw);
                
                selectField(p);
            }
			return true;
		}
        n++;
	}
	return resetMode();
}

bool FarmingScene::onTouchEnded(Touch* touch, Event* event) {
	//수확
	if (mMode == Mode_Farming) {
		onActionFinished();
		setQuest();
        return resetMode();
	}

	if (mCurrentNodeId == -1) {
        return resetMode(false);
	}

	clearOpacity();
	//클릭
    farming::field * f = NULL;
    if(!logics::hInst->getFarm()->getField(mCurrentNodeId, f))
        return resetMode();
    
    MainScene::field * p = (MainScene::field*)f;
    if (abs(mTouchDownPosition.x - touch->getLocation().x) < (p->l->getContentSize().width / 2)
        && abs(mTouchDownPosition.y - touch->getLocation().y) < (p->l->getContentSize().height / 2)) {
        if(p->sprite) {
            p->sprite->setPosition(getSpritePos(p));
            showInfo(p);
        }
        return resetMode();
	}
    
	//swap or levelup
	int n = 0;
	while (logics::hInst->getFarm()->getField(n, f)) {
		MainScene::field * pField = (MainScene::field*)f;
        if (pField->id == p->id) {
            n++;
            continue;
        }
		
		if (pField->sprite && pField->sprite->getBoundingBox().intersectsRect(p->sprite->getBoundingBox())
            && p->seedId == pField->seedId && p->level == pField->level
            && logics::hInst->getFarm()->getSeed(p->seedId)->isDeco == false
            ) {
            //merge
            levelUp(pField);
            //current clear
            clear(p);
            return resetMode();
        } else if(gui::inst()->getResizedRect(pField->l->getBoundingBox(), 0.5).containsPoint(p->sprite->getPosition())){
//            auto t = Label::create();
//            t->setString(to_string(pField->id));
//            t->setPosition(p->sprite->getPosition());
//            mMainLayoput->addChild(t);
            //swap
            swap(p, pField);
            return resetMode();
        }
        n++;
	}

    p->sprite->setPosition(getSpritePos(p));
    return resetMode();
}

void FarmingScene::onTouchMoved(Touch *touch, Event *event) {
	//int cnt = 0;
	bool isRemove = false;
	int productId = 0;
	int earning = 0;
	errorCode err;
	int n = 0;
	farming::field * f;

	switch (mMode) {
	case Mode_Farming:
		mCharacter->setPosition(touch->getLocation());
		//harvest		
		while (logics::hInst->getFarm()->getField(n, f)) {
			MainScene::field * pField = (MainScene::field*)f;
            
            Vec2 min = Vec2(mCharacter->getBoundingBox().getMinX(), mCharacter->getBoundingBox().getMinY());
            Vec2 max = Vec2(mCharacter->getBoundingBox().getMaxX(), mCharacter->getBoundingBox().getMaxY());
            Vec2 convertMin = mMainLayoput->convertToNodeSpace(min);
            Rect pRect;
            pRect.setRect(convertMin.x, convertMin.y, max.x - min.x, max.y - min.y);
            
			//if (pField->sprite != NULL && mCharacter->getBoundingBox().intersectsRect(pField->sprite->getBoundingBox())) {
            if (pField->sprite != NULL && pRect.intersectsRect(pField->sprite->getBoundingBox())) {
				switch (pField->status) {
				case farming::farming_status_decay:
				case farming::farming_status_harvest:
					isRemove = true;
				case farming::farming_status_grown:
                    stopAction(pField->sprite);
					err = logics::hInst->farmingHarvest(n, productId, earning);
					if (err != error_success) {
						CCLOG("farmingHarvest error errorCode = %d, seedId = %d,  %s", err, pField->seedId, wstring_to_utf8(logics::hInst->getErrorMessage(err)).c_str());
						return;
					}
					if (earning > 0) {
						gui::inst()->actionHarvest(mMainLayoput
							, MainScene::getItemImg(productId)
							, earning
							, getSpritePos(pField)
							, Vec2(0, mMainLayoput->getContentSize().height * -0.5)
							, Size(50, 50)
						);
					}
					break;
                default:
					break;
				}
                
				if(isRemove)
					clear(pField);

				break;
			}
            n++;
		}
		break;
	case Mode_Seed:
		if (mCurrentNodeId != -1) {			
			logics::hInst->getFarm()->getField(mCurrentNodeId, f);
			MainScene::field * pField = (MainScene::field *)f;
            if(pField->sprite) {
                pField->sprite->setPosition(mMainLayoput->convertToNodeSpace(touch->getLocation()));
            }
            
		}			
		break;
	default:
            //지도 이동
            Vec2 current = mMainLayoput->getPosition();
            Vec2 move = Vec2((mTouchDownPosition.x - touch->getLocation().x)
                             , (mTouchDownPosition.y - touch->getLocation().y)
                             );
            Vec2 movedPoint = Vec2(current.x - move.x, current.y - move.y);
            
//            CCLOG("Farm current %f, %f (width: %f, height: %f) %f, %f"
//                  , current.x, current.y
//                  , mMainLayoput->getContentSize().width, mMainLayoput->getContentSize().height
//                  , movedPoint.x
//                  , movedPoint.y
//                  );
            
//            if((movedPoint.x < mMainLayoput->getContentSize().width * -0.5)
//               || movedPoint.y < mMainLayoput->getContentSize().height * -0.5
//               || movedPoint.x > mMainLayoput->getContentSize().width
//               || movedPoint.y > mMainLayoput->getContentSize().height * 1.5
//               )
//                break;
            
            mMainLayoput->setPosition(movedPoint);
            mTouchDownPosition = touch->getLocation();
            
		break;
	}
}

void FarmingScene::swap(MainScene::field* a, MainScene::field * b) {
	MainScene::field temp;
	::memcpy(&temp, a, sizeof(temp));

	a->sprite = b->sprite;
	b->sprite = temp.sprite;

	logics::hInst->getFarm()->swap(a, b);

    if (a->sprite) 
        a->sprite->setPosition(getSpritePos(a));
    if (b->sprite)
        b->sprite->setPosition(getSpritePos(b));

    if(a->level > 0 && !logics::hInst->getFarm()->getSeed(a->seedId)->isDeco) {
        setLabel(to_string(a->level), a);
    }
    if(b->level > 0 && !logics::hInst->getFarm()->getSeed(b->seedId)->isDeco) {
        setLabel(to_string(b->level), a);
    }

	//set Zorder
	if (a->sprite)
		a->sprite->setLocalZOrder(1);
	if (b->sprite)
		b->sprite->setLocalZOrder(1);
}

void FarmingScene::levelUp(MainScene::field * p) {
	logics::hInst->getFarm()->levelup(p->id);
	//p->label->setString(to_string(p->level));
    setLabel(to_string(p->level), p);
	p->isHarvestAction = false;
	stopAction(p->sprite);
	//float ratio = mGridSize.height / p->sprite->getContentSize().height;
    float ratio = getScaleRatio(p);
	p->sprite->runAction(Sequence::create(ScaleTo::create(0.1, ratio * 1.1), ScaleTo::create(0.1, ratio), NULL));

	p->sprite->setLocalZOrder(1);
}

void FarmingScene::clear(MainScene::field * p) {
	//p->label->setString("");
	p->isHarvestAction = false;
	//this->removeChild(p->sprite);
    mMainLayoput->removeChild(p->sprite);
	p->sprite = NULL;
	logics::hInst->getFarm()->clear(p->id);
}

void FarmingScene::setOpacity() {
	if (mCurrentNodeId == EMPTY_PLANT_TAG)
		return;

	farming::field * p;
	logics::hInst->getFarm()->getField(mCurrentNodeId, p);

	int n = 0;
	farming::field *f;
	while (logics::hInst->getFarm()->getField(n, f)) {
		MainScene::field * pField = (MainScene::field*)f;
		
        if (pField->id == mCurrentNodeId || pField->sprite == NULL) {
            n++;
			continue;
        }
		if (pField->level == p->level && pField->seedId == p->seedId) {

		}
		else {
			pField->sprite->setOpacity(OPACITY_DIABLE);
		}
        n++;
	}
}
void FarmingScene::clearOpacity() {
	int n = 0;
	farming::field *f;
	while (logics::hInst->getFarm()->getField(n++, f)) {
		MainScene::field * pField = (MainScene::field*)f;

		if(pField->sprite)
			pField->sprite->setOpacity(OPACITY_MAX);
	}
}

void FarmingScene::createSeedMenu()
{
	Vec2 start, end;
	start = Vec2(1, 7);
	end = Vec2(start.x + 7, start.y -1);
	
	int margin = 10;
	int layerSize = 25;

	int cnt = (int)logics::hInst->getFarm()->getSeeds()->size();

    Size sizeOfScrollView = gui::inst()->getScrollViewSize(start, end, Size(-1, -1), Size(-1, -1), Size(-1, -1), Size(-1, -1));
	//mScrollView = gui::inst()->addScrollView(start, end, Size::ZERO, Size::ZERO, "", Size(sizeOfScrollView.width, 30 * cnt), this);
	mScrollView = gui::inst()->addScrollView(start, end, Size(-1, -1), Size(-1, -1), Size(-1, -1), Size(-1, -1), "", Size((layerSize + margin) * cnt + margin, sizeOfScrollView.height), this);
	mScrollView->setBackGroundColor(Color3B::WHITE);
	mScrollView->setBackGroundColorOpacity(64);
	mScrollView->setBackGroundColorType(Layout::BackGroundColorType::GRADIENT);
	
    farming::seeds * seeds = logics::hInst->getFarm()->getSeeds();
    CC_ASSERT(seeds);
	for (farming::seeds::iterator it = seeds->begin(); it != seeds->end(); ++it) {

		auto layout = gui::inst()->createLayout(Size(layerSize, layerSize));
		auto sprite = gui::inst()->addSpriteAutoDimension(0, 0, MainScene::getItemImg(it->second->id), layout, ALIGNMENT_CENTER, Size(1, 1), Size::ZERO, Size::ZERO);
		sprite->setContentSize(Size(layerSize * 0.8, layerSize * 0.8));
		sprite->setOpacity(192);

		gui::inst()->addTextButtonAutoDimension(0, 0
			, COIN + to_string(logics::hInst->getTrade()->getPriceBuy(it->second->id))
			, layout
			, CC_CALLBACK_1(FarmingScene::seedCallback, this, it->second->id)
			, 0
			, ALIGNMENT_CENTER
			, Color3B::BLUE
			, Size(1, 1)
			, Size::ZERO, Size::ZERO);

		gui::inst()->addLayoutToScrollView(mScrollView, layout, margin, 0);
	}
    mScrollView->setVisible(false);
}

void FarmingScene::addSprite(MainScene::field * p, int seedId) {
    p->l->removeAllChildren();
    if(seedId == 0) {
        p->sprite = Sprite::create("sprout.png");
    } else {
        mMainLayoput->removeChild(p->sprite);
        p->sprite = Sprite::create(MainScene::getItemImg(seedId));
    }
    
	//Vec2 position = gui::inst()->getPointVec2(p->x, p->y);
    p->sprite->setPosition(getSpritePos(p));
    
	//p->sprite->setScale(mGridSize.height / p->sprite->getContentSize().height);
    p->sprite->setScale(getScaleRatio(p));
    //this->addChild(p->sprite, 1);
    mMainLayoput->addChild(p->sprite);
}

void FarmingScene::seedCallback(cocos2d::Ref * pSender, int seedId)
{
    /*
	//seed * s = mSeedVector[seedIdx];		
	int n = 0;
	farming::field *f;
	while (logics::hInst->getFarm()->getField(n, f)) {
		MainScene::field * p = (MainScene::field*)f;
	
        if (p->seedId != 0) {
            n++;
			continue;
        }
		
		errorCode err = logics::hInst->farmingPlant(n, seedId);
		if (err != error_success) {
			//에러 팝업
			Director::getInstance()->pushScene(AlertScene::createScene(err));
			return;
		}
		
		addSprite(p, 0);
        setLabel(to_string(p->level), p);

		//s->itemQuantity--;
		updatePoint();
		break;
	}
     */
    if(mSelectedField->sprite)
        return;
    
    int idx = logics::hInst->getFarm()->getIdx(mSelectedField->id);
    errorCode err = logics::hInst->farmingPlant(idx, seedId);
    if (err != error_success) {
        //에러 팝업
        Director::getInstance()->pushScene(AlertScene::createScene(err));
        return;
    }
    
    addSprite(mSelectedField, 0);
    setLabel(to_string(mSelectedField->level), mSelectedField);
    updatePoint();
    
    selectField(NULL);
}

RepeatForever * FarmingScene::getFarmingAnimation() {

	auto animation = Animation::create();
	animation->setDelayPerUnit(0.1);

	string path;
	for (int n = 0; n < 8; n++) {
		path = "action/201/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}

	return RepeatForever::create(Animate::create(animation));
}

void FarmingScene::tillageCallback(Ref * pSender, MainScene::field * p){
    logics::hInst->getFarm()->remove(p->id);
    clear(p);
    closePopup(pSender);
}

void FarmingScene::showInfo(MainScene::field * p) {
    Size size = Size(150, 150);
	if (mPopupBackground == NULL) {
		mPopupLayer = gui::inst()->createModalLayer(mPopupBackground, size);
		mPopupLayer->setOpacity(192);
		this->addChild(mPopupBackground, 99);
	}

	string szName = wstring_to_utf8(
		logics::hInst->getItem(
			logics::hInst->getFarm()->getSeed(p->seedId)->farmProductId
		).name
	);

	string szHarvest, szRemain;
    string szReaped = (!logics::hInst->getFarm()->getSeed(p->seedId)->isDeco) ? "Reaped: " + to_string(p->accumulation) : "";

	switch (p->status) {
	case farming::farming_status_decay:
		szHarvest = "Decay T.T";
		break;	
	case farming::farming_status_harvest:
		szHarvest = "Harvest: " + to_string(p->getGrownCnt(logics::hInst->getFarm()->getSeed(p->seedId)->timeGrow));
		break;
	case farming::farming_status_grown:
		szHarvest = "Grown: " + to_string(p->getGrownCnt(logics::hInst->getFarm()->getSeed(p->seedId)->timeGrow));
	default:
        szRemain = "";
        if(!logics::hInst->getFarm()->getSeed(p->seedId)->isDeco){
            int nRemain = (int)(p->finishTime - getNow());
            int min = nRemain / 60;
            int sec = nRemain % 60;
            if (min > 0)
                szRemain += to_string(min) + " min ";
            szRemain += to_string(sec) + " sec..";
        }
		
		break;
	} 

	Size grid = Size(5, 8);
	int idx = 1;
    gui::inst()->setScale(
	gui::inst()->addSpriteAutoDimension(2, idx++, MainScene::getItemImg(logics::hInst->getFarm()->getSeed(p->seedId)->farmProductId), mPopupLayer, ALIGNMENT_CENTER, grid, Size::ZERO, Size::ZERO)
                          , size.height / grid.height);
	gui::inst()->addLabelAutoDimension(2, idx++, szName, mPopupLayer, 12, ALIGNMENT_CENTER, Color3B::BLACK, grid, Size::ZERO, Size::ZERO);
	if(szHarvest.size() > 1)
		gui::inst()->addLabelAutoDimension(2, idx++, szHarvest, mPopupLayer, 12, ALIGNMENT_NONE, Color3B::BLACK, grid, Size::ZERO, Size::ZERO);
	if(szReaped.size() > 1)
		gui::inst()->addLabelAutoDimension(2, idx++, szReaped, mPopupLayer, 12, ALIGNMENT_NONE, Color3B::GRAY, grid, Size::ZERO, Size::ZERO);
	if(szRemain.size() > 1)
		gui::inst()->addLabelAutoDimension(2, idx++, szRemain, mPopupLayer, 12, ALIGNMENT_CENTER, Color3B::ORANGE, grid, Size::ZERO, Size::ZERO);

    gui::inst()->addTextButtonAutoDimension(1, 6, logics::hInst->getL10N("FARM_TILLAGE"), mPopupLayer, CC_CALLBACK_1(FarmingScene::tillageCallback, this, p)
                                            , 12, ALIGNMENT_CENTER, Color3B::ORANGE, grid, Size::ZERO, Size::ZERO);
	gui::inst()->addTextButtonAutoDimension(3, 6, "CLOSE", mPopupLayer, CC_CALLBACK_1(FarmingScene::closePopup, this)
		, 12, ALIGNMENT_CENTER, Color3B::RED, grid, Size::ZERO, Size::ZERO);
	
}

void FarmingScene::closePopup(Ref * pSender) {
	if (mPopupBackground != NULL) {
		mPopupBackground->removeAllChildren();
		this->removeChild(mPopupBackground);

		delete mPopupLayer;
		delete mPopupBackground;
	}

	mPopupLayer = NULL;
	mPopupBackground = NULL;
}

Sequence * FarmingScene::getThiefAnimate() {
	auto animation = Animation::create();
	animation->setDelayPerUnit(0.1f);
	string path;
	for (int n = 0; n < 18; n++) {
		path = "action/200/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}

    auto callback = CallFunc::create(this, callfunc_selector(FarmingScene::onActionFinished));

	auto animate = Sequence::create(
		Repeat::create(Animate::create(animation), 1)
		, callback
		, NULL
	);
	return animate;
}

void FarmingScene::onActionFinished() {
    stopActionCharacter();
	mCharacter->runAction(MainScene::getIdleAnimation());
	mCharacter->setPosition(gui::inst()->getPointVec2(mCharacterInitPosition.x, mCharacterInitPosition.y));
    gui::inst()->setScale(mCharacter, 50);
}

void FarmingScene::onEnter() {
	Scene::onEnter();
	updatePoint();
}
void FarmingScene::onEnterTransitionDidFinish() {
	Scene::onEnterTransitionDidFinish();
}
void FarmingScene::onExitTransitionDidStart() {
	Scene::onExitTransitionDidStart();
}
void FarmingScene::onExit() {
	Scene::onExit();
}
void FarmingScene::setLabel(string sz, MainScene::field * p){
    if(p->sprite) {
        p->sprite->removeAllChildren();
        
        p->label = Label::create();
        p->label->setString(sz);
        p->label->setPosition(Vec2(p->sprite->getContentSize().width/2, p->sprite->getContentSize().height / 2));
        
        p->sprite->addChild(p->label);
    }
}
