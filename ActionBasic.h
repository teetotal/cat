
#ifndef PROJ_ANDROID_ACTIONBASIC_H
#define PROJ_ANDROID_ACTIONBASIC_H

#include "MainScene.h"
#include "cocos2d.h"
#include "ui/ui.h"
#include "logics.h"

USING_NS_CC;

class ActionBasic : public Scene {
public:
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(ActionBasic);
	
	bool runAction(int id);
private:
	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();
	virtual void onExit();

	bool onTouchBegan(Touch* touch, Event* event) {
		return true;
	};
	bool onTouchEnded(Touch* touch, Event* event);
	void onTouchMoved(Touch *touch, Event *event) {};
	
	void run();
	void runAction_touch(_training &t);
	void runAction_tap(_training &t);

	Sprite * createAnimate(_training &t);

	void callbackTouch(Ref* pSender);
	void callbackActionAnimation(int id, int maxTimes);

	void callback(Ref* pSender, SCENECODE type);
	void closeScene() {
		Director::getInstance()->popScene();
	};

	void addTouchCnt();
	Label * mTitle, * mRewardInfo, * mTouchInfo;
	LoadingBar * mLoadingBar;
	Layout * mLayer;

	int mActionCnt, mActionTouchCnt, mMaxTouchCnt;
	_training mAction;

	bool mIsStop;
};

#endif 