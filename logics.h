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
//HP �߰� ����
#define HPIncreaseInterval	60 * 5
//Trade �ü� ���� ����
#define tradeUpdateInterval	60 * 12
//�ִ� ���� �ջ� �ü�
#define maxTradeValue 100
//�ִ� ������ ����/���
#define maxTrainingItems 10
//�⺻ HP
#define defaultHP 4
//������ ������ ���� id
#define collectionStartId 1000
//���� ������ �湦 ������ ��
#define raceParticipantNum 4
//�ѹ� race���� ��밡���� ������ ��
#define raceItemSlot 3
//speed up �������� ������Ű�� �Ÿ�
#define raceSpeedUp 15
//race rand(�ŷ� * x)
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
//������ ����
enum itemType {
	itemType_training = 0,
	itemType_training_food,
	itemType_training_doll,
	itemType_training_etc,
	itemType_hp = 100,
	itemType_hp_meal,
	itemType_race = 200, //�湦
	itemType_race_shield,		//��� ����
	itemType_race_speedUp,		//�Ӿ�
	itemType_race_attactFront,	//���� ����
	itemType_race_attactFirst,	//1�� ����
	itemType_adorn = 300, //ġ��
	itemType_adorn_head,
	itemType_max,
	itemType_collection = 1000,	//������
};

//HP lock
static mutex lockHP;

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
typedef vector<_itemPair> itemsVector;

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
	wstring jobTitle;		//Ÿ��Ʋ. �Ӽ� �ջ��� 0 ���ϸ� �̹����� ���߾
	int point;			//����ĳ��
	int hp;				//�Ƿε�
	int exp;			//����ġ
	int level;			//����
	_property property;		//�Ӽ�
	_inventory inventory;	//�κ��丮
	keyBoolMap collection;	//����

	time_t loginTime;	//���۽ð�
	time_t lastUpdateTime; //���� HP ������Ʈ �ð�
};

//������ prefix
struct _jobTitlePrefix {
	int level;
	wstring title;
};

//������ body
struct _jobTitleBody {
	int S;
	int I;
	int A;
	wstring title;
};

//race ��� + �λ�
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
//�湦 ������
struct _raceParticipant : _property {
	int idx;			//���� ��ȣ
	int items[raceItemSlot];
	int currentLength;	//���� �̵� �Ÿ�
	int totalLength;	//��ü �̵� �Ÿ�
	float ratioLength;	//��ü �Ÿ��� �̵��� �Ÿ� ���� %
	int rank;			//���� ���
	int currentRank;	//���� ���

	//���� �ް� �ִ� ������ 
	std::queue<itemType> sufferItems;
	itemType currentSuffer; //���� �ް� �ִ� ������

	//���� �� ������ 
	int shootItem;
	bool operator <(const _raceParticipant &a) const {
		return this->totalLength > a.totalLength;
	}
};
//�湦 ��ȸ ������ ����
typedef vector<_raceParticipant> raceParticipants;
//�湦 ���� ����
struct _raceCurrent
{
	int id;
	int rank;			//����
	int prize;			//���� ���
	int rewardItemId;	//���� ������
	int rewardItemQuantity; //���� ������ ����
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

	//�湦 ��Ÿ ����
	void addRaceMeta(_race & race);

	//Training 
	errorCode isValidTraining(int id);
	errorCode runTraining(int id, itemsVector &rewards, _property * rewardProperty, int &point);

	//Trade
	errorCode runTrade(bool isBuy, int id, int quantity);

	//Race
	errorCode runRace(int id, itemsVector &items);
	//race ����
	raceParticipants* getNextRaceStatus(bool &ret, int itemIdx);
	//race ���
	_raceCurrent* getRaceResult() {
		return &mRaceCurrent;
	}
	//charge
	errorCode runRecharge(int id, int quantity);	

	//auto recharge HP
	//������ �Ǹ� true, �̹� �����̰ų� �ð��� �ƴϸ� false
	bool rechargeHP();
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

	//Actor
	_actor* mActor;

	//������ TradeUpdate�ð�
	time_t mLastTradeUpdate;

	//������
	typedef vector<_jobTitlePrefix> jobTitlePrefixVector;
	typedef vector<_jobTitleBody> jobTitleBodyVector;

	struct jobTitle {
		wstring default;
		jobTitlePrefixVector prefix;
		jobTitleBodyVector body;
	};
	jobTitle mJobTitle;

	//�湦
	raceParticipants* mRaceParticipants;

	typedef map<int, _race> raceMeta;
	raceMeta mRace;

	
	_raceCurrent mRaceCurrent;
	
	//�湦�� �����۸� ���� ��Ƴ� ����
	typedef vector<int> intVector;
	intVector mItemRace;

	//race ���� ������
	int getRandomRaceItem();

	//race ������ �ߵ�
	void invokeRaceItem(int seq, int itemIdx);

	//race AI ������ �ߵ�
	void invokeRaceItemAI();

	//race ������ ������ ����
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

