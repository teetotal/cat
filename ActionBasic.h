
#ifndef PROJ_ANDROID_ACTIONBASIC_H
#define PROJ_ANDROID_ACTIONBASIC_H

#include "MainScene.h"
#include "cocos2d.h"
#include "ui/ui.h"
#include "logics.h"

USING_NS_CC;
#define TIMING_RUNNER_CNT 3
#define TAP_BONUS_CNT 10

class ActionBasic : public Scene {
public:
	static Scene* createScene();
	virtual bool init() override;
	CREATE_FUNC(ActionBasic);
	
	bool runAction(int id);
    void update(float delta) override;
private:
	virtual void onEnter() override;
	virtual void onEnterTransitionDidFinish() override;
	virtual void onExitTransitionDidStart() override;
	virtual void onExit() override;

    bool onTouchBegan(Touch* touch, Event* event) ;
	bool onTouchEnded(Touch* touch, Event* event);
	void onTouchMoved(Touch *touch, Event *event) {};
	
    void initUI();
	void run();
	void runAction_touch(_training &t);
	void runAction_tap(_training &t);
    void runAction_timing(_training &t);
    
    void checkTouch(Touch * touch);

	Sprite * createAnimate(_training &t);
    RepeatForever * getRunningAnimation();
    Sprite * createRunner();
    Sprite * createRunner(float width, Vec2 pos, Vec2 anchor = Vec2(0.5, 0.5));
    Sprite * createDancer(float width, Vec2 pos, Vec2 anchor = Vec2(0.5, 0.5));
    
	void callbackTouch(Ref* pSender);
	void callbackActionAnimation(int id, int maxTimes);

	void callback(Ref* pSender, SCENECODE type);
    void callbackTiming(Ref* pSender, int idx);
    int getTouchYPosition(int idx);
	void closeScene() {
		Director::getInstance()->popScene();
	};

	void addTouchCnt(bool isFail = false, unsigned int val = 1);
    void setTime(float diff);
    string getScoreStar(float score, float max) {
        string sz = gui::inst()->getStar((unsigned int)(score / max * 100.f));
        return sz;
    };
    void setHighScoreLabel() {
        float max = logics::hInst->getMaxScore(mAction.id);
        if(max == -1)
            return;
        
        string sz = gui::inst()->getStar((unsigned int)((float)logics::hInst->getHighScore(mAction.id) / max * 100.f));
        mHighScore->setString("High Score: " + sz);
    };
    void increment(int &val);
	Label * mTitle, * mRewardInfo, * mTouchInfo;
    Label * mTimeLabel, * mHighScore;
    float mTime;
	Layout * mLayer;
    Sprite * mTimingRunner[TIMING_RUNNER_CNT];
    
	int mActionCnt, mActionTouchCnt, mMaxTouchCnt;
    int mPreTouchCnt; //이전 점수를 비교하기 위한 변수
    int mTimerCnt; //타이머 호출 카운터
    //for rat -------------------------------------------
    struct contextRat {
        vector<Sprite *> mBlackRatVec;
        vector<Sprite *> mWhiteRatVec;
    } mContextTouch;
    
    //for tap -------------------------------------------
    struct tapBonus{
        Sprite * sprite;
        int bonus;
    };
    
    struct contextTap{
        Sprite * mTapRunner;
        bool mIsJump; //점프 상태 체크용
        vector<tapBonus> mTapBonusVec;
        vector<Sprite *> mTapHurdleVec; //장애물
        
        int mTapBonusGenCounter[3]; //보너스 점수 생성 카운터
    } mContextTap;
    
    //for timing -------------------------------------------
    struct contextTiming {
        unsigned int nComboCnt;
    } mContextTiming;
    
    
	_training mAction;

	bool mIsStop;
    mutex lock;
};

#endif 
