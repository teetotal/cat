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
	itemType_max,
	itemType_collection = 1000,	//������
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
//������ ������ ���� id
#define collectionStartId 1000

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
	int level;      //���� ����
	time_t start;	//����
	int count;		//���� Ƚ��
	int keep;		//����
	int interval;	//������� �ð�
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
typedef map<int, bool> keyBoolMap;
//ĳ����
struct _actor {
	string userId;		//�����ID
	wstring userName;	//����ڸ�
	string id;			//�����ID
	wstring name;		//����� �̸�
	wstring title;		//Ÿ��Ʋ. �Ӽ� �ջ��� 0 ���ϸ� �̹����� ���߾
	int point;			//����ĳ��
	int hp;				//�Ƿε�
	int exp;			//����ġ
	int level;			//����
	_property property;		//�Ӽ�
	_inventory inventory;	//�κ��丮
	keyBoolMap collection;	//����
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
	__keyValInt mTrade;	//���� �ü�
	typedef map<int, _training> __training; 
	__training mTraining;//����

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

