//
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

    void cultivationCBInner(int id){
        CCLOG("cultivationCBInner %d", id);
    };

	static Sprite * getIdle(int id = 100);

private:
    gui mGrid;
    ui_gacha  mGacha;
    LayerColor * layer, *layerGray;    
    cocos2d::LayerColor * mParitclePopupLayer, * mParitclePopup;
    bool mIsParticleFinished;
    LoadingBar * loadingBar;

    Label * mName, * mJobTitle, * mExp, * mProperties;
    MenuItemFont * mPoint, * mHP, * mInventory;
    Layout * mAlertLayer;


    static void paricleCB(){
        //this->removeChild(mParitclePopupLayer);
    };
    void scheduleCB(float f);
    void callback0();
    void callback1(Ref* pSender);
    void callback2(Ref* pSender, SCENECODE type);
    

    static void cultivationCB(int id){
        hInst->cultivationCBInner(id);
    };

    static void farmingCB(int fieldId){

    }

    static void tradeCB(time_t t){

    }

    virtual void onEnter();
    virtual void onEnterTransitionDidFinish();
    virtual void onExitTransitionDidStart();
    virtual void onExit();


    void store();
    void store2();
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

	void closePopup() {
		this->removeChild(layerGray);
	};
};


#endif //PROJ_ANDROID_MAINSCENE_H
