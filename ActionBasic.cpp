//
// Created by Jung, DaeChun on 24/08/2018.
//

#include "ActionBasic.h"
#include "SimpleAudioEngine.h"
#define PLAYTIME 25.00
#define TIMING_Y    2
#define TIMING_X_START 0
#define TIMING_X_END 9
#define TIMING_X_BUTTON 7
#define TIMING_RADIUS 30
#define TIMING_MAX_TIME 150

#define TAP_MAX_TOUCH 50

float animationDelay = 0.1f;
int cntAnimationMotion = 6;
int loopAnimation = 4 * 3;
//float step = 100.f / (loopAnimation * cntAnimationMotion); //한 이미지 당 증가하는 양
float step = 100.0f / PLAYTIME * animationDelay;
int _timingInfo[10][3] =  //runner, curveCnt, difficult
{{0, 0, 0}
    , {1, 2, 1}
    , {1, 3, 2}
    , {1, 4, 3}
    
    , {2, 2, 1}
    , {2, 3, 2}
    , {2, 4, 3}
    
    , {3, 2, 1}
    , {3, 3, 2}
    , {3, 4, 3}
};

Scene* ActionBasic::createScene()
{
	return ActionBasic::create();
}


bool ActionBasic::init()
{	
    //    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto bg = Sprite::create("bg_temp.png");
    bg->setContentSize(Director::getInstance()->getVisibleSize());
    gui::inst()->addToCenter(bg, this);
	return true;
}


void ActionBasic::initUI() {
    //bg
    if(mAction.type == trainingType_study)
        gui::inst()->addBGScrolling("bg_action_tap_1.png", this, 1.5);
    
    //    mLoadingBar = gui::inst()->addProgressBar(4, 0, LOADINGBAR_IMG, this, 100, 0);
    mTime = PLAYTIME;
    mTimeLabel = gui::inst()->addLabel(4, 1, " ", this, 24, ALIGNMENT_CENTER, Color3B::BLUE);
    setTime(0);
    mTitle = gui::inst()->addLabel(0, 0, " ", this, 0, ALIGNMENT_NONE, Color3B::GRAY);
    //mTitle->setPosition(Vec2(mTitle->getPosition().x, mTitle->getPosition().y + 15));
    mRewardInfo = gui::inst()->addLabel(0, 6, " ", this, 0, ALIGNMENT_NONE, Color3B::GRAY);
    mTouchInfo = gui::inst()->addLabel(4, 0, "Score: 0", this, 32, ALIGNMENT_CENTER, Color3B::BLACK);
    mHighScore = gui::inst()->addLabel(7, 0, " ", this, 18, ALIGNMENT_CENTER, Color3B::GREEN);
    setHighScoreLabel(logics::hInst->getHighScore(mAction.id));
    //start touch count
    mActionCnt = 0;
    mActionTouchCnt = 0;
    gui::inst()->mModalTouchCnt = 0;
    
    
    //title
    mTitle->setString(wstring_to_utf8(mAction.name));
    //reward
    string pay = " ";
    if (mAction.cost.point > 0) pay += COIN + to_string(mAction.cost.point) + " ";
    if (mAction.cost.strength > 0) pay += "S: " + to_string(mAction.cost.strength) + " ";
    if (mAction.cost.intelligence > 0) pay += "I: " + to_string(mAction.cost.intelligence) + " ";
    if (mAction.cost.appeal > 0) pay += "A: " + to_string(mAction.cost.appeal) + " ";
    
    string reward = "Max ";
    if (mAction.reward.point > 0)         reward += COIN + to_string(mAction.reward.point) + " ";
    if (mAction.reward.strength > 0)      reward += "S: " + to_string(mAction.reward.strength) + " ";
    if (mAction.reward.intelligence > 0)  reward += "I: " + to_string(mAction.reward.intelligence) + " ";
    if (mAction.reward.appeal > 0)        reward += "A: " + to_string(mAction.reward.appeal) + " ";
    
    mRewardInfo->setString(reward);
    //if (pay.size() > 1)    gui::inst()->addLabelAutoDimension(2, idx++, "- " + pay, l, 12, ALIGNMENT_NONE, Color3B::RED);
    //if (reward.size() > 1)    gui::inst()->addLabel(4, 1, "Max " + reward, this, 12, ALIGNMENT_NONE);
}


bool ActionBasic::runAction(int id) {
	if (id == -1)
		return false;

	if (logics::hInst->isValidTraining(id) != error_success)
		return false;
    
    mAction = logics::hInst->getActionList()->at(id);
    
    
	return true;
}

Sprite * ActionBasic::createAnimate(_training &t) {
	string path = "action/" + to_string(t.type) + "/0.png";
	auto pMan = Sprite::create(path);
	//pMan->setPosition(gui::inst()->getCenter());

	auto animation = Animation::create();
	animation->setDelayPerUnit(animationDelay);
	for (int n = 0; n < cntAnimationMotion; n++) {
		path = "action/" + to_string(t.type) + "/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}

	auto animate = RepeatForever::create(Animate::create(animation));
    pMan->runAction(animate);

	return pMan;
}
Sprite * ActionBasic::createRunner(float width, Vec2 pos, Vec2 anchor) {
    auto p = gui::inst()->addSprite(0, 0, "race/small/0.png", this);
    p->setAnchorPoint(anchor);
    p->setPosition(pos);
    p->runAction(getRunningAnimation());
    gui::inst()->setScale(p, width);
    
    return p;
}

Sprite * ActionBasic::createRunner(){
    return createRunner(TIMING_RADIUS * 2, gui::inst()->getPointVec2(TIMING_X_START, TIMING_Y));
}

RepeatForever * ActionBasic::getRunningAnimation() {
    auto animation = Animation::create();
    animation->setDelayPerUnit(0.015);
    
    string path;
    for (int n = 0; n <= 8; n++) {
        path = "race/small/" + to_string(n) + ".png";
        animation->addSpriteFrameWithFile(path);
    }
    
    return RepeatForever::create(Animate::create(animation));
}
/* ================================================================================
    TIMING
 ================================================================================ */
void ActionBasic::callbackTiming(Ref* pSender, int idx){
    Vec2 center = gui::inst()->getPointVec2(TIMING_X_BUTTON, getTouchYPosition(idx));
    float radius = TIMING_RADIUS;
    
    /*
     auto draw = DrawNode::create();
     draw->drawCircle(mTimingRunner->getPosition(), radius, 0, 100, true, Color4F::BLACK);
     this->addChild(draw);
    */
    
    string sz = "-1";
    Color3B fontColor = Color3B::RED;
    if(mTimingRunner[idx] && mTimingRunner[idx]->getBoundingBox().intersectsCircle(center, radius)){
        sz = "+1";
        fontColor = Color3B::BLUE;
        addTouchCnt();
        mTimingRunner[idx]->stopAllActions();
        mTimingRunner[idx]->runAction(Sequence::create(
                                                       ScaleBy::create(0.1, 2)
                                                       , CallFunc::create([=]() { mTimingRunner[idx] = NULL; })
                                                       , RemoveSelf::create()
                                                       ,NULL));
        increment(mMaxTouchCnt);
    }else{
        addTouchCnt(true);
    }
    
    gui::inst()->addLabel(TIMING_X_BUTTON, getTouchYPosition(idx) - 1, sz, this, 0, ALIGNMENT_CENTER, fontColor)
    ->runAction(
                Sequence::create(
                                 ScaleTo::create(0.2, 3)
                                 , RemoveSelf::create()
                                 , NULL
                                 )
                );
}
void ActionBasic::runAction_timing(_training &t) {
    int curvePoint[3] = {3, 4, 5};
    int nRunner = _timingInfo[t.grade][0];
    int nCurveCnt = _timingInfo[t.grade][1];
    //int nDifficult = _timingInfo[t.grade][2];
    
    mActionCnt = 0;
    mTimingRunner[0] = NULL;
    
    for(int n=0; n < nRunner; n ++){
        auto btn = gui::inst()->addSpriteButton(TIMING_X_BUTTON, getTouchYPosition(n), "cat-hand1.png", "cat-hand2.png", this, CC_CALLBACK_1(ActionBasic::callbackTiming, this, n));
        gui::inst()->setScale(btn, TIMING_RADIUS * 2);
    }
    
    this->schedule([=](float delta) {
        for(int n=0; n < nRunner; n++){
            if(mTimingRunner[n] == NULL){
                mTimingRunner[n] = this->createRunner();
                mTimingRunner[n]->setPosition(gui::inst()->getPointVec2(0, getTouchYPosition(n)));
                
                Vector<FiniteTimeAction *> vec;
                for(int i=0; i< nCurveCnt; i++){
                    int val = getRandValue(100);
                    
                    int minSpeed = 40;
                    if(i == nCurveCnt -1){
                        float speed = (float)max(minSpeed, val) / 100.f;
                        vec.pushBack(MoveTo::create(speed, Vec2(gui::inst()->getPointVec2(TIMING_X_END, getTouchYPosition(n)))));
                    }
                    else{
                        minSpeed = minSpeed / 2;
                        float speed = (float)max(minSpeed, val) / 100.f;
                        vec.pushBack(
                                     EaseOut::create(
                                                    MoveTo::create(speed, Vec2(gui::inst()->getPointVec2(curvePoint[i], getTouchYPosition(n))))
                                                    , 0.3)
                                     );
                    }
                }
                
                vec.pushBack(RemoveSelf::create());
                vec.pushBack(CallFunc::create([=]() {
                    mTimingRunner[n] = NULL;
                    increment(mMaxTouchCnt);
                }));
                
                mTimingRunner[n]->runAction(Sequence::create(vec));
            }
        }

        setTime(animationDelay);
        
        if (mTime <= 0) {
            this->unschedule("updateLoadingBar");
            for(int n=0; n < nRunner; n++){
                mTimingRunner[n]->stopAllActions();
            }
            callbackActionAnimation(t.id, mMaxTouchCnt);
        }
    }, animationDelay, "updateLoadingBar");
}
/* ================================================================================
 
 TAP
 
 다양한 보너스 점수
 보너스 이동속도
 마이너스 보너스 점수
 보너스 등장 랜덤
 
 ================================================================================ */
void ActionBasic::runAction_tap(_training &t) {

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(ActionBasic::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(ActionBasic::onTouchEnded, this);
	listener->onTouchMoved = CC_CALLBACK_2(ActionBasic::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    this->scheduleUpdate();
    
    mIsJump = false;
	
    Vec2 pos = gui::inst()->getCenter();
    pos.y = 35;
	mTapRunner = createRunner(80, pos, Vec2(0.5, 0));
    
    
    gui::inst()->addTextButton(8, 6, "JUMP", this, [=](Ref* pSender){
        if(mIsJump)
            return;
        
        mIsJump = true;
        const float ratio = 0.2;
        const float jumpHeight = mTapRunner->getContentSize().height;
        mTapRunner->runAction(Sequence::create(
                                            EaseIn::create(MoveBy::create(0.3, Vec2(0, jumpHeight)), ratio)
                                            , EaseOut::create(MoveBy::create(0.3, Vec2(0, -1 * jumpHeight)), ratio)
                                            , CallFunc::create([=]() {mIsJump = false;})
                                            , NULL));
    }
                               , 20, ALIGNMENT_CENTER, Color3B::BLUE);
	//tap
	const int duration = animationDelay * 100 / step;
    const int times = 4; // 1: 4 = duration : x 
    gui::inst()->addLabel(1, 5, "Tap!!", this, 0, ALIGNMENT_CENTER, Color3B::ORANGE)->runAction(Blink::create(duration, duration * times));
	gui::inst()->addLabel(6, 3, "Tap!!", this, 0, ALIGNMENT_CENTER, Color3B::ORANGE)->runAction(Blink::create(duration, duration * times));

    mPreTouchCnt = 0;
    mTimerCnt = 0;
	this->schedule([=](float delta) {
        mTimerCnt++;
        if(mTimerCnt % 8 == 0){
            auto pBonus = gui::inst()->addSprite(9, 4, "star.png", this);
            gui::inst()->setScale(pBonus, 40);
            pBonus->setPosition(Director::getInstance()->getVisibleSize().width * 1.2, 35 + mTapRunner->getContentSize().height);
            pBonus->runAction(MoveTo::create(2, Vec2(-40, pBonus->getPosition().y)));
            mTapStarVec.push_back(pBonus);
        }
        
        //move character
        if(mTimerCnt % 10 == 0){
            const int max = TAP_MAX_TOUCH;
            int currentCnt = mActionTouchCnt - mPreTouchCnt;
            if(currentCnt > max)
                currentCnt = max;
            
            //10:1 : cnt : x = cnt/10
            float x = ((Director::getInstance()->getVisibleSize().width * 0.9) * currentCnt / max) + mTapRunner->getContentSize().width;
            mPreTouchCnt = mActionTouchCnt;
            Vec2 pos = Vec2(x, mTapRunner->getPosition().y);
            mTapRunner->runAction(MoveTo::create(animationDelay * 10, pos));
        }
        
		setTime(animationDelay);

		if (mTime <= 0) {
			this->unschedule("updateLoadingBar");
			callbackActionAnimation(t.id, mMaxTouchCnt);
		}
	}, animationDelay, "updateLoadingBar");
}

void ActionBasic::update(float delta) {
    for(int n=0; n < mTapStarVec.size(); n++){
        if(mTapStarVec[n] && mTapStarVec[n]->getPosition().x > 0){
            if(mTapRunner->getBoundingBox().intersectsRect(mTapStarVec[n]->getBoundingBox())){
                addTouchCnt(false, 5);
                auto bonusLabel = gui::inst()->addLabel(0, 0, "+5", this, 0, ALIGNMENT_CENTER, Color3B::BLUE);
                bonusLabel->setPosition(mTapStarVec[n]->getPosition());
                bonusLabel->runAction(
                                      Sequence::create(
                                                       ScaleTo::create(0.2, 3)
                                                       , RemoveSelf::create()
                                                       , NULL
                                                       )
                                      );
                mTapStarVec[n]->runAction(RemoveSelf::create());
                mTapStarVec.erase(mTapStarVec.begin() + n);
            }
        }
    }
}

bool ActionBasic::onTouchEnded(Touch* touch, Event* event) {
    addTouchCnt();
    return true;
}

int ActionBasic::getTouchYPosition(int idx){
    int nRunner = _timingInfo[mAction.grade][0];
    switch(nRunner){
        case 1:
            return 4;
        case 2: //3,5
            return idx == 0 ? 3: 5;
        case 3: //2,4,6
            switch(idx){
                case 0:
                    return 2;
                case 1:
                    return 4;
                case 2:
                    return 6;
                    
            }
        default:
            return 4;
    }
    return TIMING_Y + (idx * 2);
}

/* ================================================================================
 
 TOUCH
 
 ================================================================================ */
void ActionBasic::runAction_touch(_training &t) {	
	auto size = Size(400, 150);
	Layout * l = gui::inst()->createLayout(size);
	gui::inst()->addToCenter(l, this);
	
	//touch
	Menu * pTouchButton = NULL;
	auto item = gui::inst()->addSpriteButtonRaw(pTouchButton, 0, 0, "rat1.png", "rat2.png", l, CC_CALLBACK_1(ActionBasic::callbackTouch, this), ALIGNMENT_NONE);
    gui::inst()->setScale(item, 70);
    
	this->schedule([=](float delta) {
		//touch 이동
		if (mActionCnt % 10 < 3) {
			Vec2 position = Vec2(getRandValue(l->getContentSize().width), getRandValue(l->getContentSize().height));
			float marginX = pTouchButton->getChildren().at(0)->getContentSize().width;
			float marginY = pTouchButton->getChildren().at(0)->getContentSize().height;

			if (position.x > l->getContentSize().width - marginX)
				position.x = l->getContentSize().width - marginX;

			if (position.y > l->getContentSize().height - marginY)
				position.y = l->getContentSize().height - marginY;

			//pTouchButton->setPosition(position);
			pTouchButton->runAction(MoveTo::create(animationDelay, position));
		}

		setTime(animationDelay);
		mActionCnt++;
		
		if (mTime <= 0) {
			this->unschedule("updateLoadingBar");
			//pMan->stopAllActions();
			callbackActionAnimation(t.id, mMaxTouchCnt);
		}
	}, animationDelay, "updateLoadingBar");
	
	return;
}

void ActionBasic::callbackActionAnimation(int id, int maxTimes) {
	mIsStop = true;
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

    string szResult = to_string(mActionTouchCnt) + "/" + to_string(maxTimes);
    szResult += "\nBonus: " + to_string((int)(ratioTouch * 100.f)) + "% \n";
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
    //new score
    if(logics::hInst->getHighScore(id) < mActionTouchCnt){
        logics::hInst->setHighScore(id, mActionTouchCnt);
        szResult = "New Score " + to_string(mActionTouchCnt)+ "\n" + szResult;
        setHighScoreLabel(mActionTouchCnt);
    }
	switch (err) {
	case error_success:
	case error_levelup:
		//showResult(szResult, isInventory);
		//updateState(isInventory);
		gui::inst()->addLabel(4, 3, szResult, this, 0, ALIGNMENT_CENTER, Color3B::ORANGE)->runAction(
			Sequence::create(
				EaseIn::create(ScaleBy::create(1, 2), 0.4)
				, EaseOut::create(ScaleBy::create(1, 1 / 2), 0.4)
				, CallFunc::create([this]() { closeScene();	})
				, NULL
			)
		);
		break;
	default:
		//alert(err);
		break;
	}

	//closeScene();

}

void ActionBasic::addTouchCnt(bool isFail, unsigned int val) {
	if (!mIsStop) {
        if(isFail){
            if(mActionTouchCnt > 0)
                mActionTouchCnt -= val;
        }
		else
            mActionTouchCnt += val;
        //string sz = to_string(mActionTouchCnt) + "/" + to_string(mMaxTouchCnt);
        string sz = "Score: " + to_string(mActionTouchCnt);
		mTouchInfo->setString(sz);
	}
}

void ActionBasic::callbackTouch(Ref* pSender) {
	addTouchCnt();
};


void ActionBasic::callback(Ref* pSender, SCENECODE type) {
	
	switch(type){
	case SCENECODE_CLOSESCENE:
	default:
		closeScene();
		break;
	}
}

void ActionBasic::run() {
    initUI();
    
	mIsStop = false;
	switch (mAction.type) {
        case trainingType_play:
        case trainingType_party:
        case trainingType_training:
            runAction_timing(mAction);
            break;
        case trainingType_study:
        case trainingType_fishing:
            runAction_tap(mAction);
            break;
        default:
            runAction_touch(mAction);
            break;
	}
}

void ActionBasic::onEnter() {
	Scene::onEnter();
	mIsStop = true;
	string sz;
    float nMax = (100.f / step);
    
    /*
     trainingType_play = 0,    //놀이
     trainingType_study,        //공부
     trainingType_party,        //파티
     trainingType_training,    //수련
     trainingType_work,        //알바
     trainingType_fishing,    //낚시
     trainingType_hunting,        //사냥
     */
	switch (mAction.type) {
        case trainingType_play:
        case trainingType_party:
        case trainingType_training:
            mMaxTouchCnt = 0;
            sz = logics::hInst->getL10N("ACTION_TIMING");
            break;
        case trainingType_study:
        case trainingType_fishing:
            mMaxTouchCnt = PLAYTIME * TAP_MAX_TOUCH * 0.8; //최대 터치 수에 80% 수준이면 최대
            sz = logics::hInst->getL10N("ACTION_TAP");
            break;
        default:
            mMaxTouchCnt = nMax / 3;
            sz = logics::hInst->getL10N("ACTION_TOUCH");
            break;
	}
    
    addTouchCnt(false);
	
	gui::inst()->addLabel(4, 3, sz, this)
		->runAction(
			Sequence::create(
				FadeOut::create(2)
				, CallFunc::create([this]() { run(); })
				, NULL)
		);
}
void ActionBasic::onEnterTransitionDidFinish() {
	Scene::onEnterTransitionDidFinish();
}
void ActionBasic::onExitTransitionDidStart() {
	Scene::onExitTransitionDidStart();
}
void ActionBasic::onExit() {
	Scene::onExit();
}

void ActionBasic::setTime(float diff) {
    mTime -= diff;
    if(mTime < 0)
        mTime = 0;
    if(mTime < 3.f)
        mTimeLabel->setColor(Color3B::RED);
    else
        mTimeLabel->setColor(Color3B::BLUE);
    const int i = 10;
    int tmp = mTime * i;
    int val = tmp / i;
    int remain = tmp % i;
    
    mTimeLabel->setString(to_string(val) + "." + to_string(remain));
}

void ActionBasic::increment(int &val){
    lock.lock();
    val++;
//    CCLOG("%d, %d", val, mMaxTouchCnt);
    lock.unlock();
}
