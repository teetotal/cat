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
	error_invalid_id,
	error_not_enought_property,
	error_not_enought_point,
	error_not_enought_item,
};
enum inventoryType {
	inventoryType_growth=0,
	inventoryType_race,
	inventoryType_adorn,
};
//������ ����
enum itemType {
	itemType_strength = 0,
	itemType_intelligence,
	itemType_appeal,
	itemType_race_offense, //�湦 ����
	itemType_adorn_head, //�Ӹ�ġ��
	max_itemType
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
	}

	//Training 
	errorCode isValidTraining(int id);
	errorCode runTraining(int id, vector<_itemPair> &rewards);

	//Trade
	errorCode runTrade(bool isBuy, int id, int quantity);
private:
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
			break;
		}
		return true;
	}
	//increase property
	void addProperty(int strength, int intelligence, int appeal) {
		mActor->property.strength += strength;
		mActor->property.intelligence += intelligence;
		mActor->property.appeal += appeal;
	}
};

