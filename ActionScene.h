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
	Sprite * mRunner[raceParticipantNum + 1];
	Label * mRunnerLabel[raceParticipantNum + 1];

	//Label * mRankLabel[raceParticipantNum + 1];

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

	wstring names[raceParticipantNum + 1];
	Color3B txtColors[raceParticipantNum + 1];

};


#endif //PROJ_ANDROID_ACTIONSCENE_H
