#include "achievement.h"

bool achievement::init(achievementCallback fn)
{
	mCallback = fn;
	mIsRunThread = true;
	mThread = new thread(threadRun, this);
	return true;
}

void achievement::finalize()
{
	mIsRunThread = false;
	mThread->join();
}

void achievement::push(int category, int id, int val) {
	done p;
	p.category = category;
	p.id = id;
	p.value = val;
	mLock.lock();
	mQueue.push(p);
	mLock.unlock();
};

void achievement::accumulate() {
	mLock.lock();
	while (mQueue.size() > 0)
	{
		done p = mQueue.front();
		mQueue.pop();
		if (mAccumulation.find(p.category) == mAccumulation.end()) {
			mAccumulation[p.category] = new intMap;
		}
			
		if (mAccumulation[p.category]->find(p.id) == mAccumulation[p.category]->end())
			mAccumulation[p.category]->at(p.id) = p.value;
		else
			mAccumulation[p.category]->at(p.id) += p.value;
	}
	mLock.unlock();
}

int achievement::getAccumulation(int category, int id)
{
	mLock.lock();
	if (mAccumulation.find(category) == mAccumulation.end()) {
		mLock.unlock();
		return 0;
	}

	if (mAccumulation[category]->find(id) == mAccumulation[category]->end()) {
		mLock.unlock();
		return 0;
	}
		
	else {
		int val = mAccumulation[category]->at(id);
		mLock.unlock();		
		return val;
	}		
	mLock.unlock();
	return 0;
}

void achievement::calculate(bool isDaily, achieveVector * vec) {
	for (int n = 0; n < vec->size(); n++) {
		achieve * p = vec->at(n);
		if (p->isFinished)
			continue;
		
		int val = getAccumulation(p->category, p->id);
		if (val >= p->value) {
			//업적 달성 및 보상 지급
			p->isFinished = true;
			mCallback(isDaily, n);
		}			
	}
}

void achievement::addAchieve(bool isDaily, wstring title, int category, int id, int value, int rewardId, int rewardValue, bool isFinished)
{
	achieve * p = new achieve(title, category, id, value, rewardId, rewardValue, isFinished);
	if (isDaily)
		mDaily.push_back(p);
	else
		mTotally.push_back(p);
}

void achievement::calculate()
{
	calculate(true, &mDaily);
	calculate(false, &mTotally);
}

bool achievement::getDetail(bool isDaily, int idx, detail & p)
{
	achieve * a;
	if (isDaily) {
		if (mDaily.size() <= idx)
			return false;
		
		a = mDaily[idx];
	}		
	else {
		if (mTotally.size() <= idx)
			return false;

		a = mTotally[idx];
	}
	p.isFinished = a->isFinished;
	p.goal = a->value;
	p.rewardId = a->rewardId;
	p.rewardVal = a->rewardValue;
	p.title = a->title;
	p.accumulation = getAccumulation(a->category, a->id);		

	return true;
}

void achievement::threadRun(achievement * p)
{
	while (p->mIsRunThread)
	{
		sleepThisThread(SEC);
		p->accumulate();
		p->calculate();
	}
	
}
