﻿//
// Created by Jung, DaeCheon on 27/07/2018.
//

#ifndef PROJ_ANDROID_MAINSCENE_H
#define PROJ_ANDROID_MAINSCENE_H

#include "cocos2d.h"
#include "ui/ui.h"
#include "ui/ui_gacha.h"
#include "ui/ui_cultivation.h"
#include "logics.h"

USING_NS_CC;

#define BG_HOME "bg_home.png"
#define BG_FARM "bg_farming.jpg"
#define BG_ACHIEVEMENT "bg_achievement.png"
#define BG_BUY	"bg_buy.png"
#define BG_COLLECTION "bg_collection.png"
#define BG_INVENTORY "bg_inventory.png"
#define BG_SELL "bg_sell.png"
#define BG_RACE "bg_race.png"

enum CHILD_ID {
	CHILD_ID_INVENTORY = 1000,
	CHILD_ID_BUY,
	CHILD_ID_ACHIEVEMENT,
	CHILD_ID_COLLECTION,
	CHILD_ID_RACE
};

enum SCENECODE{
    SCENECODE_MAIN = 0,
    SCENECODE_ACTION,
    SCENECODE_RACE,
    SCENECODE_FARMING,
    SCENECODE_BUY,
    SCENECODE_SELL,
    SCENECODE_INVENTORY,
    SCENECODE_RECHARGE,
    SCENECODE_PURCHASE,
    SCENECODE_COLLECTION,
    SCENECODE_ACHIEVEMENT,
	SCENECODE_CLOSEPOPUP,
	SCENECODE_CLOSESCENE,
	SCENECODE_RACE_RUN, //Action 에서 사용됨
	SCENECODE_FARMING_SEED, //Farm 에서 사용됨
    SCENECODE_NONE,
    SCENECODE_POPUP_1,
    SCENECODE_POPUP_2,
    SCENECODE_POPUP_3,
    SCENECODE_POPUP_4,
    SCENECODE_POPUP_5,

};

class MainScene : public Scene{
public:
    static Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

	static Sprite * getIdle(int id = 100);
	static RepeatForever * getIdleAnimation(int id = 100);

	static LayerColor * createMessagePopup(LayerColor* &layerBG, Node * parent, const string title, const string msg, bool enableParticle);
private:
    gui mGrid;
    ui_gacha  mGacha;
    LayerColor * layer, *layerGray;    
    cocos2d::LayerColor * mParitclePopupLayer, * mParitclePopup;
    bool mIsParticleFinished;
    LoadingBar * loadingBar;

    Label * mName, * mJobTitle, * mExp, * mProperties;
    MenuItemFont * mPoint, * mHP, * mInventory, * mFarming, * mSell, * mBuy, * mAchievement;
    Layout * mAlertLayer;

	SCENECODE mCurrentScene; //현재 Scene 정보
	time_t mLastUpdateTrade; //최근 시세 업데이트 시각

    static void paricleCB(){
        //this->removeChild(mParitclePopupLayer);
    };
    //void scheduleCB(float f);
    void callback0();
    void callback1(Ref* pSender);
    void callback2(Ref* pSender, SCENECODE type);
  
	static void achievementCB(bool isDaily, int idx) {
		noticeEffect(hInst->mAchievement);
	};

	static void noticeEffect(MenuItemFont * p) {
		if (p) {
			p->stopAllActions();
			p->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.3, 1.2), ScaleTo::create(0.3, 1), NULL)));
		}
	};

    static void farmingCB(int fieldId){
		if(hInst->mCurrentScene != SCENECODE_FARMING)
			noticeEffect(hInst->mFarming);
	};

    static void tradeCB(time_t t){
		hInst->mLastUpdateTrade = t;
		noticeEffect(hInst->mSell);
		//noticeEffect(hInst->mBuy);
    }

    virtual void onEnter();
    virtual void onEnterTransitionDidFinish();
    virtual void onExitTransitionDidStart();
    virtual void onExit();


    void store();
    //void store2();
    void particleSample(const string sz);

    void dailyReward();

    static MainScene * hInst;

    //
    void actionList(); //액션 목록
    void updateState(bool isInventoryUpdated); // hp, exp 등 업데이트
    void alert(const string msg); //alert
	void showResult(const string msg, bool enableParticle); //결과 보기
    void alertCloseCallback(Ref* pSender);
	void showInventory(inventoryType type = inventoryType_all, bool isSell = false);	//가방조회
	void showInventoryCategory(Ref* pSender, inventoryType code, bool isSell); //가방조회 카테고리
	void scheduleRecharge(float f); // HP 충전 쓰레드
	void showBuy(inventoryType type = inventoryType_all); //구매 보기
	void showBuyCategory(Ref* pSender, inventoryType code); // 구매 카테고리
	void buyCallback(Ref* pSender, int id); //구매
	void invokeItem(Ref* pSender, int id); //아이템 발동
	void sellItem(Ref* pSender, inventoryType code, int id); //아이템 판매
	void callbackAction(Ref* pSender, int id);
	void callbackActionAnimation(Ref* pSender, int id);	
	void showAchievement(); //업적 보기
	void showAchievementCategory(Ref* pSender, bool isDaily); //업적 카테고리
	void runRace(Ref* pSender, int raceId); //Race
	void showCollection(); //도감 보기
	void showRace(); //Race 보기
	void closePopup(); //팝업 닫으면 메인

	string getTradeRemainTime() {
		int remain = logics::hInst->getTrade()->getRemainTimeUpdate();
		string sz = "Update after " + to_string((int)(remain / 60)) + ":" + to_string((int)(remain % 60));
		return sz;
	};
};


#endif //PROJ_ANDROID_MAINSCENE_H
