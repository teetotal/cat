//
// Created by Jung, DaeCheon on 27/07/2018.
//

#ifndef PROJ_ANDROID_ACTIONSCENE_H
#define PROJ_ANDROID_ACTIONSCENE_H

#include "MainScene.h"
#include "cocos2d.h"
#include "ui/ui.h"
#include "logics.h"

USING_NS_CC;

#define DANGER_CNT 5
#define DANGER_SPEED 70

class ActionScene : public Scene {
public:
	static Scene* createScene(int id);

	virtual bool init();

	static ActionScene* create(int id)
	{
		ActionScene *pRet = new(std::nothrow) ActionScene();
		pRet->setRaceId(id);

		if (pRet && pRet->init())
		{
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	};

	void setRaceId(int id) {
		mRaceId = id;
		mRaceMode = logics::hInst->getRace()->at(id).mode;
	};

	void update(float delta) override;

private:
	virtual bool onTouchBegan(Touch* touch, Event* event) {
		return true;
	};
	virtual bool onTouchEnded(Touch* touch, Event* event);
	int mTouchCnt;

	virtual void onEnter() override;
	virtual void onEnterTransitionDidFinish() override;
	virtual void onExitTransitionDidStart() override;
	virtual void onExit() override;
	
	enum SUFFER_STATE {
		SUFFER_STATE_NONE,
		SUFFER_STATE_SPEEDUP,
		SUFFER_STATE_SHIELD,
		SUFFER_STATE_ATTACK,
		SUFFER_STATE_OBSTACLE,
	};

	race_mode mRaceMode;

	LayerColor * mPopupLayer, *mPopupLayerBackground;
	bool initRace();
	void showItemSelect(errorCode err);
	void selectItem(Ref* pSender, int id); //아이템 선택
	void updateSelectItem(); //선택한 아이템 정보 갱신
	void callback2(Ref* pSender, SCENECODE type);
	void invokeItem(Ref* pSender, int idx);
	void removeSelectItem(Ref* pSender, int idx);
	Sprite* createRunner(int idx);
	void counting();
	wstring getSkillIconW(itemType type) {
		wstring szImg;
        switch (type) {
            case itemType_race_shield:        //방어 쉴드
                szImg = L"┛";
                break;
            case itemType_race_speedUp:        //속업
                szImg = L"┲";
                break;
            case itemType_race_attactFront:    //전방 공격
                szImg = L"┡";
                break;
            case itemType_race_attactFirst:    //1등 공격
                szImg = L"┼";
                break;
            default:
                break;
        }
        /*
		switch (type) {
		case itemType_race_shield:		//방어 쉴드
			szImg = L"쉴드";
			break;
		case itemType_race_speedUp:		//속업
			szImg = L"속업";
			break;
		case itemType_race_attactFront:	//전방 공격
			szImg = L"전방";
			break;
		case itemType_race_attactFirst:	//1등 공격
			szImg = L"1등";
			break;
		default:
			break;
		}
         */
		return szImg;
	};
	string getSkillIcon(itemType type) {
		if (type == itemType_max)
			return "";
		return wstring_to_utf8(getSkillIconW(type));
	};

	void updatePoint();
	void jump(Ref* pSender);
	void jumpByIdx(int idx);
	void resetHeight(int idx) {
		Vec2 position = mRunner[idx]->getPosition();
		position.y = mRunnerInitPosition[idx].y;
		mRunner[idx]->setPosition(position);
	};

	bool isItemMode() {
		switch (mRaceMode) {
		case race_mode_item:
		case race_mode_1vs1:
			return true;
		default:
			break;
		}
		return false;
	};
	
	Label * mPoint;

	Sprite * mRunner[raceParticipantNum + 1];
	Label * mRunnerLabel[raceParticipantNum + 1];
	Vec2 mRunnerInitPosition[raceParticipantNum + 1];
	//Start Count
	Label * mCounting;
	Label * mBoostPercent;
	//LoadingBar * mSpeedBoostBar; //speed전에서 얼마나 터치했는지 표시 
	int mCount;
	//Label * mRankLabel[raceParticipantNum + 1];

	MenuItemLabel * mSelectedItem[raceItemSlot];
	MenuItemLabel * mSkillItem[raceItemSlot];
	Layout * mFullLayer;

	void timer(float f);
	void counter(float f);

	raceParticipants* mRaceParticipants;
	_raceCurrent* mRaceCurrent;
	void result();
	RepeatForever * getRunningAnimation(bool isSpeedUp = false);

	SUFFER_STATE mSufferState[raceParticipantNum + 1];
	float mGoalLength;
	itemsVector mSelectItems;
	map<int, int> mSelectedItemQuantity;

	int mRaceId;
	queue<int> mInvokeItemQueue;

	wstring names[raceParticipantNum + 1];
	Color3B txtColors[raceParticipantNum + 1];
	//typedef vector<Sprite*> VecSprite;
	Sprite * mDangers[raceParticipantNum + 1][DANGER_CNT*2];

	//play횟수
	int mPlayCnt;
    float mScale;
};


#endif //PROJ_ANDROID_ACTIONSCENE_H
