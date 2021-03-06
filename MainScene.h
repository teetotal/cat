//
// Created by Jung, DaeCheon on 27/07/2018.
//

#ifndef PROJ_ANDROID_MAINSCENE_H
#define PROJ_ANDROID_MAINSCENE_H

#include "cocos2d.h"
#include "ui/ui.h"
#include "ui/ui_gacha.h"
#include "ui/ui_cultivation.h"
#include "ui/ui_deco.h"
#include "ui/ui_bgText.h"

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

#define LOADINGBAR_IMG "loadingbar_big.png"
#define LOADINGBAR_IMG_SMALL "loadingbar_small.png"

enum ZORDER {
	ZORDER_BASE	= 0,
	ZORDER_QUEST = 1,
	ZORDER_POPUP,
};
enum CHILD_ID {
	CHILD_ID_INVENTORY = 1000,
	CHILD_ID_ACTION,
	CHILD_ID_BUY,
	CHILD_ID_ACHIEVEMENT,
	CHILD_ID_COLLECTION,
	CHILD_ID_RACE,
    CHILD_ID_QUANTITY_COLOR
};

enum SCENECODE {
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
    SCENECODE_DECORATION,
	SCENECODE_CLOSEPOPUP,
	SCENECODE_CLOSESCENE,
	SCENECODE_RACE_RUN, //Action 에서 사용됨
	SCENECODE_RACE_FINISH,
	SCENECODE_FARMING_SEED, //Farm 에서 사용됨
	SCENECODE_POINT_ADVERTISEMENT,//포인트 광고 
	SCENECODE_POINT_SHOP,//포인트 충전 
	SCENECODE_HP_ADVERTISEMENT,//HP 광고 
	SCENECODE_HP_SHOP,//HP 충전 
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
    static LayerColor * getItemColor(int id);

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
    bool initDeco();
    int initDecoObject(const char * sz, ui_deco::SIDE side);
    
	int mLevel;
    gui mGrid;
    LayerColor * layer, *layerGray;    
    bool mIsParticleFinished;
    LoadingBar * loadingBar;

    Label * mName, * mJobTitle, * mExp, * mProperties;
    MenuItemLabel * mInventory, * mFarming, * mSell, * mBuy, * mAchievement;
    ui_bgText mPoint, mHP;
    Layout * mAlertLayer;

	//Quantity
    Layout * mQuantityLayout;
	Sprite * mQuantityImg;
	Label * mQuantityTitle, * mQuantityLabel, *mQuantityPrice;
	int mQuantity, mQuantityItemId;
	bool mIsSell; //buy인지 sell인지 구분

	void quantityCallback(Ref* pSender, int value); // 수량 제어

	SCENECODE mCurrentScene; //현재 Scene 정보
	time_t mLastUpdateTrade; //최근 시세 업데이트 시각
	time_t mLastUpdateQuest;
    time_t mLastUpdatePlayBonus; //최근 플레이 보너스 지급
	int mActionCnt; //action 진행 시간 카운트
	int mActionTouchCnt;  //action touch 클릭 카운트

    static void paricleCB(){
        //this->removeChild(mParitclePopupLayer);
    };
    //void scheduleCB(float f);
    void callback0();
    void callback1(Ref* pSender);
    void callback2(Ref* pSender, SCENECODE type);
  
	static void achievementCB(int level, int idx) {
		//noticeEffect(hInst->mAchievement);
        //hInst->updateQuests();
	};

	static void noticeEffect(MenuItemLabel * p) {
		if (p) {
			p->stopAllActions();
			p->runAction(gui::inst()->createActionBlink());
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
    void dailyReward();

    static MainScene * hInst;

    //
    void actionList(); //액션 목록
    void updateState(bool isInventoryUpdated); // hp, exp 등 업데이트
	void alert(errorCode err);

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
	vector<Menu*> mQuestButtons; //퀘스트 표시용 vector
	void updateQuests(); //퀘스트 표시하기
	void showAchievement(); //업적 보기
	void showAchievementCategory(Ref* pSender); //업적 카테고리
	void achievementCallback(Ref* pSender, Quest::_quest * p);
	void recieveAllAchievement(Ref * pSender) {
		logics::hInst->achieveReward();
		updateState(true);
		callback2(this, SCENECODE_CLOSEPOPUP);
	};
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

	SCENECODE getSceneCodeFromQuestCategory(int category);
    
    virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    virtual void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    //멀티 터치시 두 포인트 간의 거리
    float mTouchGap;
    Layout * mMainLayoput;
    Vec2 mTouchStart;
    Size mTouchGrid;
    float mInteriorScale;
//    vector<Vec2> mTouchPosVec;
    
    vector<Touch*> mTouchVec;
    //아이템 적용
    void applyInventory(Ref* pSender, int itemId);
    //아이템 가방
    void backToInventory(Ref* pSender);
    //아이템 회전
    void flip(Ref* pSender);
    void createDecoMenu(Vec2 pos);
    void removeDecoMenu();
    
    int getValidTouchCnt(){
        int ret = 0;
        for(int n =0; n< mTouchVec.size(); n++){
            if(mTouchVec[n])
                ret++;
        }
        return ret;
    };
};


#endif //PROJ_ANDROID_MAINSCENE_H

