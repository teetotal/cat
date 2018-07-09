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
//������ ����
enum itemType {
	itemType_training = 0,
	itemType_training_food,
	itemType_training_doll,
	itemType_training_etc,
	itemType_hp = 100,
	itemType_hp_meal,
	itemType_race = 200, //�湦
	itemType_race_attact,
	itemType_adorn = 300, //ġ��
	itemType_adorn_head,
	itemType_max
};
//������
struct _item {
	int id;
	wstring name;
	itemType type; //�⺻, ����, ����
	int grade;
	int value;
};

//meta
struct _itemPair {
	int itemId;
	int val;
};


//�ִ� ���� �ջ� �ü�
#define maxTradeValue 100
//�ִ� ������ ����/���
#define maxTrainingItems 10
//�⺻ HP
#define defaultHP 4

//���� ����
struct _reward {
	int strength;
	int intelligence;
	int appeal;
	int point;
	_itemPair* items[maxTrainingItems];
};

//���� ���
struct _cost {
	int strength;
	int intelligence;
	int appeal;
	int point;
	_itemPair* items[maxTrainingItems];
};

//����
struct _training {
	int id;
	wstring name;
	_reward reward;
	_cost cost;
};

//�Ӽ�
struct _property {
	int strength;			//ü��
	int intelligence;		//�Ѹ���
	int appeal;				//�ŷ�
};

typedef map<int, int> keyQuantity;
//�κ��丮
struct _inventory {
	keyQuantity growth;	//����� ������
	keyQuantity hp;		//HP���� ������
	keyQuantity race;	//�湦�� ������
	keyQuantity adorn;	//ġ��� ������
};

//ĳ����
struct _actor {
	string userId;		//�����ID
	wstring userName;	//����ڸ�
	string id;			//�����ID
	wstring name;		//����� �̸�
	int point;			//����ĳ��
	int hp;				//�Ƿε�
	int exp;			//����ġ
	int level;			//����
	_property property;		//�Ӽ�
	_inventory inventory;	//�κ��丮
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
	//Ʈ���̴� ����, ���
	typedef vector<int, _itemPair> itemVector;

	typedef map<int, _item> __items; //items
	__items mItems;
	typedef vector<_itemPair> __pair; //���� �ü�
	__pair mTrade;
	typedef map<int, _training> __training; //����
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
	//����ġ ����
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

