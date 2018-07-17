#include "pch.h"

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

	time_t now = getNow();
	if (mFields[fieldIdx] == NULL) {
		return;
	}
	int seedId = mFields[fieldIdx]->seedId;
	if (mSeed.find(seedId) == mSeed.end()) {
		return;
	}

	time_t finishedTime = mFields[fieldIdx]->timePlant + mSeed[seedId]->timeGrow;

	if (finishedTime + mSeed[seedId]->maxOvertime < now)
		mFields[fieldIdx]->status = farming_status_decay;
	else if (finishedTime <= now)
		mFields[fieldIdx]->status = farming_status_grown;
	else if (mSeed[seedId]->cares - mFields[fieldIdx]->cntCare > 0)
		mFields[fieldIdx]->status = farming_status_week;
	else
		mFields[fieldIdx]->status = farming_status_good;
}

//수확
int farming::harvest(int fieldIdx) {

	if (mFields[fieldIdx] == NULL) {
		return 0;
	}
	field * f = mFields[fieldIdx];
	seed * s = mSeed[f->seedId];
	
	time_t now = getNow();
	if (f->timePlant + s->timeGrow >= now) {
		int sum = (s->outputMax + f->boost) * (f->cntCare / s->cares);
		//제거
		mFields[fieldIdx] = NULL;
		delete f;
	}		

	return 0;
}

//심기
bool farming::plant(int fieldIdx, int seedId) {
	if (fieldIdx  < 0 || mFields.size() >= fieldIdx || mFields[fieldIdx] == NULL || mSeed.find(seedId) == mSeed.end()) {
		return false;
	}	

	mFields[fieldIdx]->plant(seedId);
	return true;
}