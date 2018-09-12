#include "farming.h"

bool farming::init(farmingFinshedNotiCallback fn) {
	mNoticeFn = fn;
	mThread = new thread(threadRun, this);
	return true;
}
void farming::finalize() {
	mIsThreadRun = false;
	mThread->join();	
	delete mThread;
};
void farming::addField(int x, int y) {
	field* f = new field(x, y);
	f->id = (int)mFields.size();
	mFields.push_back(f);
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
	if (mSeed.find(seedId) == mSeed.end()) {
		mLock.unlock();
		return;
	}
	int nGrown = min((int)((getNow() - mFields[fieldIdx]->timePlant) / mSeed[seedId]->timeGrow), 1 << mFields[fieldIdx]->level - 1);
	mFields[fieldIdx]->finishTime = mFields[fieldIdx]->timePlant + (mSeed[seedId]->timeGrow * mFields[fieldIdx]->level);
	time_t finishedTime = mFields[fieldIdx]->finishTime;
	mFields[fieldIdx]->percent = (float)(getNow() - mFields[fieldIdx]->timePlant) / (float)(mSeed[seedId]->timeGrow * mFields[fieldIdx]->level) * 100.0f;

	if (finishedTime + mSeed[seedId]->maxOvertime < now) {
		if(mFields[fieldIdx]->status != farming_status_decay)
			mFields[fieldIdx]->status = farming_status_decay;
	}	
	else if (finishedTime <= now) {
		if (mFields[fieldIdx]->status != farming_status_harvest) {
			mFields[fieldIdx]->status = farming_status_harvest;
			mNoticeFn(fieldIdx);
		}
	}
	else if (mFields[fieldIdx]->accumulation < mFields[fieldIdx]->level && nGrown > mFields[fieldIdx]->accumulation) {
		if (mFields[fieldIdx]->status != farming_status_grown) {
			mFields[fieldIdx]->status = farming_status_grown;			
		}
	}	
	else if (mSeed[seedId]->cares - mFields[fieldIdx]->cntCare > 0) {
		if(mFields[fieldIdx]->status != farming_status_week)
			mFields[fieldIdx]->status = farming_status_week;
	}		
	else {
		if(mFields[fieldIdx]->status != farming_status_good)
			mFields[fieldIdx]->status = farming_status_good;
	}
		
	mLock.unlock();
}

//수확
bool farming::harvest(int fieldIdx, int &farmProductId, int &output) {
	mLock.lock();
	if (mFields[fieldIdx] == NULL) {
		mLock.unlock();
		return false;
	}
	field * f = mFields[fieldIdx];
	seed * s = mSeed[f->seedId];
	int nGrown = min((int)((getNow() - f->timePlant) / s->timeGrow), 1 << (f->level - 1));
	output = 0;
	farmProductId = 0;
	//time_t now = getNow();
	//if (f->timePlant + s->timeGrow >= now) {
	if (f->status == farming_status_harvest || f->status == farming_status_decay) {
		if (f->status == farming_status_harvest) {
			farmProductId = s->farmProductId;
			//output = (int)((float)(s->outputMax + f->boost) * ((float)f->cntCare / (float)s->cares));
			//output *= f->level;
			output = nGrown - f->accumulation;
		}

		//제거
		mFields[fieldIdx]->init();
		mLock.unlock();
		return true;
	}
	else if (f->status == farming_status_grown){				
		farmProductId = s->farmProductId;		
		output = nGrown - f->accumulation;
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