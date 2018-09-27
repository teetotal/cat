#include "logics.h"

#if !defined(_WIN32) || defined(COCOS2D_DEBUG)
    #include "cocos2d.h"
    USING_NS_CC;
#endif

logics * logics::hInst = NULL;
bool logics::hIsSync = false;

bool logics::init(farmingFinshedNotiCallback farmCB, tradeUpdatedCallback tradeCB, achievementCallback achieveCB, bool isFarmingDataLoad) {
	srand(getNow());
    string szMeta, szActor, szActions;
#if defined(_WIN32) && !defined(COCOS2D_DEBUG)
		szMeta = loadJsonString(CONFIG_META);
		//szActor = loadJsonString(CONFIG_ACTOR);
		szActions = loadJsonString(CONFIG_ACTIONS);
#else
		szMeta = FileUtils::getInstance()->getStringFromFile(CONFIG_META);
		szActions = FileUtils::getInstance()->getStringFromFile(CONFIG_ACTIONS);
		/*
		string fileFullPath = FileUtils::getInstance()->getWritablePath() + CONFIG_ACTOR;
		if (!FileUtils::getInstance()->isFileExist(fileFullPath)) {
			//writable 경로에 파일 복사
			FileUtils::getInstance()->writeStringToFile(FileUtils::getInstance()->getStringFromFile(CONFIG_ACTOR), fileFullPath);
		}
		szActor = FileUtils::getInstance()->getStringFromFile(fileFullPath);
		*/
		//sqlite3
		string sqliteFullPath = FileUtils::getInstance()->getWritablePath() + CONFIG_SQLITE3;
		if (!FileUtils::getInstance()->isFileExist(sqliteFullPath)) {
			//writable 경로에 파일 복사
			FileUtils::getInstance()->writeDataToFile(FileUtils::getInstance()->getDataFromFile(CONFIG_SQLITE3), sqliteFullPath);
		}
		if(!Sql::inst()->init(sqliteFullPath))
			return false;
#endif

	hIsSync = false;
	mAchieveCB = achieveCB;

	rapidjson::Document d;
	d.Parse(szMeta.c_str());
	if (d.HasParseError())
		return false;
	/*
	rapidjson::Document dActor;
	dActor.Parse(szActor.c_str());
	if (dActor.HasParseError())
		return false;
	*/
	rapidjson::Document dActions;
	dActions.Parse(szActions.c_str());
	if (dActions.HasParseError())
		return false;

	if(!initErrorMessage(d["errors"]))
		return false;
	if (!initItems(d["items"]))
		return false;
	if (!initSeed(d["farming"], d["seed"]))
		return false;
	if (!initTraining(dActions["actions"]))
		return false;
	if (!initJobTitle(d["jobTitle"]))
		return false;
	if (!initRace(d["race"]))
		return false;
		
	if (!initActor(isFarmingDataLoad)) {
		log("init Actor failure !!!!!!!!!!!!!!!!!!!!!!!");
		return false;
	}
	
	if (!initAchievement(d["achievement"])) {
		log("init achievement failure !!!!!!!!!!!!!!!!!!!!!!!");
		return false;
	}

	if (!mFarming.init(farmCB, mQuest.getAccumulation(achievement_category_farming, achievement_farming_id_output))) {
		log("init farm failure !!!!!!!!!!!!!!!!!!!!!!!");
		return false;
	}
	
	//trade
	float trade_margin = d["trade"]["margin"].GetFloat();
	int trade_interval = d["trade"]["updateInterval"].GetInt();
	int trade_weight = d["trade"]["levelWeight"].GetInt();

	if (!mTrade.init(trade_margin, trade_interval, trade_weight, tradeCB)) {
		log("init trade failure !!!!!!!!!!!!!!!!!!!!!!!");
		return false;
	}
	log("init logics Done!!!!!!!!!!!!!!!!!!!!!!!");

	mIsRunThread = true;
	mThread = new thread(threadRun);

	return true;
}
/* private initialize */
bool logics::initActor(bool isFarmingDataLoad)
{	
	_actor* actor = new _actor;
	/*
	actor->userName = utf8_to_utf16(string(d["userName"].GetString()));
	actor->userId = d["userId"].GetString();

	actor->name = utf8_to_utf16(string(d["name"].GetString()));
	actor->id = d["id"].GetString();

	time_t lastLogin = d["lastLoginLoginTime"].GetInt64();
	actor->lastLoginLoginTime = getNow();
	actor->lastLoginLogoutTime = d["lastLoginLogoutTime"].GetInt64();
	actor->lastHPUpdateTime = d["lastHPUpdateTime"].GetInt64();

	actor->jobTitle = utf8_to_utf16(string(d["jobTitle"].GetString()));

	actor->point = d["point"].GetInt();
	actor->hp = d["hp"].GetInt();
	actor->exp = d["exp"].GetInt();
	actor->level = d["level"].GetInt();

	actor->property.strength = d["property"]["strength"].GetInt();
	actor->property.intelligence = d["property"]["intelligence"].GetInt();
	actor->property.appeal = d["property"]["appeal"].GetInt();
	*/
	//actor
	{
		sqlite3_stmt * stmt = Sql::inst()->select("SELECT userId, userName, id, name, lastLoginLoginTime, lastLoginLogoutTime, lastHPUpdateTime, jobTitle, point, hp, exp, level, strength, intelligence, appeal FROM actor WHERE idx = 1");
		if (stmt == NULL)
			return false;

		int result = sqlite3_step(stmt);

		if (result == SQLITE_ROW)
		{
			int idx = 0;
			actor->userId = (const char*)sqlite3_column_text(stmt, idx++);
			actor->userName = (const char*)sqlite3_column_text(stmt, idx++);
			actor->id = (const char*)sqlite3_column_text(stmt, idx++);
			actor->name = (const char*)sqlite3_column_text(stmt, idx++);		

			actor->lastLoginLoginTime = (time_t)sqlite3_column_int64(stmt, idx++);
			actor->lastLoginLoginTime = getNow();

			actor->lastLoginLogoutTime = (time_t)sqlite3_column_int64(stmt, idx++);
			actor->lastHPUpdateTime = (time_t)sqlite3_column_int64(stmt, idx++);

			const char * szJobTitle = (const char*)sqlite3_column_text(stmt, idx++);
			actor->jobTitle = (*szJobTitle == '!') ? "" : szJobTitle;

			actor->point = sqlite3_column_int(stmt, idx++);
			actor->hp = sqlite3_column_int(stmt, idx++);
			actor->exp = sqlite3_column_int(stmt, idx++);
			actor->level = sqlite3_column_int(stmt, idx++);

			actor->property.strength = sqlite3_column_int(stmt, idx++);
			actor->property.intelligence = sqlite3_column_int(stmt, idx++);
			actor->property.appeal = sqlite3_column_int(stmt, idx++);			
		}
			
	}

	//collection
	{
		sqlite3_stmt * stmt = Sql::inst()->select("select * from collection");
		if (stmt == NULL)
			return false;

		int result = 0;
		while (true)
		{
			result = sqlite3_step(stmt);

			if (result == SQLITE_ROW)
			{
				int id = sqlite3_column_int(stmt, 0);
				actor->collection[id] = true;
			}
			else
				break;
		}
	}
	/*
	const rapidjson::Value& collection = d["collection"];
	for (rapidjson::SizeType i = 0; i < collection.Size(); i++) {
		int id = collection[rapidjson::SizeType(i)].GetInt();
		actor->collection[id] = true;
	}
	*/
	mActor = actor;

	//inventory
	{
		sqlite3_stmt * stmt = Sql::inst()->select("select * from inventory");
		if (stmt == NULL)
			return false;

		int result = 0;
		while (true)
		{
			result = sqlite3_step(stmt);

			if (result == SQLITE_ROW)
			{
				int idx = 0;
				int category = sqlite3_column_int(stmt, idx++);
				int id = sqlite3_column_int(stmt, idx++);
				int quantity = sqlite3_column_int(stmt, idx++);

				mActor->inven.pushItem(category, id, quantity);
			}
			else
				break;
		}
	}	
	//farming
	if (isFarmingDataLoad) {
		sqlite3_stmt * stmt = Sql::inst()->select("select * from farm");
		if (stmt == NULL)
			return false;

		int result = 0;
		while (true)
		{
			result = sqlite3_step(stmt);

			if (result == SQLITE_ROW)
			{
				int idx = 0;
				int id = sqlite3_column_int(stmt, idx++);
				int x = sqlite3_column_int(stmt, idx++);
				int y = sqlite3_column_int(stmt, idx++);
				int seedId = sqlite3_column_int(stmt, idx++);
				//farming::farming_status status = (farming::farming_status)sqlite3_column_int(stmt, idx++);
				time_t timePlant = (time_t)sqlite3_column_int64(stmt, idx++);
				int cntCare = sqlite3_column_int(stmt, idx++);
				time_t timeLastGrow = (time_t)sqlite3_column_int64(stmt, idx++);
				int boost = sqlite3_column_int(stmt, idx++);
				int level = sqlite3_column_int(stmt, idx++);
				int accumulation = sqlite3_column_int(stmt, idx++);

				mFarming.addField(id, x, y, seedId, farming::farming_status_max, timePlant, cntCare, timeLastGrow, boost, level, accumulation);
			}
			else
				break;
		}
	}
	return true;
}
bool logics::initErrorMessage(rapidjson::Value & p)
{
	for (rapidjson::SizeType i = 0; i < p.Size(); i++)
	{
		addErrorMessage(utf8_to_utf16(p[i].GetString()));
	}
	return true;
}
bool logics::initItems(rapidjson::Value & p)
{
	for (rapidjson::SizeType i = 0; i < p.Size(); i++)
	{
		_item item;
		item.id = p[rapidjson::SizeType(i)]["id"].GetInt();
		item.type = (itemType)p[rapidjson::SizeType(i)]["type"].GetInt();
		item.value = p[rapidjson::SizeType(i)]["value"].GetInt();
		item.grade = p[rapidjson::SizeType(i)]["grade"].GetInt();
		string sz = p[rapidjson::SizeType(i)]["name"].GetString();
		item.name = utf8_to_utf16(sz);
		insertItem(item);
	}
	return true;
}
bool logics::initSeed(rapidjson::Value & farming, rapidjson::Value & p)
{
	mFarmingExtendFee = farming["extendFee"].GetInt();

	for (rapidjson::SizeType i = 0; i < p.Size(); i++)
	{
		farming::seed * seed = new farming::seed();
		seed->id = p[rapidjson::SizeType(i)]["id"].GetInt();
		seed->name = utf8_to_utf16(p[rapidjson::SizeType(i)]["name"].GetString());
		seed->farmProductId = p[rapidjson::SizeType(i)]["farmProductId"].GetInt();
		seed->outputMax = p[rapidjson::SizeType(i)]["outputMax"].GetInt();
		seed->timeGrow = p[rapidjson::SizeType(i)]["timeGrow"].GetInt();
		seed->cares = p[rapidjson::SizeType(i)]["cares"].GetInt();
		seed->maxOvertime = p[rapidjson::SizeType(i)]["maxOvertime"].GetInt();
		addSeed(seed);
	}
	return true;
}
bool logics::initTraining(rapidjson::Value & t)
{
	for (rapidjson::SizeType i = 0; i < t.Size(); i++) {
		_training p;
		for (int k = 0; k < maxTrainingItems; k++) {
			p.reward.items[k] = NULL;
			p.cost.items[k] = NULL;
		}
		p.id = t[rapidjson::SizeType(i)]["id"].GetInt();
		string sz = t[rapidjson::SizeType(i)]["name"].GetString();
		p.name = utf8_to_utf16(sz);
		p.type = (trainingType)t[rapidjson::SizeType(i)]["type"].GetInt();
		p.level = t[rapidjson::SizeType(i)]["level"].GetInt();

		p.reward.strength = t[rapidjson::SizeType(i)]["reward"]["strength"].GetInt();
		p.reward.intelligence = t[rapidjson::SizeType(i)]["reward"]["intelligence"].GetInt();
		p.reward.appeal = t[rapidjson::SizeType(i)]["reward"]["appeal"].GetInt();
		p.reward.point = t[rapidjson::SizeType(i)]["reward"]["point"].GetInt();

		p.cost.strength = t[rapidjson::SizeType(i)]["cost"]["strength"].GetInt();
		p.cost.intelligence = t[rapidjson::SizeType(i)]["cost"]["intelligence"].GetInt();
		p.cost.appeal = t[rapidjson::SizeType(i)]["cost"]["appeal"].GetInt();
		p.cost.point = t[rapidjson::SizeType(i)]["cost"]["point"].GetInt();

		p.start = 0;
		//  반짝 
		if (t[rapidjson::SizeType(i)].HasMember("moment")) {
			p.start = getTime(
				t[rapidjson::SizeType(i)]["moment"]["start"]["hour"].GetInt()
				, t[rapidjson::SizeType(i)]["moment"]["start"]["min"].GetInt()
				, t[rapidjson::SizeType(i)]["moment"]["start"]["sec"].GetInt()
			);
			p.count = t[rapidjson::SizeType(i)]["moment"]["count"].GetInt();
			p.keep = t[rapidjson::SizeType(i)]["moment"]["keep"].GetInt();
			p.interval = t[rapidjson::SizeType(i)]["moment"]["keep"].GetInt();
		}

		const rapidjson::Value& item = t[rapidjson::SizeType(i)]["reward"]["items"];
		for (rapidjson::SizeType m = 0; m < item.Size(); m++) {
			if (m >= maxTrainingItems)
				break;
			_itemPair* pair = new _itemPair;
			pair->itemId = item[rapidjson::SizeType(m)]["id"].GetInt();
			pair->val = item[rapidjson::SizeType(m)]["quantity"].GetInt();
			p.reward.items[rapidjson::SizeType(m)] = pair;
		}

		const rapidjson::Value& item2 = t[rapidjson::SizeType(i)]["cost"]["items"];
		for (rapidjson::SizeType m = 0; m < item2.Size(); m++) {
			if (m >= maxTrainingItems)
				break;
			_itemPair* pair = new _itemPair;
			pair->itemId = item2[rapidjson::SizeType(m)]["id"].GetInt();
			pair->val = item2[rapidjson::SizeType(m)]["quantity"].GetInt();
			p.cost.items[rapidjson::SizeType(m)] = pair;
		}
		if (!insertTraining(p))
			return false;
	}
	return true;
}
bool logics::initJobTitle(rapidjson::Value & job)
{
	setDefaultJobTitle(utf8_to_utf16(job["default"].GetString()));
	const rapidjson::Value& jobPrefix = job["prefix"];
	for (rapidjson::SizeType i = 0; i < jobPrefix.Size(); i++) {
		_jobTitlePrefix p;
		p.level = jobPrefix[rapidjson::SizeType(i)]["level"].GetInt();
		p.title = utf8_to_utf16(jobPrefix[rapidjson::SizeType(i)]["title"].GetString());

		addJobTitlePrefix(p);
	}

	const rapidjson::Value& jobBody = job["body"];
	for (rapidjson::SizeType i = 0; i < jobBody.Size(); i++) {
		_jobTitleBody p;
		p.S = jobBody[rapidjson::SizeType(i)]["S"].GetInt();
		p.I = jobBody[rapidjson::SizeType(i)]["I"].GetInt();
		p.A = jobBody[rapidjson::SizeType(i)]["A"].GetInt();
		p.title = utf8_to_utf16(jobBody[rapidjson::SizeType(i)]["title"].GetString());

		addJobTitleBody(p);
	}
	return true;
}
bool logics::initRace(rapidjson::Value & race)
{
	const int baseMin = 10;
	int id = 0;
	int minItem = baseMin;
	int levelItem = 1;
	int minSpeed = baseMin;
	int levelSpeed = 1;
	int min1vs1 = baseMin;
	int level1vs1 = 1;
	const int gap = 100;
	const int baseFee = 200;

	for (rapidjson::SizeType i = 0; i < race.Size(); i++) {
		_race r;
		r.id = id++;
		r.mode = (race_mode)race[rapidjson::SizeType(i)]["mode"].GetInt();
		r.title = utf8_to_utf16(race[rapidjson::SizeType(i)]["title"].GetString());
		switch (r.mode) {
		case race_mode_item:
			r.level = levelItem++;
			r.min = minItem;
			r.max = minItem * 2 + gap; //10-120, 120-340, 340-780, 780-1660
			minItem = r.max;
			break;
		case race_mode_speed:
			r.level = levelSpeed++;
			r.min = minSpeed;
			r.max = minSpeed * 2 + gap;
			minSpeed = r.max;
			break;
			/*
			case race_mode_1vs1:
			break;
			case race_mode_friend_1:
			break;
			*/
		default:
			r.level = level1vs1++;
			r.min = min1vs1;
			r.max = min1vs1 *2 + gap;
			min1vs1 = r.max;
			break;
		}

		r.fee = baseFee * r.level;
		const int maxRank = (r.mode == race_mode_1vs1) ? 1: 2;
		for (int n = 0; n < maxRank; n++) {
			_raceReward rr;
			rr.prize = r.fee * (3 - n);
			//레벨에 맞는 도감 제공
			for (__items::iterator it = mItems.find(collectionStartId); it != mItems.end(); ++it) {
				if (it->second.type == itemType_collection) {
					if (it->second.grade == r.level) {
						_itemPair ip;
						ip.itemId = it->first;
						ip.val = 1;
						rr.items.push_back(ip);
					}
				}
			}
			r.rewards.push_back(rr);
		}
		addRaceMeta(r);
		mRaceModeCnt[r.mode]++;
	}
	return true;
}

bool logics::initAchievement(rapidjson::Value & v) {
	//specific
	map<int, queue<Quest::_quest*>> specificQuests;
	for (rapidjson::SizeType i = 0; i < v.Size(); i++)
	{
		Quest::_quest* p = new Quest::_quest(
			v[rapidjson::SizeType(i)]["uniqueId"].GetInt()
			, utf8_to_utf16(v[rapidjson::SizeType(i)]["title"].GetString())
			, v[rapidjson::SizeType(i)]["category"].GetInt()
			, v[rapidjson::SizeType(i)]["id"].GetInt()
			, v[rapidjson::SizeType(i)]["value"].GetInt()
			, v[rapidjson::SizeType(i)]["rewardId"].GetInt()
			, v[rapidjson::SizeType(i)]["rewardValue"].GetInt()
		);
		specificQuests[v[rapidjson::SizeType(i)]["level"].GetInt()].push(p);
	}

	//basic 	
	int goals[] =		{ 0, 10,	20,		50,		114,	242,	498,	1010,	2034,	4082,	8178,	16370,	32754 }; //레벨 별 속성
	
	int raceItem[] =	{ 0, 211,	211,	211,	212,	212,	213,	213,	214,	214,	215,	215,	215	}; //레벨별 지급 race 아이템 
	int raceItem_a[] =	{ 0, 0,		0,		0,		0,		0,		0,		0,		0,		222,	222,	222,	222 }; //레벨별 지급 race 전방 공격 아이템
	int raceItem_b[] =	{ 0, 0,		230,	230,	231,	231,	231,	232,	232,	233,	233,	234,	234 }; //레벨별 지급 race 1등 공격 아이템 
	int raceItem_c[] =	{ 0, 0,		0,		220,	220,	220,	221,	221,	221,	0,		0,		0,		0}; //레벨별 지급 race 전방 공격 아이템


	int farmItem[] =	{ 0, 0,		0,		400,	401,	401,	402,	403,	404,	405,	406,	407,	407 }; //레벨별 지급 farm 아이템 
	int actionItem[] =	{ 0, 0,		0,		1,		2,		3,		4,		5,		6,		7,		8,		8,		8 }; //레벨별 지급 action 아이템 
	int uniqueId = 0;
	for (int n = mActor->level; n <= LEVEL_MAX; n++) {

		if (specificQuests.find(n) != specificQuests.end()) {
			queue<Quest::_quest*> q = specificQuests[n];

			while (q.size() > 0)
			{
				mQuest.addQuest(q.front());
				q.pop();
			}
		}

		//action
		int totalProperty = goals[n];
		mQuest.addQuest(
			uniqueId++
			, L"능력치 " + to_wstring(totalProperty) + L" 올리기"
			, achievement_category_property
			, achievement_property_id_total
			, totalProperty
			, raceItem[n]
			, n //레벨 만큼 준다
		);
		//race
		if (raceItem_a[n] > 0) {
			int nRaceTry = n / 2;
			mQuest.addQuest(
				uniqueId++
				, L"경묘 1등 " + to_wstring(nRaceTry) + L"번 하기"
				, achievement_category_race
				, achievement_race_id_first
				, nRaceTry
				, raceItem_a[n]
				, n
			);
		}
		//farm
		if (raceItem_b[n] > 0) {
			int nFarmTry = 1 << n;
			mQuest.addQuest(
				uniqueId++
				, L"농사 " + to_wstring(nFarmTry) + L"번 씨 뿌리기"
				, achievement_category_farming
				, achievement_farming_id_plant
				, nFarmTry
				, raceItem_b[n]
				, n  //레벨 만큼 준다
			);
		}

		//race item
		if (raceItem_c[n] > 0) {
			int use = n;
			mQuest.addQuest(
				uniqueId++
				, L"경묘 1등 공격 아이템 " + to_wstring(use) + L"번 사용하기"
				, achievement_category_race_use_item_type
				, 204
				, use
				, raceItem_c[n]
				, n  //레벨 만큼 준다
			);
		}
		for (__training::iterator it = mTraining.begin(); it != mTraining.end(); ++it) {			
			//int nRaceTry = n * 1.5;
			if(it->second.level == n){
                mQuest.addQuest(
                        uniqueId++
                        , it->second.name + L" 하기"
                        , achievement_category_training
                        , it->first
                        , 1
                        , 51
                        , n
                );
			}
		}		
	}

	
	//achievement		
	/*
	rapidjson::Value & pQuests = pAchievement["quests"];
	for (rapidjson::SizeType i = 0; i < pQuests.Size(); i++) {
		mAchievement.setAchievementAccumulation(
			pQuests[rapidjson::SizeType(i)]["level"].GetInt()
			, pQuests[rapidjson::SizeType(i)]["category"].GetInt()
			, pQuests[rapidjson::SizeType(i)]["id"].GetInt()
			, pQuests[rapidjson::SizeType(i)]["accumulation"].GetInt()
			, pQuests[rapidjson::SizeType(i)]["isFinished"].GetBool()
			, pQuests[rapidjson::SizeType(i)]["isReceived"].GetBool()
		);
	}
	*/
	{
		sqlite3_stmt * stmt = Sql::inst()->select("select uniqueId, accumulation, isFinished, isReceived from quest");
		if (stmt == NULL)
			return false;
		
		int result = 0;
		while (true)
		{
			result = sqlite3_step(stmt);

			if (result == SQLITE_ROW)
			{
				int idx = 0;				
				int id = sqlite3_column_int(stmt, idx++);
				int accumulation = sqlite3_column_int(stmt, idx++);
				bool isFinished = (sqlite3_column_int(stmt, idx++) == 0) ? false : true;
				bool isReceived = (sqlite3_column_int(stmt, idx++) == 0) ? false : true;

				mQuest.setQuest(id, accumulation, isFinished, isReceived);
			}
			else
				break;
		}
	}
	

	//accumulation	
	/*
	rapidjson::Value & pAccumulation = pAchievement["accumulation"];
	for (rapidjson::Value::ConstMemberIterator it = pAccumulation.MemberBegin();
		it != pAccumulation.MemberEnd(); ++it)
	{
		const char * category = it->name.GetString();
		//const rapidjson::Value& list = d["achievement"]["accumulation"][category];
		const rapidjson::Value& list = it->value;

		for (rapidjson::SizeType i = 0; i < list.Size(); i++) {
			int id = list[rapidjson::SizeType(i)]["id"].GetInt();
			int value = list[rapidjson::SizeType(i)]["value"].GetInt();
			mAchievement.setAccumulation(atoi(category)
				, id
				, value
			);
		}
	}
	*/
	{
		sqlite3_stmt * stmt = Sql::inst()->select("select category, id, cnt from achievement_accumulation");
		if (stmt == NULL)
			return false;

		int result = 0;
		while (true)
		{
			result = sqlite3_step(stmt);

			if (result == SQLITE_ROW)
			{
				int idx = 0;
				int category = sqlite3_column_int(stmt, idx++);
				int id = sqlite3_column_int(stmt, idx++);
				int cnt = sqlite3_column_int(stmt, idx++);

				mQuest.setAccumulation(category, id, cnt);
			}
			else
				break;
		}
	}	

	if (!mQuest.init(achievementCallbackFn, questCnt))
		return false;
	
	return true;
}
/*
bool logics::initAchievement(rapidjson::Value & p)
{
	bool isDaily = true;
	for (int n = 0; n < 2; n++) 
	{
		const rapidjson::Value &v = isDaily ? p["daily"]: p["totally"];
		
		for (rapidjson::SizeType i = 0; i < v.Size(); i++)
		{			
			mAchievement.addAchieve(
				n
				, utf8_to_utf16(v[rapidjson::SizeType(i)]["title"].GetString())
				, v[rapidjson::SizeType(i)]["category"].GetInt()
				, v[rapidjson::SizeType(i)]["id"].GetInt()
				, v[rapidjson::SizeType(i)]["value"].GetInt()
				, v[rapidjson::SizeType(i)]["rewardId"].GetInt()
				, v[rapidjson::SizeType(i)]["rewardValue"].GetInt()
			);
		}
		isDaily = false;
	}
	return true;
}
*/

void logics::finalize() {
	if(mActor)
		mActor->lastLoginLogoutTime = getNow();
	mIsFinalized = true;
	saveActor();
	mIsRunThread = false;
	mThread->join();
	mFarming.finalize();
	mTrade.finalize();
	mQuest.finalize();
	Sql::inst()->finalize();
}
/* temporary print */
void logics::printInven(inventoryType type, wstring &sz) {
	vector<intPair> vec;
	mActor->inven.getWarehouse(vec, type);
	for (int n = 0; n < (int)vec.size(); n++) {
		sz += to_wstring(vec[n].key)
			+ L". "
			+ mItems[vec[n].key].name
			+ L"(" + to_wstring(vec[n].val)
			+ L"개) "
			+ to_wstring(mTrade.getPriceSell(vec[n].key))
			+ L"\n ";
	}
}
void logics::print(int type) {
#if defined(_WIN32) && !defined(COCOS2D_DEBUG)
	if (type == 0) {
		wstring sz, szGrowth, szHP, szRace, szAdorn;
		vector<intPair> vec;
		mActor->inven.getWarehouse(vec);
		for (int n = 0; n < (int)vec.size(); n++) {
			sz += to_wstring(vec[n].key) + L"-" + mItems[vec[n].key].name + L"(" + to_wstring(vec[n].val) + L")\n ";
		}
		int hp = getHP();
		printf(" %ls(%ls) lv.%d(exp.%d / %d) hp: (%d / %d)\n %ls\n Point:%d \n 체력: %d, 지력: %d, 매력: %d \n\n 고양이 가방 --- \n %ls \n"
			, mActor->name.c_str()
			, mActor->userName.c_str()
			, mActor->level
			, mActor->exp
			, getMaxExp()
			, hp
			, getMaxHP()
			, mActor->jobTitle.c_str()
			, mActor->point
			, mActor->property.strength
			, mActor->property.intelligence
			, mActor->property.appeal
			, sz.c_str()
		);

	}else if (type == 1) {
		printf("[Training]\n ------------------------------------------------------------------------ \n");

		for (__training::iterator it = mTraining.begin(); it != mTraining.end(); ++it) {
			if (!isValidTraningTime(it->first))
				continue;

			wstring rewardItems;
			wstring costItems;
			
			for (int n = 0; n < maxTrainingItems; n++) {
				_itemPair* p = it->second.reward.items[n];
				if (p != NULL) {
					rewardItems += mItems[p->itemId].name + L"(";
					rewardItems += to_wstring(p->val) + L") ";
				}
				p = it->second.cost.items[n];
				if (p != NULL) {
					costItems += mItems[p->itemId].name + L"(";
					costItems += to_wstring(p->val) + L") ";
				}
			}			
			printf(" ID: %d,\t %ls ($%d) [%ls] S(%d) I(%d) A(%d) \n \t Reward $%d  [%ls] S(%d) I(%d) A(%d) \n\n"
				, it->first
				, it->second.name.c_str()
				, it->second.cost.point
				, costItems.c_str()
				, it->second.cost.strength
				, it->second.cost.intelligence
				, it->second.cost.appeal
				
				, it->second.reward.point
				, rewardItems.c_str()
				, it->second.reward.strength
				, it->second.reward.intelligence
				, it->second.reward.appeal
				
			);
		}
	}else if (type == 2) { //상품 구매
		printf("[Buy]\n ------------------------------------------------------------------------ \n");
		trade::tradeMap * m = mTrade.get();
		for (trade::tradeMap::iterator it = m->begin(); it != m->end(); ++it) {
			if (mItems[it->first].type < itemType_max) {
				wstring szColor = L"[0m";
				if (mItems[it->first].type > itemType_race && mItems[it->first].type < itemType_adorn)
					szColor = L"[1;32m";

				printf("%c%ls ID: %03d, %d /%d \t %ls \n"
					, 27
					, szColor.c_str()
					, it->first
					, mTrade.getPriceBuy(it->first)
					, mTrade.getPriceSell(it->first)
					, mItems[it->first].name.c_str()
				);
			}
		}
	}
	else if (type == 3) // 상품보기
	{
		for (__items::iterator it = mItems.begin(); it != mItems.end(); ++it) {
			printf("[Items] ID: %d,\t Type: %d,\t %ls \n", it->first, it->second.type, it->second.name.c_str());
		}
	}
	else if (type == 4) { //도감 보기
		for (__items::iterator it = mItems.find(collectionStartId); it != mItems.end(); ++it) {
			if (it->second.type == itemType_collection) {
				bool has = false;
				if (mActor->collection.find(it->first) != mActor->collection.end())
					has = true;
				printf(" [%ls] %ls(%d) lv.%d \n"
					, has ? L"O" : L" "
					, it->second.name.c_str()
					, it->first
					, it->second.grade
				);
			}	
		}
	}
	else if (type == 5) { //경묘 목록
		printf("------------- 경묘 대회 목록 -------------\n");
		for (raceMeta::iterator it = mRace.begin(); it != mRace.end(); ++it) {
			printf("\n");
			printf("[%03d] %ls \n▷ 참가비: %d, Mode: %d m, lv.%d  \n▷ 우승상금 --------\n"
				, it->second.id
				, it->second.title.c_str()
				, it->second.fee
				, it->second.mode
				, it->second.level
			);
			
			for (int m = 0; m < (int)it->second.rewards.size(); m++) {
				printf("%d등 상금: %d\n"
					, m+1 
					, it->second.rewards[m].prize					
				);
			}
		}
		printf("\n");
	}
	else if (type == 6) { //아이템 판매
		printf("[Sell]\n ------------------------------------------------------------------------ \n");
		wstring szGrowth, szHP, szRace, szAdorn, szFarm;
		printInven(inventoryType_growth, szGrowth);
		printInven(inventoryType_HP, szHP);
		printInven(inventoryType_race, szRace);
		printInven(inventoryType_adorn, szAdorn);
		printInven(inventoryType_farming, szFarm);

		printf("성장 아이템 ----\n %ls \n", szGrowth.c_str());
		printf("HP 아이템 ----\n %ls \n", szHP.c_str());
		printf("경묘 아이템 ----\n %ls \n", szRace.c_str());
		printf("농사 아이템 ----\n %ls \n", szFarm.c_str());
		printf("꾸미기 아이템 ----\n %ls \n", szAdorn.c_str());
    } else if(type == 7){ //farming
		wchar_t * szStatus[] = {
			L"새싹"
			, L"돌봄 필요"
			, L"건강"
			, L"썩음"
			, L"다 자람"
			, L" "
		};
        farming::fields* f = mFarming.getFields();
		printf("--------------------------- Farming\n");
        for(int n = 0; n < (int)f->size(); n++){
			if (f->at(n) == NULL)
				continue;

            printf("%d. [%ls] seed id: %d, 가꿈 횟수: %d, 자란 시간: %I64d sec\n"
				, n
				, szStatus[(int)f->at(n)->status]
				, f->at(n)->seedId
				, f->at(n)->cntCare				
				, f->at(n)->timePlant == 0 ? 0: getNow() - f->at(n)->timePlant
			);
        }
		printf("-----------------------------------\n");
    }
#endif
}
int logics::getTradeInvenTypeCnt(inventoryType type) {
	if (mInvenTypeCntMap.find(type) == mInvenTypeCntMap.end())
		return 0;
	else
		return mInvenTypeCntMap[type];

}
bool logics::insertItem(_item item) {
	mItems[item.id] = item;
	
	//inventype별로 카운팅
	inventoryType invenType = getInventoryType(item.id);
	if (mInvenTypeCntMap.find(invenType) == mInvenTypeCntMap.end())
		mInvenTypeCntMap[invenType] = 1;
	else
		mInvenTypeCntMap[invenType]++;

	if (invenType == inventoryType_race)
		mItemRace.push_back(item.id);
    //trade 정보 
	trade::tradeProduct * p = new trade::tradeProduct;
	p->itemId = item.id;
	p->level = item.grade;
	p->price = 0;
	mTrade.add(p);
	return true;
}

bool logics::insertTraining(_training traning) {
		mTraining[traning.id] = traning;
	return true;
}
/*
bool logics::setTradeMarketPrice() {
	time_t now = time(0);
	if (now - mLastTradeUpdate < tradeUpdateInterval)
		return false;

	mLastTradeUpdate = now;

	unsigned int avg = (unsigned int)(maxTradeValue / mItems.size());
	mTrade.clear();
	for (__items::iterator it = mItems.begin(); it != mItems.end(); ++it) {
		mTrade[it->first] = avg;		
	}

	int sum = 0;
	size_t cnt = 0;
	for (__keyValInt::iterator it = mTrade.begin(); it != mTrade.end(); ++it) {
		//int id = it->first;
		int r = (rand() % it->second);
		int val = r - (it->second / 2);

		it->second += val;
		sum += it->second;
		
		int gap = val / int(mTrade.size() - cnt);
		for (__keyValInt::iterator it2 = next(it); it2 != mTrade.end(); ++it2) {
			it2->second -= gap;
		}
		cnt++;
	}

	//printf("sum = %d \n", sum);
	return true;
}
*/
errorCode logics::isValidTraining(int id) {
	//check validation
	if (mTraining.find(id) == mTraining.end()) {
		return error_invalid_id;
	}

	if (isValidTraningLevel(id) == false) {
		return error_not_enough_level;
	}

	if (isValidTraningTime(id) == false) {
		return error_invalid_time;
	}

	_training t = mTraining[id];
	if (getHP() <= 0) {
		return error_not_enough_hp;
	}
	else if (t.cost.point > mActor->point)
		return error_not_enough_point;
	else if (t.cost.strength > mActor->property.strength || t.cost.intelligence > mActor->property.intelligence || t.cost.appeal > mActor->property.appeal)
		return error_not_enough_property;

	//check item validation
	for (int n = 0; n < maxTrainingItems; n++) {
		_itemPair * p = t.cost.items[n];
		if (p == NULL)
			break;

		if(!mActor->inven.checkItemQuantity(inventoryType_growth, p->itemId, p->val))
			return error_not_enough_item;
			/*
		if (mActor->inventory.growth.find(p->itemId) == mActor->inventory.growth.end()
			|| mActor->inventory.growth[p->itemId] < p->val)
			return error_not_enough_item;
			*/
	}

	return error_success;
}

errorCode logics::runTraining(int id, itemsVector &rewards, _property * rewardProperty, int &point, trainingType &type, float preservationRatio) {
    errorCode err = isValidTraining(id);
    if (err != error_success) {
        return err;
    }
	//hp
	increaseHP(-1);
	//pay point
	mActor->point -= mTraining[id].cost.point;
	//substract property
	addProperty(mTraining[id].cost.strength * -1, mTraining[id].cost.intelligence * -1, mTraining[id].cost.appeal * -1);
	
	//substract item
	for (int n = 0; n < maxTrainingItems; n++) {
		_itemPair * p = mTraining[id].cost.items[n];
		if (p == NULL)
			break;
		if(!addInventory(p->itemId, p->val * -1))
			return error_not_enough_item;		
	}

	//preservationRaio만큼 보전해서 줌
	//give point
	int preservation = (int)(preservationRatio * (float)mTraining[id].reward.point);
	printf("point: %d \n" , preservation);
	point = getRandValue(mTraining[id].reward.point);
	if (point < preservation)
		point = preservation;

	mActor->point += point;
	//give growth
	preservation = (int)(preservationRatio * (float)mTraining[id].reward.strength);
	printf("strength: %d \n", preservation);
	rewardProperty->strength = getRandValue(mTraining[id].reward.strength);
	if (rewardProperty->strength < preservation)
		rewardProperty->strength = preservation;

	preservation = (int)(preservationRatio * (float)mTraining[id].reward.intelligence);
	printf("intelligence: %d \n", preservation);
	rewardProperty->intelligence = getRandValue(mTraining[id].reward.intelligence);
	if (rewardProperty->intelligence < preservation)
		rewardProperty->intelligence = preservation;

	preservation = (int)(preservationRatio * (float)mTraining[id].reward.appeal);
	printf("appeal: %d \n", preservation);
	rewardProperty->appeal = getRandValue(mTraining[id].reward.appeal);
	if (rewardProperty->appeal < preservation)
		rewardProperty->appeal = preservation;

	addProperty(rewardProperty->strength
		, rewardProperty->intelligence
		, rewardProperty->appeal);

	//give only 1 item	
	int cnt = 0;
	for (int n = 0; n < maxTrainingItems; n++) {
		_itemPair * p = mTraining[id].reward.items[n];
		if (p == NULL) {
			cnt = n;
			break;
		}		
	}
	if(cnt > 0){
		int idx = (rand() % cnt);
		_itemPair * p = mTraining[id].reward.items[idx];
		_itemPair item;
		item.itemId = p->itemId;
		item.val = (rand() % p->val);
		if (item.val > 0) {			
			if (!addInventory(item.itemId, item.val))
				return error_not_enough_item; //어차피 추가라 이부분은 필요 없지만 걍 넣어둠

			rewards.push_back(item);
		}
	}
	type = mTraining[id].type;
	//업적
	mQuest.push(achievement_category_training, id, 1);
	if (increaseExp(mTraining[id].level))
		return error_levelup;
	return error_success;
}

errorCode logics::runTrade(bool isBuy, int id, int quantity) {
	if (!mTrade.exist(id)) {
		return error_invalid_id;
	}
	if (mItems[id].type >= itemType_max) {
		return error_invalid_id;
	}
	int amount;
	if (isBuy) {
		amount = mTrade.getPriceBuy(id) * quantity;
		if (mActor->point < amount) 
			return error_not_enough_point;		
		//substract point
		amount = amount * -1;
	}		
	else {
		amount = mTrade.getPriceSell(id) * quantity;
		quantity = quantity * -1;
	}	
	//give items
	_item item = mItems[id];

	if (!addInventory(item.id, quantity))
		return error_not_enough_item;

	mActor->point += amount;

	if (amount < 0) {
		mQuest.push(achievement_category_trade_buy, 0, 1);
		mQuest.push(achievement_category_trade_buy, id, quantity);
	}
	else if (amount > 0){
		mQuest.push(achievement_category_trade_sell, 0, 1);
		mQuest.push(achievement_category_trade_sell, id, quantity * -1);
	}

	return error_success;
}

errorCode logics::runRecharge(int id, int quantity) {
	
	if (getHP() == getMaxHP())
		return error_full_hp;
	if (getInventoryType(id) != inventoryType_HP)
		return error_invalid_id;
	if (!addInventory(id, quantity * -1)) 
		return error_not_enough_item;
	
	int val = mItems[id].value * quantity;
	increaseHP(val);
	mQuest.push(achievement_category_recharge, 0, quantity);
	return error_success;
}

//add inventory
bool logics::addInventory(int itemId, int quantity) {
	inventoryType type = getInventoryType(itemId);
	if (type == inventoryType_max)
		return false;
	if (type == inventoryType_collection)
		mActor->collection[itemId] = true;
	else {
		if(quantity > 0)
			mActor->inven.pushItem(type, itemId, quantity);
		else {
			if (!mActor->inven.popItem(type, itemId, quantity * -1))
				return false;
		}
	}

	return true;
}
//increase property
void logics::addProperty(int strength, int intelligence, int appeal) {
	mActor->property.strength += strength;
	mActor->property.intelligence += intelligence;
	mActor->property.appeal += appeal;

	mQuest.push(achievement_category_property, achievement_property_id_total, strength + intelligence + appeal);
	mQuest.push(achievement_category_property, achievement_property_id_S, strength);
	mQuest.push(achievement_category_property, achievement_property_id_I, intelligence);
	mQuest.push(achievement_category_property, achievement_property_id_A, appeal);
}

//경험치 증가
bool logics::increaseExp(int value) {
	mActor->exp += value;

	int maxExp = getMaxExp();
	if (maxExp <= mActor->exp) {
		mActor->level++;
		
		mActor->exp = 0;
		setMaxHP();
		//give $
		mActor->point += mActor->level * bonusCashPerLevel;
		//set job title
		setJobTitle();
		
		return true;
	}	
	return false;
}
int logics::getMaxExp() {
	return 1 << mActor->level;
}
int logics::getMaxHP() {
	return (int)(defaultHP + (1.5*mActor->level));
}
void logics::setMaxHP() {	
	mActor->hp = getMaxHP();	
}

bool logics::increaseHP(int val) {
	int maxHP = getMaxHP();	
	bool ret = true;
	lockHP.lock();
	if (val < 0 && mActor->hp + val < 0) {
		lockHP.unlock();
		return false;
	}
		
	if (val > 0 && maxHP == mActor->hp)
		ret = false;	
	else if (mActor->hp + val >= maxHP)
		setMaxHP();
	else
		mActor->hp += val;
	lockHP.unlock();

	return ret;
}

int logics::getHP() {
	lockHP.lock();
	int hp = mActor->hp;
	lockHP.unlock();
	return hp;
}

bool logics::rechargeHP() {
	time_t now = getNow();
	time_t waitTime = now - mActor->lastHPUpdateTime;
	if (waitTime >= HPIncreaseInterval) {		
		mActor->lastHPUpdateTime = now;
		return increaseHP((int)((float)waitTime / (float)HPIncreaseInterval));
	}
	return false;
}

bool logics::isValidTraningLevel(int id) {
	_training* t = &mTraining[id];

	if (mActor->level < t->level)
		return false;

	return true;
}

bool logics::isValidTraningTime(int id) {

	_training* t = &mTraining[id];

	if (t->start == 0)
		return true;

	time_t now = time(0);
	const int c = (t->keep + t->interval);
	time_t end = t->start + (t->count * c);
	if (now < t->start || end < now)
		return false;

	for (int x = 0; x < t->count; x++) {
		time_t s = t->start + (x * c);
		time_t e = s + t->keep;

		if (s <= now && now < e)
			return true;
	}
	
	return false;
}

void logics::setDefaultJobTitle(wstring sz) {
	mJobTitle._default = sz;
}
void logics::addJobTitlePrefix(_jobTitlePrefix& prefix) {
	mJobTitle.prefix.push_back(prefix);
}
void logics::addJobTitleBody(_jobTitleBody& body) {
	mJobTitle.body.push_back(body);
}

void logics::setJobTitle() {
	wstring szPrefix;
	wstring szBody = mJobTitle._default;
	
	for (int n = 0; n < (int)mJobTitle.prefix.size(); n++) {
		if (mActor->level <= mJobTitle.prefix[0].level) {
			szPrefix = mJobTitle.prefix[n].title;
			break;
		}			
	}

	int S = mActor->property.strength;
	int I = mActor->property.intelligence;
	int A = mActor->property.appeal;

	int sum = S + I + A;
	if (sum < 0) {		
		//mActor->jobTitle = szPrefix + L" " + szBody;
		mActor->jobTitle = wstring_to_utf8(szPrefix + L" " + szBody);
		return;
	}
	int min = std::min(std::min(S, I), A);
	if (min < 0) {
		S += min;
		I += min;
		A += min;
	}
	float sum2 = (float)(S + I + A);
	int pS, pI, pA;
	pS = (int)((S / sum2) * 100.0f);
	pI = (int)((I / sum2) * 100.0f);
	pA = (int)((A / sum2) * 100.0f);

	for (int n = 0; n < (int)mJobTitle.body.size(); n++) {
		if (pS >= mJobTitle.body[n].S
			&& pI >= mJobTitle.body[n].I
			&& pA >= mJobTitle.body[n].A) {
			szBody = mJobTitle.body[n].title;
			break;
		}
	}

	//mActor->jobTitle = szPrefix + L" " + szBody;
	mActor->jobTitle = wstring_to_utf8(szPrefix + L" " + szBody);
	
	return;
}

void logics::addRaceMeta(_race & race) {
	mRace[race.id] = race;
}

errorCode logics::runRaceValidate(int id) {
	if (mRace.find(id) == mRace.end())
		return error_invalid_id;

	if (mActor->property.total() <= 0) {
		return error_not_enough_strength;
	}
	if (getHP() < 1) {
		return error_not_enough_hp;
	}

	if (mActor->point < mRace[id].fee)
		return error_not_enough_point;

	return error_success;
}

errorCode logics::runRaceSetRunners(int id) {
	if (mRace.find(id) == mRace.end())
		return error_invalid_id;

	_race race = mRace[id];

	if (mActor->property.total() < race.min) {
		return error_not_enough_property;
	}
	if (!increaseHP(-1)) {
		return error_not_enough_hp;
	}

	if (mActor->point < race.fee)
		return error_not_enough_point;
	
	mActor->point -= race.fee;
	mRaceCurrent.id = id;
	mRaceCurrent.prize = 0;
	mRaceCurrent.rewardItemId = 0;
	mRaceCurrent.rewardItemQuantity = 0;

	mRaceParticipants->clear();
	//내 능력치랑 비슷하게 구성
	int sum = min(race.max, mActor->property.total()); //경묘 능력 최대치랑 유저 능력치 중 낮은 값으로 설정
	if (race.mode == race_mode_speed) {
		sum += (int)((float)sum * raceAIAdvantageRatio * (float)max(1, getRandValue(4))); // raceAIAdvantageRatio * 1 ~ 3 올림
	}
	else {
		sum += (int)((float)sum * raceAIAdvantageRatio); // raceAIAdvantageRatio만 올림
	}

	
    //참가자 목록
	for (int n = 0; n < raceParticipantNum; n++) {
		_raceParticipant p;
		p.idx = n;
		if ((race.mode == race_mode_1vs1 && n == 0) || race.mode != race_mode_1vs1) {
			p.strength = getRandValue(sum);
			p.strength == 0 ? p.strength = 1 : p.strength = p.strength;
			p.intelligence = getRandValue(sum - p.strength);
			p.appeal = sum - p.strength - p.intelligence;
		}

		//AI advantage
		//p.strength += (int)(p.strength * raceAIAdvantageRatio * mRace[id].level);
		
		mRaceParticipants->push_back(p);
	}	

	if (mRaceWin.raceId != id) {
		mRaceWin.init(id);
	}
	return error_success;
}
errorCode logics::runRaceSetItems(itemsVector &items) {
	/*
	for (int m = 0; m < (int)items.size(); m++) {
		if (!mActor->inven.checkItemQuantity(inventoryType_race, items[m].itemId, items[m].val))
			return error_not_enough_item;
	}	
	*/
	//차감
	int mPrice = 0;
	for (int m = 0; m < (int)items.size(); m++) {
		mPrice += mTrade.getPriceBuy(items[m].itemId);		
	}
	if (mActor->point < mPrice)
		return error_not_enough_point;
	else
		mActor->point -= mPrice;


	_raceParticipant p;
	p.idx = raceParticipantNum;
	p.strength = mActor->property.strength;
	p.intelligence = mActor->property.intelligence;
	p.appeal = mActor->property.appeal;

	int idx = 0;
	for (int m = 0; m < (int)items.size(); m++) {
		for (int k = 0; k < items[m].val; k++) {
			p.items[idx] = items[m].itemId;
			//if (!addInventory(items[m].itemId, -1))	return error_not_enough_item;
			idx++;
			mQuest.push(achievement_category_race_use_item, items[m].itemId, 1);
			mQuest.push(achievement_category_race_use_item_type, mItems[items[m].itemId].type, 1);
		}
	}
	mRaceParticipants->push_back(p);
	
	return error_success;
}

errorCode logics::runRace(int id, itemsVector &items) {
	errorCode err = runRaceSetRunners(id);
	if (err != error_success)
		return err;
	return runRaceSetItems(items);
}

int logics::getRaceReward(int id, int rankIdx) {
	_race race = mRace[id];
	//최대 능력치 보다 유저 능력치가 높으면 참가비 보다 상금이 적음.
	//1등은 참가비 + 20%
	//2등은 참가비 + 10%
	if (race.max < mActor->property.total()) {
		return race.fee - (int)(race.fee * (rankIdx + 1) * 0.1);
	}	
	if (race.rewards.size() <= rankIdx)
		return 0;

	return race.rewards.at(rankIdx).prize;
}

void logics::invokeRaceByRank(int rank, itemType type, int quantity) {
	for (int n = 0; n <= raceParticipantNum; n++) {
		if (mRaceParticipants->at(n).currentRank == rank) {
			for(int m=0; m < quantity; m++)
				mRaceParticipants->at(n).sufferItems.push(type);
			return;
		}
	}
}

void logics::invokeRaceItem(int seq, itemType type, int quantity, int currentRank) {
	if (mRaceParticipants->at(seq).shootItemCount >= raceItemSlot)
		return;
	//아이템 대상에게 적용
	switch (type) {
	case itemType_race_shield:		//방어 쉴드
		while (!mRaceParticipants->at(seq).sufferItems.empty())
		{
			mRaceParticipants->at(seq).sufferItems.pop();
		}
		mRaceParticipants->at(seq).sufferItems.push(itemType_race_shield);
		break;
	case itemType_race_speedUp:		//속업
		for (int n = 0; n < quantity; n++) {
			mRaceParticipants->at(seq).sufferItems.push(type);
		}
		break;
	case itemType_race_attactFront:	//전방 공격
		if (currentRank > 1)
			invokeRaceByRank(currentRank - 1, itemType_race_attactFront, quantity);
		break;
	case itemType_race_attactFirst:	//1등 공격
		if (currentRank > 1)
			invokeRaceByRank(1, itemType_race_attactFirst, quantity);
		break;

        default:
            break;
	}
	mRaceParticipants->at(seq).shootItemCount++;
	mRaceParticipants->at(seq).shootCurrentType = type;
	mRaceParticipants->at(seq).shootCurrentQuantity = quantity;
}

void logics::invokeRaceItemByIdx(int seq, int itemIdx) {
	//아이템 목록에서 제거
	int itemId = mRaceParticipants->at(seq).items[itemIdx];
	int currentRank = mRaceParticipants->at(seq).currentRank;
	int quantity = mItems[itemId].value;
	mRaceParticipants->at(seq).items[itemIdx] = 0;
	if (itemId < 1) {
		return;
	}

	invokeRaceItem(seq, mItems[itemId].type, quantity, currentRank);
}

void logics::invokeRaceItemAI() {
	_race race = mRace[mRaceCurrent.id];
	int level = race.level;
	for (int i = 0; i < raceParticipantNum; i++) {

		if (race.mode == race_mode_1vs1 && i > 0) //1:1모드에선 0번만 사용
			return;

		//1등이 raceInvokeThreshold 이상 달리고 나서 부터 아이템 사용
		if (mRaceOrderedVector.size() > 0 && mRaceParticipants->at(mRaceOrderedVector[0].idx).ratioLength < raceInvokeThreshold)
			return;
		//아이템 사용할지 않할지 판단
		int r = getRandValue(raceAIRandom);
		if (r != 0)
			continue;
		//아이템 사용 횟수 초과시 
		if (mRaceParticipants->at(i).shootItemCount >= raceItemSlot) {
			continue;
		}
		//사용
		//1. 앞으로 당할게 있으면 부적 사용
		if (mRaceParticipants->at(i).sufferItems.size() > 0) {
			switch (mRaceParticipants->at(i).sufferItems.front()) {
			case itemType_race_shield:
			case itemType_race_speedUp:
				break;
			default:
				invokeRaceItem(i, itemType_race_shield, level, mRaceParticipants->at(i).currentRank);
				return;
			}
		}
		int quantity = level + getRandValue(3);
		switch (mRaceParticipants->at(i).currentRank) {
		case 1: // 1등이면 50%이상 왔을때 스피드 업
			if (mRaceParticipants->at(i).ratioLength > raceSpurt)
				invokeRaceItem(i, itemType_race_speedUp, quantity, mRaceParticipants->at(i).currentRank);
			break;
		case 2: //2, 3등이면 앞 고냥이 공격. 50%이상 왔을 부터 스퍼트	
		case 3:
			invokeRaceItem(i, itemType_race_attactFront, quantity, mRaceParticipants->at(i).currentRank);
			invokeRaceItem(i, itemType_race_speedUp, quantity, mRaceParticipants->at(i).currentRank);
			break;
		case 4:  //4, 5등이면 1등 공격
		case 5:
			invokeRaceItem(i, itemType_race_attactFirst, quantity, mRaceParticipants->at(i).currentRank);
			break;
		}
	}
}

int logics::getBaseSpeed(int s, int i, int a, float ranPercent /* 달린 거리 */, int boost) {
	
	int boostLength = (int)((float)(s + i + a) * raceTouchBoostRatio * (float)boost / 100.f);//최대 raceTouchBoostRatio까지만 부스트	
	/* 전반은 I:S = 7: 3 후반은 S:I 7:3 */
	int s1 = (int)((ranPercent <= 50.f) ? s * 0.3 : s * 0.7);
	int i1 = (int)((ranPercent <= 50.f) ? i * 0.7 : i * 0.3);
	int a1 = getRandValue(a); //raceAppealRatio
	return (s1 + i1 + a1) + boostLength;
	
	/*
	// 체력 50% + 지력 100% + rand(매력) - (전체 * (1 - 체력비율) * 달린거리 / 100 )
	float total = (float)(s + i + a);
	float s1 = (float)s / 2.f;
	float i1 = (float)i;
	float a1 = (float)getRandValue(a * raceAppealRatio);

	int length = (int)(s1 + i1 + a1);

	//지침 정도. 체력 100% = 0
	float tiredRatio = 1.f - ((float)s / total);
	int decrease = length / 3.f * tiredRatio * ranPercent / 100.f;

	return length - decrease;
	*/
}

raceParticipants* logics::getNextRaceStatus(bool &ret, int itemIdx, int boost) {
	 int lastRank = 0;	 
	 int raceLength = (int)((float)mActor->property.total() / 2.f * 0.5f * 30.f * 5.f);
	 //순위 산정용 벡터
	 mRaceOrderedVector.clear();

	 for (int n = 0; n < (int)mRaceParticipants->size(); n++) {
		 // 순위 산정 하는 김에 사용 아이템 초기화.
		 mRaceParticipants->at(n).shootCurrentType = itemType_max;
		 mRaceParticipants->at(n).shootCurrentQuantity = 0;

		 if (mRaceParticipants->at(n).rank == 0) {
			 mRaceOrderedVector.push_back(mRaceParticipants->at(n));
		 }
		 else {
			 mRaceParticipants->at(n).currentRank = 0;
		 }
		 if (mRaceParticipants->at(n).rank > 0) {
			 lastRank++;
		 }			 
	 }

	 //현재 순위 산정
	 sort(mRaceOrderedVector.begin(), mRaceOrderedVector.end());
	 for (int n = 0; n < (int)mRaceOrderedVector.size(); n++) {
		 mRaceParticipants->at(mRaceOrderedVector[n].idx).currentRank = n + 1;
	 }

	 //내가 사용한 아이템 발동
	 if (itemIdx > -1) {
		 invokeRaceItemByIdx(raceParticipantNum, itemIdx);
	 }
	 //AI 아이템 발동
	 if(mRace[mRaceCurrent.id].mode != race_mode_speed)
		invokeRaceItemAI();

	 //진행 값 설정
	 for (int n = 0; n < (int)mRaceParticipants->size(); n++) {
		 if (mRaceParticipants->at(n).rank > 0)
			 continue;

		 //공격받거나 자신한테 사용한 아이템 꺼내기.
		 mRaceParticipants->at(n).currentSuffer = itemType_max;
		 if (mRaceParticipants->at(n).sufferItems.size() > 0) {
			 mRaceParticipants->at(n).currentSuffer = mRaceParticipants->at(n).sufferItems.front();
			 mRaceParticipants->at(n).sufferItems.pop();
		 }
		 
		 //기초 체력 + random appeal		 
		 int length = getBaseSpeed(
			 mRaceParticipants->at(n).strength
			 , mRaceParticipants->at(n).intelligence
			 , mRaceParticipants->at(n).appeal
			 , mRaceParticipants->at(n).ratioLength
			 , (n == raceParticipantNum) ? boost : 0	//boost 설정. 0 ~ 100
		 );
		 
		 switch (mRaceParticipants->at(n).currentSuffer) {
		 case itemType_race_shield:
			 if (mRaceParticipants->at(n).sufferItems.size() > 0 ) {
				 switch (mRaceParticipants->at(n).sufferItems.front()) {
				 case itemType_race_attactFront:
				 case itemType_race_attactFirst:
					 mRaceParticipants->at(n).sufferItems.pop();
					 break;
					 default:
						 break;
				 }
			 }
			 break;
		 case itemType_race_speedUp:			 
			 length = (int)((float)mRaceParticipants->at(n).total() * raceSpeedUp);
			 break;
		 case itemType_race_attactFront:
		 case itemType_race_attactFirst:
			 length = 0;
			 break;
		 default:
			 break;
		 }

		 mRaceParticipants->at(n).totalLength += length;
		 mRaceParticipants->at(n).currentLength = length;
		 mRaceParticipants->at(n).ratioLength = (((float)mRaceParticipants->at(n).totalLength / (float)raceLength) * 100.0f);
		 if (mRaceParticipants->at(n).totalLength >= raceLength)
			 mRaceParticipants->at(n).rank = lastRank + 1;
	 }
	 
	 // 순위 결정
	 if ((mRace[mRaceCurrent.id].mode == race_mode_1vs1 && lastRank == 2) || lastRank >= raceParticipantNum) {
		 //if (mRaceParticipants->at(raceParticipantNum).rank != 0 ) { 
		 ret = false;
		 for (int n = 0; n < (int)mRaceParticipants->size(); n++) {
			 if (mRaceParticipants->at(n).rank == 0)
				 mRaceParticipants->at(n).rank = raceParticipantNum + 1;
		 }
		 //보상 지급
		 mRaceCurrent.rank = mRaceParticipants->at(raceParticipantNum).rank;
		 //업적
		 mQuest.push(achievement_category_race, achievement_race_id_try, 1);	//경묘 전체
		 achievement_category ac;
		 switch (mRace[mRaceCurrent.id].mode) {
		 case race_mode_item:
			 ac = achievement_category_race_item;
			 break;
		 case race_mode_speed:
			 ac = achievement_category_race_speed;
			 break;
		 case race_mode_1vs1:
			 ac = achievement_category_race_1vs1;
			 break;
		 case race_mode_friend_1:
			 ac = achievement_category_race_friend_1;
			 break;
		 }
		 mQuest.push(ac, achievement_race_id_try, 1); //모드별 플레이 횟수
		 
		 if (mRaceCurrent.rank == 1) {
			 mQuest.push(achievement_category_race, achievement_race_id_first, 1);
			 mQuest.push(ac, achievement_race_id_first, 1);
			 
			 mRaceWin.winCnt++;

		 }			 
		 else if (mRaceCurrent.rank == 2) {
			 mQuest.push(achievement_category_race, achievement_race_id_second, 1);
			 mQuest.push(ac, achievement_race_id_second, 1);
		 }			 
			 
		 for (int n = 0; n < (int)mRace[mRaceCurrent.id].rewards.size(); n++) {
			 if (mRaceParticipants->at(raceParticipantNum).rank - 1 == n) {
				 //상금 지급
				 mActor->point += getRaceReward(mRaceCurrent.id, n); //mRace[mRaceCurrent.id].rewards[n].prize;

				 if (mRace[mRaceCurrent.id].rewards[n].items.size() > 0) {
					 int idx = getRandValue((int)mRace[mRaceCurrent.id].rewards[n].items.size());
					 int itemId = mRace[mRaceCurrent.id].rewards[n].items[idx].itemId;
					 int val = mRace[mRaceCurrent.id].rewards[n].items[idx].val;
					 if (!addInventory(itemId, val)) {
						 CCLOG("getNextRaceStatus addInventory failure itemId=%d, val= %d", itemId, val);
					 }

					 mRaceCurrent.rewardItemId = itemId;
					 mRaceCurrent.rewardItemQuantity = val;
				 }
				 mRaceCurrent.prize = mRace[mRaceCurrent.id].rewards[n].prize;

				 break;
			 }
		 }
	}		
	 else {
		 ret = true;
	}

	return mRaceParticipants;
}

int logics::getRandomRaceItem() {
	int idx = getRandValue((int)mItemRace.size());
	return mItemRace[idx];
}
/* --------------
	farming
	-------------- */
errorCode logics::farmingHarvest(int idx, int &productId, int &earning) {	
	if(!mFarming.harvest(idx, productId, earning))	
		return error_invalid_id;
	
	mActor->inven.pushItem(getInventoryType(productId), productId, earning);
	//increaseExp();

	mQuest.push(achievement_category_farming, achievement_farming_id_harvest, 1);
	mQuest.push(achievement_category_farming, achievement_farming_id_output, earning);
	return error_success;
};

errorCode logics::farmingPlant(int idx, int seedId) {
	int price = mTrade.getPriceBuy(seedId);
	if (mActor->point < price)
		return error_not_enough_point;

	//if (!mActor->inven.popItem(inventoryType_farming, seedId, 1))
	//	return error_not_enough_item;

	if (!mFarming.plant(idx, seedId))
		return error_farming_failure;

	mActor->point -= price;
	
	mQuest.push(achievement_category_farming, achievement_farming_id_plant, 1);
	return error_success;
};
errorCode logics::farmingCare(int idx) {
	int boost = 0;
	//지능이 farmGluttonyIntelThreshold 이하면 농작물 훔쳐먹음
	float ratio = ((float)mActor->property.intelligence / (float)mActor->property.total()) * 100;
	if (ratio < farmGluttonyIntelThreshold) {
		boost = farmGluttony * -1;
	}
	if (mFarming.care(idx, boost)) {

		mQuest.push(achievement_category_farming, achievement_farming_id_care, 1);

		if (boost == 0)
			return error_success;
		else
			return error_farming_gluttony;
	}
	return error_farming_failure;
}
errorCode logics::farmingExtend(int x, int y)
{
	if (mActor->point < mFarmingExtendFee)
		return error_not_enough_point;
	
	mActor->point -= mFarmingExtendFee;

	farmingAddField(x, y);
	
	//if (increaseExp())	return error_levelup;
	return error_success;
}

farming::field * logics::farmingAddField(int x, int y) {	
	return mFarming.addField(x, y);	
}

void logics::farmingAddField(farming::field * f) {
	mFarming.addField(f);
}

int logics::farmingQuestReward(int idx) {
	int money = 0;

	farming::quest * q = mFarming.getQuest(idx);
	if (q) {		
		for (int n = 0; n < sizeof(q->items) / sizeof(q->items[0]); n++) {
			if (q->items[n].itemId != -1) {
				money += mTrade.getPriceBuy(q->items[n].itemId) * q->items[n].quantity;
			}
		}		
	}

	return money;
}

bool logics::farmingQuestDone(int idx) {
	farming::quest * q = mFarming.getQuest(idx);
	if (q) {
		int money = 0;
		for (int n = 0; n < sizeof(q->items) / sizeof(q->items[0]); n++) {
			if (q->items[n].itemId != -1) {
				money += mTrade.getPriceBuy(q->items[n].itemId) * q->items[n].quantity;

				inventoryType type = getInventoryType(q->items[n].itemId);
				if (!mActor->inven.popItem(type, q->items[n].itemId, q->items[n].quantity))
					return false;
			}
		}
		mActor->point += money;
		mFarming.clearQuest(idx);
	}
	return true;
};
//farming end

void logics::achievementCallbackFn(int uniqueId, int idx) {
	printf("%d achievementCallback \n", idx);
	/*
	achievement::detail d;
	hInst->mAchievement.getDetail(d, type, idx);

	hInst->mAchievement.rewardReceive(type, idx);
	hInst->addInventory(d.rewardId, d.rewardVal);
	*/
	hInst->mAchieveCB(uniqueId, idx);
}

void logics::threadRun()
{
	int count = 0;
	while (logics::hInst->mIsRunThread) {
		sleepThisThread(SEC);
		if (count >= actorSaveInterval) {
			logics::hInst->saveActor();
			count = 0;
		}
		count++;
	}
}

inventoryType logics::getInventoryType(int itemId) {
	if (mItems.find(itemId) == mItems.end())
		return inventoryType_max;

	_item item = mItems[itemId];
	inventoryType t;
	if (item.type >= itemType_training && item.type < itemType_hp) {
		t = inventoryType_growth;
	}
	else if (item.type >= itemType_hp && item.type < itemType_race) {
		t = inventoryType_HP;
	}
	else if (item.type >= itemType_race && item.type < itemType_adorn) {
		t = inventoryType_race;
	}
	else if (item.type >= itemType_adorn && item.type < itemType_farming) {
		t = inventoryType_adorn;
	}
	else if (item.type >= itemType_farming && item.type < itemType_max) {
		t = inventoryType_farming;
	}
	else {
		t = inventoryType_collection;
	}

	return t;
}
void logics::saveActorInventory(rapidjson::Document &d, rapidjson::Value &v, inventoryType type) {
	vector<intPair> vec;
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	mActor->inven.getWarehouse(vec, type);
	for (int n = 0; n < (int)vec.size(); n++) {

		rapidjson::Value objValue;
		objValue.SetObject();
		objValue.AddMember("id", vec[n].key, allocator);
		objValue.AddMember("quantity", vec[n].val, allocator);

		v.PushBack(objValue, allocator);
	}
}

void logics::saveActor() {

	if (hIsSync) {
		CCLOG("saveActor locked!!");
		return;
	}

	hIsSync = true;
	/*
	if (mActorStringFromJSON.size() == 0) {
#if defined(_WIN32) && !defined(COCOS2D_DEBUG)
		mActorStringFromJSON = loadJsonString(CONFIG_ACTOR);
#else
		mActorStringFromJSON = FileUtils::getInstance()->getStringFromFile(FileUtils::getInstance()->getWritablePath() + CONFIG_ACTOR);
#endif
	}
	*/

	int rc = 0;

    //rapidjson::Document d;
	//d.Parse(mActorStringFromJSON.c_str());

	string szQuery = "";
	char bufActor[1024] = { 0 };
	sprintf(bufActor
		, "UPDATE actor SET userId='%s', userName='%s', id='%s', name='%s', lastLoginLoginTime = %lld, lastLoginLogoutTime= %lld, lastHPUpdateTime=%lld, jobTitle= '%s', point = %d, hp = %d, exp = %d, level = %d, strength= %d, intelligence = %d, appeal= %d WHERE idx = 1;"
		, mActor->userId.c_str()
		, mActor->userName.c_str()
		, mActor->id.c_str()
		, mActor->name.c_str()
		, (int64_t)mActor->lastLoginLoginTime
		, (int64_t)mActor->lastLoginLogoutTime
		, (int64_t)mActor->lastHPUpdateTime
		, mActor->jobTitle.c_str()
		, mActor->point
		, mActor->hp
		, mActor->exp
		, mActor->level
		, mActor->property.strength
		, mActor->property.intelligence
		, mActor->property.appeal
	);
	
	szQuery += bufActor;
		
	/*
	string userName = wstring_to_utf8(mActor->userName);
	d["userName"] = rapidjson::StringRef(userName.c_str());
	d["userId"] = rapidjson::StringRef(mActor->userId.c_str());
	d["id"] = rapidjson::StringRef(mActor->id.c_str());
	string name = wstring_to_utf8(mActor->name);
	d["name"] = rapidjson::StringRef(name.c_str());
	d["lastLoginLoginTime"].SetInt64(mActor->lastLoginLoginTime);
	d["lastLoginLogoutTime"].SetInt64(mActor->lastLoginLogoutTime);
	d["lastHPUpdateTime"].SetInt64(mActor->lastHPUpdateTime);

	string jobTitle = wstring_to_utf8(mActor->jobTitle);
	d["jobTitle"] = rapidjson::StringRef(jobTitle.c_str());
	d["point"].SetInt(mActor->point);
	d["hp"].SetInt(mActor->hp);
	d["exp"].SetInt(mActor->exp);
	d["level"].SetInt(mActor->level);
	d["property"]["strength"].SetInt(mActor->property.strength);
	d["property"]["intelligence"].SetInt(mActor->property.intelligence);
	d["property"]["appeal"].SetInt(mActor->property.appeal);
	*/
	/*
	d["inventory"]["growth"].Clear();
	saveActorInventory(d, d["inventory"]["growth"], inventoryType_growth);
	d["inventory"]["race"].Clear();
	saveActorInventory(d, d["inventory"]["race"], inventoryType_race);
	d["inventory"]["adorn"].Clear();
	saveActorInventory(d, d["inventory"]["adorn"], inventoryType_adorn);
	d["inventory"]["HP"].Clear();
	saveActorInventory(d, d["inventory"]["HP"], inventoryType_HP);
	d["inventory"]["farming"].Clear();
	saveActorInventory(d, d["inventory"]["farming"], inventoryType_farming);
	*/

	//collection
	//d["collection"].Clear();		
	if (mActor->collection.size() > 0) {
		szQuery += "\nDELETE FROM collection;\nINSERT INTO collection(id) VALUES";
		for (keyBoolMap::iterator it = mActor->collection.begin(); it != mActor->collection.end(); ++it) {
			if (it->second == true) {
				//d["collection"].PushBack(it->first, d.GetAllocator());
				if(it != mActor->collection.begin())
					szQuery += ",";

				szQuery += "(" + to_string(it->first) + ")";
			}
		}
		szQuery += ";";
	}

	//inventory	
	vector<intPair> vec;
	mActor->inven.getWarehouse(vec);
	if (vec.size() > 0) {
		szQuery += "\nDELETE FROM inventory;\nINSERT INTO inventory(category, id, quantity) VALUES";
		for (int n = 0; n < (int)vec.size(); n++) {
			if (n > 0) {
				szQuery += ",";
			}
			szQuery += "(" + to_string((int)getInventoryType(vec[n].key)) + ", " + to_string(vec[n].key) + ", " + to_string(vec[n].val) + ")";
		}
		szQuery += ";";
	}
	
	//farming
	if (mFarming.countField() > 0) {
		int n = 0;		
		szQuery += "\nDELETE FROM farm;\nINSERT INTO farm(id, x, y, seedId, timePlant, cntCare, timeLastGrow, boost, level, accumulation) VALUES";
		char buffer[8 * 1024] = { 0, };
		int len = 0;

		farming::field * f;
		while (mFarming.getField(n++, f)) {
			if (n > 1)
				len += sprintf(buffer + len, ",");

			len += sprintf(buffer + len, "(%d, %d, %d, %d, %lld, %d, %lld, %d, %d, %d)"
				, f->id
				, f->x
				, f->y
				, f->seedId
				, (int64_t)f->timePlant
				, f->cntCare
				, (int64_t)f->timeLastGrow
				, f->boost
				, f->level
				, f->accumulation
			);
		}

		szQuery += buffer;
		szQuery += ";";
	}

	rc = Sql::inst()->exec(szQuery);
	if (rc != 0) {
		CCLOG("Actor Saving failure !!! \n rc: %d \n%s", rc, szQuery.c_str());
	}

	szQuery = "";

	//d["achievement"]["quests"].Clear();	
	if (mQuest.getQuests()->size() > 0) {
		int nCnt = 0;
		szQuery += "\nDELETE FROM quest;\nINSERT INTO quest(uniqueid, accumulation, isFinished, isReceived) VALUES";

		for (int n = 0; n < mQuest.getQuests()->size(); n++) {
			Quest::_quest * p = mQuest.getQuests()->at(n);
			/*
			rapidjson::Value objValue;
			objValue.SetObject();
			objValue.AddMember("level", n, d.GetAllocator());
			objValue.AddMember("category", p.category, d.GetAllocator());
			objValue.AddMember("id", p.id, d.GetAllocator());
			objValue.AddMember("accumulation", p.accumulation, d.GetAllocator());
			objValue.AddMember("isFinished", p.isFinished, d.GetAllocator());
			objValue.AddMember("isReceived", p.isReceived, d.GetAllocator());

			d["achievement"]["quests"].PushBack(objValue, d.GetAllocator());
			*/
			if (nCnt > 0) {
				szQuery += ",";
			}
			int isFinished = p->isFinished ? 1 : 0;
			int isReceived = p->isReceived ? 1 : 0;

			szQuery += "(" + to_string(p->uniqueId) + "," + to_string(p->accumulation) + "," + to_string(isFinished) + "," + to_string(isReceived) + ")";
			nCnt++;

		}
		szQuery += ";";
	}

	if (szQuery.size() > 10) {
		rc = Sql::inst()->exec(szQuery);
		if (rc != 0) {
			CCLOG("Actor Saving failure !!! \n rc: %d \n%s", rc, szQuery.c_str());
		}
	}
	
	szQuery = "";

	
	/*
	d["achievement"]["accumulation"].RemoveAllMembers();
	const rapidjson::Value& accumulation = d["achievement"]["accumulation"];
	queue<string> gabages;
	*/
	Quest::intDoubleDepthMap * pAccumulation = mQuest.getAccumulation();
	if (pAccumulation->size() > 0) {
		int nCnt = 0;
		szQuery += "\nDELETE FROM achievement_accumulation;\nINSERT INTO achievement_accumulation(category, id, cnt) VALUES";
		for (Quest::intDoubleDepthMap::iterator it = pAccumulation->begin(); it != pAccumulation->end(); ++it) {
			intMap * pIntMap = it->second;
			/*
			string category = to_string(it->first);
			gabages.push(category);

			if (d["achievement"]["accumulation"].HasMember(rapidjson::StringRef(category.c_str())) == false) {
				rapidjson::Value arr;
				arr.SetArray();
				d["achievement"]["accumulation"].AddMember(rapidjson::StringRef(category.c_str()), arr, d.GetAllocator());
			}
			*/
			for (intMap::iterator it2 = pIntMap->begin(); it2 != pIntMap->end(); ++it2) {
				/*
				rapidjson::Value objValue;
				objValue.SetObject();
				objValue.AddMember("id", it2->first, d.GetAllocator());
				objValue.AddMember("value", it2->second, d.GetAllocator());

				d["achievement"]["accumulation"].FindMember(rapidjson::StringRef(category.c_str()))->value.PushBack(objValue, d.GetAllocator());
				*/
				if (nCnt > 0) {
					szQuery += ",";
				}
				szQuery += "(" + to_string(it->first) + "," + to_string(it2->first) + "," + to_string(it2->second) +")";
				nCnt++;
			}
		}
		szQuery += ";";
	}

	if (szQuery.size() > 10) {
		rc = Sql::inst()->exec(szQuery);
		if (rc != 0) {
			CCLOG("Actor Saving failure !!! \n rc: %d \n%s", rc, szQuery.c_str());
		}
	}
	
	/*
	rapidjson::StringBuffer bufferJson;
	bufferJson.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(bufferJson);
	d.Accept(writer);
	//printf("%s \n", buffer.GetString());
		
	string input(bufferJson.GetString());
	//saveFile(CONFIG_ACTOR, input);
	string fullPath = FileUtils::getInstance()->getWritablePath() + CONFIG_ACTOR;
	bool ret = FileUtils::getInstance()->writeStringToFile(input, fullPath);
	if (!ret)
		CCLOG("failure save Actor");

	while (gabages.size() > 0) {
		string p = gabages.front();
		gabages.pop();
		//delete p;
	}
	*/
	hIsSync = false;
}