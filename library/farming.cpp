#include "farming.h"

bool farming::init(farmingFinshedNotiCallback fn, int cntHarvest, bool isThreadRun) {
	mCntHarvest = cntHarvest;
	mNoticeFn = fn;

	if(isThreadRun)
		mThread = new thread(threadRun, this);
	return true;
}
void farming::finalize() {
	mIsThreadRun = false;
	mThread->join();	
	delete mThread;
};
farming::field * farming::addField(int x, int y) {
	field* f = new field(x, y);
	f->id = (int)mFields.size();
	mFields.push_back(f);

	return f;
}

void farming::addField(int id, int x, int y, int seedId, farming_status status, time_t timePlant, int cntCare, time_t timeLastGrow, int boost, int level, int accumulation)
{
	mFields.resize(id + 1);
	field* f = new field(x, y);
	f->id = id;
	f->seedId = seedId;
	f->status = status;
	f->timePlant = timePlant;
	f->cntCare = cntCare;
	f->timeLastGrow = timeLastGrow;
	f->boost = boost;

	f->level = level;
	f->accumulation = accumulation;
	mFields[id] = f;
}

//작물 상태 확인
void farming::setStatus() {
	for (size_t n = 0; n < mFields.size(); n++)
		setStatus((int)n);
}
void farming::setStatus(int fieldIdx) {
	mLock.lock();
	time_t now = getNow();
	if (mFields[fieldIdx] == NULL) {
		mLock.unlock();
		return;
	}
	int seedId = mFields[fieldIdx]->seedId;
    if(seedId == 0){
        mLock.unlock();
        return;
    }

	if (mSeed.find(seedId) == mSeed.end()) {
		mLock.unlock();
		return;
	}

	seed * s = mSeed[seedId];

	int maxGrown = 1 << (mFields[fieldIdx]->level - 1);
	int currentGrown = (int)((now - mFields[fieldIdx]->timePlant) / s->timeGrow);

	int nGrown = min(maxGrown, currentGrown);
	mFields[fieldIdx]->finishTime = mFields[fieldIdx]->timePlant + (s->timeGrow * maxGrown);
	time_t finishedTime = mFields[fieldIdx]->finishTime;
	mFields[fieldIdx]->percent = (float)currentGrown / (float)maxGrown * 100.0f;

    if(s->isDeco) {
        if(mFields[fieldIdx]->status != farming_status_deco)
            mFields[fieldIdx]->status = farming_status_deco;
    }
	else if (finishedTime + s->maxOvertime < now) {
		if(mFields[fieldIdx]->status != farming_status_decay)
			mFields[fieldIdx]->status = farming_status_decay;
	}	
	else if (finishedTime <= now) {
		if (mFields[fieldIdx]->status != farming_status_harvest) {
			mFields[fieldIdx]->status = farming_status_harvest;
			mNoticeFn(fieldIdx);
		}
	}
	else if (nGrown > mFields[fieldIdx]->accumulation) {
		if (mFields[fieldIdx]->status != farming_status_grown) {
			mFields[fieldIdx]->status = farming_status_grown;			
		}
	}	
	else if (s->cares - mFields[fieldIdx]->cntCare > 0) {
		if(mFields[fieldIdx]->status != farming_status_week)
			mFields[fieldIdx]->status = farming_status_week;
	}		
	else {
		if(mFields[fieldIdx]->status != farming_status_good)
			mFields[fieldIdx]->status = farming_status_good;
	}
		
	mLock.unlock();
}
//제거
void farming::remove(int fieldId) {
    int fieldIdx = getIdx(fieldId);
    mLock.lock();
    mFields[fieldIdx]->init();
    mLock.unlock();
}
//수확
bool farming::harvest(int fieldIdx, int &farmProductId, int &output) {
	mLock.lock();
	if (mFields[fieldIdx] == NULL ||  mSeed[mFields[fieldIdx]->seedId] == NULL) {
		mLock.unlock();
		return false;
	}	

	field * f = mFields[fieldIdx];
	seed * s = mSeed[f->seedId];

	int maxGrown = 1 << (f->level - 1);
	int currentGrown = (int)((getNow() - f->timePlant) / s->timeGrow);
	int nGrown = min(currentGrown, maxGrown);

	output = 0;
	farmProductId = 0;
	
	if (f->status == farming_status_harvest || f->status == farming_status_decay) {
		if (f->status == farming_status_harvest) {
			farmProductId = s->farmProductId;	
			output = nGrown - f->accumulation;
			//level -1 만큼 더 줌.
			output += f->level - 1;
			mCntHarvest += output;
		}

		//제거
		mFields[fieldIdx]->init();
		mLock.unlock();
		return true;
	}
	else if (f->status == farming_status_grown){				
		farmProductId = s->farmProductId;		
		output = nGrown - f->accumulation;
		mCntHarvest += output;

		f->accumulation = nGrown;
		
		mLock.unlock();
        return true;
	}
	mLock.unlock();
	return false;
}

//심기
bool farming::plant(int fieldIdx, int seedId) {
	if (fieldIdx  < 0 
		|| mFields.size() <= fieldIdx 
		|| mFields[fieldIdx] == NULL 
		|| mFields[fieldIdx]->status != farming_status_max
		|| mSeed.find(seedId) == mSeed.end()) {
		return false;
	}	

	mFields[fieldIdx]->plant(seedId);
	return true;
}
bool farming::care(int fieldIdx, int boost) {
	if (fieldIdx  < 0
		|| mFields.size() <= fieldIdx
		|| mFields[fieldIdx] == NULL
		|| mFields[fieldIdx]->status != farming_status_week) {
		return false;
	}
	mFields[fieldIdx]->cntCare++;
	if (boost != 0)
		mFields[fieldIdx]->boost += boost;

	return true;
}

int farming::getLevel() {
    //level
    int threshold = 0;
    int i = 1;
    int level = 0;
    
    while (true) {
        if (i >= 0xFF)
            break;
        
        threshold += FARM_LEVEL_PER_HARVEST * i++;
        if (mCntHarvest < threshold) {
            break;
        }
        else {
            level++;
        }
    }
    
    return level;
}

void farming::makeQuest(int cnt) {
	int nMax = (int)(cnt - mQuestVector.size());
	//level 
//    int threshold = 0;
//    int i = 1;
//    int level = 0;
//
//    while (true) {
//        if (i >= 0xFF)
//            break;
//
//        threshold += FARM_LEVEL_PER_HARVEST * i++;
//        if (mCntHarvest < threshold) {
//            break;
//        }
//        else {
//            level++;
//        }
//    }
    int level = getLevel();
	float expRatio = (float)((mCntHarvest % FARM_LEVEL_PER_HARVEST) +1 ) / (float)FARM_LEVEL_PER_HARVEST;

	for (int n = 0; n < nMax; n++) {

		quest q;
		q.timeStamp = getNow();
		//현재 레벨은 무조건 할당
		q.items[0].itemId = mSeedProducts[level];
		q.items[0].quantity = getRandValueOverZero(FARM_LEVEL_PER_HARVEST * expRatio); // 점진적으로 많은 수확량 요구
		int k = 1;
		int i = FARM_QUEST_ITEM_MAX - 1;
		while (i > 0) {
			int itemId = mSeedProducts[getRandValue(level)];
			bool isValid = true;
			for (int j = 0; j < FARM_QUEST_ITEM_MAX; j++) {
				if (q.items[j].itemId == itemId) {
					isValid = false;
					break;
				}
			}
			if (isValid) {
				q.items[k].itemId = itemId;				
				q.items[k].quantity = getRandValueOverZero(FARM_LEVEL_PER_HARVEST);
				k++;
			}
			i--;
		}

		mQuestVector.push_back(q);

	}
};
