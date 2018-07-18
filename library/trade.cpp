#include "trade.h"

bool trade::init(float margin, int updateInterval, int weight, tradeUpdatedCallback cb) {
	mTradeMargin = margin;
	mUpdateInterval = updateInterval;
	mlevelWeight = weight;
	mIsRunThread = true;
	mThread = new thread(threadRun, this);
	mLastUpdate = 0;
	mCallback = cb;
	return true;
}

bool trade::exist(int itemId) {
	if (mTrade.find(itemId) != mTrade.end())
		return true;

	return false;
}

int trade::getPriceBuy(int itemId) {
	if (mTrade.find(itemId) != mTrade.end())
		return mTrade[itemId]->price + (mTrade[itemId]->level * mlevelWeight);
	else
		return TRADE_INVALID_PRICE;
}

int trade::getPriceSell(int itemId) {
	int price = getPriceBuy(itemId);
	if (price == TRADE_INVALID_PRICE)
		return TRADE_INVALID_PRICE;
	else {
		return (int)(price * (1.0f - mTradeMargin));
	}
}

bool trade::update() {
	time_t now = getNow();
	
	if (mLastUpdate > 0) {
		if (mLastUpdate + mUpdateInterval >= now || mTrade.size() <= 0)
			return false;
	}
	
	mLastUpdate = now;

	unsigned int avg = (unsigned int)(TRADE_PRICE_SUM / mTrade.size());
	
	for (tradeMap::iterator it = mTrade.begin(); it != mTrade.end(); ++it) {
		it->second->price = avg;
	}

	int sum = 0;
	size_t cnt = 0;
	for (tradeMap::iterator it = mTrade.begin(); it != mTrade.end(); ++it) {
		//int id = it->first;
		int r = getRandValue(it->second->price);
		int val = rounding((float)r - ((float)it->second->price / 2.f));

		it->second->price += val;
		sum += it->second->price;

		int gap = val / int(mTrade.size() - cnt);
		for (tradeMap::iterator it2 = next(it); it2 != mTrade.end(); ++it2) {
			it2->second->price -= gap;
		}
		cnt++;
	}
	printf("sum = %d \n", sum);

	if (mCallback)
		mCallback(mLastUpdate);

	return true;
}

void trade::threadRun(trade * p) {
	while (p->mIsRunThread)
	{
		p->update();
		sleepThisThread(SEC);
	}
}