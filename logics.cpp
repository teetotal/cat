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
		for (int n = 0; n < mTrade.size(); n++) {			
			wprintf(L" ID: %03d, Price: %d(%d),\t %s \n", n, getItemPriceBuy(n), getItemPriceSell(n), mItems[mTrade[n].itemId].name.c_str());
		}
	}

	/*
	for (__items::iterator it = mItems.begin(); it != mItems.end(); ++it) {
		wprintf( L"[Items] ID: %d,\t Type: %d,\t %s \n", it->first, it->second.type, it->second.name.c_str());
	}
	*/

	
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
	int avg = maxTradeValue / mItems.size();
	mTrade.clear();
	for (__items::iterator it = mItems.begin(); it != mItems.end(); ++it) {
		_itemPair p;
		p.itemId = it->second.id;
		p.val = avg;
		mTrade.push_back(p);
	}

	int sum = 0;
	for (int n = 0; n < mTrade.size(); n++) {
		int id = mTrade[n].itemId;
		int r = (rand() % mTrade[n].val);
		int val = r - (mTrade[n].val / 2);

		mTrade[n].val += val;
		sum += mTrade[n].val;
		
		int gap = val / int(mTrade.size() - n);
		for (int j = n + 1; j < mTrade.size(); j++) {
			mTrade[j].val -= gap;
		}
	}

	//printf("sum = %d \n", sum);
	return true;
}

bool logics::setActor(_actor* actor) {
	mActor = actor;
	return true;
}


errorCode logics::isValidTraining(int id) {
	//check validation
	if (mTraining.find(id) == mTraining.end()) {
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
		if(!addInventory(inventoryType_growth, p->itemId, p->val * -1))
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
	//give item
	for (int n = 0; n < maxTrainingItems; n++) {
		_itemPair * p = mTraining[id].reward.items[n];
		if (p == NULL)
			break;

		_itemPair item;
		item.itemId = p->itemId;
		item.val = (rand() % p->val);
		if (item.val > 0) {			
			if (!addInventory(inventoryType_growth, item.itemId, item.val))
				return error_not_enough_item; //어차피 추가라 이부분은 필요 없지만 걍 넣어둠

			rewards.push_back(item);
		}
	}
	if (increaseExp())
		return error_levelup;
	return error_success;
}

errorCode logics::runTrade(bool isBuy, int id, int quantity) {
	if (mTrade.size() -1 < id) {
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
	_item item = mItems[mTrade[id].itemId];
	bool ret = false;
	inventoryType t;
	if (item.type > itemType_training && item.type < itemType_hp) {
		t = inventoryType_growth;
	} else if (item.type > itemType_hp && item.type < itemType_race) {
		t = inventoryType_HP;
	} else if (item.type > itemType_race && item.type < itemType_adorn) {
		t = inventoryType_race;
	} else if (item.type > itemType_adorn && item.type < itemType_max) {
		t = inventoryType_adorn;
	}

	if (!addInventory(t, item.id, quantity))
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
	if (!addInventory(inventoryType_HP, id, quantity * -1)) {
		return error_not_enough_item;
	}

	recharge(val);
	return error_success;
}