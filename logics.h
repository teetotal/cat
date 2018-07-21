#pragma once
#include "stdafx.h"
#include "library/farming.h"
#include "library/inventory.h"
#include "library/trade.h"
#include "library/achievement.h"

#include <locale>
#include <algorithm>

//Config
#define CONFIG_ACHIEVEMENT "resource/achievement.json"
#define CONFIG_ACTOR "resource/actor.json"
#define CONFIG_ACTOR_BACKUP "resource/actor_backup.json"
#define CONFIG_META "resource/meta.json"

//HP 추가 간격
#define HPIncreaseInterval	60 * 5
//최대 아이템 보상/비용
#define maxTrainingItems 10
//기본 HP
#define defaultHP 4
//도감용 아이템 시작 id
#define collectionStartId 1000
//나를 제외한 경묘 참가자 수
#define raceParticipantNum 4
//한번 race에서 사용가능한 아이템 수
#define raceItemSlot 3
//speed up 아이템이 증가시키는 거리
#define raceSpeedUp 50
//race rand(매력 * x)
#define raceAppealRatio 2
//race AI advantage ratio
#define raceAIAdvantageRatio 0.1
//race 경묘에서 지력이 차지하는 비율
#define raceIntelligenceRatio 0.5
//race level에 따른 아이템 quantity
#define raceItemQuantityPerLevel 2
//race 스퍼트 구간
#define raceSpurt 70 
//race 아이템을 처음 사용하기 시작하는 시점
#define raceInvokeThreshold 50
//race AI 스킬발동 랜덤 확률 1/raceAIRandom 
#define raceAIRandom 10
//farming 식탐 방지 지능 threshold
#define farmGluttonyIntelThreshold 30
//farming 식탐
#define farmGluttony 2
//Actor save interval
#define actorSaveInterval	10

enum errorCode {
	error_success = 0,
	error_levelup,
	error_invalid_id,
	error_invalid_quantity,
	error_not_enough_property,
	error_not_enough_point,
	error_not_enough_item,
	error_not_enough_hp,
	error_not_enough_strength,
	error_farming_failure,
	error_farming_gluttony
};
enum inventoryType {
	inventoryType_growth=0,
	inventoryType_HP,
	inventoryType_race,
	inventoryType_adorn,
	inventoryType_farming,
	inventoryType_collection,
};
//아이템 종류
enum itemType {
	itemType_training = 0,
	itemType_training_food,
	itemType_training_doll,
	itemType_training_etc,
	itemType_hp = 100,
	itemType_hp_meal,
	itemType_race = 200, //경묘
	itemType_race_shield,		//방어 쉴드
	itemType_race_speedUp,		//속업
	itemType_race_attactFront,	//전방 공격
	itemType_race_attactFirst,	//1등 공격
	itemType_adorn = 300, //치장
	itemType_adorn_head,
    itemType_farming = 400, //농사
	itemType_max,
	itemType_collection = 1000,	//도감용
};
//training type
enum trainingType {
	trainingType_play = 0,		//놀이
	trainingType_fishing,		//낚시
	trainingType_entertainment,	//화류계
	trainingType_improve,		//수련
	trainingType_study,			//학습
	trainingType_max,
};

//achievement category
enum achievement_category {
	achievement_category_training = 0,
	achievement_category_trade_buy,
	achievement_category_trade_sell,
	achievement_category_recharge, //체력 충전
	achievement_category_farming,   
	achievement_category_race	
};
enum achievement_farming_id {
	achievement_farming_id_plant = 0, //심기 횟수
	achievement_farming_id_care,	//돌보기 횟수
	achievement_farming_id_harvest,		//수확 횟수
	achievement_farming_id_output		//수확량
};
enum achievement_race_id {
	achievement_race_id_try = 0, //참가 횟수
	achievement_race_id_first,	//1등
	achievement_race_id_second,	//2등
};

//HP lock
static mutex lockHP;

//아이템
struct _item {
	int id;
	wstring name;
	itemType type; //기본, 공격, 수비
	int grade;
	int value;
};

//meta
struct _itemPair {
	int itemId;
	int val;
};
typedef vector<_itemPair> itemsVector;

//성장 보상
struct _reward {
	int strength;
	int intelligence;
	int appeal;
	int point;
	_itemPair* items[maxTrainingItems];
};

//성장 비용
struct _cost {
	int strength;
	int intelligence;	
	int appeal;
	int point;
	_itemPair* items[maxTrainingItems];	
};

//성장
struct _training {
	int id;
	wstring name;
	trainingType type;
	int level;      //시작 레벨
	time_t start;	//시작
	int count;		//등장 횟수
	int keep;		//유지
	int interval;	//사라지는 시간
	_reward reward;
	_cost cost;
};

//속성
struct _property {
	int strength;			//체력
	int intelligence;		//총명함
	int appeal;				//매력

	int total() {
		return strength + intelligence + appeal;
	};
};
typedef map<int, bool> keyBoolMap;
//캐릭터
struct _actor {
	string userId;		//사용자ID
	wstring userName;	//사용자명
	string id;			//고양이ID
	wstring name;		//고양이 이름
	wstring jobTitle;		//타이틀. 속성 합산이 0 이하면 이번생은 망했어묘
	int point;			//보유캐시
	int hp;				//피로도
	int exp;			//경험치
	int level;			//레벨
	_property property;		//속성	
	inventory inven;
	keyBoolMap collection;	//도감

	time_t lastLoginLoginTime;	//마지막 로그인 시간
	time_t lastLoginLogoutTime; //마지막 로그아웃 시간
	time_t lastHPUpdateTime; //지난 HP 업데이트 시간
};

//직업명 prefix
struct _jobTitlePrefix {
	int level;
	wstring title;
};

//직업명 body
struct _jobTitleBody {
	int S;
	int I;
	int A;
	wstring title;
};

//race 상금 + 부상
struct _raceReward {
	int prize;
	itemsVector items;
};
//race
struct _race {
	int id;
	wstring title;
	int length;
	int level;
	int fee;
	vector<_raceReward> rewards;
};
//경묘 참가자
struct _raceParticipant : _property {
	int idx;			//경주 번호
	int items[raceItemSlot];
	int currentLength;	//현재 이동 거리
	int totalLength;	//전체 이동 거리
	float ratioLength;	//전체 거리중 이동한 거리 비율 %
	int rank;			//최종 등수
	int currentRank;	//현재 등수

	//내가 겪고 있는 아이템 
	std::queue<itemType> sufferItems;
	itemType currentSuffer; //현재 겪고 있는 아이템

	//아이템 사용 횟수
	int shootItemCount;

	_raceParticipant() {
		this->idx = 0;
		for (int n = 0; n < raceItemSlot; n++)
			this->items[n] = 0;
		this->currentLength = 0;
		this->totalLength = 0;
		this->ratioLength = 0;
		this->rank = 0;			
		this->currentRank = 0;
		this->shootItemCount = 0;
	};
	bool operator <(const _raceParticipant &a) const {
		return this->totalLength > a.totalLength;		
	}
};
//경묘 대회 참가자 벡터
typedef vector<_raceParticipant> raceParticipants;
//경묘 진행 정보
struct _raceCurrent
{
	int id;
	int rank;			//순위
	int prize;			//보상 상금
	int rewardItemId;	//보상 아이템
	int rewardItemQuantity; //보상 아이템 수량
};

class logics 
{
public:
	logics() {
		mIsFinalized = false;
		hInst = this;
		srand((int)time(0));		
		mRaceParticipants = new raceParticipants;
	};
	~logics() {
		if(!mIsFinalized)
			finalize();
		hInst = NULL;
	};
	bool init(farmingFinshedNotiCallback
		, tradeUpdatedCallback		
	);
	void finalize();
	void saveActor(); //현재 정보 저장
	bool insertItem(_item);	
	bool insertTraining(_training);
	
	void print(int type = 0);
	
	_item getItem(int id) {
		return mItems[id];
	};
	bool isAvailableHP() {
		return getHP() > 0 ? true : false;
	}
	void addErrorMessage(const wstring & sz) {
		mErrorMessages.push_back(sz);
	};

	const wchar_t * getErrorMessage(errorCode code) {
		return mErrorMessages[code].c_str();
	};

	void setDefaultJobTitle(wstring);
	void addJobTitlePrefix(_jobTitlePrefix&);
	void addJobTitleBody(_jobTitleBody&);

	//경묘 메타 정보
	void addRaceMeta(_race & race);

	//achievement
	int getAchievementSize(bool isDaily) {
		return mAchievement.getSize(isDaily);
	};
	bool getAchievementDetail(bool isDaily, int idx, achievement::detail &p) {
		return mAchievement.getDetail(isDaily, idx, p);
	};
    
    //farming
    void addSeed(farming::seed * p){
        mFarming.addSeed(p);
    };
	errorCode farmingPlant(int idx, int seedId);
	errorCode farmingHarvest(int idx, int &productId, int &earning);
	errorCode farmingCare(int idx);
	//돈 차감이랑 최대 밭 개수 지정
	errorCode farmingExtend();

	//Training 
	errorCode isValidTraining(int id);
	errorCode runTraining(int id, itemsVector &rewards, _property * rewardProperty, int &point, trainingType &type);

	//Trade
	errorCode runTrade(bool isBuy, int id, int quantity);

	//Race
	errorCode runRace(int id, itemsVector &items);
	//race 진행
	raceParticipants* getNextRaceStatus(bool &ret, int itemIdx);
	//race 결과
	_raceCurrent* getRaceResult() {
		return &mRaceCurrent;
	}
	//charge
	errorCode runRecharge(int id, int quantity);	

	//auto recharge HP 충전이 되면 true, 이미 만땅이거나 시간이 아니면 false
	bool rechargeHP();
	
	//add inventory
	bool addInventory(int itemId, int quantity);

	//achievement
	achievement mAchievement;
	static logics * hInst;

	bool mIsRunThread;
private:
	//error messages
	typedef vector<wstring> errorMessages;
	errorMessages mErrorMessages;	
	
	typedef map<int, _item> __items; 
	__items mItems;		//items
	
	typedef map<int, _training> __training; 
	__training mTraining;//성장

	//Actor
	_actor* mActor;

	//직업명
	typedef vector<_jobTitlePrefix> jobTitlePrefixVector;
	typedef vector<_jobTitleBody> jobTitleBodyVector;

	struct jobTitle {
        std::wstring _default;
		jobTitlePrefixVector prefix;
		jobTitleBodyVector body;
	};
	jobTitle mJobTitle;

	//경묘
	raceParticipants* mRaceParticipants;
	typedef map<int, _race> raceMeta;
	raceMeta mRace;		
	_raceCurrent mRaceCurrent;	
	//경묘용 아이템만 따로 모아논 벡터
	typedef vector<int> intVector;
	intVector mItemRace;
	//race 랜덤 아이템
	int getRandomRaceItem();
	//race 아이템 발동 by Actor
	void invokeRaceItemByIdx(int seq, int itemIdx);
	//race 아이템 발동
	void invokeRaceItem(int seq, itemType type, int quantity, int currentRank);
	//race AI 아이템 발동
	void invokeRaceItemAI();
	//race 순위에 아이템 적용
	void invokeRaceByRank(int rank, itemType type, int quantity);
	//SIA를 고려한 기본 스피드
	int getBaseSpeed(int s, int i, int a);
	
	//increase property
	void addProperty(int strength, int intelligence, int appeal);
	
	//increment exp
	bool increaseExp();
	//get max exp
	int getMaxExp();

	//get max HP
	int getMaxHP();
	//increment HP
	bool increaseHP(int);
	//set Max HP
	void setMaxHP();
	//get HP
	int getHP();
	
	//check traning time validation
	bool isValidTraningTime(int id);
	//set jobTitle
	void setJobTitle();

	inventoryType getInventoryType(int itemId);

	bool initActor();
	void insertInventory(Value &p, inventoryType type);
	bool initErrorMessage(Value &p);
	bool initItems(Value &p);
	bool initSeed(Value &farming, Value &seed);
	bool initTraining(Value &p);
	bool initJobTitle(Value &p);
	bool initRace(Value &p);
	void printInven(inventoryType type, wstring &sz);    
	static void achievementCallback(bool isDaily, int idx);
	static void threadRun();
	
	void saveActorInventory(Document &d, Value &v, inventoryType type);
    //farming
    farming mFarming;
	int mFarmingExtendFee;
	//Trade
	trade mTrade;

	thread* mThread;
	bool mIsFinalized;
	
};

