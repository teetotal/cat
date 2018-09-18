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

#define BG_HOME "bg_home.png"
#define BG_ACTION "bg_action.png"
#define BG_FARM "bg_farming.jpg"
#define BG_ACHIEVEMENT "bg_achievement.png"
#define BG_BUY	"bg_buy.png"
#define BG_COLLECTION "bg_collection.png"
#define BG_INVENTORY "bg_inventory.png"
#define BG_SELL "bg_sell.png"
#define BG_RACE "bg_race.png"

enum CHILD_ID {
	CHILD_ID_INVENTORY = 1000,
	CHILD_ID_ACTION,
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
    SCENECODE_TEMP,

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
	static string getItemImg(int id);


	struct field : farming::field {
		Layout * l;
		Label * label;
		Sprite * sprite;
		bool isHarvestAction;
		field() {

		};
		field(int x, int y) : farming::field(x, y) {
			isHarvestAction = false;
		};
	};


private:

	bool initFarm();

	int mLevel;
    gui mGrid;
    ui_gacha  mGacha;
    LayerColor * layer, *layerGray;    
    cocos2d::LayerColor * mParitclePopupLayer, * mParitclePopup;
    bool mIsParticleFinished;
    LoadingBar * loadingBar;

    Label * mName, * mJobTitle, * mExp, * mProperties;
    MenuItemFont * mPoint, * mHP, * mInventory, * mFarming, * mSell, * mBuy, * mAchievement;
    Layout * mAlertLayer;

	//Quantity
	Sprite * mQuantityImg;
	Label * mQuantityTitle, * mQuantityLabel, *mQuantityPrice;
	int mQuantity, mQuantityItemId;
	bool mIsSell; //buy인지 sell인지 구분

	void quantityCallback(Ref* pSender, int value); // 수량 제어


	SCENECODE mCurrentScene; //현재 Scene 정보
	time_t mLastUpdateTrade; //최근 시세 업데이트 시각
	int mActionCnt; //action 진행 시간 카운트

    static void paricleCB(){
        //this->removeChild(mParitclePopupLayer);
    };
    //void scheduleCB(float f);
    void callback0();
    void callback1(Ref* pSender);
    void callback2(Ref* pSender, SCENECODE type);
  
	static void achievementCB(int level, int idx) {
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
	void alert(errorCode err) {
		return alert(wstring_to_utf8(logics::hInst->getErrorMessage(err)));
	}; //alert
    void alert(const string msg); //alert
	void showResult(const string msg, bool enableParticle); //결과 보기
    void alertCloseCallback(Ref* pSender);	
	void scheduleRecharge(float f); // HP 충전 쓰레드
	
	//Quantity	
	void selectCallback(Ref* pSender, int id); //수량 선택
									
	//Buy
	void showBuy(inventoryType type = inventoryType_all); //구매 보기
	void showBuyCategory(Ref* pSender, inventoryType code); // 구매 카테고리
	void buyCallback(Ref* pSender); //구매
	
	//Inventory
	void showInventory(inventoryType type = inventoryType_all, bool isSell = false);	//가방조회
	void showInventoryCategory(Ref* pSender, inventoryType code, bool isSell); //가방조회 카테고리
	void invokeItem(Ref* pSender, int id); //아이템 발동
	//Sell
	void sellItem(Ref* pSender); //아이템 판매
	//Action
	void showActionCategory(Ref* pSender, int type);
	void callbackAction(Ref* pSender, int id);
	void callbackActionAnimation(int id, int maxTimes);
	//Achievement
	vector<Label*> mQuestButtons; //퀘스트 표시용 vector
	void updateQuests(); //퀘스트 표시하기
	void showAchievement(); //업적 보기
	void showAchievementCategory(Ref* pSender); //업적 카테고리
	//Collection
	void showCollection(); //도감 보기
	//Race
	void showRaceCategory(Ref* pSender, race_mode mode);
	void showRace(); //Race 보기
	void runRace(Ref* pSender, int raceId); 


	void closePopup(); //팝업 닫으면 메인

	string getTradeRemainTime() {
		int remain = logics::hInst->getTrade()->getRemainTimeUpdate();
		string sz = "Update after " + to_string((int)(remain / 60)) + ":" + to_string((int)(remain % 60));
		return sz;
	};
};


#endif //PROJ_ANDROID_MAINSCENE_H
