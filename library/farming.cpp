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

	time_t finishedTime = mFields[fieldIdx]->timePlant + mSeed[seedId]->timeGrow;

	if (finishedTime + mSeed[seedId]->maxOvertime < now) {
		if(mFields[fieldIdx]->status != farming_status_decay)
			mFields[fieldIdx]->status = farming_status_decay;
	}		
	else if (finishedTime <= now) {
		if (mFields[fieldIdx]->status != farming_status_grown) {
			mFields[fieldIdx]->status = farming_status_grown;
			mNoticeFn(fieldIdx);
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
	
	//time_t now = getNow();
	//if (f->timePlant + s->timeGrow >= now) {
	if (f->status == farming_status_grown || f->status == farming_status_decay){
		int sum = 0;
		if (f->status == farming_status_grown) {
			output = (int)((float)(s->outputMax + f->boost) * ((float)f->cntCare / (float)s->cares));
			farmProductId = s->farmProductId;
		}
			
		//제거
		mFields[fieldIdx]->init();
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