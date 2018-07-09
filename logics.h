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
	itemType_max
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

//캐릭터
struct _actor {
	string userId;		//사용자ID
	wstring userName;	//사용자명
	string id;			//고양이ID
	wstring name;		//고양이 이름
	int point;			//보유캐시
	int hp;				//피로도
	int exp;			//경험치
	int level;			//레벨
	_property property;		//속성
	_inventory inventory;	//인벤토리
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
	//트레이닝 보상, 비용
	typedef vector<int, _itemPair> itemVector;

	typedef map<int, _item> __items; //items
	__items mItems;
	typedef vector<_itemPair> __pair; //무역 시세
	__pair mTrade;
	typedef map<int, _training> __training; //성장
	__training mTraining;

	_actor* mActor;

	//price at buy
	int getItemPriceBuy(int tradeIndex) {
		return mTrade[tradeIndex].val + (mItems[mTrade[tradeIndex].itemId].grade * 10);
		
	};
	//price at sell
	int getItemPriceSell(int tradeIndex) {
		return getItemPriceBuy(tradeIndex) * 0.9;
	};
	//add inventory
	bool addInventory(inventoryType type, int itemId, int quantity) {	
		/*
		keyQuantity * p;
		switch (type)
		{
		case inventoryType_growth:
			p = &mActor->inventory.growth;
			break;
		case inventoryType_HP:
			p = &mActor->inventory.hp;
			break;
		case inventoryType_race:
			p = &mActor->inventory.race;
			break;
		case inventoryType_adorn:
			p = &mActor->inventory.adorn;
			break;
		default:
			return false;
		}
		int v = p->find(itemId)->second;
		if ((quantity < 0 && p->find(itemId) == p->end())
			|| p->find(itemId)->second + quantity < 0
			)
			return false;
		p->find(itemId)->second += quantity;
		if (p->find(itemId)->second == 0)
			p->erase(itemId);
		*/
		
		switch (type)
		{
		case inventoryType_growth:					
			if ((quantity < 0 && mActor->inventory.growth.find(itemId) == mActor->inventory.growth.end())
				|| mActor->inventory.growth[itemId] + quantity < 0)
				return false;
			mActor->inventory.growth[itemId] += quantity;
			if (mActor->inventory.growth[itemId] == 0)
				mActor->inventory.growth.erase(itemId);
			break;
		case inventoryType_HP:
			if ((quantity < 0 && mActor->inventory.hp.find(itemId) == mActor->inventory.hp.end())
				|| mActor->inventory.hp[itemId] + quantity < 0)
				return false;
			mActor->inventory.hp[itemId] += quantity;
			if (mActor->inventory.hp[itemId] == 0)
				mActor->inventory.hp.erase(itemId);
			break;
		case inventoryType_race:
			if ((quantity < 0 && mActor->inventory.race.find(itemId) == mActor->inventory.race.end())
				|| mActor->inventory.race[itemId] + quantity < 0)
				return false;
			mActor->inventory.race[itemId] += quantity;
			if (mActor->inventory.race[itemId] == 0)
				mActor->inventory.race.erase(itemId);
			break;
		case inventoryType_adorn:
			if ((quantity < 0 && mActor->inventory.adorn.find(itemId) == mActor->inventory.adorn.end())
				|| mActor->inventory.adorn[itemId] + quantity < 0)
				return false;
			mActor->inventory.adorn[itemId] += quantity;
			if (mActor->inventory.adorn[itemId] == 0)
				mActor->inventory.adorn.erase(itemId);
			break;
		default:
			return false;
		}
		return true;
	}
	//increase property
	void addProperty(int strength, int intelligence, int appeal) {
		mActor->property.strength += strength;
		mActor->property.intelligence += intelligence;
		mActor->property.appeal += appeal;
	}

	int getRandValue(int max) {
		if (max == 0)
			return 0;
		return rand() % max;
	}
	//경험치 증가
	bool increaseExp() {
		mActor->exp++;
		int maxExp = getMaxExp();
		if (maxExp <= mActor->exp) {
			mActor->level++;
			mActor->exp = 0;
			mActor->hp = getMaxHP();
			return true;
		}

		return false;
	}
	int getMaxExp() {
		return 1 << mActor->level;
	}
	int getMaxHP() {
		return defaultHP + (1.5*mActor->level);
	}
};

