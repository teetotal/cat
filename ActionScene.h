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

class ActionScene : public Scene {
public:
    static Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(ActionScene);

	void setRaceId(int id) {
		mRaceId = id;
	};

private:
	enum SUFFER_STATE {
		SUFFER_STATE_NONE,
		SUFFER_STATE_SPEEDUP,
		SUFFER_STATE_SHIELD,
		SUFFER_STATE_ATTACK,
	};
	LayerColor * mPopupLayer, *mPopupLayerBackground;
	void initRace();
	void showItemSelect();
	void selectItem(Ref* pSender, int id); //아이템 선택
	void updateSelectItem(); //선택한 아이템 정보 갱신
    void callback2(Ref* pSender, SCENECODE type);
	void invokeItem(Ref* pSender, int idx);
	void removeSelectItem(Ref* pSender, int idx);
	Sprite* createRunner(int idx);
	Sprite * mRunner[5];
	Label* mRankLabel, * mTimeDisplay;
	int mTimeDisplayValue;
	MenuItemFont * mSelectedItem[raceItemSlot];
	MenuItemFont * mSkillItem[raceItemSlot];
	Layout * mFullLayer;

	void timer(float f);

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
};


#endif //PROJ_ANDROID_ACTIONSCENE_H
