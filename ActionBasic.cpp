//
// Created by Jung, DaeChun on 24/08/2018.
//

#include "ActionBasic.h"
#include "SimpleAudioEngine.h"

Scene* ActionBasic::createScene()
{
	return ActionBasic::create();
}


bool ActionBasic::init()
{	
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	auto bg = Sprite::create("bg_temp.png");
	bg->setContentSize(Director::getInstance()->getVisibleSize());
	gui::inst()->addToCenter(bg, this);

	mLoadingBar = gui::inst()->addProgressBar(4, 0, LOADINGBAR_IMG, this, 10);
	mTitle = gui::inst()->addLabel(4, 0, " ", this);
	mTitle->setPosition(Vec2(mTitle->getPosition().x, mTitle->getPosition().y + 15));
	//mRewardInfo = gui::inst()->addLabel(0, 0, " ", this);
	return true;
}


bool ActionBasic::runAction(int id) {
	if (id == -1)
		return false;

	if (logics::hInst->isValidTraining(id) != error_success)
		return false;

	mAction = logics::hInst->getActionList()->at(id);
	//title
	mTitle->setString(wstring_to_utf8(mAction.name));
	//reward
	string pay = " ";
	if (mAction.cost.point > 0) pay += COIN + to_string(mAction.cost.point) + " ";
	if (mAction.cost.strength > 0) pay += "S: " + to_string(mAction.cost.strength) + " ";
	if (mAction.cost.intelligence > 0) pay += "I: " + to_string(mAction.cost.intelligence) + " ";
	if (mAction.cost.appeal > 0) pay += "A: " + to_string(mAction.cost.appeal) + " ";

	string reward;
	if (mAction.reward.point > 0)         reward += COIN + to_string(mAction.reward.point) + " ";
	if (mAction.reward.strength > 0)      reward += "S: " + to_string(mAction.reward.strength) + " ";
	if (mAction.reward.intelligence > 0)  reward += "I: " + to_string(mAction.reward.intelligence) + " ";
	if (mAction.reward.appeal > 0)        reward += "A: " + to_string(mAction.reward.appeal) + " ";

	//if (pay.size() > 1)	gui::inst()->addLabelAutoDimension(2, idx++, "- " + pay, l, 12, ALIGNMENT_NONE, Color3B::RED);
	if (reward.size() > 1)	gui::inst()->addLabel(0, 0, "Max " + reward, this, 12, ALIGNMENT_NONE);
}


void ActionBasic::runAction_touch(_training &t) {	
	
	auto size = Size(400, 200);
	int fontSize = 12;
	Layout * l = gui::inst()->createLayout(size, "", true);
	l->setOpacity(128);
	gui::inst()->addToCenter(l, this);
	
	int idx = 2;
	float animationDelay = 0.1f;
	int cntAnimationMotion = 6;
	int loopAnimation = 4 * 3;// * t.level; //레벨이 높을 수록 오래 
	float step = 100.f / (loopAnimation * cntAnimationMotion); //한 이미지 당 증가하는 양

	string path = "action/" + to_string(t.type) + "/0.png";
	auto pMan = Sprite::create(path);
	pMan->setPosition(Point(l->getContentSize().width / 2, l->getContentSize().height / 3));
	l->addChild(pMan);

	auto animation = Animation::create();
	animation->setDelayPerUnit(animationDelay);
	for (int n = 0; n < cntAnimationMotion; n++) {
		path = "action/" + to_string(t.type) + "/" + to_string(n) + ".png";
		animation->addSpriteFrameWithFile(path);
	}

	auto animate = RepeatForever::create(Animate::create(animation));
	pMan->runAction(animate);
	
	//touch
	Menu * pTouchButton = NULL;
	gui::inst()->addSpriteButtonRaw(pTouchButton, 0, idx, "rat1.png", "rat2.png", l, CC_CALLBACK_1(ActionBasic::callbackTouch, this), ALIGNMENT_NONE);

	//start touch count
	mActionCnt = 0;
	mActionTouchCnt = 0;
	gui::inst()->mModalTouchCnt = 0;

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


		float percent = mLoadingBar->getPercent();
		percent += step;
		mActionCnt++;
		float ratio = getTouchRatio(animationDelay, gui::inst()->mModalTouchCnt);
		gui::inst()->mModalTouchCnt = 0;
		//CCLOG("%f", ratio * step);
		percent += ratio * step;

		mLoadingBar->setPercent(percent);

		if (percent >= 100.0f) {
			this->unschedule("updateLoadingBar");
			pMan->stopAllActions();
			callbackActionAnimation(t.id, mActionCnt / 2);
		}
	}, animationDelay, "updateLoadingBar");
	
	return;
}

void ActionBasic::callbackActionAnimation(int id, int maxTimes) {

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

	string szResult = "Bonus: " + to_string((int)(ratioTouch * 100.f)) + "% \n";
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
	switch (err) {
	case error_success:
	case error_levelup:
		//showResult(szResult, isInventory);
		//updateState(isInventory);
		gui::inst()->addLabel(4, 3, szResult, this, 0, ALIGNMENT_CENTER, Color3B::ORANGE)->runAction(
			Sequence::create(
				EaseIn::create(ScaleBy::create(1, 3), 0.4)
				, EaseOut::create(ScaleBy::create(1, 1 / 3), 0.4)
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

void ActionBasic::callback(Ref* pSender, SCENECODE type) {
	
	switch(type){
	case SCENECODE_CLOSESCENE:
	default:
		closeScene();
		break;
	}
}


void ActionBasic::onEnter() {
	Scene::onEnter();
	runAction_touch(mAction);
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
