#include "quest.h"

bool Quest::init(achievementCallback fn, int cnt)
{	
	mLastUpdated = 1;
	mCnt = cnt;
	mCallback = fn;
	mIsRunThread = true;
	mThread = new thread(threadRun, this);
	return true;
}

void Quest::push(int category, int id, int val) {
	done p;
	p.category = category;
	p.id = id;
	p.value = val;

	mLock.lock();
	mQueue.push(p);
	mLock.unlock();
};

void Quest::accumulate() {
	mLock.lock();
	while (mQueue.size() > 0)
	{		
		done p = mQueue.front();
		mQueue.pop();
		setAccumulation(p.category, p.id, p.value);				
		calculate(&p);
	}
	mLock.unlock();
}

void Quest::setAccumulation(int category, int id, int accumulation)
{
	if (mAccumulation.find(category) == mAccumulation.end()) {
		mAccumulation[category] = new intMap;
	}

	if (mAccumulation[category]->find(id) == mAccumulation[category]->end())
		mAccumulation[category]->insert(std::pair<int, int>(id, accumulation));
	else
		mAccumulation[category]->at(id) += accumulation;
}

void Quest::calculate(done * pDone) {
	//앞에서 부터 아직 완료되지 않은 퀘스트에 대해서만 누적해준다.
	int cnt = 0;
	for (unsigned int n = 0; n < mQuests.size(); n++) {
		_quest * p = mQuests[n];
		if (p->isFinished)
			continue;

		if (cnt >= mCnt)
			return;

		if (p->category == pDone->category && p->id == pDone->id) {
			p->accumulation += pDone->value;
			mLastUpdated = getNow();

			if (p->accumulation >= p->value) {
				p->isFinished = true;
				mCallback(p->uniqueId, n);
			}
			return;
		}

		cnt++;
	}
}

void Quest::setQuest(int uniqueId, int accumulation, bool isFinished, bool isReceived)
{
	for (unsigned int n = 0; n < mQuests.size(); n++) {
		if (mQuests[n]->uniqueId == uniqueId) {
			mQuests[n]->accumulation = accumulation;
			mQuests[n]->isFinished = isFinished;
			mQuests[n]->isReceived = isReceived;

			return;
		}
	}
}

bool Quest::rewardReceive(_quest * p) {
	if (p == NULL)
		return false;

	if (p->isFinished == false)
		return false;

	p->isReceived = true;
	return true;
}

bool Quest::rewardReceive(int uniqueId) {
	for (unsigned int n = 0; n < mQuests.size(); n++) {
		if (mQuests[n]->uniqueId == uniqueId) {
			return rewardReceive(mQuests[n]);
		}
	}
	
	return false;
}