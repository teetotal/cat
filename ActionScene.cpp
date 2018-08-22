//
// Created by Jung, DaeCheon on 27/07/2018.
//

#include "ActionScene.h"
#include "SimpleAudioEngine.h"

#define RACE_UPDATE_INTERVAL 0.3
#define RACE_DEFAULT_IMG "race/0.png"
//#define RUNNER_WIDTH 80
Scene* ActionScene::createScene()
{
    return ActionScene::create();
}

bool ActionScene::init() {		
	bool ret;
	mRaceParticipants = logics::hInst->getNextRaceStatus(ret, -1);
	if(!ret)
		return false;
	mRaceCurrent = logics::hInst->getRaceResult();
	
	mGoalLength = Director::getInstance()->getVisibleSize().width * 2 - 50;

    gui::inst()->addBGScrolling("layers/sky.png", this, 3000);

    gui::inst()->addBGScrolling("layers/clouds_1.png", this, 25);
    gui::inst()->addBGScrolling("layers/rocks.png", this, 100);
    gui::inst()->addBGScrolling("layers/clouds_2.png", this, 20);

    gui::inst()->addBGScrolling("layers/plant.png", this, 100);
    gui::inst()->addBGScrolling("layers/ground_1.png", this, 100);
	gui::inst()->addBGScrolling("layers/ground_2.png", this, 200);
	gui::inst()->addBGScrolling("layers/ground_3.png", this, 300);

	auto finishLine = gui::inst()->addSprite(0, 0, "finish.png", this);
	finishLine->setPosition(Vec2(mGoalLength, 120));

	for (int n = 0; n <= raceParticipantNum; n++) {
		//float fast = mRaceParticipants->at(n).appeal / mRaceParticipants->at(n).strength;
		mRunner[n] = createRunner(n);
	}

	this->runAction(Follow::create(mRunner[raceParticipantNum]
		, Rect(0, 0, Director::getInstance()->getVisibleSize().width * 2, Director::getInstance()->getVisibleSize().height * 2)
	));
	

	//rank
	mRankLabel = gui::inst()->addLabel(4, 0, "0", this, 24, ALIGNMENT_CENTER, Color3B::WHITE);
	
	this->schedule(schedule_selector(ActionScene::timer), RACE_UPDATE_INTERVAL);
	
    return true;
}

void ActionScene::callback2(Ref* pSender, SCENECODE type){
    //auto pScene = MainScene::createScene();
    //Director::getInstance()->replaceScene(pScene);
    Director::getInstance()->popScene();
}

RepeatForever * ActionScene::getRunningAnimation() {
	auto animation = Animation::create();
	int ranValue = getRandValue(9);
	animation->setDelayPerUnit(0.03 + (ranValue * 0.001) );

	string path;
	for (int n = 0; n <= 8; n++) {
		path = "race/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}

	return RepeatForever::create(Animate::create(animation));
}

Sprite* ActionScene::createRunner(int idx) {
	mSufferState[idx] = false;
	wstring names[raceParticipantNum + 1] = { L"꼴등이" , L"시그" , L"김밥이" , L"인절미" , logics::hInst->getActor()->name };
    Color3B txtColors[raceParticipantNum + 1] = { Color3B::YELLOW, Color3B::GRAY, Color3B::MAGENTA, Color3B::ORANGE, Color3B::WHITE };
		
	auto p = gui::inst()->addSprite(0, 7, RACE_DEFAULT_IMG, this);
	p->setPosition(Vec2(0, p->getPosition().y + p->getContentSize().height / 2 + ((raceParticipantNum - idx) * 10)));
	//gui::inst()->setScale(p, RUNNER_WIDTH);
    p->runAction(getRunningAnimation());

    Color3B color = txtColors[idx];
	auto label = gui::inst()->addLabelAutoDimension(9, 0, wstring_to_utf8(names[idx]), p, 10, ALIGNMENT_CENTER, color);
	label->setPosition(p->getContentSize().width / 2, p->getContentSize().height - 15);

	return p;
}

void ActionScene::timer(float f) {	
	bool ret;
	int itemIdx = -1;
	mRaceParticipants = logics::hInst->getNextRaceStatus(ret, itemIdx);
	if(!ret){
		unschedule(schedule_selector(ActionScene::timer));		
		mRunner[raceParticipantNum]->setPosition(Vec2(100, mRunner[raceParticipantNum]->getPosition().y));
		result();
		return;
	}

	for (int n = 0; n <= raceParticipantNum; n++) {
		_raceParticipant p = mRaceParticipants->at(n);
		if (p.ratioLength >= 100) {
			if(n != raceParticipantNum)
				mRunner[n]->stopAllActions();
			continue;
		}

		if (p.currentSuffer != itemType_max) {
			switch (p.currentSuffer)
			{
			case itemType_race_speedUp:
				//printf("달려라!! 스피드 업!! \n");
				break;
			case itemType_race_shield:
				//printf("모두 없던 일로~ \n");
				//isSleep = true;
				break;
			default:
				//isSleep = true;
				if (mSufferState[n]) {
					mRunner[n]->stopAllActions();
					auto animation = Animation::create();
					animation->setDelayPerUnit(0.1);
					animation->addSpriteFrameWithFile("action/99/0.png");
					animation->addSpriteFrameWithFile("action/99/1.png");
					animation->addSpriteFrameWithFile("action/99/2.png");
					animation->addSpriteFrameWithFile("action/99/3.png");
					animation->addSpriteFrameWithFile("action/99/4.png");
					animation->addSpriteFrameWithFile("action/99/5.png");
					animation->addSpriteFrameWithFile("action/99/6.png");
					animation->addSpriteFrameWithFile("action/99/7.png");					
					mRunner[n]->runAction(RepeatForever::create(Animate::create(animation)));
					mSufferState[n] = false;
				}				
				break;
			}
		}
		else {
			if (!mSufferState[n]) {			
				mRunner[n]->stopAllActions();
				mRunner[n]->runAction(getRunningAnimation());				
				mSufferState[n] = true;
			}
			float x = mRaceParticipants->at(n).ratioLength / 100 * mGoalLength;
			Vec2 position = mRunner[n]->getPosition();
			position.x = x;
			mRunner[n]->runAction(MoveTo::create(0.3, position));
		}		
	}

	mRankLabel->setString(to_string(mRaceParticipants->at(raceParticipantNum).currentRank));
	
	//mMe->setPosition(point);
}

void ActionScene::result() {
	
	//결과처리
	wstring sz;
	sz += L"순위: ";
	sz += to_wstring(mRaceCurrent->rank);
	sz += L"\n상금: ";
	sz += to_wstring(mRaceCurrent->prize);
	sz += L"\n상품: ";
	sz += logics::hInst->getItem(mRaceCurrent->rewardItemId).name;
	sz += L"(";
	sz += to_wstring(mRaceCurrent->rewardItemQuantity);
	sz += L")";
	auto l = gui::inst()->createLayout(Size(250, 150), "", true);
	Vec2 point;
	gui::inst()->getPoint(4, 3, point, ALIGNMENT_CENTER);
	l->setPosition(point);
	l->setAnchorPoint(Vec2(0.5, 0.5));

	gui::inst()->addLabelAutoDimension(0, 1, wstring_to_utf8(sz), l, 14, ALIGNMENT_CENTER, Color3B::BLACK, Size(1, 5), Size::ZERO, Size::ZERO);
	this->addChild(l);
	gui::inst()->addTextButtonAutoDimension(0, 3, "OK", l
		, CC_CALLBACK_1(ActionScene::callback2, this, SCENECODE_ACTION), 32, ALIGNMENT_CENTER, Color3B::BLUE, Size(1, 5), Size::ZERO, Size::ZERO);
}