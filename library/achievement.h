#pragma once
#include "pch.h"

#define ACHIEVEMENT_TYPE_DAILY	0
#define ACHIEVEMENT_TYPE_TOALLY	1
/*
	레벨별 퀘스트와
	업적을 모두 처리할 수 있게 되어있다.
	기본적으로 퀘스트 기준이지만 업적 전용 함수들이 있다.
*/
class achievement
{
public:
	achievement() {};
	virtual ~achievement() {};

	typedef map<int, intMap *> intDoubleDepthMap;

	struct detail {
		wstring title;
		int category;
		int id;

		int goal;			//목표
		int accumulation;	//현재 누적

		int rewardId;		//보상ID
		int rewardVal;		//보상 값(수량)

		bool isFinished;
		bool isReceived;
	};

	struct done {
		int category;
		int id;
		int value;
	};

	bool mIsRunThread;

	bool init(achievementCallback fn, time_t lastLogin, bool isLevelMode = false, int level = 0);
	void setLevel(int level);
	void addAchieve(int type
		, wstring title
		, int category
		, int id
		, int value
		, int rewardId
		, int rewardValue
		);
	//업적 전용.  업적에 대한 누적  초기화 관련
	void setAchievementAccumulation(int type, int category, int id, int accumulation, bool isFinished, bool isReceived);
	//누적
	void setAccumulation(int category, int id, int accumulation);

	void finalize();

	//모든 한일을 일단 queue에 넣는다.
	void push(int category, int id, int val); 

	//queue에 쌓인걸 누적한다.
	void accumulate(); 
	
	//업적 전용.
	bool getDetail(bool isDaily, int idx, detail &p) {
		return getDetail(p, isDaily ? ACHIEVEMENT_TYPE_DAILY : ACHIEVEMENT_TYPE_TOALLY, idx);
	};
	bool getDetail(detail &p, int type, int idx);

	int getSize(int type) {
		if(mMap.find(type) != mMap.end())
			return (int)mMap[type].size();
		return 0;
	}
	//상품 수령
	bool rewardReceive(int type, int idx);

	int getAccumulation(int category, int id) {
		if (hasAccumulation(category, id)) {
			return mAccumulation[category]->at(id);
		}
		return 0;
	};

	intDoubleDepthMap * getAccumulation() {
		return &mAccumulation;
	};
private:	
	struct achieve {
		wstring title;
		int category;
		int id;
		int value;

		int rewardId;
		int rewardValue;

		bool isFinished;
		bool isReceived;

		int accumulation; // 누적 합산

		achieve(wstring title, int category, int id, int value, int rewardId, int rewardValue, bool isFinished = false, bool isReceived = false) {
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
	typedef vector<achieve*> achieveVector;
	typedef map<int, achieveVector> achieveMap;
	achieveMap mMap;
	/*
	achieveVector mDaily;		//일일
	achieveVector mTotally;		//누적
	*/
	queue<done> mQueue;
	intDoubleDepthMap mAccumulation;

	thread * mThread;
	mutex mLock;
	time_t mLastLogin;
	achievementCallback mCallback;
	bool mIsLevelMode;
	int mLevel;
		
	void calculate(done * pDone);
	//업적 전용
	void resetDaily();
	static void threadRun(achievement *);
	//퀘스트 업데이트
	void calculateSet(int type, achieveVector * vec, done * pDone);

	bool hasAccumulation(int category, int id);
};

