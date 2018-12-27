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

#define FARM_QUEST_ITEM_MAX 3 //quest당 최대 생산물
#define FARM_LEVEL_PER_HARVEST 32 //FARM_LEVEL_PER_HARVEST 마다 level up
//#define FARM_HARVEST_QUANTITY_PER_EXP 8
	//----------------------------------------------------Field
	enum farming_status {
		farming_status_sprout = 0, //새싹
		farming_status_week, //돌봄 필요
		farming_status_good,	 //건강
		farming_status_decay,	 //썩음
		farming_status_grown,	//수확할게 있음
		farming_status_harvest,	//다 자람
        farming_status_deco,    //장식용
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
        bool isDeco;    //장식
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
        void copy(crop * src) {
            seedId = src->seedId;
            status = src->status;
            timePlant = src->timePlant;
            cntCare = src->cntCare;
            timeLastGrow = src->timeLastGrow;
            boost = src->boost;
            percent = src->percent;
            level = src->level;
            accumulation = src->accumulation;
        }
		void plant(int seedId) {
			this->seedId = seedId;
			level = 1;
			status = farming_status_sprout;			
			timePlant = getNow();
			timeLastGrow = timePlant; //처음 심을때 한번은 돌본 걸로 설정
			cntCare++;
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

		int getGrownCnt(int timeGrow) { //현재 수확 가능한 작물. seed정보를 가져올 수 없어 파라미터로 받는다
			int maxGrown = 1 << (this->level - 1);;
			int currentGrown = (int)((getNow() - this->timePlant) / timeGrow);
			int nGrown = min(maxGrown, currentGrown);
			if(nGrown < maxGrown)
				return nGrown - accumulation;
			return nGrown - accumulation + this->level - 1;
		};
	};
	
	typedef vector<field*> fields;
	typedef map<int, seed*> seeds;
	//-----------------------------------------------------Quest
	struct questItem {
		int itemId;
		int quantity;
	};
	struct quest {
		questItem items[FARM_QUEST_ITEM_MAX];
		time_t timeStamp;

		quest() {
			items[0].itemId = -1;
			items[1].itemId = -1;
			items[2].itemId = -1;
		};
	};
	typedef vector<quest> questVector;

	questVector * createQuest(int cnt) {
		if (cnt > mQuestVector.size() && mSeedProducts.size() > 0) {
			// make quest
			makeQuest(cnt);
		}			
		return &mQuestVector;
	};

	void clearQuest(int idx) {
		if (mQuestVector.size() <= idx)
			return;

		mQuestVector.erase(mQuestVector.begin() + idx);		
	};
	
	farming::quest * getQuest(int idx) {
		if (mQuestVector.size() <= idx)
			return NULL;

		return &mQuestVector[idx];
	};

	//------------------------------------------------------functions	
	bool init(farmingFinshedNotiCallback fn, int cntHarvest, bool isThreadRun = true);
	void finalize();

	//추가	
	void swap(field * f1, field * f2) {
		crop temp;
        temp.copy((crop*)f1);
        f1->copy((crop*)f2);
        f2->copy(&temp);
        
//        ::memcpy(&temp, (crop*)f1, sizeof(crop));
//        ::memcpy((crop*)f1, (crop*)f2, sizeof(crop));        
//        ::memcpy((crop*)f2, &temp, sizeof(crop));
	};
    int getIdx(int id) {
        int ret = -1;
        mLock.lock();
        for (int n = 0; n < mFields.size(); n++) {
            if (mFields[n]->id == id) {
                ret = n;
                break;
            }
        }
        mLock.unlock();
        return ret;
    };
	void levelup(int id) {
        int idx = getIdx(id);
        if(idx == -1)
            return;
		mLock.lock();
        /*
		for (int n = 0; n < mFields.size(); n++) {
			if (mFields[n]->id == id) {
				mFields[n]->level++;
				break;
			}
		}
         */
        mFields[idx]->level++;
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
	bool harvest(int fieldIdx, int &farmProductId, int &output); //수확
	bool plant(int fieldIdx, int seedId);	//심기		
	bool care(int fieldIdx, int boost = 0); //가꾸기
	void addSeed(seed *s) {	//씨앗 등록
		if (s) {
			mSeed[s->id] = s;
			mSeedProducts.push_back(s->farmProductId);			
		}
            
	};
	seed * getSeed(int id) {
		if (mSeed.find(id) != mSeed.end()) {
			return mSeed[id];
		}
		return NULL;
	};

	seeds * getSeeds() {
		return &mSeed;
	};

private:	
	fields mFields;	
	seeds mSeed;
	thread * mThread;
	mutex mLock;
	farmingFinshedNotiCallback mNoticeFn;
	questVector mQuestVector;
	int mCntHarvest; //수확량 카운팅. 수확 횟수 아님
	vector<int> mSeedProducts; // seed 수확물에 대한 
	

	bool mIsThreadRun;

	void setStatus(int fieldIdx);
	void makeQuest(int cnt);
	static void threadRun(farming * inst) {
		while (inst->mIsThreadRun)
		{
			sleepThisThread(500);
			inst->setStatus();
		}		
	}
};

