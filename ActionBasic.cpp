//
// Created by Jung, DaeChun on 24/08/2018.
//

#include "ActionBasic.h"
#include "ui/ui_color.h"
#include "SimpleAudioEngine.h"

#define PARTICLE "particles/particle_clink.plist"

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
int _timingInfo[][3] =  //runner, curveCnt, difficult
{{0, 0, 0}
    , {1, 2, 120}
    , {1, 3, 80}
    , {1, 4, 40}
    
    , {2, 2, 120}
    , {2, 3, 80}
    , {2, 4, 40}
    
    , {3, 2, 120}
    , {3, 3, 80}
    , {3, 4, 40}
};

int _catchRatInfo[][3] = //slot, 등장 주기, 시작 슬롯idx
{{0,0}
    , {4, 5, 3}
    , {5, 5, 2}
    , {6, 5, 1}
    
    , {6, 4, 1}
    , {6, 3, 1}
    , {6, 2, 1}
    
    , {9, 4, 0}
    , {9, 3, 0}
    , {9, 2, 0}
    
};

int _tapInfo[][3] = //enable tap, hurdle level. 보너스 점수 종류
{
   {0, 0, 0},
    
    {0, 1, 1},  //grade 1
    {0, 1, 2},  //grade 2
    {0, 1, 3},  //grade 3
    
    {0, 2, 1},  //grade 4
    {0, 2, 2},  //grade 5
    {0, 3, 3},  //grade 6
    
    {0, 3, 1},  //grade 7
    {0, 3, 2},  //grade 8
    {0, 3, 3},  //grade 9
    
    {1, 1, 1},  //grade 10
    {1, 1, 2},  //grade 11
    {1, 1, 3},  //grade 12
    
    {1, 2, 1},  //grade 13
    {1, 2, 2},  //grade 14
    {1, 2, 3},  //grade 15
    
    {1, 3, 1},  //grade 16
    {1, 3, 2},  //grade 17
    {1, 3, 3}   //grade 18
};

Scene* ActionBasic::createScene()
{
	return ActionBasic::create();
}


bool ActionBasic::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
    gui::inst()->addBG("bg_temp.png", this);
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
    mHighScore = gui::inst()->addLabel(7, 0, " ", this, 18, ALIGNMENT_CENTER, Color3B::MAGENTA);
    setHighScoreLabel();
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
    animation->setDelayPerUnit(0.02);
    
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
Sprite * ActionBasic::createDancer(float width, Vec2 pos, Vec2 anchor) {
    auto p = gui::inst()->addSprite(0, 0, "action/97/0.png", this);
    p->setAnchorPoint(anchor);
    p->setPosition(pos);
    
    auto animation = Animation::create();
    animation->setDelayPerUnit(0.12);
    
    string path;
    for (int n = 0; n <= 5; n++) {
        path = "action/97/" + to_string(n) + ".png";
        animation->addSpriteFrameWithFile(path);
    }
    
    p->runAction(RepeatForever::create(Animate::create(animation)));
    gui::inst()->setScale(p, width);
    
    return p;
}

void ActionBasic::callbackTiming(Ref* pSender, int idx){
    Vec2 center = gui::inst()->getPointVec2(TIMING_X_BUTTON, getTouchYPosition(idx));
    float radius = TIMING_RADIUS;
    
    /*
     auto draw = DrawNode::create();
     draw->drawCircle(mTimingRunner->getPosition(), radius, 0, 100, true, Color4F::BLACK);
     this->addChild(draw);
    */
    
    string sz = "Fail";
    Color3B fontColor = Color3B::RED;
    if(mTimingRunner[idx] && mTimingRunner[idx]->getBoundingBox().intersectsCircle(center, radius)){
        this->addChild(gui::inst()->createParticle(PARTICLE, TIMING_X_BUTTON, getTouchYPosition(idx) - 1));
        Device::vibrate(0.01);
        mContextTiming.nComboCnt++;
        sz = to_string(mContextTiming.nComboCnt) + " COMBO";
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
        mContextTiming.nComboCnt = 0;
        addTouchCnt(true);
    }
    
    gui::inst()->addLabel(TIMING_X_BUTTON, getTouchYPosition(idx) - 1, sz, this, 0, ALIGNMENT_CENTER, fontColor)
    ->runAction(
                Sequence::create(
                                 ScaleTo::create(0.4, 2.5)
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
    
    mContextTiming.nComboCnt = 0;
    mActionCnt = 0;
    mTimingRunner[0] = NULL;
    
    for(int n=0; n < nRunner; n ++){
        auto btn = gui::inst()->addSpriteButton(TIMING_X_BUTTON, getTouchYPosition(n), "cat-hand1.png", "cat-hand2.png", this, CC_CALLBACK_1(ActionBasic::callbackTiming, this, n));
        gui::inst()->setScale(btn, TIMING_RADIUS * 2);
    }
    
    //blink bg
//    auto blackBG = LayerColor::create(Color4B::BLACK);
//    blackBG->setContentSize(gui::inst()->getVisibleSize());
//    blackBG->setPosition(gui::inst()->mOrigin);
//    blackBG->runAction(RepeatForever::create(Blink::create(1, 4)));
//    addChild(blackBG, 2);
    
    
    this->schedule([=](float delta) {
        for(int n=0; n < nRunner; n++){
            
            if(mTimingRunner[n] == NULL){
                mTimingRunner[n] = this->createRunner();
//                mTimingRunner[n]->setLocalZOrder(1);
                mTimingRunner[n]->setPosition(gui::inst()->getPointVec2(0, getTouchYPosition(n)));
                
                Vector<FiniteTimeAction *> vec;
                for(int i=0; i< nCurveCnt; i++){
                    
                    if(i == nCurveCnt -1){
                        float speed = (float)max(40, getRandValue(80)) / 100.f;
                        vec.pushBack(MoveTo::create(speed, Vec2(gui::inst()->getPointVec2(TIMING_X_END, getTouchYPosition(n)))));
                    }
                    else{
                        int val = getRandValue(200);
                        float speed = (float)max(_timingInfo[t.grade][2], val) / 100.f;
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
            
//            //dancer
//            if(mActionCnt % 7 == 0) {
//                auto dancer = createDancer(TIMING_RADIUS * 2, gui::inst()->getPointVec2(0, getTouchYPosition(n)));
//                float speedDancer = (float)getRandValue(10) / 10.f + 1.5f;
//                dancer->runAction(Sequence::create(
//                                  MoveTo::create(speedDancer, Vec2(gui::inst()->getPointVec2(TIMING_X_END + 1, getTouchYPosition(n))))
//                                                   , RemoveSelf::create()
//                                                   , NULL)
//                                                   );
//            }
        }

        setTime(animationDelay);
        mActionCnt++;
        
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
        //const float ratio = 0.2;
        const float jumpHeight = mContextTap.mTapRunner->getContentSize().height * 0.8;
        mContextTap.mTapRunner->runAction(Sequence::create(
                                            //EaseIn::create(MoveBy::create(0.25, Vec2(0, jumpHeight)), ratio)
                                            //, EaseOut::create(MoveBy::create(0.25, Vec2(0, -1 * jumpHeight)), ratio)
                                            JumpBy::create(0.5, Vec2::ZERO, jumpHeight, 1)
                                            , CallFunc::create([=]() {mContextTap.mIsJump = false;})
                                            , NULL));
    }, 32, ALIGNMENT_CENTER, Color3B::BLUE);
	
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
            pBonus->setPosition(Director::getInstance()->getVisibleSize().width * 1.2, 20 + mContextTap.mTapRunner->getContentSize().height);
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
            pBonus->runAction( JumpTo::create(duration, Vec2(-40, pBonus->getPosition().y), 120, 3)
//                              Spawn::create(MoveTo::create(duration, Vec2(-40, pBonus->getPosition().y))
//                                            ,
//                                            , NULL
//                                            )
                              
                              );
            mContextTap.mTapBonusVec.push_back(p);
        }
        
        //hurdle
        if(_tapInfo[t.grade][1] >= 1){
            int nGenHurdle = (4 - _tapInfo[t.grade][1]) * 15;
            
            if(mTimerCnt % nGenHurdle == 0) {
                auto hurdle = gui::inst()->addSprite(9, 4, "danger.png", this);
                gui::inst()->setScale(hurdle, mContextTap.mTapRunner->getContentSize().height / 6);
                hurdle->setAnchorPoint(Vec2(0.5, 0));
                hurdle->setPosition(Director::getInstance()->getVisibleSize().width * 1.2, TAP_MARGIN_Y);
                //float duration = _tapInfo[t.grade][1] == 1 ? 2 : (float)getRandValueMinMax(150, 250) / 100.f;
                float duration = (float)getRandValueMinMax(150, 250) / 100.f;
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
    Vec2 position = mContextTap.mTapRunner->getPosition();
    float scale = mContextTap.mTapRunner->getScale();
    Size size = Size(mContextTap.mTapRunner->getContentSize().width * scale, mContextTap.mTapRunner->getContentSize().height * scale);
    Vec2 center = Vec2(position.x - size.width / 2, position.y + size.height / 2);
    float radius = size.height / 3;
    
    for(int n=0; n < mContextTap.mTapHurdleVec.size(); n++){
        if(mContextTap.mTapHurdleVec[n] && mContextTap.mTapHurdleVec[n]->getPosition().x > 0){
            //if(mContextTap.mTapRunner->getBoundingBox().intersectsRect(mContextTap.mTapHurdleVec[n]->getBoundingBox())){
            if(mContextTap.mTapHurdleVec[n]->getBoundingBox().intersectsCircle(center, radius)){
                //gui::inst()->drawCircle(this, center, radius, Color4F::GRAY);
                Device::vibrate(0.01);
                mContextTap.mTapRunner->runAction(Blink::create(0.5, 2));
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

bool ActionBasic::onTouchBegan(Touch* touch, Event* event) {
    if(mAction.type == trainingType_touch){
        checkTouch(touch);
    }
    return true;
}

bool ActionBasic::onTouchEnded(Touch* touch, Event* event) {
    if(mAction.type == trainingType_tap) {
        addTouchCnt();
    }
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
 쥐를 잡자
 
 ================================================================================ */
//void ActionBasic::runAction_touch(_training &t) {
//    auto size = Size(400, 150);
//    Layout * l = gui::inst()->createLayout(size);
//    gui::inst()->addToCenter(l, this);
//
//    //touch
//    Menu * pTouchButton = NULL;
//    auto item = gui::inst()->addSpriteButtonRaw(pTouchButton, 0, 0, "rat1.png", "rat2.png", l, CC_CALLBACK_1(ActionBasic::callbackTouch, this), ALIGNMENT_NONE);
//    gui::inst()->setScale(item, 70);
//
//    this->schedule([=](float delta) {
//        //touch 이동
//        if (mActionCnt % 10 < 3) {
//            Vec2 position = Vec2(getRandValue(l->getContentSize().width), getRandValue(l->getContentSize().height));
//            float marginX = pTouchButton->getChildren().at(0)->getContentSize().width;
//            float marginY = pTouchButton->getChildren().at(0)->getContentSize().height;
//
//            if (position.x > l->getContentSize().width - marginX)
//                position.x = l->getContentSize().width - marginX;
//
//            if (position.y > l->getContentSize().height - marginY)
//                position.y = l->getContentSize().height - marginY;
//
//            //pTouchButton->setPosition(position);
//            pTouchButton->runAction(MoveTo::create(animationDelay, position));
//        }
//
//        setTime(animationDelay);
//        mActionCnt++;
//
//        if (mTime <= 0) {
//            this->unschedule("updateLoadingBar");
//            //pMan->stopAllActions();
//            callbackActionAnimation(t.id, mMaxTouchCnt);
//        }
//    }, animationDelay, "updateLoadingBar");
//
//    return;
//}
void ActionBasic::runAction_touch(_training &t) {
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(ActionBasic::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(ActionBasic::onTouchEnded, this);
    listener->onTouchMoved = CC_CALLBACK_2(ActionBasic::onTouchMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto size = gui::inst()->mVisibleSize;
    Layout * l = gui::inst()->createLayout(Size(size.x, size.y));
    l->setPosition(gui::inst()->mOrigin);
    this->addChild(l);
    mMaxTouchCnt = 0;
    
    this->schedule([=](float delta) {
        //touch 이동
        if (mActionCnt % _catchRatInfo[mAction.grade][1] == 0) {
            const float imgSize = 50;
            bool isBlack = getRandValue(2) == 0;
            int slotIdx = _catchRatInfo[mAction.grade][2] + getRandValue(_catchRatInfo[mAction.grade][0]);
            auto img = gui::inst()->addSprite(slotIdx, 0, isBlack ? "rat1.png" : "rat2.png", l);
            if(isBlack) {
                mContextTouch.mBlackRatVec.push_back(img);
                increment(mMaxTouchCnt);
            } else {
                mContextTouch.mWhiteRatVec.push_back(img);
            }
            Vec2 pos = img->getPosition();
            pos.y = gui::inst()->mVisibleY;
            img->setPosition(pos);
            gui::inst()->setScale(img, imgSize);
            const float speed =((float)getRandValueOverZero(10) / 10.f) + 0.5;
            img->runAction(MoveTo::create(speed, Vec2(pos.x, -1 * imgSize)));
            
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

void ActionBasic::checkTouch(Touch * touch) {
    Vec2 pos = touch->getLocation();
    pos.x -= gui::inst()->mOrigin.x;
    pos.y -= gui::inst()->mOrigin.y;
//    CCLOG("checkTouch %f, %f", touch->getLocation().x, touch->getLocation().y);
    
    for(int n=0; n < mContextTouch.mWhiteRatVec.size(); n++){
        Sprite * sprite = mContextTouch.mWhiteRatVec[n];
        if(sprite && sprite->getBoundingBox().containsPoint(pos)) {
//            CCLOG("rect white %f, %f, %f, %f", sprite->getBoundingBox().getMinX(), sprite->getBoundingBox().getMinY(), sprite->getBoundingBox().getMaxX(), sprite->getBoundingBox().getMaxY());
            
            addTouchCnt(true);
            sprite->stopAllActions();
            sprite->runAction(
                              Sequence::create(Blink::create(1, 5), RemoveSelf::create(), NULL)
                              );
            Device::vibrate(0.01);
            mContextTouch.mWhiteRatVec[n] = NULL;
            return;
        }
    }
    
    for(int n=0; n < mContextTouch.mBlackRatVec.size(); n++){
        Sprite * sprite = mContextTouch.mBlackRatVec[n];
        if(sprite && sprite->getBoundingBox().containsPoint(pos)){
            addTouchCnt();
            sprite->stopAllActions();
            sprite->runAction(
                              Sequence::create(ScaleBy::create(0.2, 2), ScaleBy::create(0.2, 0.5), RemoveSelf::create(), NULL)
                              );
            mContextTouch.mBlackRatVec[n] = NULL;
            return;
        }
    }

    
}
//결과 =============================================================
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
    switch (err) {
        case error_success:
        case error_levelup:
            break;
        default:
            //alert(err);
            break;
    }

	wstring szRewards = L"";
	for (int n = 0; n < rewards.size(); n++) {
		_item item = logics::hInst->getItem(rewards[n].itemId);
		szRewards += item.name + L"x" + to_wstring(rewards[n].val) + L" ";
	}

	bool isInventory = false;
    
    //UI구성
    Color4B bgColor = Color4B::WHITE;
    //bgColor.a = 225;
    auto layer = LayerColor::create(bgColor, 300, 200);
    Vec2 pos = gui::inst()->getCenter();
    pos.x -= layer->getContentSize().width / 2;
    pos.y -= layer->getContentSize().height / 2;
    layer->setPosition(pos);
    this->addChild(layer);
    
    gui::inst()->addLabelAutoDimension(4, 1, "RESULT", layer, 20, ALIGNMENT_CENTER, Color3B::BLACK);
    gui::inst()->addTextButtonAutoDimension(4, 7, "CLOSE", layer, [=](Ref* pSender){ closeScene(); } , 0, ALIGNMENT_CENTER, Color3B::BLUE);
    
    int idx = 4;
    gui::inst()->addLabelAutoDimension(2, idx, "Score", layer, 12, ALIGNMENT_NONE, Color3B::GRAY);
    string szResultDetail = to_string(mActionTouchCnt) + " / " + to_string(maxTimes);
    gui::inst()->addLabelAutoDimension(4, idx++, szResultDetail, layer, 0, ALIGNMENT_NONE, Color3B::GREEN);
    
    gui::inst()->addLabelAutoDimension(2, idx, "Reward", layer, 12, ALIGNMENT_NONE, Color3B::GRAY);
    
    if (point > 0) gui::inst()->addLabelAutoDimension(4, idx++, COIN + to_string(point), layer, 0, ALIGNMENT_NONE);
	if (property.strength > 0) gui::inst()->addLabelAutoDimension(4, idx++, "S: " + to_string(property.strength), layer, 0, ALIGNMENT_NONE);
	if (property.intelligence > 0) gui::inst()->addLabelAutoDimension(4, idx++, "I: " + to_string(property.intelligence), layer, 0, ALIGNMENT_NONE);
	if (property.appeal > 0) gui::inst()->addLabelAutoDimension(4, idx++, "A: " + to_string(property.appeal), layer, 0, ALIGNMENT_NONE);
	
	string szRewardsUTF8 = wstring_to_utf8(szRewards, true);
	if (szRewardsUTF8.size() > 1) {
		isInventory = true;
        gui::inst()->addLabelAutoDimension(3, idx++, szRewardsUTF8, layer);
	}
    //new score
    string szResult = getScoreStar(mActionTouchCnt, maxTimes);
    if(mActionTouchCnt <= 0){
        szResult = "T.T";
    }
    else if(logics::hInst->getHighScore(id) < mActionTouchCnt){
        logics::hInst->setHighScore(id, mActionTouchCnt, maxTimes);
        setHighScoreLabel();
        gui::inst()->addLabelAutoDimension(4, 3, "New Record!", layer, 0, ALIGNMENT_CENTER, Color3B::MAGENTA)->runAction(Blink::create(10, 30));
    }
    
    gui::inst()->addLabelAutoDimension(4, 2, szResult, layer, 32, ALIGNMENT_CENTER, Color3B::MAGENTA)->runAction(
                          Sequence::create(
                                           EaseIn::create(ScaleTo::create(0.8, 2), 0.4)
                                           , EaseOut::create(ScaleTo::create(0.4, 1), 0.4)
                                           //, DelayTime::create(10)
                                           //, CallFunc::create([=]() { closeScene();    })
                                           , NULL
                                           )
                          );
    
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
