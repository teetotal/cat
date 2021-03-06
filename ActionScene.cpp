//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "ActionScene.h"
#include "AlertScene.h"
#include "SimpleAudioEngine.h"

#define PARTICLE "particles/particle_clink.plist"

#define RACE_UPDATE_INTERVAL 0.3
//#define RACE_MAX_TOUCH 200.f //초당 max 터치
#define RACE_DEFAULT_IMG "race/0.png"

#define RACE_SIZE 	auto size = DEFAULT_LAYER_SIZE; auto margin = Size(10, 10); auto nodeSize = Size(120, 45); auto gridSize = Size(3, 4);
#define POPUP_NODE_MARGIN  5
#define RACE_BG_SIZE 8
#define RACE_GOAL_DISTANCE 7.5
#define RACE_RUNNING_ACTION_SPEED 0.02
#define RUNNER_MARGIN 35
float RUNNER_WIDTH;

//#define RUNNER_WIDTH 80
Scene* ActionScene::createScene(int id)
{	
	return ActionScene::create(id);
}

bool ActionScene::init() {
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

	mPlayCnt = 0;
    mScale = -1;
    RUNNER_WIDTH = 75 * gui::inst()->mVisibleY / 270;
    
    for (int idx = 0; idx <= raceParticipantNum; idx++) {
        for (int n = 0; n < DANGER_CNT * 2; n++) {
            mDangers[idx][n] = NULL;
            mGiftBox[idx][n] = NULL;
        }
    }

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
    txtColors[idx] = Color3B::YELLOW;
	names[idx++] = L"꼴등이";
    txtColors[idx] = Color3B::GRAY;
	names[idx++] = L"시그";
    txtColors[idx] = Color3B::MAGENTA;
	names[idx++] = L"김밥이";
    txtColors[idx] = Color3B::ORANGE;
	names[idx++] = L"인절미";
	txtColors[idx] = Color3B::BLUE;
	names[idx++] = utf8_to_utf16(logics::hInst->getActor()->name);

	mFullLayer = gui::inst()->createLayout(Size(Director::getInstance()->getVisibleSize().width * RACE_GOAL_DISTANCE, Director::getInstance()->getVisibleSize().height));
	//background
	for (int n = 0; n < RACE_BG_SIZE; n++) {
		for (int i = 1; i <= 3; i++) {
			auto s = Sprite::create("layers/race/" + to_string(i) + ".png");
			Vec2 point = Vec2(gui::inst()->mVisibleX / 2 + gui::inst()->mOriginX + (n * gui::inst()->mVisibleX)
				, gui::inst()->mVisibleY / 2 + gui::inst()->mOriginY);
			s->setContentSize(Size(gui::inst()->mVisibleX, gui::inst()->mVisibleY));
			s->setAnchorPoint(Vec2(0.5, 0.5));
			s->setPosition(point);

			mFullLayer->addChild(s);
		}
	}
	//finish
	auto finishLine = gui::inst()->addSprite(0, 0, "finish.png", mFullLayer);
	finishLine->setAnchorPoint(Vec2(0,0));
    gui::inst()->setScale(finishLine, RUNNER_WIDTH / 2);

	mGoalLength = Director::getInstance()->getVisibleSize().width * RACE_GOAL_DISTANCE - finishLine->getContentSize().width;
	
	finishLine->setPosition(Vec2(mGoalLength - RUNNER_MARGIN, 120));

	//구간 표시
	auto label0 = gui::inst()->addLabelAutoDimension(0, 2, "75%", mFullLayer, 0, ALIGNMENT_NONE, Color3B::GRAY);
	label0->setPosition(Vec2(mGoalLength / 4, label0->getPosition().y));
	auto label1 = gui::inst()->addLabelAutoDimension(0, 2, "50%", mFullLayer, 0, ALIGNMENT_NONE, Color3B::GRAY);
	label1->setPosition(Vec2(mGoalLength/2, label1->getPosition().y));
	auto label2 = gui::inst()->addLabelAutoDimension(0, 2, "25%", mFullLayer, 0, ALIGNMENT_NONE, Color3B::GRAY);
	label2->setPosition(Vec2(mGoalLength * 3 / 4, label2->getPosition().y));
	auto label3 = gui::inst()->addLabelAutoDimension(0, 2, "0%", mFullLayer, 0, ALIGNMENT_NONE, Color3B::GRAY);
	label3->setPosition(Vec2(mGoalLength - RUNNER_MARGIN, label2->getPosition().y));

	_race race = logics::hInst->getRace()->at(mRaceId);

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
    
    //mini map layer
    start = gui::inst()->getPointVec2(6, 1, ALIGNMENT_NONE);
    end = gui::inst()->getPointVec2(9, 0, ALIGNMENT_NONE);
    mLayerMiniMap = gui::inst()->createLayout(Size(end.x - start.x, end.y - start.y), "", true);
    mLayerMiniMap->setOpacity(64);
    mLayerMiniMap->setPosition(start);
    this->addChild(mLayerMiniMap);
    
    mMinimapRunnerRadius = (mLayerMiniMap->getContentSize().height / (raceParticipantNum + 1)) / 2;
    
    for(int n=0; n < raceParticipantNum; n++) {
        Color4F color = Color4F(txtColors[n]);
        color.a = 0.8;
        float y = mLayerMiniMap->getContentSize().height;
        y -= (n == 0) ? mMinimapRunnerRadius : n * mMinimapRunnerRadius * 2 + mMinimapRunnerRadius;
        mMiniMapRunner[n] = gui::inst()->drawCircle(mLayerMiniMap, Vec2(mMinimapRunnerRadius, y), mMinimapRunnerRadius, color);
    }
    Color4F color = Color4F(txtColors[raceParticipantNum]);
    color.a = 0.5;
    mMiniMapRunner[raceParticipantNum] = gui::inst()->drawDiamond(mLayerMiniMap, Vec2(mMinimapRunnerRadius, mMinimapRunnerRadius), Size(mMinimapRunnerRadius * 2, mMinimapRunnerRadius * 2), color);
    
    //순위 표시
    mMyRank = gui::inst()->addLabel(0, 0, "1st", this, 28, ALIGNMENT_CENTER, Color3B::GRAY);

	//boost label
	if (mRaceMode == race_mode_speed) {
		mBoostPercent = gui::inst()->addLabel(0, 1, " ", this);
		mBoostPercent->setColor(Color3B::GRAY);
	}

	//Race 초기 상태	
	errorCode err = logics::hInst->runRaceValidate(mRaceId);
	if (err != error_success)
		Director::getInstance()->popScene();
	
	err = logics::hInst->runRaceSetRunners(mRaceId);
	if (err != error_success)
		Director::getInstance()->popScene();

	if (isItemMode()) 
		showItemSelect(err);
	else 
		initRace();
	
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

	_race race = logics::hInst->getRace()->at(mRaceId);

	for (int n = 0; n < raceParticipantNum; n++) {
		if (race.mode == race_mode_1vs1 && n != 0)
			continue;
		mRunner[n] = createRunner(n);
        mMiniMapRunner[n]->setPosition(Vec2(mMinimapRunnerRadius, mMiniMapRunner[n]->getPosition().y));
	}
	//나는 무조건 있어야 하니까.
	mRunner[raceParticipantNum] = createRunner(raceParticipantNum);
    mMiniMapRunner[raceParticipantNum]->setPosition(Vec2(mMinimapRunnerRadius, mMiniMapRunner[raceParticipantNum]->getPosition().y));

	mFullLayer->runAction(Follow::create(mRunner[raceParticipantNum]
		, Rect(0, 0, Director::getInstance()->getVisibleSize().width * RACE_GOAL_DISTANCE, Director::getInstance()->getVisibleSize().height * 2)
	));
			
	//아이템 설정
	if (isItemMode()) {
        //btn
        for (int i = 0; i < raceItemSlot; i++) {
            mRaceItems.btns[i] = gui::inst()->addTextButton(0, 2 + i, " ", this,
                                                            CC_CALLBACK_1(ActionScene::invokeItem, this, i), 24, ALIGNMENT_CENTER, Color3B::BLUE);
        }
		for (int i = 0; i < raceItemSlot; i++) {
            if(i < mSelectItems.size()){
                setItem(mSelectItems[i].itemId);
            }
		}
		gui::inst()->addTextButton(8, 6, "JUMP", this, CC_CALLBACK_1(ActionScene::jump, this), 24, ALIGNMENT_CENTER, Color3B::BLUE);
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
	}
	else {
		mCounting->setColor(Color3B::YELLOW);
	}
	mCounting->runAction(Sequence::create(EaseIn::create(ScaleTo::create(0.5, 4), 0.4f), EaseOut::create(ScaleTo::create(0.5, 1), 0.4f), NULL));
	mCounting->setString(sz);
	
	if (mCount < 0) {
		this->removeChild(mCounting);
		unschedule(schedule_selector(ActionScene::counter));
		this->schedule(schedule_selector(ActionScene::timer), RACE_UPDATE_INTERVAL);
        //장애물과 아이템박스 배치
		if (isItemMode()) {
			this->scheduleUpdate();
			//Danger
			for (int idx = 0; idx <= raceParticipantNum; idx++) {
				for (int n = 0; n < DANGER_CNT * 2; n++) {
					Sprite * p = mDangers[idx][n];
					if (p == NULL)
						break;

					Vec2 position = p->getPosition();
					//시간 = 거리 / 속력
					float distance = position.x;
					float speed = DANGER_SPEED;
					float time = distance / speed;
					p->runAction(MoveTo::create(time, Vec2(-1 * p->getContentSize().width, position.y)));
                    
                   
                    if(mGiftBox[idx][n] == NULL)
                        break;
                    
                    float duration = mGiftBox[idx][n]->getPosition().x / DANGER_SPEED;
                    auto spawn = Spawn::create(
                                               MoveTo::create(duration, Vec2(-1 * mGiftBox[idx][n]->getContentSize().width, mGiftBox[idx][n]->getPosition().y))
                                               , JumpBy::create(duration, Vec2(0, 0), 20, duration * 3)
                                               , NULL);
                    mGiftBox[idx][n]->runAction(spawn);
				}
			}
		}
		else {
			gui::inst()->addLabel(0, 6, "Tap!!", this, 12, ALIGNMENT_CENTER, Color3B::GRAY)->runAction(Blink::create(20, 15*5));
		}
	}	
	mCount--;
}

void ActionScene::invokeItem(Ref* pSender, int idx) {
	//mSkillItem[idx]->setString("");
	//item은 무조건 제자리에서
	this->resetHeight(raceParticipantNum);
    //아이템 사용
    if(raceItemSlot > idx && mRaceItems.btns[idx] && mRaceItems.itemIds[idx] != 0) {
        mRaceItems.btns[idx]->setString(" ");
        mInvokeItemQueue.push(mRaceItems.itemIds[idx]);
        mRaceItems.itemIds[idx] = 0;
    }
}

void ActionScene::callback2(Ref* pSender, SCENECODE type){
	errorCode err;
	switch (type)
	{
	case SCENECODE_RACE_RUN:		
		if(initRace())
			this->removeChild(mPopupLayerBackground);
//        updatePoint();
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
		if (isItemMode()) {
			showItemSelect(err);
		}		
		break;
	default:		
		break;
	}   
}

RepeatForever * ActionScene::getRunningAnimation(bool isSpeedUp) {
	auto animation = Animation::create();
	if (isSpeedUp) {
		animation->setDelayPerUnit(RACE_RUNNING_ACTION_SPEED / 3);
	}
	else {
		animation->setDelayPerUnit(RACE_RUNNING_ACTION_SPEED + (getRandValue(9) * 0.001));
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
    mFinalFlags[idx] = false;
	auto p = gui::inst()->addSprite(0, 8, RACE_DEFAULT_IMG, mFullLayer);
    
    if(mScale == -1)
        mScale = gui::inst()->getScale(p, RUNNER_WIDTH);
    p->setScale(mScale);
    
	p->setAnchorPoint(Vec2(1, 0));
	p->setPosition(Vec2(0
		, 10 + p->getPosition().y + p->getContentSize().height * mScale / 2 +
		((raceParticipantNum - idx) * (p->getContentSize().height * mScale / 2))
	));
	p->runAction(getRunningAnimation());

    Color3B color = txtColors[idx];
	auto label = gui::inst()->addLabelAutoDimension(9, 0, wstring_to_utf8(names[idx]), p, 20, ALIGNMENT_CENTER, color);
	label->setPosition(p->getContentSize().width / 2, p->getContentSize().height - 20);

	mRunnerLabel[idx] = gui::inst()->addLabelAutoDimension(9, 0, " ", p, 28, ALIGNMENT_CENTER, color);
	mRunnerLabel[idx]->setPosition(p->getContentSize().width / 2, p->getContentSize().height);
	mRunnerInitPosition[idx] = p->getPosition();

	
	// danger 
	if (isItemMode()) {
        float nDanger = DANGER_CNT;
        if(mRaceMode == race_mode_1vs1)
            nDanger *= 2;
        
		const float baseDistance = (mFullLayer->getContentSize().width * 1.2 / nDanger);
		for (int n = 0; n < nDanger; n++) {
			float y = mRunnerInitPosition[idx].y + 10/*magin*/;
			auto sprite = Sprite::create("danger.png");
            gui::inst()->setScale(sprite, RUNNER_WIDTH / 5);
			sprite->setAnchorPoint(Vec2(0, 0));
			float x = baseDistance * (n + 1) + getRandValue(100);
			sprite->setPosition(x, y);
			mDangers[idx][n] = sprite;
			mFullLayer->addChild(sprite);
            
            auto spriteGift = Sprite::create("star.png");
            gui::inst()->setScale(spriteGift, RUNNER_WIDTH / 5);
            spriteGift->setAnchorPoint(Vec2(0, 0));
            x = baseDistance * (n + 1) + 300;
            spriteGift->setPosition(x, y);
            mGiftBox[idx][n] = spriteGift;
            mFullLayer->addChild(spriteGift);
		}
	}

	return p;
}

void ActionScene::timer(float f) {	
	bool ret;
	int itemId = -1;
	_race race = logics::hInst->getRace()->at(mRaceId);

	if (mInvokeItemQueue.size() > 0) {
		itemId = mInvokeItemQueue.front();
		mInvokeItemQueue.pop();
	}

	/*	
	RACE_MAX_TOUCH : 1 = x : RACE_UPDATE_INTERVAL
	RACE_MAX_TOUCH * RACE_UPDATE_INTERVAL = x
	*/
	
	float boost = 0.f;
	if (mRaceMode == race_mode_speed) {
		float ratio = getTouchRatio(RACE_UPDATE_INTERVAL, mTouchCnt);
		boost = min(ratio * 100.f, 100.f);
		mBoostPercent->setString(to_string(boost).substr(0, 4));
		mTouchCnt = 0;
	}

	mRaceParticipants = logics::hInst->getNextRaceStatus(ret, itemId, boost);
	if(!ret){
		unschedule(schedule_selector(ActionScene::timer));		
		for (int n = 0; n <= raceParticipantNum; n++) {
			if (mRunner[n]) {
				mRunner[n]->stopAllActions();
				mRunner[n]->runAction(getRunningAnimation());
				mRunner[n]->runAction(MoveTo::create(RACE_UPDATE_INTERVAL, Vec2(0, mRunner[n]->getPosition().y)));
			}
		}
		
		result();
		return;
	}
    
    int finishCnt = 0;
	
	for (int n = 0; n <= raceParticipantNum; n++) {
		_raceParticipant p = mRaceParticipants->at(n);

		if (race.mode == race_mode_1vs1 && n > 0 && n < raceParticipantNum)
			continue;
        
        float x = mRaceParticipants->at(n).ratioLength / 100 * mGoalLength;
		
        //rank
        if (p.ratioLength >= 100.f) {	//결승점에서는 순위
            if(mFinalFlags[n] == false) {
                mFinalFlags[n] = true;
                mRunner[n]->stopAllActions();
                mRunner[n]->runAction(getRunningAnimation());
                mRunner[n]->runAction(MoveTo::create(RACE_UPDATE_INTERVAL * 4, Vec2(mFullLayer->getContentSize().width + mRunner[n]->getContentSize().width
                                                                                , mRunnerInitPosition[n].y)));
                //mRunner[n]->setPosition(Vec2(mGoalLength, mRunnerInitPosition[n].y));
                //mRunnerLabel[n]->setString(to_string(p.rank));
                
                if(n == raceParticipantNum) {
                    setRankInfo(p.rank);
                    mMyRank->runAction(Blink::create(RACE_UPDATE_INTERVAL * 4, 6));
//                    Color3B fontColor = Color3B::BLACK;
//                    if(p.rank <= 2) {
//                        auto pt = ParticleFireworks::create();
//                        pt->setDuration(0.5);
//                        this->addChild(pt);
//                        fontColor = Color3B::ORANGE;
//                    }
//                    gui::inst()->addLabel(4, 3, getRankString(p.rank), this, 32, ALIGNMENT_CENTER, fontColor)->runAction(
//                                                                                            Sequence::create(ScaleTo::create(0.5, 3)
//                                                                                                             , RemoveSelf::create()
//                                                                                                             , NULL)
//                                                                                            );
                }
            }
            finishCnt++;
            continue;
		}
		else { //순위 전
            //공격 아이템 사용시 효과
            if(p.shootCurrentType == itemType_race_attactFront || p.shootCurrentType == itemType_race_attactFirst) {
                mRunnerLabel[n]->setString(getSkillIcon(p.shootCurrentType));
                string sz  = getSkillIcon(p.shootCurrentType) + " " + wstring_to_utf8(names[n]) + "->" + wstring_to_utf8(names[p.shootTarget]);
                gui::inst()->addLabel(4, 2, sz, this, 0, ALIGNMENT_CENTER, Color3B::GRAY)->runAction(
                                                                 Sequence::create(MoveBy::create(RACE_UPDATE_INTERVAL, Vec2(0, 20))
                                                                                  , FadeOut::create(RACE_UPDATE_INTERVAL)
                                                                                  , RemoveSelf::create()
                                                                                  , NULL)
                );
            }
            else{
                mRunnerLabel[n]->setString(" ");
            }
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
					for(int i=0; i < 8; i++)
						animation->addSpriteFrameWithFile("action/98/"+ to_string(i) +".png");
					mRunner[n]->runAction(RepeatForever::create(Animate::create(animation)));
					mSufferState[n] = SUFFER_STATE_SHIELD;
				}
				break;
			case itemType_race_obstacle:
				if (mSufferState[n] != SUFFER_STATE_OBSTACLE) {
					resetHeight(n);
					mRunner[n]->stopAllActions();
					auto animation = Animation::create();
					animation->setDelayPerUnit(0.1);
					for (int i = 0; i < 6; i++)
						animation->addSpriteFrameWithFile("action/97/" + to_string(i) + ".png");
					mRunner[n]->runAction(RepeatForever::create(Animate::create(animation)));
					mSufferState[n] = SUFFER_STATE_OBSTACLE;

				}
				break;
				
			default:				
				if (mSufferState[n] != SUFFER_STATE_ATTACK) {
					resetHeight(n);
					mRunner[n]->stopAllActions();
					auto animation = Animation::create();
					animation->setDelayPerUnit(0.1);
					for (int i = 0; i < 8; i++)
						animation->addSpriteFrameWithFile("action/99/"+ to_string(i) +".png");
					mRunner[n]->runAction(RepeatForever::create(Animate::create(animation)));
					mSufferState[n] = SUFFER_STATE_ATTACK;
				}
                //minimap
                mMiniMapRunner[n]->runAction(Blink::create(RACE_UPDATE_INTERVAL, 2));
				break;
			}
		}
		else {
			if (mSufferState[n] != SUFFER_STATE_NONE) {
				resetHeight(n);

				mRunner[n]->stopAllActions();
				mRunner[n]->runAction(getRunningAnimation());					
				mSufferState[n] = SUFFER_STATE_NONE;

			}
		}	
		
		Vec2 position = mRunner[n]->getPosition();
		position.x = x;
		mRunner[n]->runAction(MoveTo::create(RACE_UPDATE_INTERVAL, position));
        
        //minimap
        x = (mLayerMiniMap->getContentSize().width  - mMinimapRunnerRadius * 2) * mRaceParticipants->at(n).ratioLength / 100;
        mMiniMapRunner[n]->setPosition(Vec2(x, mMiniMapRunner[n]->getPosition().y));
        
        //순위
        if(n == raceParticipantNum) {
            setRankInfo(finishCnt + p.currentRank);
        }
		
		//mRunnerLabel[n]->setString(to_string(p.currentLength) + "-" + to_string(p.sufferItems.size()) + "," + to_string(p.currentSuffer));
	}
}

void ActionScene::setRankInfo(int rank) {
    mMyRank->setString(getRankString(rank));
    switch(rank) {
        case 1:
            mMyRank->setColor(Color3B::RED);
            break;
        case 2:
            mMyRank->setColor(Color3B::ORANGE);
            break;
        default:
            mMyRank->setColor(Color3B::GRAY);
            break;
    }
}

void ActionScene::result() {
	//remove danger
	if (isItemMode()) {
		this->unscheduleUpdate();
		for (int i = 0; i <= raceParticipantNum; i++) {
			for (int j = 0; j < DANGER_CNT * 2; j++) {
				if (mDangers[i][j]) {
					mDangers[i][j]->stopAllActions();
					mFullLayer->removeChild(mDangers[i][j]);
					mDangers[i][j] = NULL;
				}
                if(mGiftBox[i][j]){
                    mGiftBox[i][j]->stopAllActions();
                    mFullLayer->removeChild(mGiftBox[i][j]);
                    mGiftBox[i][j] = NULL;
                }
			}
		}
	}
	Size grid = Size(5, 8);
	mPopupLayer = gui::inst()->addPopup(mPopupLayerBackground, this, Size(300, 200), BG_RACE, Color4B::WHITE);

	//결과처리
	int idx = 1;
	gui::inst()->addLabelAutoDimension(2, idx++, getRankString(mRaceCurrent->rank), mPopupLayer, 28, ALIGNMENT_CENTER, Color3B::BLACK
		, grid, Size::ZERO, Size::ZERO);

	wstring sz = L"";
	int prize = logics::hInst->getRaceReward(mRaceCurrent->id, mRaceCurrent->rank - 1);
	if (mRaceCurrent->prize > 0) {
		sz += L"상금: ";
		sz += COIN_W;
		sz += to_wstring(prize);
	}

	if (mRaceCurrent->rewardItemQuantity > 0) {
		sz += L"\n상품: ";
		sz += logics::hInst->getItem(mRaceCurrent->rewardItemId).name;
		sz += L"x";
		sz += to_wstring(mRaceCurrent->rewardItemQuantity);
	}
	
	gui::inst()->addLabelAutoDimension(2, idx++,  wstring_to_utf8(sz), mPopupLayer, 12, ALIGNMENT_CENTER, Color3B::BLACK
		, grid, Size::ZERO, Size::ZERO);
	//this->addChild(l);
	idx++;
	if (mRaceCurrent->rank == 1) {
		
		for (int n = 0; n < 5; n++) {
			auto star = gui::inst()->addSpriteAutoDimension(n, idx, "star.png", mPopupLayer, ALIGNMENT_CENTER, grid, Size::ZERO, Size::ZERO);
            star->setContentSize(Size(40, 40));
			if (logics::hInst->mRaceWin.winCnt == n + 1) {
				star->runAction(Sequence::create(ScaleTo::create(0.5, 2), ScaleTo::create(0.5, 1), NULL));
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

//    updatePoint();
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

void ActionScene::setItem(int itemId) {
    _item item = logics::hInst->getItem(itemId);
    wstring sz = getSkillIconW(item.type) + to_wstring(item.grade);
    
    for(int n=0; n<raceItemSlot; n++){
        if(mRaceItems.itemIds[n] == 0)
        {
            mRaceItems.itemIds[n] = itemId;
            mRaceItems.btns[n]->setString(wstring_to_utf8(sz));
            //mRaceItems.btns[n]->setEnabled(true);
            break;
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
		if (mRaceItems.btns[n] != NULL) {
			this->removeChild(mRaceItems.btns[n]->getParent());
			mRaceItems.btns[n] = NULL;
            mRaceItems.itemIds[n] = 0;
		}		
	}
	this->removeChild(mPopupLayerBackground);
	mPopupLayer = gui::inst()->addPopup(mPopupLayerBackground, this, size, BG_RACE, Color4B::WHITE);
    
    //point
    //mPoint =
    gui::inst()->addLabelAutoDimension(8, 0, COIN + numberFormat(logics::hInst->getActor()->point), mPopupLayer
                                       , 0, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);
    
    //title
    _race race = logics::hInst->getRace()->at(mRaceId);
    gui::inst()->addLabelAutoDimension(4, 0, getRomeNumber(race.level) + ". " + wstring_to_utf8(race.title), mPopupLayer
                                       , 0, ALIGNMENT_CENTER, Color3B::BLACK, Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE), Size::ZERO, margin);

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
	
	trade::tradeMap * m = logics::hInst->getTrade()->get();
	
	POPUP_LIST(mPopupLayer
		, gridSize
		, newLine
		, Vec2(0, 5)
		, Vec2(9, 1)
		, margin
		, POPUP_NODE_MARGIN
		, nodeSize
		, (trade::tradeMap::iterator it = m->begin(); it != m->end(); ++it)
		, if (logics::hInst->getItem(it->first).type <= itemType_race || logics::hInst->getItem(it->first).type >= itemType_adorn) continue;
          if (logics::hInst->getItem(it->first).grade != logics::hInst->getRace()->at(mRaceId).level + 1) continue;
		, MainScene::getItemImg(it->first)
		, CC_CALLBACK_1(ActionScene::selectItem, this, it->first)
		, getRomeNumber(logics::hInst->getItem(it->first).grade)
		, wstring_to_utf8(logics::hInst->getItem(it->first).name)
		, COIN + to_string(logics::hInst->getTrade()->getPriceBuy(it->first))
		, gui::inst()->EmptyString
		, gui::inst()->EmptyString
        , NULL
	)
    
    //random 지급할 아이템 목록.
    for(trade::tradeMap::iterator it = m->begin(); it != m->end(); ++it){
        if (logics::hInst->getItem(it->first).type <= itemType_race || logics::hInst->getItem(it->first).type >= itemType_adorn)
            continue;
        if (logics::hInst->getItem(it->first).grade == logics::hInst->getRace()->at(mRaceId).level){
            mRandomItemIds.push_back(it->first);
        }
    }
    
//    updatePoint();
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


//void ActionScene::updatePoint() {
//    string sz = COIN + to_string(logics::hInst->getActor()->point);
//    if (sz.compare(mPoint->getString()) != 0) {
//        mPoint->setString(sz);
//        mPoint->runAction(gui::inst()->createActionFocus());
//    }
//}

void ActionScene::jump(Ref* pSender) {	
	jumpByIdx(raceParticipantNum);
}

void ActionScene::jumpByIdx(int idx) {
	if (mRunnerInitPosition[idx].y == mRunner[idx]->getPosition().y) {
		float jumpHeight = mRunner[idx]->getContentSize().height * mScale * 0.5;
		auto callfuncAction = CallFunc::create([this, idx]() {
			resetHeight(idx);
		});
		//const float ratio = 0.2;
		mRunner[idx]->runAction(
			Sequence::create(
				//EaseIn::create(MoveBy::create(RACE_UPDATE_INTERVAL, Vec2(0, jumpHeight)), ratio)
				//, EaseOut::create(MoveBy::create(RACE_UPDATE_INTERVAL, Vec2(0, -1 * jumpHeight)), ratio)
                JumpBy::create(RACE_UPDATE_INTERVAL * 2, Vec2(0,0), jumpHeight, 1)
				, callfuncAction
				, NULL));
	}
}

void ActionScene::update(float delta) {
	
	for (int i = 0; i <= raceParticipantNum; i++) {
		if (mRunner[i] == NULL)
			continue;

		Vec2 position = mRunner[i]->getPosition();
		Size size = Size(mRunner[i]->getContentSize().width * mScale, mRunner[i]->getContentSize().height * mScale);
		Vec2 center = Vec2(position.x - size.width / 2, position.y + size.height / 2);
		float radius = size.height / 3;
		/*
		if (i == raceParticipantNum) {
			auto draw = DrawNode::create();
			draw->drawCircle(center, radius, 0, 100, true, Color4F::BLACK);
			mFullLayer->addChild(draw);
		}
		*/
		for (int j = 0; j < DANGER_CNT * 2; j++) {
            if(mDangers[i][j]) {
                if (mDangers[i][j]->getTag() != 1 && mDangers[i][j]->getBoundingBox().intersectsCircle(center, radius)) {
                    logics::hInst->invokeRaceObstacle(i, logics::hInst->getRace()->at(mRaceId).level);
                    mDangers[i][j]->runAction(Blink::create(1, 10));
                    mDangers[i][j]->setTag(1);
                }
                else if(i != raceParticipantNum
                        && mDangers[i][j]->getPosition().x - position.x < 5
                        && mDangers[i][j]->getPosition().x - position.x > 0)
                { // jump
                    jumpByIdx(i);
                }
            }
            //itembox 획득
            if(mGiftBox[i][j] && mGiftBox[i][j]->getBoundingBox().intersectsCircle(center, radius)){
                Vec2 pos = mGiftBox[i][j]->getPosition();
                mGiftBox[i][j]->runAction(RemoveSelf::create());
                mGiftBox[i][j] = NULL;
                mFullLayer->addChild(gui::inst()->createParticle(PARTICLE, pos));
                
                //assign random item
                if(i == raceParticipantNum) {
                    int randomItemId = mRandomItemIds[getRandValue((int)mRandomItemIds.size())];
                    setItem(randomItemId);
                } else {
                    logics::hInst->decreaseRaceItemCountAI(i);
                }
            }
			
		}
	}
}

void ActionScene::onEnter() {
	Scene::onEnter();
//    updatePoint();
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
