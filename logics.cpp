#include "stdafx.h"
#include "logics.h"

bool logics::init() {
	srand((int)time(0));

	if (!setTradeMarketPrice())
		return false;

	return true;
}

void logics::print(int type) {
	if (type == 0) {
		wstring szGrowth, szHP, szRace, szAdorn;
		for (keyQuantity::iterator it = mActor->inventory.growth.begin(); it != mActor->inventory.growth.end(); ++it) {
			szGrowth += to_wstring(it->first) + L"-" + mItems[it->first].name + L"(" + to_wstring(it->second) + L"), ";
		}
		for (keyQuantity::iterator it = mActor->inventory.hp.begin(); it != mActor->inventory.hp.end(); ++it) {
			szHP += to_wstring(it->first) + L"-" + mItems[it->first].name + L"(" + to_wstring(it->second) + L"), ";
		}
		for (keyQuantity::iterator it = mActor->inventory.race.begin(); it != mActor->inventory.race.end(); ++it) {
			szRace += to_wstring(it->first) + L"-" + mItems[it->first].name + L"(" + to_wstring(it->second) + L"), ";
		}
		for (keyQuantity::iterator it = mActor->inventory.adorn.begin(); it != mActor->inventory.adorn.end(); ++it) {
			szAdorn += to_wstring(it->first) + L"-" + mItems[it->first].name + L"(" + to_wstring(it->second) + L"), ";
		}
		printf("[Actor]\n ------------------------------------------------------------------------ \n");
		wprintf(L" Name: %s(%s) lv.%d(exp.%d / %d) hp: (%d / %d)\n Point:%d \n S: %d, I: %d, A: %d \n GROWTH\t %s \n HP\t %s \n RACE\t %s \n ADORN\t %s \n"
			, mActor->name.c_str()
			, mActor->userName.c_str()
			, mActor->level
			, mActor->exp
			, getMaxExp()
			, mActor->hp
			, getMaxHP()
			, mActor->point
			, mActor->property.strength
			, mActor->property.intelligence
			, mActor->property.appeal
			, szGrowth.c_str()
			, szHP.c_str()
			, szRace.c_str()
			, szAdorn.c_str()
		);
	}else if (type == 1) {
		printf("[Training]\n ------------------------------------------------------------------------ \n");

		for (__training::iterator it = mTraining.begin(); it != mTraining.end(); ++it) {
			if (!isValidTraningTime(it->first))
				continue;

			wstring rewardItems;
			wstring costItems;
			
			for (int n = 0; n < maxTrainingItems; n++) {
				_itemPair* p = it->second.reward.items[n];
				if (p != NULL) {
					rewardItems += mItems[p->itemId].name + L"(";
					rewardItems += to_wstring(p->val) + L") ";
				}
				p = it->second.cost.items[n];
				if (p != NULL) {
					costItems += mItems[p->itemId].name + L"(";
					costItems += to_wstring(p->val) + L") ";
				}
			}			
			wprintf(L" ID: %d,\t %s ($%d) [%s] S(%d) I(%d) A(%d) \n \t Reward $%d  [%s] S(%d) I(%d) A(%d) \n\n"
				, it->first
				, it->second.name.c_str()
				, it->second.cost.point
				, costItems.c_str()
				, it->second.cost.strength
				, it->second.cost.intelligence
				, it->second.cost.appeal
				
				, it->second.reward.point
				, rewardItems.c_str()
				, it->second.reward.strength
				, it->second.reward.intelligence
				, it->second.reward.appeal
				
			);
		}
	}else if (type == 2) {
		printf("[Trade]\n ------------------------------------------------------------------------ \n");
		for (__keyValInt::iterator it = mTrade.begin(); it != mTrade.end(); ++it) {
			if (mItems[it->first].type < itemType_max) {
				wprintf(L" ID: %03d, Price: %d(%d),\t %s \n"
					, it->first
					, getItemPriceBuy(it->first)
					, getItemPriceSell(it->first)
					, mItems[it->first].name.c_str()
				);
			}
		}
	}
	else if (type == 3) // 상품보기
	{
		for (__items::iterator it = mItems.begin(); it != mItems.end(); ++it) {
			wprintf(L"[Items] ID: %d,\t Type: %d,\t %s \n", it->first, it->second.type, it->second.name.c_str());
		}
	}
	else if (type == 4) { //도감 보기
		for (__items::iterator it = mItems.find(collectionStartId); it != mItems.end(); ++it) {
			if (it->second.type == itemType_collection) {
				bool has = false;
				if (mActor->collection.find(it->first) != mActor->collection.end())
					has = true;
				wprintf(L" [%s] %s(%d) lv.%d \n"
					, has ? L"O" : L" "
					, it->second.name.c_str()
					, it->first
					, it->second.grade
				);
			}	
		}
	}
}

bool logics::insertItem(_item item) {
	mItems[item.id] = item;
	return true;
}

bool logics::insertTraining(_training traning) {
		mTraining[traning.id] = traning;
	return true;
}

bool logics::setTradeMarketPrice() {
	unsigned int avg = (unsigned int)(maxTradeValue / mItems.size());
	mTrade.clear();
	for (__items::iterator it = mItems.begin(); it != mItems.end(); ++it) {
		mTrade[it->first] = avg;		
	}

	int sum = 0;
	size_t cnt = 0;
	for (__keyValInt::iterator it = mTrade.begin(); it != mTrade.end(); ++it) {
		int id = it->first;
		int r = (rand() % it->second);
		int val = r - (it->second / 2);

		it->second += val;
		sum += it->second;
		
		int gap = val / int(mTrade.size() - cnt);
		for (__keyValInt::iterator it2 = next(it); it2 != mTrade.end(); ++it2) {
			it2->second -= gap;
		}
		cnt++;
	}

	printf("sum = %d \n", sum);
	return true;
}

bool logics::setActor(_actor* actor) {
	mActor = actor;
	return true;
}


errorCode logics::isValidTraining(int id) {
	//check validation
	if (mTraining.find(id) == mTraining.end() || isValidTraningTime(id) == false) {
		return error_invalid_id;
	}
	else if (mActor->hp <= 0) {
		return error_not_enough_hp;
	}
	else if (mTraining[id].cost.point > mActor->point)
		return error_not_enough_point;
	/*
	else if (mTraining[id].cost.strength > mActor->property.strength ||
		mTraining[id].cost.intelligence > mActor->property.intelligence ||
		mTraining[id].cost.appeal > mActor->property.appeal) 
	{
		return error_not_enought_property;
	}
	*/
	//check item validation
	for (int n = 0; n < maxTrainingItems; n++) {
		_itemPair * p = mTraining[id].cost.items[n];
		if (p == NULL)
			break;
		if (mActor->inventory.growth.find(p->itemId) == mActor->inventory.growth.end()
			|| mActor->inventory.growth[p->itemId] < p->val)
			return error_not_enough_item;
	}

	return error_success;
}

errorCode logics::runTraining(int id, vector<_itemPair> &rewards, _property * rewardProperty, int &point) {
	//hp
	mActor->hp--;
	//pay point
	mActor->point -= mTraining[id].cost.point;
	//substract property
	addProperty(mTraining[id].cost.strength * -1, mTraining[id].cost.intelligence * -1, mTraining[id].cost.appeal * -1);
	
	//substract item
	for (int n = 0; n < maxTrainingItems; n++) {
		_itemPair * p = mTraining[id].cost.items[n];
		if (p == NULL)
			break;
		if(!addInventory(p->itemId, p->val * -1))
			return error_not_enough_item;		
	}

	//give point
	point = getRandValue(mTraining[id].reward.point);
	mActor->point += point;
	//give growth
	rewardProperty->strength = getRandValue(mTraining[id].reward.strength);
	rewardProperty->intelligence = getRandValue(mTraining[id].reward.intelligence);
	rewardProperty->appeal = getRandValue(mTraining[id].reward.appeal);
	addProperty(rewardProperty->strength
		, rewardProperty->intelligence
		, rewardProperty->appeal);

	//give only 1 item	
	int cnt = 0;
	for (int n = 0; n < maxTrainingItems; n++) {
		_itemPair * p = mTraining[id].reward.items[n];
		if (p == NULL) {
			cnt = n;
			break;
		}		
	}
	if(cnt > 0){
		int idx = (rand() % cnt);
		_itemPair * p = mTraining[id].reward.items[idx];
		_itemPair item;
		item.itemId = p->itemId;
		item.val = (rand() % p->val);
		if (item.val > 0) {			
			if (!addInventory(item.itemId, item.val))
				return error_not_enough_item; //어차피 추가라 이부분은 필요 없지만 걍 넣어둠

			rewards.push_back(item);
		}
	}
	if (increaseExp())
		return error_levelup;
	return error_success;
}

errorCode logics::runTrade(bool isBuy, int id, int quantity) {
	if (mTrade.find(id) == mTrade.end()) {
		return error_invalid_id;
	}
	if (mItems[id].type >= itemType_max) {
		return error_invalid_id;
	}
	int amount;
	if (isBuy) {
		amount = getItemPriceBuy(id) * quantity;
		if (mActor->point < amount) 
			return error_not_enough_point;		
		//substract point
		amount = amount * -1;
	}		
	else {
		amount = getItemPriceSell(id) * quantity;				
		quantity = quantity * -1;
	}	
	//give items
	_item item = mItems[id];
	bool ret = false;
	/*
	if (item.type > itemType_training && item.type < itemType_hp) {
		t = inventoryType_growth;
	} else if (item.type > itemType_hp && item.type < itemType_race) {
		t = inventoryType_HP;
	} else if (item.type > itemType_race && item.type < itemType_adorn) {
		t = inventoryType_race;
	} else if (item.type > itemType_adorn && item.type < itemType_max) {
		t = inventoryType_adorn;
	}
	*/
	if (!addInventory(item.id, quantity))
		return error_not_enough_item;

	mActor->point += amount;
	if (increaseExp())
		return error_levelup;
	return error_success;
}

void logics::recharge(int val) {
	int maxHP = getMaxHP();
	if (mActor->hp + val >= maxHP)
		mActor->hp = maxHP;
	else
		mActor->hp += val;
}

errorCode logics::runRecharge(int id, int quantity) {
	int val = mItems[id].value;
	if (!addInventory(id, quantity * -1)) {
		return error_not_enough_item;
	}

	recharge(val);
	return error_success;
}

//add inventory
bool logics::addInventory(int itemId, int quantity) {
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
	inventoryType type = getInventoryType(itemId);
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
	case inventoryType_collection:
		mActor->collection[itemId] = true;
		break;
	default:
		return false;
	}
	return true;
}
//increase property
void logics::addProperty(int strength, int intelligence, int appeal) {
	mActor->property.strength += strength;
	mActor->property.intelligence += intelligence;
	mActor->property.appeal += appeal;
}

int logics::getRandValue(int max) {
	if (max == 0)
		return 0;
	return rand() % max;
}
//경험치 증가
bool logics::increaseExp() {
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
int logics::getMaxExp() {
	return 1 << mActor->level;
}
int logics::getMaxHP() {
	return (int)(defaultHP + (1.5*mActor->level));
}

bool logics::isValidTraningTime(int id) {

	_training* t = &mTraining[id];
	if (t->start == 0)
		return true;

	time_t now = time(0);
	const int c = (t->keep + t->interval);
	time_t end = t->start + (t->count * c);
	if (now < t->start || end < now)
		return false;

	for (int x = 0; x < t->count; x++) {
		time_t s = t->start + (x * c);
		time_t e = s + t->keep;

		if (s <= now && now < e)
			return true;
	}
	
	return false;
}