#include "achievement.h"

bool achievement::init(achievementCallback fn, time_t lastLogin, bool isLevelMode, int level)
{
	mIsLevelMode = isLevelMode;
	setLevel(level);
	mLastLogin = lastLogin;
	resetDaily();
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

void achievement::setLevel(int level) {
	mLevel = level;
	//accumulation 에서 읽어 와 채우기
	if (mMap.find(level) == mMap.end())
		return;

	achieveVector vec = mMap[level];
	for (int n = 0; n < vec.size(); n++) {
		if (!hasAccumulation(vec[n]->category, vec[n]->id))
			continue;
		vec[n]->accumulation = mAccumulation[vec[n]->category]->at(vec[n]->id);
		if (vec[n]->accumulation >= vec[n]->value) {
			vec[n]->accumulation = vec[n]->value;
			vec[n]->isFinished = true;
			mCallback(level, n);
		}
			
	}
};

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
	resetDaily();
	while (mQueue.size() > 0)
	{		
		done p = mQueue.front();
		mQueue.pop();
		setAccumulation(p.category, p.id, p.value);				
		calculate(&p);
	}
	mLock.unlock();
}

void achievement::calculateSet(int type, achieveVector * vec, done * pDone) {
	for (int n = 0; n < (int)vec->size(); n++) {
		achieve * p = vec->at(n);
		if (p->isFinished)
			continue;

		if (p->category == pDone->category && p->id == pDone->id && p->isFinished == false) {
			p->accumulation += pDone->value;
			if (p->accumulation >= p->value) {
				p->isFinished = true;
				mCallback(type, n);
			}
		}
	}
}

void achievement::calculate(done * pDone) {
	//퀘스트면 해당 업적만 추가
	if (mIsLevelMode) {
		calculateSet(mLevel, &mMap[mLevel], pDone);
	}
	else {
		for (achieveMap::iterator it = mMap.begin(); it != mMap.end(); ++it) {
			calculateSet(it->first, &it->second, pDone);
		}
	}	
}

void achievement::resetDaily()
{
	if (mIsLevelMode)
		return;
	//update daily
	int lastYday = getLocalTm(mLastLogin)->tm_yday;
	int nowYday = getLocalTm(getNow())->tm_yday;
	if (lastYday != nowYday) {
		//초기화
		achieveVector v = mMap[ACHIEVEMENT_TYPE_DAILY];
		for (int n = 0; n < (int)v.size(); n++) {
			v[n]->isFinished = false;
			v[n]->isReceived = false;
			v[n]->accumulation = 0;
		}
		mLastLogin = getNow();
	}
}

void achievement::addAchieve(int type
	, wstring title
	, int category
	, int id
	, int value
	, int rewardId
	, int rewardValue
)
{
	achieve * p = new achieve(title, category, id, value, rewardId, rewardValue);
	mMap[type].push_back(p);	
}

void achievement::setAchievementAccumulation(int type, int category, int id, int accumulation, bool isFinished, bool isReceived)
{
	achieveVector * a = &mMap[type];
	for (int n = 0; n < (int)a->size(); n++) {
		if (a->at(n)->category == category && a->at(n)->id == id) {
			a->at(n)->accumulation = accumulation;
			a->at(n)->isFinished = isFinished;
			a->at(n)->isReceived = isReceived;
		}
	}
}

bool achievement::hasAccumulation(int category, int id) {
	if (mAccumulation.find(category) == mAccumulation.end()) {
		return false;
	}

	if (mAccumulation[category]->find(id) == mAccumulation[category]->end())
		return false;

	return true;
}

void achievement::setAccumulation(int category, int id, int accumulation)
{
	if (mAccumulation.find(category) == mAccumulation.end()) {
		mAccumulation[category] = new intMap;
	}

	if (mAccumulation[category]->find(id) == mAccumulation[category]->end())
		mAccumulation[category]->insert(std::pair<int, int>(id, accumulation));
	else
		mAccumulation[category]->at(id) += accumulation;
}

bool achievement::getDetail(detail &p, int type, int idx)
{
	if (mMap.find(type) == mMap.end())
		return false;
	if (mMap[type].size() <= idx)
		return false;

	achieve * a = mMap[type][idx];
	
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

bool achievement::rewardReceive(int type, int idx) {
	if (mMap[type].size() > idx && mMap[type][idx]->isReceived == false) {
		mMap[type][idx]->isReceived = true;
		return true;
	}	
	return false;
}