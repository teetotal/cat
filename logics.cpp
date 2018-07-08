#include "stdafx.h"
#include "logics.h"

bool logics::init() {
	srand((int)time(0));

	if (!setTradeMarketPrice())
		return false;

	return true;
}

void logics::print() {
	for (__items::iterator it = mItems.begin(); it != mItems.end(); ++it) {
		std::cout << "item => " << it->first << ":\t" << it->second.name << endl;
	}

	for (__training::iterator it = mTraining.begin(); it != mTraining.end(); ++it) {
		std::cout << "training => " << it->first << ":\t" << it->second.name << endl;
	}

	std::cout << "Actor => " << mActor->name << "," << mActor->userName << ", Point: " << mActor->point << endl
		<< mActor->property.strength << ", " << mActor->property.intelligence << "," << mActor->property.appeal
		<< endl;
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
		int price = mTrade[n].val + (mItems[id].grade * 10);
		int fee = price * 0.1;
		cout << "trade: \t" << mTrade[n].itemId << "-" << mItems[id].name << " = " << price << "(" << price - fee << ")" << endl;
		int gap = val / int(mTrade.size() - n);
		for (int j = n + 1; j < mTrade.size(); j++) {
			mTrade[j].val -= gap;
		}
	}

	printf("sum = %d \n", sum);
	return true;
}

bool logics::setActor(_actor* actor) {
	mActor = actor;
	return true;
}
