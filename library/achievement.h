#pragma once
#include "pch.h"
class achievement
{
public:
	achievement() {};
	virtual ~achievement() {};

	struct detail {
		wstring title;
		int goal;			//목표
		int accumulation;	//현재 누적

		int rewardId;		//보상ID
		int rewardVal;		//보상 값(수량)

		bool isFinished;
	};

	struct done {
		int category;
		int id;
		int value;
	};

	bool mIsRunThread;

	bool init(achievementCallback fn);
	void finalize();

	void addAchieve(bool isDaily, wstring title, int category, int id, int value, int rewardId, int rewardValue, bool isFinished);
	
	void push(int category, int id, int val); //모든 한일을 일단 queue에 넣는다.
	
	void accumulate(); //queue에 쌓인걸 누적한다.
	void calculate(); //정산

	bool getDetail(bool isDaily, int idx, detail &p);
	int getSize(bool isDaily) {
		return (isDaily) ? (int)mDaily.size() : (int)mTotally.size();
	}
private:	
	struct achieve {
		wstring title;
		int category;
		int id;
		int value;

		int rewardId;
		int rewardValue;

		bool isFinished;

		achieve(wstring title, int category, int id, int value, int rewardId, int rewardValue, bool isFinished = false) {
			this->title = title;
			this->category = category;
			this->id = id;
			this->value = value;

			this->rewardId = rewardId;
			this->rewardValue = rewardValue;
			this->isFinished = isFinished;
		}
	};
	typedef vector<achieve*> achieveVector;
	achieveVector mDaily;		//일일
	achieveVector mTotally;		//누적

	queue<done> mQueue;
	map<int, intMap *> mAccumulation;

	thread * mThread;
	mutex mLock;

	achievementCallback mCallback;

	int getAccumulation(int category, int id);
	void calculate(bool isDaily, achieveVector * vec);
	static void threadRun(achievement *);
};

