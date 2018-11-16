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
#define TAP_MARGIN_Y 35
#define TAP_DEFAULT_POINT 3

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

int _tapInfo[][3] = //enable tap, hurdle level. 보너스 점수 종류
{
   {0, 0, 0},
    
    {0, 0, 1},  //grade 1
    {0, 0, 2},  //grade 2
    {0, 0, 3},  //grade 3
    
    {0, 1, 1},  //grade 4
    {0, 1, 2},  //grade 5
    {0, 1, 3},  //grade 6
    
    {0, 2, 1},  //grade 7
    {0, 2, 2},  //grade 8
    {0, 2, 3},  //grade 9
    
    {1, 0, 1},  //grade 10
    {1, 0, 2},  //grade 11
    {1, 0, 3},  //grade 12
    
    {1, 1, 1},  //grade 13
    {1, 1, 2},  //grade 14
    {1, 1, 3},  //grade 15
    
    {1, 2, 1},  //grade 16
    {1, 2, 2},  //grade 17
    {1, 2, 3}   //grade 18
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
    if(mAction.type == trainingType_tap)
        gui::inst()->addBGScrolling("bg_action_tap_" + to_string(_tapInfo[mAction.grade][2]) + ".png", this, 1.5);
    
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
 
 ================================================================================ */
void ActionBasic::runAction_tap(_training &t) {

    if(_tapInfo[t.grade][0] == 1){
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = CC_CALLBACK_2(ActionBasic::onTouchBegan, this);
        listener->onTouchEnded = CC_CALLBACK_2(ActionBasic::onTouchEnded, this);
        listener->onTouchMoved = CC_CALLBACK_2(ActionBasic::onTouchMoved, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        
        //tap
        const int duration = animationDelay * 100 / step;
        const int times = 4; // 1: 4 = duration : x
        gui::inst()->addLabel(1, 5, "Tap!!", this, 0, ALIGNMENT_CENTER, Color3B::ORANGE)->runAction(Blink::create(duration, duration * times));
        gui::inst()->addLabel(6, 3, "Tap!!", this, 0, ALIGNMENT_CENTER, Color3B::ORANGE)->runAction(Blink::create(duration, duration * times));

        mMaxTouchCnt += 300; //터치는 기본 300번 추가
    }
	
    this->scheduleUpdate();
    
    mContextTap.mIsJump = false;
    for(int n=0; n < sizeof(mContextTap.mTapBonusGenCounter) / sizeof(mContextTap.mTapBonusGenCounter[0]); n++)
        mContextTap.mTapBonusGenCounter[n] = 0;
    
    Vec2 pos = gui::inst()->getCenter();
    pos.y = TAP_MARGIN_Y;
	mContextTap.mTapRunner = createRunner(80, pos, Vec2(0.5, 0));
    
    
    gui::inst()->addTextButton(8, 6, "JUMP", this, [=](Ref* pSender){
        if(mContextTap.mIsJump)
            return;
        
        mContextTap.mIsJump = true;
        const float ratio = 0.2;
        const float jumpHeight = mContextTap.mTapRunner->getContentSize().height * 0.8;
        mContextTap.mTapRunner->runAction(Sequence::create(
                                            EaseIn::create(MoveBy::create(0.25, Vec2(0, jumpHeight)), ratio)
                                            , EaseOut::create(MoveBy::create(0.25, Vec2(0, -1 * jumpHeight)), ratio)
                                            , CallFunc::create([=]() {mContextTap.mIsJump = false;})
                                            , NULL));
    }
                               , 32, ALIGNMENT_CENTER, Color3B::BLUE);
	
    mPreTouchCnt = 0;
    mTimerCnt = 0;
	this->schedule([=](float delta) {
        mTimerCnt++;
        //bonus
        const int nGenBonus = 8;
        const int nMaxGenCnt = ((PLAYTIME * (1/animationDelay)) / nGenBonus) / _tapInfo[t.grade][2];
        const float nScales[] = {0, 40, 30, 20};
        
        if(mTimerCnt % nGenBonus == 0 && mTime > 2.5) { //2초 이상 남았을 때만 생성
            auto pBonus = gui::inst()->addSprite(9, 4, "star.png", this);
            pBonus->setPosition(Director::getInstance()->getVisibleSize().width * 1.2, 35 + mContextTap.mTapRunner->getContentSize().height);
            tapBonus p;
            p.sprite = pBonus;
            
            while(true){
                int n = getRandValueOverZero(_tapInfo[t.grade][2] + 1);
                if(mContextTap.mTapBonusGenCounter[n] <= nMaxGenCnt){
                    p.bonus = TAP_DEFAULT_POINT * n;
                    mMaxTouchCnt += p.bonus;
                    mContextTap.mTapBonusGenCounter[n]++;
                    gui::inst()->setScale(pBonus, nScales[n]);
                    break;
                }
            }
            float duration = (float)getRandValueMinMax(150, 250) / 100.f;
            pBonus->runAction(MoveTo::create(duration, Vec2(-40, pBonus->getPosition().y)));
            mContextTap.mTapBonusVec.push_back(p);
        }
        
        //hurdle
        if(_tapInfo[t.grade][1] >= 1){
            const int nGenHurdle = 15;
            if(mTimerCnt % nGenHurdle == 0) {
                auto hurdle = gui::inst()->addSprite(9, 4, "danger.png", this);
                gui::inst()->setScale(hurdle, mContextTap.mTapRunner->getContentSize().height / 6);
                hurdle->setAnchorPoint(Vec2(0.5, 0));
                hurdle->setPosition(Director::getInstance()->getVisibleSize().width * 1.2, TAP_MARGIN_Y);
                float duration = _tapInfo[t.grade][1] == 1 ? 2 : (float)getRandValueMinMax(150, 250) / 100.f;
                hurdle->runAction(MoveTo::create(duration, Vec2(-40, hurdle->getPosition().y)));
                mContextTap.mTapHurdleVec.push_back(hurdle);
            }
        }
        
        //move character
        if(mTimerCnt % 10 == 0){
            const int max = TAP_MAX_TOUCH;
            int currentCnt = mActionTouchCnt - mPreTouchCnt;
            if(currentCnt > max)
                currentCnt = max;
            
            //10:1 : cnt : x = cnt/10
            float x = ((Director::getInstance()->getVisibleSize().width * 0.9) * currentCnt / max) + mContextTap.mTapRunner->getContentSize().width;
            mPreTouchCnt = mActionTouchCnt;
            Vec2 pos = Vec2(x, mContextTap.mTapRunner->getPosition().y);
            mContextTap.mTapRunner->runAction(MoveTo::create(animationDelay * 10, pos));
        }
        
		setTime(animationDelay);

		if (mTime <= 0) {
			this->unschedule("updateLoadingBar");
			callbackActionAnimation(t.id, mMaxTouchCnt);
		}
	}, animationDelay, "updateLoadingBar");
}

void ActionBasic::update(float delta) {
    //bonus
    for(int n=0; n < mContextTap.mTapBonusVec.size(); n++){
        if(mContextTap.mTapBonusVec[n].sprite && mContextTap.mTapBonusVec[n].sprite->getPosition().x > 0){
            if(mContextTap.mTapRunner->getBoundingBox().intersectsRect(mContextTap.mTapBonusVec[n].sprite->getBoundingBox())){
                addTouchCnt(false, mContextTap.mTapBonusVec[n].bonus);
                auto bonusLabel = gui::inst()->addLabel(0, 0, "+" + to_string(mContextTap.mTapBonusVec[n].bonus), this, 0, ALIGNMENT_CENTER, Color3B::BLUE);
                Vec2 pos = mContextTap.mTapBonusVec[n].sprite->getPosition();
                pos.y += 10;
                bonusLabel->setPosition(pos);
                bonusLabel->runAction(
                                      Sequence::create(
                                                       ScaleTo::create(0.2, 3)
                                                       , RemoveSelf::create()
                                                       , NULL
                                                       )
                                      );
                mContextTap.mTapBonusVec[n].sprite->runAction(RemoveSelf::create());
                mContextTap.mTapBonusVec.erase(mContextTap.mTapBonusVec.begin() + n);
            }
        }
    }
    
    //hurdle
    for(int n=0; n < mContextTap.mTapHurdleVec.size(); n++){
        if(mContextTap.mTapHurdleVec[n] && mContextTap.mTapHurdleVec[n]->getPosition().x > 0){
            if(mContextTap.mTapRunner->getBoundingBox().intersectsRect(mContextTap.mTapHurdleVec[n]->getBoundingBox())){
                addTouchCnt(true, TAP_DEFAULT_POINT);
                auto bonusLabel = gui::inst()->addLabel(0, 0, "-" + to_string(TAP_DEFAULT_POINT), this, 0, ALIGNMENT_CENTER, Color3B::RED);
                Vec2 pos = mContextTap.mTapHurdleVec[n]->getPosition();
                pos.y += 10;
                bonusLabel->setPosition(pos);
                bonusLabel->runAction(
                                      Sequence::create(
                                                       ScaleTo::create(0.2, 3)
                                                       , RemoveSelf::create()
                                                       , NULL
                                                       )
                                      );
                mContextTap.mTapHurdleVec[n]->runAction(RemoveSelf::create());
                mContextTap.mTapHurdleVec.erase(mContextTap.mTapHurdleVec.begin() + n);
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
        logics::hInst->setHighScore(id, mActionTouchCnt, maxTimes);
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
        case trainingType_timing:
        case trainingType_training:
            runAction_timing(mAction);
            break;
        case trainingType_tap:
        case trainingType_fishing:
            runAction_tap(mAction);
            break;
        case trainingType_touch:
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
    
    mMaxTouchCnt = 0;
	
    switch (mAction.type) {
        case trainingType_timing:
        case trainingType_training:
            sz = logics::hInst->getL10N("ACTION_TIMING");
            break;
        case trainingType_tap:
        case trainingType_fishing:
            sz = logics::hInst->getL10N("ACTION_TAP");
            break;
        case trainingType_touch:
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
