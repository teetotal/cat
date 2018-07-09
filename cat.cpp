// cat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "logics.h"
#include <fstream>
#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;
logics logic;

std::wstring utf8_to_utf16(const std::string& utf8)
{
	std::vector<unsigned long> unicode;
	size_t i = 0;
	while (i < utf8.size())
	{
		unsigned long uni;
		size_t todo;
		bool error = false;
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

void init() {
	//setlocale(LC_ALL, "en_US.UTF-8");
	//setlocale(LC_ALL, "");
	_wsetlocale(LC_ALL, L"korean");
	

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

		p.reward.strength = t[i]["reward"]["strength"].GetInt();
		p.reward.intelligence = t[i]["reward"]["intelligence"].GetInt();
		p.reward.appeal = t[i]["reward"]["appeal"].GetInt();
		p.reward.point = t[i]["reward"]["point"].GetInt();

		p.cost.strength = t[i]["cost"]["strength"].GetInt();
		p.cost.intelligence = t[i]["cost"]["intelligence"].GetInt();
		p.cost.appeal = t[i]["cost"]["appeal"].GetInt();
		p.cost.point = t[i]["cost"]["point"].GetInt();

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

	logic.init();

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
	const Value& race = d2["inventory"]["race"];
	for (SizeType i = 0; i < race.Size(); i++) {
		int id = race[i]["id"].GetInt();
		int quantity = race[i]["quantity"].GetInt();
		actor->inventory.race[id] = quantity;
	}
	const Value& adorn = d2["inventory"]["adorn"];
	for (SizeType i = 0; i < adorn.Size(); i++) {
		int id = adorn[i]["id"].GetInt();
		int quantity = adorn[i]["quantity"].GetInt();
		actor->inventory.adorn[id] = quantity;
	}

	logic.setActor(actor);
	logic.print();

	
}

void training() {
	logic.print(1);
	int key;
	printf("choose > ");
	scanf("%d", &key);

	errorCode err = logic.isValidTraining(key);
	if (err != error_success) {
		wprintf( L"%s \n", logic.getErrorMessage(err));
		return;
	}
	vector<_itemPair> rewards;
	_property property;
	int point;
	wprintf( L"%s \n", logic.getErrorMessage(logic.runTraining(key, rewards, &property, point)));
	wprintf( L"보상 내용 ---- \n Point: %d \n S: %d, I: %d, A: %d \n", point, property.strength, property.intelligence, property.appeal);
	for (int n = 0; n < rewards.size(); n++) {
		wprintf(L" %s(%d) \n"
			, logic.getItem(rewards[n].itemId).name.c_str()
			, rewards[n].val
		);		
	}	
}

void buy() {
	logic.setTradeMarketPrice();
	logic.print(2);

	int key, quantity;
	printf("ID > ");
	scanf("%d", &key);
	printf("Quantity > ");
	scanf("%d", &quantity);
	wprintf( L"%s \n", logic.getErrorMessage(logic.runTrade(true, key, quantity)));
	logic.print(0);
}

void sell() {
	logic.setTradeMarketPrice();	
	logic.print(0); 
	logic.print(2);

	int key, quantity;
	printf("ID > ");
	scanf("%d", &key);
	printf("Quantity > ");
	scanf("%d", &quantity);
	wprintf( L"%s \n", logic.getErrorMessage(logic.runTrade(false, key, quantity)));
	logic.print(0);
}

void hp() {
	int key, quantity;
	printf("HP Item ID > ");
	scanf("%d", &key);
	printf("Quantity > ");
	scanf("%d", &quantity);
	wprintf(L"%s \n", logic.getErrorMessage(logic.runRecharge(key, quantity)));
	logic.print(0);
}

void ask() {
	printf("------------------------------------------------------------------------ \n");
	wprintf( L" > 1: 액션, 2: 아이템 구매, 3. 아이템 판매, 4: 경묘, 5: 체력보충, 0: Actor Info \n > ");
	int key;
	scanf("%d", &key);
	switch (key)
	{
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
		break;
	case 5:
		hp();
		break;
	default:
		break;
	}
}


int main()
{
	init();
	while (true) {
		ask();					
	}
    return 0;
}

