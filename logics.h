#pragma once

#include <vector>
#include <map>
#include <string>
#include <time.h>
#include <iostream>
#include <locale>
#include <codecvt>
using namespace std;
enum errorCode {
	error_success = 0,
	error_levelup,
	error_invalid_id,
	error_not_enough_property,
	error_not_enough_point,
	error_not_enough_item,
	error_not_enough_hp
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
	itemType_race_attact,
	itemType_adorn = 300, //치장
	itemType_adorn_head,
	itemType_max,
	itemType_collection = 1000,	//도감용
};
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


//최대 무역 합산 시세
#define maxTradeValue 100
//최대 아이템 보상/비용
#define maxTrainingItems 10
//기본 HP
#define defaultHP 4
//도감용 아이템 시작 id
#define collectionStartId 1000

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
	wstring title;		//타이틀. 속성 합산이 0 이하면 이번생은 망했어묘
	int point;			//보유캐시
	int hp;				//피로도
	int exp;			//경험치
	int level;			//레벨
	_property property;		//속성
	_inventory inventory;	//인벤토리
	keyBoolMap collection;	//도감
};

class logics 
{
public:
	logics() {};
	~logics() {};
	bool init();
	bool insertItem(_item item);
	bool setTradeMarketPrice();
	bool insertTraining(_training traning);
	bool setActor(_actor* actor);
	void print(int type = 0);
	_item getItem(int id) {
		return mItems[id];
	};
	void addErrorMessage(const wstring & sz) {
		mErrorMessages.push_back(sz);
	};

	const wchar_t * getErrorMessage(errorCode code) {
		return mErrorMessages[code].c_str();
	};

	//Training 
	errorCode isValidTraining(int id);
	errorCode runTraining(int id, vector<_itemPair> &rewards, _property * rewardProperty, int &point);

	//Trade
	errorCode runTrade(bool isBuy, int id, int quantity);

	//charge
	errorCode runRecharge(int id, int quantity);
	void recharge(int val);
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

	_actor* mActor;

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
	//check traning time validation
	bool isValidTraningTime(int id);

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

