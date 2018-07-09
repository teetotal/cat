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
		wstring szGrowth, szRace, szAdorn;
		for (keyQuantity::iterator it = mActor->inventory.growth.begin(); it != mActor->inventory.growth.end(); ++it) {
			szGrowth += mItems[it->first].name + L"(" + to_wstring(it->second) + L"), ";
		}
		for (keyQuantity::iterator it = mActor->inventory.race.begin(); it != mActor->inventory.race.end(); ++it) {
			szRace += mItems[it->first].name + L"(" + to_wstring(it->second) + L"), ";
		}
		for (keyQuantity::iterator it = mActor->inventory.adorn.begin(); it != mActor->inventory.adorn.end(); ++it) {
			szAdorn += mItems[it->first].name + L"(" + to_wstring(it->second) + L"), ";
		}
		printf("[Actor]\n ------------------------------------------------------------------------ \n");
		wprintf(L" Name: %s(%s), Point:%d,\n strength: %d, intelligence: %d, appeal: %d,\n GROWTH\t %s \n RACE\t %s \n ADORN\t %s \n"
			, mActor->name.c_str()
			, mActor->userName.c_str()
			, mActor->point
			, mActor->property.strength
			, mActor->property.intelligence
			, mActor->property.appeal
			, szGrowth.c_str()
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
			wprintf(L" ID: %d,\t %s \n \t Reward Point: %d \t [S: %d, I: %d, A: %d, %s ] \n \t Cost Point: %d \t [S: %d, I: %d, A: %d, %s] \n"
				, it->first
				, it->second.name.c_str()
				, it->second.reward.point
				, it->second.reward.strength
				, it->second.reward.intelligence
				, it->second.reward.appeal
				, rewardItems.c_str()
				, it->second.cost.point
				, it->second.cost.strength
				, it->second.cost.intelligence
				, it->second.cost.appeal
				, costItems.c_str()
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
	} else if (mTraining[id].cost.point > mActor->point)  
		return error_not_enought_point;
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
			return error_not_enought_item;
	}

	return error_success;
}

errorCode logics::runTraining(int id, vector<_itemPair> &rewards ) {
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
			return error_not_enought_item;		
	}

	//give point
	mActor->point += mTraining[id].reward.point;
	//give growth
	addProperty(mTraining[id].reward.strength, mTraining[id].reward.intelligence, mTraining[id].reward.appeal);
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
				return error_not_enought_item; //어차피 추가라 이부분은 필요 없지만 걍 넣어둠

			rewards.push_back(item);
		}
	}

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
			return error_not_enought_point;		
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
	switch (item.type) {
	case itemType_strength: 
	case itemType_intelligence:
	case itemType_appeal:
		ret = addInventory(inventoryType_growth, item.id, quantity);
		break;
	case itemType_race_offense:
		ret = addInventory(inventoryType_race, item.id, quantity);
		break;
	case itemType_adorn_head:
		ret = addInventory(inventoryType_adorn, item.id, quantity);
		break;
	}
	
	if (ret)
		mActor->point += amount;
	else
		return error_not_enought_item;

	return error_success;
}