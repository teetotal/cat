#pragma once

#include <vector>
#include <map>
#include <string>
#include <time.h>
#include <iostream>
#include <locale>
#include <codecvt>
using namespace std;

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
	string name;
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
	string name;
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
	string userName;	//����ڸ�
	string id;			//�����ID
	string name;		//����� �̸�
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
	void print();
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
};

