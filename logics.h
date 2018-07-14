#pragma once

#include <vector>
#include <map>
#include <queue>
#include <string>
#include <time.h>
#include <iostream>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <mutex>

using namespace std;
//HP 추가 간격
#define HPIncreaseInterval	60 * 5
//Trade 시세 변경 간격
#define tradeUpdateInterval	60 * 12
//최대 무역 합산 시세
#define maxTradeValue 100
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
#define raceSpeedUp 15
//race rand(매력 * x)
#define raceAppealRatio 2
//race AI advantage ratio
#define raceAIAdvantageRatio 0.1

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
};
enum inventoryType {
	inventoryType_growth=0,
	inventoryType_HP,
	inventoryType_race,
	inventoryType_adorn,
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
	itemType_max,
	itemType_collection = 1000,	//도감용
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
};

typedef map<int, int> keyQuantity;
//인벤토리
struct _inventory {
	keyQuantity growth;	//성장용 아이템
	keyQuantity hp;		//HP충전 아이템
	keyQuantity race;	//경묘용 아이템
	keyQuantity adorn;	//치장용 아이템
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
	_inventory inventory;	//인벤토리
	keyBoolMap collection;	//도감

	time_t loginTime;	//시작시간
	time_t lastUpdateTime; //지난 HP 업데이트 시간
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

	//내가 쏜 아이템 
	int shootItem;
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
		srand((int)time(0));
		mLastTradeUpdate = 0;
		mRaceParticipants = new raceParticipants;
	};
	~logics() {};
	bool init();
	bool insertItem(_item);
	bool setTradeMarketPrice();
	bool insertTraining(_training);
	bool setActor(_actor*);
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

	//Training 
	errorCode isValidTraining(int id);
	errorCode runTraining(int id, itemsVector &rewards, _property * rewardProperty, int &point);

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

	//auto recharge HP
	//충전이 되면 true, 이미 만땅이거나 시간이 아니면 false
	bool rechargeHP();
private:
	//error messages
	typedef vector<wstring> errorMessages;
	errorMessages mErrorMessages;	
	
	typedef map<int, _item> __items; 
	__items mItems;		//items
	typedef map<int, int> __keyValInt; 
	__keyValInt mTrade;	//무역 시세
	typedef map<int, _training> __training; 
	__training mTraining;//성장

	//Actor
	_actor* mActor;

	//마지막 TradeUpdate시간
	time_t mLastTradeUpdate;

	//직업명
	typedef vector<_jobTitlePrefix> jobTitlePrefixVector;
	typedef vector<_jobTitleBody> jobTitleBodyVector;

	struct jobTitle {
		wstring default;
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

	//race 아이템 발동
	void invokeRaceItem(int seq, int itemIdx);

	//race AI 아이템 발동
	void invokeRaceItemAI();

	//race 순위에 아이템 적용
	void invokeRaceByRank(int rank, itemType type, int quantity);

	//price at buy
	int getItemPriceBuy(int itemId) {
		return (int)(mTrade[itemId] + (mItems[itemId].grade * 10));
	};
	//price at sell
	int getItemPriceSell(int itemId) {
		return (int)(getItemPriceBuy(itemId) * 0.9);
	};
	//add inventory
	bool addInventory(int itemId, int quantity);
	//increase property
	void addProperty(int strength, int intelligence, int appeal);
	//get random value
	int getRandValue(int max);
	
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

	inventoryType getInventoryType(int itemId) {
		_item item = mItems[itemId];
		inventoryType t;
		if (item.type > itemType_training && item.type < itemType_hp) {
			t = inventoryType_growth;
		}
		else if (item.type > itemType_hp && item.type < itemType_race) {
			t = inventoryType_HP;
		}
		else if (item.type > itemType_race && item.type < itemType_adorn) {
			t = inventoryType_race;
		}
		else if (item.type > itemType_adorn && item.type < itemType_max) {
			t = inventoryType_adorn;
		}
		else {
			t = inventoryType_collection;
		}

		return t;
	};
};

