
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


	void runAction_touch(_training &t);
	void callbackTouch(Ref* pSender) {
		mActionTouchCnt++;;
	};
	void callbackActionAnimation(int id, int maxTimes);

	void callback(Ref* pSender, SCENECODE type);
	void closeScene() {
		Director::getInstance()->popScene();
	};
	Label * mTitle, * mRewardInfo;
	LoadingBar * mLoadingBar;
	Layout * mLayer;

	int mActionCnt, mActionTouchCnt;
	_training mAction;
};

#endif 