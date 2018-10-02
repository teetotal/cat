/*
	Quest Class
*/
#pragma once
#include "pch.h"

class Quest
{
public:
	Quest() {};
	virtual ~Quest() {};
	
	struct _quest {
		int uniqueId;
		wstring title;
		int category;
		int id;
		int value;

		int rewardId;
		int rewardValue;

		bool isFinished;
		bool isReceived;

		int accumulation; // 누적 합산

		_quest(int uniqueId, wstring title, int category, int id, int value, int rewardId, int rewardValue, bool isFinished = false, bool isReceived = false) {
			this->uniqueId = uniqueId;
			this->title = title;
			this->category = category;
			this->id = id;
			this->value = value;

			this->rewardId = rewardId;
			this->rewardValue = rewardValue;
			this->isFinished = isFinished;
			this->isReceived = isReceived;
			this->accumulation = 0;
		}
	};
	struct done {
		int category;
		int id;
		int value;
	};

	typedef map<int, intMap *> intDoubleDepthMap;
	typedef vector<_quest*> QuestVector;

	bool mIsRunThread;
	time_t mLastUpdated;

	bool init(achievementCallback fn, int cnt /* 한번에 표시되는 quest 수 */);
	void finalize() {
		mIsRunThread = false;
		mThread->join();
	};

	QuestVector * getQuests() {
		return &mQuests;
	};
	_quest * getQuest(int uniqueId) {
		for (int n = 0; n < mQuests.size(); n++) {
			if (mQuests[n]->uniqueId == uniqueId)
				return mQuests[n];
		}
		return nullptr;
	};

	void addQuest(int uniqueId
		, wstring title
		, int category
		, int id
		, int value
		, int rewardId
		, int rewardValue
	) {
		_quest * p = new _quest(uniqueId, title, category, id, value, rewardId, rewardValue);
		addQuest(p);
	};

	void addQuest(_quest * p) {
		mQuests.push_back(p);
	};
	//초기화
	void setQuest(int uniqueId, int accumulation, bool isFinished, bool isReceived);
	//누적
	void setAccumulation(int category, int id, int accumulation);
	
	//모든 한일을 일단 queue에 넣는다.
	void push(int category, int id, int val); 

	//queue에 쌓인걸 누적한다.
	void accumulate(); 
	
	//상품 수령
	bool rewardReceive(int uniqueId);
	bool rewardReceive(_quest * p);
	
	//여태까지 진행한 정보 조회
	int getAccumulation(int category, int id) {
		if (mAccumulation.find(category) == mAccumulation.end()) {
			return 0;
		}

		if (mAccumulation[category]->find(id) == mAccumulation[category]->end())
			return 0;

		return mAccumulation[category]->at(id);
	};
	intDoubleDepthMap * getAccumulation() {
		return &mAccumulation;
	};

private:	
	int mCnt;	
	QuestVector mQuests;
	
	queue<done> mQueue;
	intDoubleDepthMap mAccumulation;

	thread * mThread;
	mutex mLock;
	achievementCallback mCallback;
		
	void calculate(done * pDone);
	
	static void threadRun(Quest * p) {
		while (p->mIsRunThread)
		{
			sleepThisThread(SEC);
			p->accumulate();
		}
	};	
};

