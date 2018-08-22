#include "logics.h"

#if !defined(_WIN32) || defined(COCOS2D_DEBUG)
    #include "cocos2d.h"
    USING_NS_CC;
#endif

logics * logics::hInst = NULL;

bool logics::init(farmingFinshedNotiCallback farmCB
	    , tradeUpdatedCallback tradeCB
	) {

    string szMeta;
#if defined(_WIN32) && !defined(COCOS2D_DEBUG)
		szMeta = loadJsonString(CONFIG_META);
#else
		szMeta = FileUtils::getInstance()->getStringFromFile(CONFIG_META);
#endif

	rapidjson::Document d;
	d.Parse(szMeta.c_str());
	if (d.HasParseError())
		return false;

	if(!initErrorMessage(d["errors"]))
		return false;
	if (!initItems(d["items"]))
		return false;
	if (!initSeed(d["farming"], d["seed"]))
		return false;
	if (!initTraining(d["training"]))
		return false;
	if (!initJobTitle(d["jobTitle"]))
		return false;
	if (!initRace(d["race"]))
		return false;
	if (!initAchievement(d["achievement"]))
		return false;

	mFarming.init(farmCB);

	if (!initActor())
		return false;

	//trade
	float trade_margin = d["trade"]["margin"].GetFloat();
	int trade_interval = d["trade"]["updateInterval"].GetInt();
	int trade_weight = d["trade"]["levelWeight"].GetInt();

	if (!mTrade.init(trade_margin, trade_interval, trade_weight, tradeCB))
		return false;

	return true;
}
void logics::insertInventory(rapidjson::Value &p, inventoryType type)
{	
	for (rapidjson::SizeType i = 0; i < p.Size(); i++) {
		int id = p[rapidjson::SizeType(i)]["id"].GetInt();
		int quantity = p[rapidjson::SizeType(i)]["quantity"].GetInt();
		mActor->inven.pushItem(type, id, quantity);
	}
}
/* private initialize */
bool logics::initActor()
{
	string sz;
#if defined(_WIN32) && !defined(COCOS2D_DEBUG)
		sz = loadJsonString(CONFIG_ACTOR);
#else
    string fileFullPath = FileUtils::getInstance()->getWritablePath() + CONFIG_ACTOR;
    if(!FileUtils::getInstance()->isFileExist(fileFullPath)){
        //writable 경로에 파일 복사
        FileUtils::getInstance()->writeStringToFile(FileUtils::getInstance()->getStringFromFile(CONFIG_ACTOR), fileFullPath);
    }
    sz = FileUtils::getInstance()->getStringFromFile(fileFullPath);
#endif

	rapidjson::Document d;
	d.Parse(sz.c_str());
	_actor* actor = new _actor;
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

	const rapidjson::Value& collection = d["collection"];
	for (rapidjson::SizeType i = 0; i < collection.Size(); i++) {
		int id = collection[rapidjson::SizeType(i)].GetInt();
		actor->collection[id] = true;
	}

	mActor = actor;
	insertInventory(d["inventory"]["growth"], inventoryType_growth);
	insertInventory(d["inventory"]["HP"], inventoryType_HP);
	insertInventory(d["inventory"]["race"], inventoryType_race);
	insertInventory(d["inventory"]["adorn"], inventoryType_adorn);
	insertInventory(d["inventory"]["farming"], inventoryType_farming);

	//farming
	const rapidjson::Value& farms = d["farming"];
	for (rapidjson::SizeType i = 0; i < farms.Size(); i++) {
		mFarming.addField(
			farms[rapidjson::SizeType(i)]["id"].GetInt()
			, farms[rapidjson::SizeType(i)]["x"].GetInt()
			, farms[rapidjson::SizeType(i)]["y"].GetInt()
			, farms[rapidjson::SizeType(i)]["seedId"].GetInt()
			, (farming::farming_status)farms[rapidjson::SizeType(i)]["status"].GetInt()
			, farms[rapidjson::SizeType(i)]["timePlant"].GetInt64()
			, farms[rapidjson::SizeType(i)]["cntCare"].GetInt()
			, farms[rapidjson::SizeType(i)]["timeLastGrow"].GetInt64()
			, farms[rapidjson::SizeType(i)]["boost"].GetInt()
		);
	}

	//achievement
	//daily, totally
	bool isDaily = true;
	for (int n = 0; n < 2; n++) {
		const rapidjson::Value& p = isDaily ? d["achievement"]["daily"]: d["achievement"]["totally"];
		for (rapidjson::SizeType i = 0; i < p.Size(); i++) {
			mAchievement.setAchievementAccumulation(isDaily
				, p[rapidjson::SizeType(i)]["category"].GetInt()
				, p[rapidjson::SizeType(i)]["id"].GetInt()
				, p[rapidjson::SizeType(i)]["accumulation"].GetInt()
				, p[rapidjson::SizeType(i)]["isFinished"].GetBool()
				, p[rapidjson::SizeType(i)]["isReceived"].GetBool()
				);
		}
		isDaily = false;
	}

	//accumulation
	const rapidjson::Value& achieveAccumulation = d["achievement"]["accumulation"];
	for (rapidjson::Value::ConstMemberIterator it = achieveAccumulation.MemberBegin();
		it != achieveAccumulation.MemberEnd(); ++it)
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

	if (!mAchievement.init(achievementCallback, lastLogin))
		return false;

	//save backup
	saveFile(CONFIG_ACTOR_BACKUP, sz);

	mIsRunThread = true;
	mThread = new thread(threadRun);

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
	for (rapidjson::SizeType i = 0; i < race.Size(); i++) {
		_race r;
		r.id = race[rapidjson::SizeType(i)]["id"].GetInt();
		r.title = utf8_to_utf16(race[rapidjson::SizeType(i)]["title"].GetString());
		r.fee = race[rapidjson::SizeType(i)]["fee"].GetInt();
		r.length = race[rapidjson::SizeType(i)]["length"].GetInt();
		r.level = race[rapidjson::SizeType(i)]["level"].GetInt();

		const rapidjson::Value& rewards = race[rapidjson::SizeType(i)]["rewards"];
		for (rapidjson::SizeType j = 0; j < rewards.Size(); j++) {
			_raceReward rr;
			rr.prize = rewards[rapidjson::SizeType(j)]["prize"].GetInt();

			const rapidjson::Value& items = rewards[rapidjson::SizeType(j)]["items"];
			for (rapidjson::SizeType k = 0; k < items.Size(); k++) {
				_itemPair ip;
				ip.itemId = items[rapidjson::SizeType(k)]["id"].GetInt();
				ip.val = items[rapidjson::SizeType(k)]["quantity"].GetInt();

				rr.items.push_back(ip);
			}
			r.rewards.push_back(rr);
		}
		addRaceMeta(r);
	}
	return true;
}

bool logics::initAchievement(rapidjson::Value & p)
{
	bool isDaily = true;
	for (int n = 0; n < 2; n++) 
	{
		const rapidjson::Value &v = isDaily ? p["daily"]: p["totally"];
		
		for (rapidjson::SizeType i = 0; i < v.Size(); i++)
		{			
			mAchievement.addAchieve(
				isDaily
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

void logics::finalize() {
	if(mActor)
		mActor->lastLoginLogoutTime = getNow();
	mIsFinalized = true;
	saveActor();
	mIsRunThread = false;
	mThread->join();
	mFarming.finalize();
	mTrade.finalize();
	mAchievement.finalize();	
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
			printf("[%03d] %ls \n▷ 참가비: %d, 경주거리: %d m, lv.%d  \n▷ 우승상금 --------\n"
				, it->second.id
				, it->second.title.c_str()
				, it->second.fee
				, it->second.length
				, it->second.level
			);
			
			for (int m = 0; m < (int)it->second.rewards.size(); m++) {
				printf("%d등 상금: %d (%ls 외 %d)\n"
					, m+1 
					, it->second.rewards[m].prize
					, mItems[it->second.rewards[m].items[0].itemId].name.c_str()
					, (int)it->second.rewards[m].items.size() -1
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

bool logics::insertItem(_item item) {
	mItems[item.id] = item;
	if (getInventoryType(item.id) == inventoryType_race)
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
	if (mTraining.find(id) == mTraining.end() || isValidTraningTime(id) == false) {
		return error_invalid_id;
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
	int preservation = preservationRatio * mTraining[id].reward.point;
	printf("point: %d \n" , preservation);
	point = getRandValue(mTraining[id].reward.point);
	if (point < preservation)
		point = preservation;

	mActor->point += point;
	//give growth
	preservation = preservationRatio * mTraining[id].reward.strength;
	printf("strength: %d \n", preservation);
	rewardProperty->strength = getRandValue(mTraining[id].reward.strength);
	if (rewardProperty->strength < preservation)
		rewardProperty->strength = preservation;

	preservation = preservationRatio * mTraining[id].reward.intelligence;
	printf("intelligence: %d \n", preservation);
	rewardProperty->intelligence = getRandValue(mTraining[id].reward.intelligence);
	if (rewardProperty->intelligence < preservation)
		rewardProperty->intelligence = preservation;

	preservation = preservationRatio * mTraining[id].reward.appeal;
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
	mAchievement.push(achievement_category_training, id, 1);
	if (increaseExp())
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
		mAchievement.push(achievement_category_trade_buy, 0, 1);
		mAchievement.push(achievement_category_trade_buy, id, quantity);
	}
	else if (amount > 0){
		mAchievement.push(achievement_category_trade_sell, 0, 1);
		mAchievement.push(achievement_category_trade_sell, id, quantity * -1);
	}

	//if (increaseExp())	return error_levelup;
	return error_success;
}

errorCode logics::runRecharge(int id, int quantity) {
	
	if (getInventoryType(id) != inventoryType_HP)
		return error_invalid_id;
	if (!addInventory(id, quantity * -1)) {
		return error_not_enough_item;
	}
	int val = mItems[id].value * quantity;
	increaseHP(val);
	mAchievement.push(achievement_category_recharge, 0, quantity);
	return error_success;
}

//add inventory
bool logics::addInventory(int itemId, int quantity) {
	inventoryType type = getInventoryType(itemId);
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
}

//경험치 증가
bool logics::increaseExp() {
	mActor->exp++;
	int maxExp = getMaxExp();
	if (maxExp <= mActor->exp) {
		mActor->level++;
		mActor->exp = 0;
		setMaxHP();

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
	if (val < 0 && mActor->hp + val < 0)
		return false;

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
	time_t now = time(0);
	if (now - mActor->lastHPUpdateTime >= HPIncreaseInterval) {
		mActor->lastHPUpdateTime = now;
		return increaseHP(1);
	}
	return false;
}

bool logics::isValidTraningTime(int id) {

	_training* t = &mTraining[id];

	if (mActor->level < t->level)
		return false;

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
		mActor->jobTitle = szPrefix + L" " + szBody;
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

	mActor->jobTitle = szPrefix + L" " + szBody;
	
	return;
}

void logics::addRaceMeta(_race & race) {
	mRace[race.id] = race;
}

errorCode logics::runRace(int id, itemsVector &items) {
	if (mRace.find(id) == mRace.end())
		return error_invalid_id;

	if (mActor->property.strength < 0) {
		return error_not_enough_strength;
	}
	if (!increaseHP(-1)) {
		return error_not_enough_hp;
	}

	if (mActor->point < mRace[id].fee)
		return error_not_enough_point;

	for (int m = 0; m < (int)items.size(); m++) {
		if(!mActor->inven.checkItemQuantity(inventoryType_race, items[m].itemId, items[m].val))
			return error_not_enough_item;
	}
	mActor->point -= mRace[id].fee;
	mRaceCurrent.id = id;
	mRaceCurrent.prize = 0;
	mRaceCurrent.rewardItemId = 0;
	mRaceCurrent.rewardItemQuantity = 0;

	mRaceParticipants->clear();
	//내 능력치랑 비슷하게 구성
	int sum = mActor->property.strength + mActor->property.intelligence + mActor->property.appeal;
	sum += (int)((float)sum * raceAIAdvantageRatio);
	//참가자 목록
	for (int n = 0; n < raceParticipantNum; n++) {
		_raceParticipant p;
		p.idx = n;
		if (n == 0) { //0번은 완전 동일한 능력치
			p.strength = mActor->property.strength;
			p.intelligence = mActor->property.intelligence;
			p.appeal = mActor->property.appeal;
		}
		else {
			p.strength = getRandValue(sum);
			p.strength == 0 ? p.strength = 1 : p.strength = p.strength;
			p.intelligence = getRandValue(sum - p.strength);
			p.appeal = sum - p.strength - p.intelligence;
		}
		
		//AI advantage
		//p.strength += (int)(p.strength * raceAIAdvantageRatio * mRace[id].level);
		/*
		for (int m = 0; m < raceItemSlot; m++) {
			p.items[m] = getRandomRaceItem();
		}*/
		//printf("(%d) S: %d, I: %d, A: %d\n",n+1,  p.strength, p.intelligence, p.appeal);
		//_sleep(100);
		mRaceParticipants->push_back(p);
	}
	_raceParticipant p;
	p.idx = raceParticipantNum;
	p.strength = mActor->property.strength;
	p.intelligence = mActor->property.intelligence;
	p.appeal = mActor->property.appeal;
	
	int idx = 0;
	for (int m = 0; m < (int)items.size(); m++) {
		for (int k = 0; k < items[m].val; k++) {
			p.items[idx] = items[m].itemId;
			addInventory(items[m].itemId, -1);
			idx++;
		}		
	}
	mRaceParticipants->push_back(p);
	mAchievement.push(achievement_category_race, achievement_race_id_try, 1);
	if (increaseExp())
		return error_levelup;

	return error_success;
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
	int level = mRace[mRaceCurrent.id].level;
	for (int i = 0; i < raceParticipantNum; i++) {
		//내가 raceInvokeThreshold 이상 달리고 나서 부터 아이템 사용
		if (mRaceParticipants->at(i).ratioLength < raceInvokeThreshold)
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
				invokeRaceItem(i, itemType_race_shield, level * raceItemQuantityPerLevel, mRaceParticipants->at(i).currentRank);
				return;
			}
		}
		switch (mRaceParticipants->at(i).currentRank) {
		case 1: // 1등이면 50%이상 왔을때 스피드 업
			if (mRaceParticipants->at(i).ratioLength > raceSpurt)
				invokeRaceItem(i, itemType_race_speedUp, level * raceItemQuantityPerLevel, mRaceParticipants->at(i).currentRank);
			break;
		case 2: //2, 3등이면 앞 고냥이 공격. 50%이상 왔을 부터 스퍼트		
			invokeRaceItem(i, itemType_race_attactFront, level * raceItemQuantityPerLevel, mRaceParticipants->at(i).currentRank);
			if (mRaceParticipants->at(i).ratioLength > raceSpurt)
				invokeRaceItem(i, itemType_race_speedUp, level * raceItemQuantityPerLevel, mRaceParticipants->at(i).currentRank);
			break;
		case 3: //3, 4, 5등이면 스피드 업 50% 이상 부터 1등 공격
		case 4: 
		case 5:
			invokeRaceItem(i, itemType_race_attactFirst, level * raceItemQuantityPerLevel, mRaceParticipants->at(i).currentRank);
			break;
		}
	}
}

int logics::getBaseSpeed(int s, int i, int a) {
	//float ratioI = ((float)i / (float)(s + i + a));
	float s1 = (float)(s * (1.0f - raceIntelligenceRatio));
	float i1 = (float)(i * raceIntelligenceRatio);
	float a1 = (float)getRandValue(a * raceAppealRatio);
	int length = (int)(s1 + i1 + a1);

	return length;
}

raceParticipants* logics::getNextRaceStatus(bool &ret, int itemIdx) {
	 int lastRank = 0;
	 //int raceLength = mRace[mRaceCurrent.id].length;
	 int raceLength = (int)((float)mActor->property.total() / 2.f * 0.5f * 30.f * 5.f);
	 //순위 산정용 벡터
	 vector<_raceParticipant> orderedVector;

	 for (int n = 0; n < (int)mRaceParticipants->size(); n++) {
		 if (mRaceParticipants->at(n).rank == 0) {
			 orderedVector.push_back(mRaceParticipants->at(n));
		 }
		 else {
			 mRaceParticipants->at(n).currentRank = 0;
		 }
		 if (mRaceParticipants->at(n).rank > 0) {
			 lastRank++;
		 }			 
	 }

	 //현재 순위 산정
	 sort(orderedVector.begin(), orderedVector.end());
	 for (int n = 0; n < (int)orderedVector.size(); n++) {		 
		 mRaceParticipants->at(orderedVector[n].idx).currentRank = n + 1;
	 }

	 //내가 사용한 아이템 발동
	 if (itemIdx > -1) {
		 invokeRaceItemByIdx(raceParticipantNum, itemIdx);
	 }
	 //AI가 사용한 아이템 발동
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
		 //int length = mRaceParticipants->at(n).strength + getRandValue(mRaceParticipants->at(n).appeal * raceAppealRatio);
		 int length = getBaseSpeed(
			 mRaceParticipants->at(n).strength
			 , mRaceParticipants->at(n).intelligence
			 , mRaceParticipants->at(n).appeal
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
			 length += (int)((float)mRaceParticipants->at(n).total() * raceSpeedUp);
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
	 if (lastRank >= raceParticipantNum) {
		 //if (mRaceParticipants->at(raceParticipantNum).rank != 0 ) { 
		 ret = false;
		 for (int n = 0; n < (int)mRaceParticipants->size(); n++) {
			 if (mRaceParticipants->at(n).rank == 0)
				 mRaceParticipants->at(n).rank = raceParticipantNum + 1;
		 }
		 //보상 지급
		 mRaceCurrent.rank = mRaceParticipants->at(raceParticipantNum).rank;
		 //업적
		 if(mRaceCurrent.rank == 1)
			 mAchievement.push(achievement_category_race, achievement_race_id_first, 1);
		 else if (mRaceCurrent.rank == 2)
			 mAchievement.push(achievement_category_race, achievement_race_id_second, 1);
			 
		 for (int n = 0; n < (int)mRace[mRaceCurrent.id].rewards.size(); n++) {
			 if (mRaceParticipants->at(raceParticipantNum).rank - 1 == n) {
				 mActor->point += mRace[mRaceCurrent.id].rewards[n].prize;
				 int idx = getRandValue((int)mRace[mRaceCurrent.id].rewards[n].items.size());
				 int itemId = mRace[mRaceCurrent.id].rewards[n].items[idx].itemId;
				 int val = mRace[mRaceCurrent.id].rewards[n].items[idx].val;
				 addInventory(itemId, val);

				 mRaceCurrent.prize = mRace[mRaceCurrent.id].rewards[n].prize;
				 mRaceCurrent.rewardItemId = itemId;
				 mRaceCurrent.rewardItemQuantity = val;

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
	increaseExp();

	mAchievement.push(achievement_category_farming, achievement_farming_id_harvest, 1);
	mAchievement.push(achievement_category_farming, achievement_farming_id_output, earning);
	return error_success;
};

errorCode logics::farmingPlant(int idx, int seedId) {
	if (!mFarming.plant(idx, seedId))
		return error_farming_failure;
	if (!mActor->inven.popItem(inventoryType_farming, seedId, 1))
		return error_not_enough_item;

	mAchievement.push(achievement_category_farming, achievement_farming_id_plant, 1);
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

		mAchievement.push(achievement_category_farming, achievement_farming_id_care, 1);

		if (boost == 0)
			return error_success;
		else
			return error_farming_gluttony;
	}
	return error_farming_failure;
}
errorCode logics::farmingExtend()
{
	if (mActor->point < mFarmingExtendFee)
		return error_not_enough_point;
	
	mActor->point -= mFarmingExtendFee;

	mFarming.addField(mFarming.countField(), 0);
	if (increaseExp())
		return error_levelup;
	return error_success;
}
;

void logics::achievementCallback(bool isDaily, int idx) {
	printf("%d achievementCallback \n", idx);
	achievement::detail d;
	hInst->mAchievement.getDetail(isDaily, idx, d);
	hInst->mAchievement.rewardReceive(isDaily, idx);
	hInst->addInventory(d.rewardId, d.rewardVal);

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

    string sz;
#if defined(_WIN32) && !defined(COCOS2D_DEBUG)
    sz = loadJsonString(CONFIG_ACTOR);
#else
    sz = FileUtils::getInstance()->getStringFromFile(FileUtils::getInstance()->getWritablePath() + CONFIG_ACTOR);
#endif

    rapidjson::Document d;
	d.Parse(sz.c_str());
	
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

	d["collection"].Clear();		
	for (keyBoolMap::iterator it = mActor->collection.begin(); it != mActor->collection.end(); ++it) {		
		if(it->second == true)
			d["collection"].PushBack(it->first, d.GetAllocator());
	}

	d["farming"].Clear();
	farming::fields* f = mFarming.getFields();
	for (int n = 0; n < (int)f->size(); n++) {
		if (f->at(n)) {
			rapidjson::Value objValue;
			objValue.SetObject();
			objValue.AddMember("id"	, (int)f->at(n)->id, d.GetAllocator());
			objValue.AddMember("x"	, (int)f->at(n)->x, d.GetAllocator());
			objValue.AddMember("y"	, (int)f->at(n)->y, d.GetAllocator());
			objValue.AddMember("seedId", (int)f->at(n)->seedId, d.GetAllocator());
			objValue.AddMember("status", (int)f->at(n)->status, d.GetAllocator());
			objValue.AddMember("timePlant", (int64_t)f->at(n)->timePlant, d.GetAllocator());
			objValue.AddMember("cntCare", (int)f->at(n)->cntCare, d.GetAllocator());
			objValue.AddMember("timeLastGrow", (int64_t)f->at(n)->timeLastGrow, d.GetAllocator());
			objValue.AddMember("boost", (int)f->at(n)->boost, d.GetAllocator());
		
			d["farming"].PushBack(objValue, d.GetAllocator());
		}
	}

	d["achievement"]["daily"].Clear();
	d["achievement"]["totally"].Clear();

	bool isDaily = true;
	for (int n = 0; n < 2; n++) {
		int nSize = mAchievement.getSize(isDaily);
		for (int i = 0; i < nSize; i++) {
			achievement::detail p;
			mAchievement.getDetail(isDaily, i, p);

			rapidjson::Value objValue;
			objValue.SetObject();
			objValue.AddMember("category", p.category, d.GetAllocator());
			objValue.AddMember("id", p.id, d.GetAllocator());
			objValue.AddMember("accumulation", p.accumulation, d.GetAllocator());
			objValue.AddMember("isFinished", p.isFinished, d.GetAllocator());
			objValue.AddMember("isReceived", p.isReceived, d.GetAllocator());

			if (isDaily) {
				d["achievement"]["daily"].PushBack(objValue, d.GetAllocator());
			}
			else {
				d["achievement"]["totally"].PushBack(objValue, d.GetAllocator());
			}
		}
		isDaily = isDaily ? false : true;
	}
	
	d["achievement"]["accumulation"].RemoveAllMembers();
	const rapidjson::Value& accumulation = d["achievement"]["accumulation"];

	queue<string> gabages;
	achievement::intDoubleDepthMap * pAccumulation = mAchievement.getAccumulation();
	for (achievement::intDoubleDepthMap::iterator it = pAccumulation->begin(); it != pAccumulation->end(); ++it) {
		intMap * pIntMap = it->second;
		//char category[10] = { 0 };

		string category = to_string(it->first);
		gabages.push(category);
		/*
		string sz;
		sz += to_string(it->first);
		//itoa(it->first, category, 10);
		category = sz.c_str();
		*/
		/*
		switch (it->first) {
		case 0:
			category = "0";
			break;
		case 1:
			category = "1";
			break;
		case 2:
			category = "2";
			break;
		case 3:
			category = "3";
			break;
		case 4:
			category = "4";
			break;
		}*/

		if (d["achievement"]["accumulation"].HasMember(rapidjson::StringRef(category.c_str())) == false) {
			rapidjson::Value arr;
			arr.SetArray();
			d["achievement"]["accumulation"].AddMember(rapidjson::StringRef(category.c_str()), arr, d.GetAllocator());
		}
		for (intMap::iterator it2 = pIntMap->begin(); it2 != pIntMap->end(); ++it2) {
			rapidjson::Value objValue;
			objValue.SetObject();
			objValue.AddMember("id", it2->first, d.GetAllocator());
			objValue.AddMember("value", it2->second, d.GetAllocator());

			d["achievement"]["accumulation"].FindMember(rapidjson::StringRef(category.c_str()))->value.PushBack(objValue, d.GetAllocator());
			//d["achievement"]["accumulation"][category].PushBack(objValue, d.GetAllocator());
			
		}
	}

	rapidjson::StringBuffer buffer;
	buffer.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	//printf("%s \n", buffer.GetString());
		
	string input(buffer.GetString());
	saveFile(CONFIG_ACTOR, input);

	while (gabages.size() > 0) {
		string p = gabages.front();
		gabages.pop();
		//delete p;
	}
}