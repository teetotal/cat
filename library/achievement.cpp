#include "achievement.h"

bool achievement::init(achievementCallback fn)
{
	mCallback = fn;
	mIsRunThread = true;
	mThread = new thread(threadRun, this);
	return true;
}
/*
bool achievement::loadConfig(string json) {
	const char * t[] = { "daily", "totally" };	
	Document d;
	d.Parse(json.c_str());	
	if (d.HasParseError())	
		return false;
	for (int n = 0; n < 2; n++) {		
		
		const Value& v = d[t[n]];
		for (SizeType i = 0; i < v.Size(); i++)
		{
			addAchieve(
				n == 0 ? true : false
				, utf8_to_utf16(v[i]["title"].GetString())
				, v[i]["category"].GetInt()
				, v[i]["id"].GetInt()
				, v[i]["value"].GetInt()
				, v[i]["rewardId"].GetInt()
				, v[i]["rewardValue"].GetInt()
				, v[i]["isFinished"].GetBool()
				, v[i]["isReceived"].GetBool()
			);

		}		
	}
	return true;
}
*/
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
			mAccumulation[p.category]->insert(std::pair<int, int>(p.id, p.value));			
		else
			mAccumulation[p.category]->at(p.id) += p.value;

		//update daily
		calculate(true, &p);
		//update totally
		calculate(false, &p);
	}
	mLock.unlock();
}
/*
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
*/
void achievement::calculate(bool isDaily, done *pDone) {
	achieveVector * vec = isDaily ? &mDaily : &mTotally;
	for (int n = 0; n < (int)vec->size(); n++) {
		achieve * p = vec->at(n);
		if (p->isFinished)
			continue;
		
		if (p->category == pDone->category && p->id == pDone->id && p->isFinished == false) {
			p->accumulation += pDone->value;
			if (p->accumulation >= p->value) {
				p->isFinished = true;
				mCallback(isDaily, n);
			}
		}				
	}
}

void achievement::addAchieve(bool isDaily
	, wstring title
	, int category
	, int id
	, int value
	, int rewardId
	, int rewardValue
)
{
	achieve * p = new achieve(title, category, id, value, rewardId, rewardValue);
	if (isDaily)
		mDaily.push_back(p);
	else
		mTotally.push_back(p);
}

void achievement::setAchievementAccumulation(bool isDaily, int category, int id, int accumulation, bool isFinished, bool isReceived)
{
}

void achievement::setAccumulation(int category, int id, int accumulation)
{
}

bool achievement::getDetail(bool isDaily, int idx, detail & p)
{
	achieve * a;
	if (isDaily) {
		if ((int)mDaily.size() <= idx)
			return false;
		
		a = mDaily[idx];
	}		
	else {
		if ((int)mTotally.size() <= idx)
			return false;

		a = mTotally[idx];
	}
	p.category = a->category;
	p.id = a->id;
	p.isFinished = a->isFinished;
	p.isReceived = a->isReceived;
	p.goal = a->value;
	p.rewardId = a->rewardId;
	p.rewardVal = a->rewardValue;
	p.title = a->title;
	p.accumulation = a->accumulation;//getAccumulation(a->category, a->id);		

	return true;
}

void achievement::threadRun(achievement * p)
{
	while (p->mIsRunThread)
	{
		sleepThisThread(SEC);
		p->accumulate();
	}
	
}

bool achievement::rewardReceive(bool isDaily, int idx) {
	if (isDaily == true && (int)mDaily.size() > idx && mDaily[idx]->isReceived == false) {
		mDaily[idx]->isReceived = true;
		return true;
	}
	else if (isDaily == false && (int)mTotally.size() > idx && mTotally[idx]->isReceived == false) {
		mTotally[idx]->isReceived = true;
		return true;
	}
	return false;
}