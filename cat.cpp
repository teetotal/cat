// cat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "logics.h"
#include <fstream>
#include <iostream>
#include <thread>
#ifdef _WIN32
    #include <conio.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
void enable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

bool _kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

#endif
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#ifdef _WIN32
	#include <Windows.h>
#endif // WIN32
#define RACE_SLEEP 150
#define IDLE_NUM 6
using namespace rapidjson;
logics logic;
bool isRunThread = false;

string imgResult;
string imgTraining[trainingType_max];
string imgRecharge;
string imgIdle[IDLE_NUM];
string raceSuffer;

const wchar_t raceIcons[] = { L'◈', L'◎', L'▶', L'◇', L'♥' };

void cls() {
#ifdef _WIN32
	system("cls");
#else
    system("clear");
#endif
}

void loadImg(const char* file, string *str) {
	std::ifstream fileopen;
	char sz[100] = { 0, };
	sprintf(sz, "resource/%s.txt", file);
	fileopen.open(sz, ios::in | ios::binary);
	str->assign((std::istreambuf_iterator<char>(fileopen)),
		std::istreambuf_iterator<char>());
	fileopen.close();
}

void display(const char* sz, int sleep = 0, bool isCls = true) {
	if(isCls)
		cls();
	printf("%s", sz);
	if(sleep > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
}
void intro() {
	char sz[100] = { 0, };
	//result
	loadImg("result", &imgResult);
	//training
	for (int n = 0; n < trainingType_max; n++) {
		sprintf(sz, "training-%d", n);
		loadImg(sz, &imgTraining[n]);
	}
	//recharge
	loadImg("recharge", &imgRecharge);
	//idle
	for (int n = 0; n < IDLE_NUM; n++) {
		sprintf(sz, "idle-%d", n);
		loadImg(sz, &imgIdle[n]);
	}
	//race suffer
	loadImg("race-suffer", &raceSuffer);
	//intro
	const int num = 6;
	string szIntro[num];
	for (size_t i = 0; i <  num; i++) {
		sprintf(sz, "intro-%d", (int)i + 1);
		loadImg(sz, &szIntro[i]);
	}
	
	for (int i = 0; i < num; i++) {
		display(szIntro[i].c_str(), 1000);
	}	
}

void progress() {
	float progress = 0.0;
	while (progress < 1.0) {
		int barWidth = 70;

		std::cout << "[";
		int pos = (int)(barWidth * progress);
		for (int i = 0; i < barWidth; ++i) {
			if (i < pos) std::cout << "=";
			else if (i == pos) std::cout << ">";
			else std::cout << " ";
		}
		std::cout << "] " << int(progress * 100.0) << " %\r";
	

		std::cout.flush();

		progress += (float)0.12; // for demonstration only
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	std::cout << std::endl;
}

void result(const wchar_t * sz, errorCode err = error_success) {
	if(err == error_success)
		progress();
	cls();
	display(imgResult.c_str());
	printf("%c[1;36m %ls %c[0m        \n\n", 27, sz, 27);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	cls();
}

void runThread() {
	isRunThread = true;

	while (isRunThread) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		if (logic.rechargeHP()) {
			printf("\n\n★ 얏호 체력이 보충됐어요~~~!! [HP +1] ☆\n\n > ");
		}
		if(logic.setTradeMarketPrice())
			printf("\n\n 아이템 시세가 변경 됐어요~ \n\n > ");
	}
}
void printRaceRunning(int ratio, int id, int rank, int length, itemType currentItem) {
	wstring sz;
	if(id == raceParticipantNum)
		printf("%c[1;32m %d [me]",27, rank);
	else
		printf("%c[0m %d [%02d]", 27, rank, id + 1);
	
	for (int n = 0; n < ratio; n++) {
		sz += L" ";
	}
	//printf("%s", sz.c_str());
	sz += raceIcons[id];
	//sz += L" ";
	//sz += to_wstring(ratio);

	if (currentItem != itemType_max) {
		sz += L" ";
		sz += to_wstring((int)currentItem);
	}
		
	printf("%ls", sz.c_str());
}
void runRace() {
#ifdef _WIN32
	const int line = 50;
#else
	const int line = 103;
#endif
	bool ret = true;
	raceParticipants* p = NULL;
	_raceCurrent* r = logic.getRaceResult();
	int key = -1;
	while (ret)
	{
		if (key > 0)
			key--;
		p = logic.getNextRaceStatus(ret, key);
		key = -1;

		if (!ret)
			break;
		cls();
		printf("%c[0m━━━", 27);
		string sz;
		for (int n = 0; n < line; n++)
			sz += "━";
		printf("%s┓\n", sz.c_str());
		for (int n = 0; n < p->size(); n++) {
			printRaceRunning(
				(int)p->at(n).ratioLength
				, n
				, p->at(n).currentRank
				, p->at(n).currentLength
				, p->at(n).currentSuffer
			);
			printf("\n");
		}
		printf("%c[0m━━━", 27);
		sz = "";
		for (int n = 0; n < line; n++)
			sz += "━";
		printf("%s┛\n", sz.c_str());

		//당하고 있는 스킬
		if (p->at(raceParticipantNum).currentSuffer != itemType_max) {
			bool isSleep = false;
			switch (p->at(raceParticipantNum).currentSuffer)
			{
			case itemType_race_speedUp:
				printf("달려라!! 스피드 업!! \n");
				break;
			case itemType_race_shield:
				printf("모두 없던 일로~ \n");
				isSleep = true;
				break;
			default:
				isSleep = true;
				printf("으악 공격 당하고 있다옹 %d\n", p->at(raceParticipantNum).currentSuffer);
				display(raceSuffer.c_str(), 0, false);
				break;
			}

			if (p->at(raceParticipantNum).sufferItems.size() > 0)
				printf("예약된 스킬: %d (+%d) \n"
					, (int)p->at(raceParticipantNum).sufferItems.front()
					, (int)p->at(raceParticipantNum).sufferItems.size()
				);

			if(isSleep)
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}	
		
		//보유 아이템 목록
		for (int n = 0; n < raceItemSlot; n++) {
			int itemId = p->at(raceParticipantNum).items[n];
			if (itemId > 0) {
				printf("> %d. %ls \n", n +1, logic.getItem(itemId).name.c_str());
			}
		}
#ifdef _WIN32
		if (_kbhit() != 0)
		{
			key = _getch() - 48;
			printf("\n★ 아이템 발동!! %d ★ \n", key);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
#else
        enable_raw_mode();
        if (_kbhit()) {
            key = getchar() - 48;
            printf("\n★ 아이템 발동!! %d ★ \n", key);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        disable_raw_mode();
        tcflush(0, TCIFLUSH); // Clear stdin to prevent characters appearing on prompt
#endif
		std::this_thread::sleep_for(std::chrono::milliseconds(RACE_SLEEP));
	}
	//순위 정보
	//sort(p->begin(), p->end());
	for (int n = 0; n <= raceParticipantNum; n++) {
		printf("%d등 idx: %d ( %c[1;32m  S:%d, I: %d, A: %d  %c[0m ) item cnt: %d  \n"
			, p->at(n).rank
			, p->at(n).idx
			, 27
			, p->at(n).strength
			, p->at(n).intelligence
			, p->at(n).appeal
			, 27
			, p->at(n).shootItemCount
			);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	//결과처리
	wstring sz;
	sz += L"순위: ";
	sz += to_wstring(r->rank);
	sz += L"\n상금: ";
	sz += to_wstring(r->prize);
	sz += L"\n상품: ";
	sz += logic.getItem(r->rewardItemId).name;
	sz += L"(";
	sz += to_wstring(r->rewardItemQuantity);
	sz += L")";
	result(sz.c_str());
}

std::wstring utf8_to_utf16(const std::string& utf8)
{
	std::vector<unsigned long> unicode;
	size_t i = 0;
	while (i < utf8.size())
	{
		unsigned long uni;
		size_t todo;
		//bool error = false;
		unsigned char ch = utf8[i++];
		if (ch <= 0x7F)
		{
			uni = ch;
			todo = 0;
		}
		else if (ch <= 0xBF)
		{
			throw std::logic_error("not a UTF-8 string");
		}
		else if (ch <= 0xDF)
		{
			uni = ch & 0x1F;
			todo = 1;
		}
		else if (ch <= 0xEF)
		{
			uni = ch & 0x0F;
			todo = 2;
		}
		else if (ch <= 0xF7)
		{
			uni = ch & 0x07;
			todo = 3;
		}
		else
		{
			throw std::logic_error("not a UTF-8 string");
		}
		for (size_t j = 0; j < todo; ++j)
		{
			if (i == utf8.size())
				throw std::logic_error("not a UTF-8 string");
			unsigned char ch = utf8[i++];
			if (ch < 0x80 || ch > 0xBF)
				throw std::logic_error("not a UTF-8 string");
			uni <<= 6;
			uni += ch & 0x3F;
		}
		if (uni >= 0xD800 && uni <= 0xDFFF)
			throw std::logic_error("not a UTF-8 string");
		if (uni > 0x10FFFF)
			throw std::logic_error("not a UTF-8 string");
		unicode.push_back(uni);
	}
	std::wstring utf16;
	for (size_t i = 0; i < unicode.size(); ++i)
	{
		unsigned long uni = unicode[i];
		if (uni <= 0xFFFF)
		{
			utf16 += (wchar_t)uni;
		}
		else
		{
			uni -= 0x10000;
			utf16 += (wchar_t)((uni >> 10) + 0xD800);
			utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
		}
	}
	return utf16;
}
time_t getTime(int hour, int min, int sec) {
	time_t now = time(0);
	tm *ltm = localtime(&now);
	ltm->tm_hour = hour;
	ltm->tm_min = min;
	ltm->tm_sec = sec;
	
	return mktime(ltm);

}
void init() {
	//setlocale(LC_ALL, "en_US.UTF-8");
	//setlocale(LC_ALL, "");
#ifdef _WIN32
	_wsetlocale(LC_ALL, L"korean");
#else
    setlocale( LC_ALL, "" );
    fwide( stdout, -1 );
#endif

	std::ifstream fileopen;
	fileopen.open("resource/meta.json", ios::in | ios::binary);
	std::string str((std::istreambuf_iterator<char>(fileopen)), std::istreambuf_iterator<char>());
	fileopen.close();


	Document d;
	d.Parse(str.c_str());
	//error message
	const Value& valueErr = d["errors"];
	for (SizeType i = 0; i < valueErr.Size(); i++)
	{
		logic.addErrorMessage(utf8_to_utf16(valueErr[i].GetString()));
	}
	//Add Items
	const Value& valueInfo = d["items"];
	for (SizeType i = 0; i < valueInfo.Size(); i++)
	{
		_item p;
		p.id = valueInfo[i]["id"].GetInt();
		p.type = (itemType)valueInfo[i]["type"].GetInt();
		p.value = valueInfo[i]["value"].GetInt();
		p.grade = valueInfo[i]["grade"].GetInt();
		string sz = valueInfo[i]["name"].GetString();
		p.name = utf8_to_utf16(sz);
		logic.insertItem(p);
	}

	const  Value& t = d["training"];
	for (SizeType i = 0; i < t.Size(); i++) {
		_training p;
		for (int k = 0; k < maxTrainingItems; k++) {
			p.reward.items[k] = NULL;
			p.cost.items[k] = NULL;
		}
		p.id = t[i]["id"].GetInt();
		string sz = t[i]["name"].GetString();
		p.name = utf8_to_utf16(sz);
		p.type = (trainingType)t[i]["type"].GetInt();
		p.level = t[i]["level"].GetInt();

		p.reward.strength = t[i]["reward"]["strength"].GetInt();
		p.reward.intelligence = t[i]["reward"]["intelligence"].GetInt();
		p.reward.appeal = t[i]["reward"]["appeal"].GetInt();
		p.reward.point = t[i]["reward"]["point"].GetInt();

		p.cost.strength = t[i]["cost"]["strength"].GetInt();
		p.cost.intelligence = t[i]["cost"]["intelligence"].GetInt();
		p.cost.appeal = t[i]["cost"]["appeal"].GetInt();
		p.cost.point = t[i]["cost"]["point"].GetInt();
		
		p.start = 0;
		//  반짝 
		if (t[i].HasMember("moment")) {
			p.start = getTime(
				t[i]["moment"]["start"]["hour"].GetInt()
				, t[i]["moment"]["start"]["min"].GetInt()
				, t[i]["moment"]["start"]["sec"].GetInt()
			);
			p.count = t[i]["moment"]["count"].GetInt();
			p.keep = t[i]["moment"]["keep"].GetInt();
			p.interval = t[i]["moment"]["keep"].GetInt();

		}

		const Value& item = t[i]["reward"]["items"];
		for (SizeType m = 0; m < item.Size(); m++) {
			if (m >= maxTrainingItems)
				break;
			_itemPair* pair = new _itemPair;
			pair->itemId = item[m]["id"].GetInt();
			pair->val = item[m]["quantity"].GetInt();
			p.reward.items[m] = pair;
		}

		const Value& item2 = t[i]["cost"]["items"];
		for (SizeType m = 0; m < item2.Size(); m++) {
			if (m >= maxTrainingItems)
				break;
			_itemPair* pair = new _itemPair;
			pair->itemId = item2[m]["id"].GetInt();
			pair->val = item2[m]["quantity"].GetInt();
			p.cost.items[m] = pair;
		}
		logic.insertTraining(p);
	}

	//jobTitle
	const Value& job = d["jobTitle"];
	logic.setDefaultJobTitle(utf8_to_utf16(job["default"].GetString()));
	const Value& jobPrefix = job["prefix"];
	for (SizeType i = 0; i < jobPrefix.Size(); i++) {
		_jobTitlePrefix p;
		p.level = jobPrefix[i]["level"].GetInt();
		p.title = utf8_to_utf16(jobPrefix[i]["title"].GetString());

		logic.addJobTitlePrefix(p);
	}

	const Value& jobBody = job["body"];
	for (SizeType i = 0; i < jobBody.Size(); i++) {
		_jobTitleBody p;
		p.S = jobBody[i]["S"].GetInt();
		p.I = jobBody[i]["I"].GetInt();
		p.A = jobBody[i]["A"].GetInt();
		p.title = utf8_to_utf16(jobBody[i]["title"].GetString());

		logic.addJobTitleBody(p);
	}

	//Race Meta
	const Value& race = d["race"];
	for (SizeType i = 0; i < race.Size(); i++) {
		_race r;
		r.id = race[i]["id"].GetInt();
		r.title = utf8_to_utf16(race[i]["title"].GetString());
		r.fee = race[i]["fee"].GetInt();
		r.length = race[i]["length"].GetInt();
		r.level = race[i]["level"].GetInt();

		const Value& rewards = race[i]["rewards"];
		for (SizeType j = 0; j < rewards.Size(); j++) {
			_raceReward rr;
			rr.prize = rewards[j]["prize"].GetInt();

			const Value& items = rewards[j]["items"];
			for (SizeType k = 0; k < items.Size(); k++) {
				_itemPair ip;
				ip.itemId = items[k]["id"].GetInt();
				ip.val = items[k]["quantity"].GetInt();

				rr.items.push_back(ip);
			}
			r.rewards.push_back(rr);
		}
		logic.addRaceMeta(r);
	}
	

	//Set Actor

	fileopen.open("resource/actor.json", ios::in | ios::binary);
	std::string sz((std::istreambuf_iterator<char>(fileopen)), std::istreambuf_iterator<char>());
	fileopen.close();

	Document d2;
	d2.Parse(sz.c_str());
	_actor* actor = new _actor;
	actor->userName = utf8_to_utf16(string(d2["userName"].GetString()));
	actor->userId = d2["userId"].GetString();

	actor->name = utf8_to_utf16(string(d2["name"].GetString()));
	actor->id = d2["id"].GetString();

	actor->point = d2["point"].GetInt();
	actor->hp = d2["hp"].GetInt();
	actor->exp = d2["exp"].GetInt();
	actor->level = d2["level"].GetInt();

	actor->property.strength = d2["property"]["strength"].GetInt();
	actor->property.intelligence = d2["property"]["intelligence"].GetInt();
	actor->property.appeal = d2["property"]["appeal"].GetInt();

	//인벤토리
	const Value& growth = d2["inventory"]["growth"];
	for (SizeType i = 0; i < growth.Size(); i++) {
		int id = growth[i]["id"].GetInt();
		int quantity = growth[i]["quantity"].GetInt();
		actor->inventory.growth[id] = quantity;
	}
	const Value& raceItem = d2["inventory"]["race"];
	for (SizeType i = 0; i < raceItem.Size(); i++) {
		int id = raceItem[i]["id"].GetInt();
		int quantity = raceItem[i]["quantity"].GetInt();
		actor->inventory.race[id] = quantity;
	}
	const Value& adorn = d2["inventory"]["adorn"];
	for (SizeType i = 0; i < adorn.Size(); i++) {
		int id = adorn[i]["id"].GetInt();
		int quantity = adorn[i]["quantity"].GetInt();
		actor->inventory.adorn[id] = quantity;
	}
	const Value& collection = d2["collection"];
	for (SizeType i = 0; i < collection.Size(); i++) {
		int id = collection[i].GetInt();		
		actor->collection[id] = true;
	}

	logic.setActor(actor);

	logic.init();
	logic.print(3);
} 

void training() {
	logic.print(1);
	int key;
	printf("choose > ");
	scanf("%d", &key);
	if (key == -1)
		return;

	errorCode err = logic.isValidTraining(key);
	if (err != error_success) {
		result(logic.getErrorMessage(err), err);
		return;
	}
	vector<_itemPair> rewards;
	_property property;
	int point;
	trainingType type;
	wstring sz = logic.getErrorMessage(logic.runTraining(key, rewards, &property, point, type));
	display(imgTraining[type].c_str());
	sz += L"\n- 보상 내용 -\n";
	sz += L"\n Point: " + std::to_wstring(point);
	sz += L"\n 체력: " + std::to_wstring(property.strength);
	sz += L"\n 지력: " + std::to_wstring(property.intelligence);
	sz += L"\n 매력: " + std::to_wstring(property.appeal);
	sz += L"\n";
	for (int n = 0; n < rewards.size(); n++) {
		sz += logic.getItem(rewards[n].itemId).name + L"(";
		sz += std::to_wstring(rewards[n].val);
		sz += L") \n";		
	}

	result(sz.c_str());
}

void buy() {
	
	logic.print(2);

	int key, quantity;
	printf("ID > ");
	scanf("%d", &key);
	if (key == -1)
		return;
	printf("Quantity > ");
	scanf("%d", &quantity);
	
	errorCode err = logic.runTrade(true, key, quantity);

	result(logic.getErrorMessage(err), err);
}

void sell() {	
	logic.print(6);

	int key, quantity;
	printf("ID > ");
	scanf("%d", &key);
	if (key == -1)
		return;
	printf("Quantity > ");
	scanf("%d", &quantity);
	errorCode err = logic.runTrade(false, key, quantity);
	result(logic.getErrorMessage(err), err);
}

void hp() {
	logic.print();
	int key, quantity;
	printf("HP Item ID > ");
	scanf("%d", &key);
	if (key == -1)
		return;
	printf("Quantity > ");
	scanf("%d", &quantity);
	display(imgRecharge.c_str());
	errorCode err = logic.runRecharge(key, quantity);
	result(logic.getErrorMessage(err), err);
}

void race() {
	if (!logic.isAvailableHP()) {
		result(logic.getErrorMessage(error_not_enough_hp), error_not_enough_hp);
		return;
	}
		
	logic.print(5);

	int id, key, quantity, sum = 0;
	printf("ID > ");
	scanf("%d", &id);
	if (id == -1)
		return;

	itemsVector v;

	logic.print();
	for(int n=0; n < raceItemSlot; n ++){
		if (sum == raceItemSlot)
			break;

		printf("경묘에 사용할 ID > ");
		scanf("%d", &key);
		if (key == 0)
			break;
		printf("수량 > ");
		scanf("%d", &quantity);
		sum += quantity;
		if (sum > raceItemSlot) {
			result(logic.getErrorMessage(error_invalid_quantity), error_invalid_quantity);
			return;
		}			
		else {
			_itemPair p;
			p.itemId = key;
			p.val = quantity;
			v.push_back(p);
		}
	}
	errorCode err = logic.runRace(id, v);
	switch (err) {
	case error_success:
		runRace();
		break;
	case error_levelup:
		result(logic.getErrorMessage(err), err);
		runRace();
		break;
	default:
		result(logic.getErrorMessage(err), err);
		break;
	}	
}

bool ask() {
	display(imgIdle[logic.getRandValue(IDLE_NUM)].c_str());
	logic.print();
	printf("------------------------------------------------------------------------ \n");
	printf( " 1: 액션 \n 2: 아이템 구매 \n 3. 아이템 판매 \n 4: 경묘 \n 5: 체력보충 \n 6: 도감 보기  \n > ");
	int key;
	scanf("%d", &key);
	cls();
	switch (key)
	{
	case -1:
		return false;
	case 0:
		logic.print();
		break;
	case 1:
		training();
		break;
	case 2:
		buy();
		break;
	case 3:
		sell();
		break;
	case 4:
		race();		
		break;
	case 5:
		hp();
		break;
	case 6:
		logic.print(4);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		break;
	default:
		break;
	}
	return true;
}

int main()
{	
	intro();
	init();
	thread p(runThread);
		
	while (true) {
		if (!ask())
			break;		
	}
	isRunThread = false;
	p.join();	
    return 0;
}

