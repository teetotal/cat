#pragma once
#include "pch.h"
#define TRADE_INVALID_PRICE -1
#define TRADE_PRICE_SUM 150
class trade
{
public:
	trade() : mThread(NULL), mCallback(NULL) {};
	virtual ~trade() {};
	
	struct tradeProduct {
		int itemId;
		int level;
		int price;
	};
	typedef map<int, tradeProduct *> tradeMap;	
	bool mIsRunThread;

	void add(tradeProduct *p) {
		if (p)
			mTrade[p->itemId] = p;
	};
	bool init(float margin, int updateInterval, int weight, tradeUpdatedCallback cb);
	void finalize() {
		mIsRunThread = false;
		if (mThread)
			mThread->join();
	};
	bool update();					//update price
	tradeMap * get() {
		return &mTrade;
	};
	bool exist(int itemId);
	int getPriceBuy(int itemId);	//price at buy	
	int getPriceSell(int itemId);	//price at sell

	//다음 update까지 남은 초
	int getRemainTimeUpdate() {
		return (int)(mLastUpdate + mUpdateInterval - getNow());
	};


private:
	int mlevelWeight; //level에 따른 금액 가중치
	float mTradeMargin; //마진율
	int mUpdateInterval;
	time_t mLastUpdate;
	thread * mThread;
	tradeUpdatedCallback mCallback;
	tradeMap mTrade;

	static void threadRun(trade *p);
};

