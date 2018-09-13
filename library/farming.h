#pragma once
#include "pch.h"

class farming
{
public:
	farming() {
		mIsThreadRun = true;
	};
	virtual ~farming() {
	};

	enum farming_status {
		farming_status_sprout = 0, //새싹
		farming_status_week, //돌봄 필요
		farming_status_good,	 //건강
		farming_status_decay,	 //썩음
		farming_status_grown,	//수확할게 있음
		farming_status_harvest,	//다 자람
		farming_status_max
	};

	struct seed { //씨앗
		int id;
		wstring name;	//이름		
		int farmProductId; //수확장물
		int outputMax;	//개당 최대 아웃풋		
		int timeGrow;	//성장에 필요한 시간 (초)		
		int cares;		//필요한 돌봄 회수
		int maxOvertime;	//완료 이후 상하지 않고 버틸 수 있는 시간
	};

	struct crop { //작물		
		int seedId;
		farming_status status;	//상태	
		float percent; //성장한 %
		time_t timePlant;	 //심은 시간
		int cntCare;	//돌본 횟수
		time_t timeLastGrow; //지난 돌봄 시간
		int boost;		// 추가 output
		//
		int level;
		int accumulation;	//현재까지 누적 수확

		crop() {
			init();
		};
		void init() {
			seedId = 0;
			status = farming_status_max;	//상태				
			timePlant = 0;	 //심은 시간
			cntCare = 0;	//돌본 횟수
			timeLastGrow = 0; //지난 돌봄 시간
			boost = 0;		// 추가 output
			percent = 0;
			
			level = 0;
			accumulation = 0;
		}
		void plant(int seedId) {
			this->seedId = seedId;
			level = 1;
			status = farming_status_sprout;			
			timePlant = getNow();
			timeLastGrow = timePlant; //처음 심을때 한번은 돌본 걸로 설정
			cntCare++;
		}

		int getGrownCnt() { //현재 수확 가능한 작물
			return 1;
		}
	};	

	struct field : crop { //밭
		int id;		
		int x;
		int y;	
		time_t finishTime; // 다 자라는 시간
		field() {};
		field(int x, int y) {			
			this->x = x;
			this->y = y;
			finishTime = 0;
		};
		void init() {
			finishTime = 0;
			crop::init();
		};
	};
	//소유중인 밭
	typedef vector<field*> fields;
	bool mIsThreadRun;
	/*
	functions
	*/
	bool init(farmingFinshedNotiCallback fn);
	void finalize();

	//추가	
	void swap(field * f1, field * f2) {
		crop temp;
		::memcpy(&temp, (crop*)f1, sizeof(temp));		
		::memcpy((crop*)f1, (crop*)f2, sizeof(crop));		
		::memcpy((crop*)f2, &temp, sizeof(temp));
	};
	void levelup(int id) {
		mLock.lock();
		for (int n = 0; n < mFields.size(); n++) {
			if (mFields[n]->id == id) {
				mFields[n]->level++;
				break;
			}
		}		
		mLock.unlock();
	};
	void clear(int id) {
		mLock.lock();
		for (int n = 0; n < mFields.size(); n++) {
			if (mFields[n]->id == id) {
				mFields[n]->init();
				break;
			}
		}
		mLock.unlock();		
	};
	void addField(field * f) {
		mFields.push_back(f);
	};
	//
	field * addField(int x, int y);	//밭 늘리기	
	void addField(int id
		, int x
		, int y
		, int seedId
		, farming_status status
		, time_t timePlant
		, int cntCare
		, time_t timeLastGrow
		, int boost
		, int level
		, int accumulation
	);
	fields* getFields() {			//밭 목록 
		return &mFields;
	};			
	bool getField(int idx, field *&pField) {
		bool p = false;
		mLock.lock();
		if (idx < mFields.size()) {						
			//::memcpy(&pField, mFields[idx], sizeof(field));
			pField = mFields[idx];
			p = true;
		}
		mLock.unlock();
		return p;
	}
	int countField() {
		return (int)mFields.size();
	};
	void setStatus();				//농작물 상태 설정	
	void setStatus(int fieldIdx);	
	bool harvest(int fieldIdx, int &farmProductId, int &output); //수확
	bool plant(int fieldIdx, int seedId);	//심기		
	bool care(int fieldIdx, int boost = 0); //가꾸기
	void addSeed(seed *s) {	//씨앗 등록
        if(s)
            mSeed[s->id] = s;
	};
	seed * getSeed(int id) {
		if (mSeed.find(id) != mSeed.end()) {
			return mSeed[id];
		}
		return NULL;
	};

private:	
	fields mFields;	
	map<int, seed*> mSeed;
	thread * mThread;
	mutex mLock;
	farmingFinshedNotiCallback mNoticeFn;

	static void threadRun(farming * inst) {
		while (inst->mIsThreadRun)
		{
			sleepThisThread(500);
			inst->setStatus();
		}		
	}
};

