#pragma once

#if defined(_WIN32) && !defined(COCOS2D_DEBUG)
    #include "stdafx.h"
    #include "./rapidjson/document.h"
    #include "./rapidjson/stringbuffer.h"
    #include "./rapidjson/writer.h"
#else
    //#include "json/rapidjson.h"
    #include "json/document.h"
    #include "json/stringbuffer.h"
    #include "json/writer.h"
#endif

//using namespace rapidjson;

#include "library/farming.h"
#include "library/inventory.h"
#include "library/trade.h"
#include "library/quest.h"
#include "library/sql.h"

#include <locale>
#include <algorithm>

//Config
#define CONFIG_ACHIEVEMENT "achievement.json"
#define CONFIG_ACTOR "actor.json"
#define CONFIG_ACTOR_BACKUP "actor_backup.json"
#define CONFIG_META "meta.json"
#define CONFIG_ACTIONS "actions.json"

#define CONFIG_SQLITE3 "cat.s3db"

//만랩
#define LEVEL_MAX	12
//levelup bonus $
#define bonusCashPerLevel 100
//HP 추가 간격 5분?
#define HPIncreaseInterval	(30.f * 60.f)
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
//speed up 아이템이 증가시키는 거리 비율 += total * raceSpeedUp
#define raceSpeedUp 1.2f
//race rand(매력 * x)
#define raceAppealRatio 0.8
//race AI advantage ratio
#define raceAIAdvantageRatio 0.1
//race 경묘에서 지력이 차지하는 비율
#define raceIntelligenceRatio 0.5f
//race boost 최대 반영 비율
#define raceTouchBoostRatio 0.2f
//race AI item 등급별 추가 지급
//#define raceItemQuantityPerLevel 1
//race 스퍼트 구간
#define raceSpurt 50 
//race 아이템을 처음 사용하기 시작하는 시점
#define raceInvokeThreshold 60
//race AI 스킬발동 랜덤 확률 1/raceAIRandom 
#define raceAIRandom 10
//farming 식탐 방지 지능 threshold
#define farmGluttonyIntelThreshold 30
//farming 식탐
#define farmGluttony 2
//farming 최대 밭개수
#define farmMaxField 3
//Actor save interval
#define actorSaveInterval 5
//Quest 노출 quest 수
#define questCnt	3 
#define COIN	"c"
#define COIN_W	L"c"

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
	error_farming_gluttony,
	error_full_hp,
	error_invalid_time,
	error_not_enough_level,
};
enum inventoryType {
	inventoryType_all = -1,
	inventoryType_growth=0,
	inventoryType_HP,
	inventoryType_race,
	inventoryType_adorn,
	inventoryType_farming,
    inventoryType_wall,
    inventoryType_bottom,
    inventoryType_interior,
	inventoryType_max,
    inventoryType_collection
};
//아이템 종류
enum itemType {
	itemType_training = 0,
	itemType_training_material,
	itemType_hp = 100,
	itemType_hp_meal,
	itemType_race = 200, //경묘
	itemType_race_shield,		//방어 쉴드
	itemType_race_speedUp,		//속업
	itemType_race_attactFront,	//전방 공격
	itemType_race_attactFirst,	//1등 공격
	itemType_race_obstacle,		//장애물
	itemType_adorn = 300, //치장
	itemType_adorn_head,
    itemType_farming = 400, //농사
    itemType_wall = 500, //벽지
    itemType_bottom, //바닥지
    itemType_interior, //인테리어
	itemType_max,
	itemType_collection,	//도감용
};
//training type
enum trainingType {
	trainingType_play = 0,	//놀이
	trainingType_study,		//공부
	trainingType_party,		//파티
	trainingType_training,	//수련
	trainingType_work,		//알바
	trainingType_fishing,	//낚시
	trainingType_hunting,		//사냥
	trainingType_max,
};

//achievement category
enum achievement_category {
	achievement_category_training = 0,
	achievement_category_trade_buy,
	achievement_category_trade_sell,
	achievement_category_recharge, //체력 충전
	achievement_category_farming,   // 농사 전체
	achievement_category_property,	//능력치

	achievement_category_race, //경묘 전체

	achievement_category_race_use_item, // 경묘 아이템 사용
	achievement_category_race_use_item_type, // 경묘 아이템 사용 타입

	achievement_category_race_item, // 경묘 아이템 모드
	achievement_category_race_speed, // 경묘 스피드 모드
	achievement_category_race_1vs1, //경묘 1:1
	achievement_category_race_friend_1, // 경묘 friend 모드
	
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
enum achievement_property_id {
	achievement_property_id_total = 0, //전체 능력치
	achievement_property_id_S,
	achievement_property_id_I,
	achievement_property_id_A,
};

//race
enum race_mode {
	race_mode_item = 0,		//vs cpu
	race_mode_speed,
	race_mode_1vs1,
	race_mode_friend_1,	//user vs user
	race_mode_max
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

	_property() {
		this->appeal = 0;
		this->intelligence = 0;
		this->strength = 0;
	};
};
typedef map<int, bool> keyBoolMap;
//캐릭터
struct _actor {
	string userId;		//사용자ID
	string userName;	//사용자명
	string id;			//고양이ID
	string name;		//고양이 이름
	string jobTitle;		//타이틀. 속성 합산이 0 이하면 이번생은 망했어묘
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
	race_mode mode;
	int min;
	int max;
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
	itemType shootCurrentType; //현재 사용한 아이템 타입
	int shootCurrentQuantity; //현재 사용한 아이템 수량

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
		this->shootCurrentType = itemType_max;
		this->shootCurrentQuantity = 0;
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

typedef map<int, _item> __items;
typedef map<int, _training, std::greater<int>> __training;
typedef map<int, _race> raceMeta;

class logics 
{
public:
	logics() {
		if (hInst == NULL) {
			mIsFinalized = false;
			hInst = this;
			srand((int)time(0));
			mRaceParticipants = new raceParticipants;
			for (int n = 0; n < race_mode_max; n++)
				mRaceModeCnt[n] = 0;
		}		
	};
	~logics() {
		if(!mIsFinalized)
			finalize();
		hInst = NULL;
	};
	bool init(farmingFinshedNotiCallback, tradeUpdatedCallback, achievementCallback, bool isFarmingDataLoad = true);
	void finalize();
	void saveActor(); //현재 정보 저장
	bool insertItem(_item);	
	bool insertTraining(_training);
	
	void print(int type = 0);

    _actor * getActor() {
      return mActor;
    };

    //check traning time validation
    bool isValidTraningTime(int id);
	bool isValidTraningLevel(int id);

    __training * getActionList(){
        return &mTraining;
    };

	__items * getItems() {
		return &mItems;
	};
	_item getItem(int id) {
		return mItems[id];
	};

	//get Trade
	trade * getTrade() {
		return &mTrade;
	};

	//get max exp
	int getMaxExp();
	//increment HP
	bool increaseHP(int);
	//increment Point
	void increasePoint(int amount) {
		getActor()->point += amount;
	};

    float getExpRatio(){
        return (float)mActor->exp / (float)getMaxExp() * 100.0f;
    };

	//get HP
	int getHP();

	//get max HP
	int getMaxHP();

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

	//quest
	Quest * getQuests() {
		return &mQuest;
	};
    
	//----------------------------------------------------------------------Farm
	//farming
	farming * getFarm() {
		return &mFarming;
	};

    //farming
    void addSeed(farming::seed * p){
        mFarming.addSeed(p);
    };
	errorCode farmingPlant(int idx, int seedId);
	errorCode farmingHarvest(int idx, int &productId, int &earning);
	errorCode farmingCare(int idx);
	//돈 차감이랑 최대 밭 개수 지정
	errorCode farmingExtend(int x, int y);
	farming::field * farmingAddField(int x, int y);
	void farmingAddField(farming::field * f);
	farming::questVector * farmingGetQuest(int cnt) {
		return mFarming.createQuest(cnt);
	};
	bool farmingQuestDone(int idx);
	int farmingQuestReward(int idx);
	//----------------------------------------------------------------------Action
	//Training 
	errorCode isValidTraining(int id);
	errorCode runTraining(int id, itemsVector &rewards, _property * rewardProperty, int &point, trainingType &type, float preservationRatio = 0.f);

	//Trade
	errorCode runTrade(bool isBuy, int id, int quantity);
	int getTradeInvenTypeCnt(inventoryType type); //Trade 인벤타입 갯수

	//----------------------------------------------------------------------Race
	errorCode runRace(int id, itemsVector &items);
	//위에 함수를 2개로 나눔
	errorCode runRaceSetRunners(int id);
	errorCode runRaceSetItems(itemsVector &items);
	raceParticipants* getRaceRunners() {
		return mRaceParticipants;
	}
	errorCode runRaceValidate(int id);
	//get race info
	raceMeta * getRace() {
		return &mRace;
	};
	//race 진행
	raceParticipants* getNextRaceStatus(bool &ret, int itemIdx, int boost = 0);
	//race 결과
	_raceCurrent* getRaceResult() {
		return &mRaceCurrent;
	}
	//race 순위
	raceParticipants * getRaceRank() {
		return &mRaceOrderedVector;
	};
	int getRaceModeCnt(race_mode mode) {
		return mRaceModeCnt[mode];
	};
	//능력치에 따라 상금을 달리 지급
	int getRaceReward(int id, int rankIdx /*1등 = 0*/);
	struct RaceWin {
		int winCnt;
		int raceId;
		RaceWin() {
			winCnt = 0;
			raceId = 0;
		};
		void init(int id) {
			raceId = id;
			winCnt = 0;
		};
	};
	void invokeRaceItem(int idx, itemType type, int quantity);
	//장애물에 걸림
	void invokeRaceObstacle(int idx, int level) {
		invokeRaceItem(idx, itemType_race_obstacle, level);
	}
	RaceWin mRaceWin;
	//-----------------------------------------------------------------------charge
	errorCode runRecharge(int id, int quantity);	
	//auto recharge HP 충전이 되면 true, 이미 만땅이거나 시간이 아니면 false
	bool rechargeHP();
	
	//------------------------------------------------------------------------add inventory
	bool addInventory(int itemId, int quantity);
	inventoryType getInventoryType(int itemId);

	//achievement
	//achievement mAchievement;
	void achieveReward() {
		for (int n = 0; n < getQuests()->getQuests()->size(); n++) {
			Quest::_quest * p = getQuests()->getQuests()->at(n);
			if (p->isFinished && p->isReceived == false)
				achieveReward(p);
		}
	};
	void achieveReward(Quest::_quest * p) {
		addInventory(p->rewardId, p->rewardValue);		
		getQuests()->rewardReceive(p);
		//level만큼 exp 추가
		increaseExp(mActor->level);
	};
	//L10N
	string getL10N(const string L10NKey) {		
		return wstring_to_utf8(mL10NMap[L10NKey]);
	};
	wstring getL10N(const string L10NKey, int val) {
		wstring title = sprint(mL10NMap[L10NKey], val);
		return title;
	};

	//quest
	Quest mQuest;
	static logics * hInst;

	bool mIsRunThread;
private:
	//error messages
	typedef vector<wstring> errorMessages;
	errorMessages mErrorMessages;	
	map<inventoryType, int> mInvenTypeCntMap;

    __items mItems;		//items
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

	//------------------------------------------------------------------ 경묘
	raceParticipants* mRaceParticipants;	
	raceMeta mRace;		
	_raceCurrent mRaceCurrent;	
	raceParticipants mRaceOrderedVector;

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
	void clearRaceItem(int idx) {
		while (!mRaceParticipants->at(idx).sufferItems.empty())
		{
			mRaceParticipants->at(idx).sufferItems.pop();
		}
	};
	//SIA를 고려한 기본 스피드
	int getBaseSpeed(int s, int i, int a, float ranPercent, int boost);
	
	//increase property
	void addProperty(int strength, int intelligence, int appeal);
	
	//increment exp
	bool increaseExp(int value = 1);

	
	//set Max HP
	void setMaxHP();
	
	//set jobTitle
	void setJobTitle();
    
    void initColor(rapidjson::Value &p);
	bool initActor(bool isFarmingDataLoad);
	bool initErrorMessage(rapidjson::Value &p);
	bool initItems(rapidjson::Value &p, rapidjson::Value &pRace);
	bool initSeed(rapidjson::Value &farming, rapidjson::Value &seed);
	bool initTraining(rapidjson::Value &p);
	bool initJobTitle(rapidjson::Value &p);
	bool initRace(rapidjson::Value &p);
	bool initAchievement(rapidjson::Value &p);

	void printInven(inventoryType type, wstring &sz);    
	static void achievementCallbackFn(int type, int idx);
	static void threadRun();
	
	void saveActorInventory(rapidjson::Document &d, rapidjson::Value &v, inventoryType type);

	//quest
	int getQuestRewardItem(int level) {
		return (level < LEVEL_MAX / 2) ? 50 : 51;
	};	
	void addQuest_property(int uniqueId, int level) {
		//action				
		int val = level * 8;
		mQuest.addQuest(uniqueId, getL10N("QUEST_TITLE_PROPERTY", val)
			, achievement_category_property
			, achievement_property_id_total
			, val, getQuestRewardItem(level), level);
	};
	void addQuest_race_win(int uniqueId, int level) {
		//race				
		int val = max(1, level / 2);		
		mQuest.addQuest(uniqueId, getL10N("QUEST_TITLE_RACE_WIN", val)
			, achievement_category_race
			, achievement_race_id_first
			, val, getQuestRewardItem(level), level);
	};
	void addQuest_race_foremost(int uniqueId, int level) {
		//QUEST_TITLE_RACE_ITEM_FOREMOST				
		int val = level;
		mQuest.addQuest(uniqueId, getL10N("QUEST_TITLE_RACE_ITEM_FOREMOST", val)
			, achievement_category_race_use_item_type
			, 204
			, val, getQuestRewardItem(level), level);
	};
	void addQuest_race_front(int uniqueId, int level) {
		//QUEST_TITLE_RACE_ITEM_FRONT				
		int val = level;
		mQuest.addQuest(uniqueId, getL10N("QUEST_TITLE_RACE_ITEM_FRONT", val)
			, achievement_category_race_use_item_type
			, 203
			, val, getQuestRewardItem(level), level);
	};
	void addQuest_race_speedup(int uniqueId, int level) {
		//QUEST_TITLE_RACE_ITEM_SPEEDUP				
		int val = level;
		mQuest.addQuest(uniqueId, getL10N("QUEST_TITLE_RACE_ITEM_SPEEDUP", val)
			, achievement_category_race_use_item_type
			, 202
			, val, getQuestRewardItem(level), level);
	};
	void addQuest_race_shield(int uniqueId, int level) {
		//QUEST_TITLE_RACE_ITEM_SHIELD				
		int val = level;
		mQuest.addQuest(uniqueId, getL10N("QUEST_TITLE_RACE_ITEM_SHIELD", val)
			, achievement_category_race_use_item_type
			, 201
			, val, getQuestRewardItem(level), level);
	};	
	
	void addQuest_farm_seed(int uniqueId, int level) {
		//action				
		int val = 1 << level;
		mQuest.addQuest(uniqueId, getL10N("QUEST_TITLE_FARM_SEED", val)
			, achievement_category_farming
			, achievement_farming_id_plant
			, val, getQuestRewardItem(level), level);
	};
    
    void addQuest_farm_harvest(int uniqueId, int level) {
        //action
        int val = 1 << level;
        mQuest.addQuest(uniqueId, getL10N("QUEST_TITLE_FARM_HARVEST", val)
                        , achievement_category_farming
                        , achievement_farming_id_harvest
                        , val, getQuestRewardItem(level), level);
    };



	map<string, wstring> mL10NMap;

    //farming
    farming mFarming;
	
	int mFarmingExtendFee;
	//Trade
	trade mTrade;
	//업적 콜백
	achievementCallback mAchieveCB;

	thread* mThread;
	bool mIsFinalized;
	int mRaceModeCnt[race_mode_max];
	//actor.json 파일 내용
	//string mActorStringFromJSON;

	Sql mSql;
	static bool hIsSync;
};

