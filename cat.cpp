// cat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "logics.h"
#include <fstream>
#include <iostream>
#include <thread>
#include "library\util.h"

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
	if (sleep > 0)
		sleepThisThread(sleep);
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
		display(szIntro[i].c_str(), 500);
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
		sleepThisThread(200);		
	}
	std::cout << std::endl;
}

void result(const wchar_t * sz, errorCode err = error_success) {
	if(err == error_success)
		progress();
	cls();
	display(imgResult.c_str());
	printf("%c[1;36m %ls %c[0m        \n\n", 27, sz, 27);
	sleepThisThread(2000);
	cls();
}

void runThread() {
	isRunThread = true;

	while (isRunThread) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		if (logic.rechargeHP()) {
			printf("\n\n★ 얏호 체력이 보충됐어요~~~!! [HP +1] ☆\n\n > ");
		}
		/*
		if(logic.setTradeMarketPrice())
			printf("\n\n 아이템 시세가 변경 됐어요~ \n\n > ");
		*/
	}
}
void printRaceRunning(int id, _raceParticipant *p) {
	wstring sz;
	if(id == raceParticipantNum)
		printf("%c[1;32m %d [me]",27, p->rank == 0 ? p->currentRank : 0);
	else
		printf("%c[0m %d [%02d]", 27, p->rank == 0 ? p->currentRank : 0, id + 1);
	
	for (int n = 0; n < p->ratioLength; n++) {
		sz += L" ";
	}
	//아이콘 표시
	sz += raceIcons[id];	

	if (p->currentSuffer != itemType_max) {
		sz += L" ";
		sz += to_wstring((int)p->currentSuffer);
	}

	sz += L" ";
	if(p->rank > 0)
		sz += to_wstring(p->rank);
		
	printf("%ls", sz.c_str());
}
void runRace() {
#ifdef _WIN32
	//const int line = 50;
	const int line = 103;
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
		printf("%s━\n", sz.c_str());
		for (int n = 0; n < p->size(); n++) {
			printRaceRunning(n, &p->at(n));
			if(n < p->size() -1)
				printf("\n━━━━%s\n", sz.c_str());
			else
				printf("\n");
		}
		printf("%c[0m━━━", 27);
		sz = "";
		for (int n = 0; n < line; n++)
			sz += "━";
		printf("%s━\n", sz.c_str());

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
		sleepThisThread(RACE_SLEEP);		
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
		sleepThisThread(1000);
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

void farmNotice(int fieldId) {
	printf("%d 농작물을 수확하세요 늦으면 썩어버립니다. \n", fieldId);
}

void tradeNotice(time_t updated) {
	printf("%I64d Trade 시세가 변경되었습니다. \n", updated);
}

bool init() {
	//setlocale(LC_ALL, "en_US.UTF-8");	
#ifdef _WIN32
	_wsetlocale(LC_ALL, L"korean");
#else
    setlocale( LC_ALL, "" );
    fwide( stdout, -1 );
#endif	
	if(!logic.init(farmNotice, tradeNotice))
		return false;
	
	logic.print(3);

	return true;
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

void farm_plant() {
	logic.print();	
	int idx;
	printf("심을 밭 > ");
	scanf("%d", &idx);
	int seedId;
	printf("심을 씨앗 > ");
	scanf("%d", &seedId);
	errorCode err = logic.farmingPlant(idx, seedId);
	result(logic.getErrorMessage(err), err);	
}

void farm_harvest() {		
	int idx;
	printf("수확할 밭 > ");
	scanf("%d", &idx);
	int productId;
	int point;
	errorCode err = logic.farmingHarvest(idx, productId, point);
	wstring sz = logic.getErrorMessage(err);
	sz += L"\n 소득 ----\n ";
	sz += logic.getItem(productId).name;
	sz += L" - 수량: ";
	sz += to_wstring(point);

	result(sz.c_str(), err);
}

void farm_care() {		
	int idx;
	printf("가꿀 밭 > ");
	scanf("%d", &idx);
	errorCode err = logic.farmingCare(idx);
	result(logic.getErrorMessage(err), err);
}

void farm_extend() {
	errorCode err = logic.farmingExtend();
	result(logic.getErrorMessage(err), err);
}

void farm() {
	logic.print(7);
	printf("1. 씨앗 심기, 2. 가꾸기, 3. 수확하기, 4, 밭 추가(200 차감), 0: 상태 보기 \n > ");
	int type;
	scanf("%d", &type);
	switch (type)
	{
	case 0:
		farm();
		break;
	case 1:
		farm_plant();
		break;
	case 2:
		farm_care();
		break;
	case 3:
		farm_harvest();
		break;
	case 4:
		farm_extend();
		break;
	default:
		break;
	}
}
void achieves() {	
	for (int i = 0; i < 2; i++) {		
		bool isDaily = i == 0 ? true : false;
		int size = logic.getAchievementSize(isDaily);
		for (int n = 0; n < size; n++) {
			achievement::detail p;
			logic.getAchievementDetail(isDaily, n, p);
			printf("[%s %02d] %ls \t %ls (%d 개) [%d / %d] %s %s \n"
				, isDaily ? "매일" : "전체"
				, n + 1
				, p.title.c_str()
				, logic.getItem(p.rewardId).name.c_str()
				, p.rewardVal
				, p.accumulation
				, p.goal
				, p.isFinished ? "완료" : ""
				, p.isReceived ? "수령" : ""
			);
		}
		printf("\n");
	}
}

bool ask() {
	display(imgIdle[getRandValue(IDLE_NUM)].c_str());
	logic.print();
	printf("------------------------------------------------------------------------ \n");
    printf( " 1: 액션 \n 2: 아이템 구매 \n 3. 아이템 판매 \n 4: 경묘 \n 5: 농사 \n 6: 체력보충 \n 7: 도감 보기 \n 8: 업적 보기  \n > ");
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
		farm();
        break;
    case 6:
		hp();
		break;
	case 7:
		logic.print(4);
		sleepThisThread(3000);        
		break;
	case 8:
		achieves();
		sleepThisThread(4000);
		break;
	default:
		break;
	}
	return true;
}

int main()
{	
	if (!init())
		return -1;
	intro();
	thread p(runThread);
		
	while (true) {
		if (!ask())
			break;		
	}
	logic.finalize();
	isRunThread = false;
	p.join();	
    return 0;
}

