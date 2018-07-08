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

int main()
{
	setlocale(LC_ALL, "en_US.UTF-8");
	//setlocale(LC_ALL, "");
	//_wsetlocale(LC_ALL, L"korean");
	logics logic;

	std::ifstream fileopen;
	fileopen.open("resource/meta.json", ios::in | ios::binary);
	std::string str((std::istreambuf_iterator<char>(fileopen)), std::istreambuf_iterator<char>());
	fileopen.close();

	Document d;
	d.Parse(str.c_str());
	//Add Items
	const Value& valueInfo = d["items"];
	for (SizeType i = 0; i < valueInfo.Size(); i++)
	{
		_item p;	
		p.id = valueInfo[i]["id"].GetInt();
		p.type = (itemType)valueInfo[i]["type"].GetInt();
		p.value = valueInfo[i]["value"].GetInt();
		p.grade = valueInfo[i]["grade"].GetInt();
		p.name = valueInfo[i]["name"].GetString();
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
		p.name = t[i]["name"].GetString();

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

	Document d2;
	d2.Parse(sz.c_str());
	_actor* actor = new _actor;

	actor->userName = d2["userName"].GetString();
	actor->userId = d2["userId"].GetString();

	actor->name = d2["name"].GetString();
	actor->id = d2["id"].GetString();

	actor->point = d2["point"].GetInt();
	actor->property.strength = d2["property"]["strength"].GetInt();
	actor->property.intelligence = d2["property"]["intelligence"].GetInt();
	actor->property.appeal = d2["property"]["appeal"].GetInt();

	//인벤토리
	logic.setActor(actor);

	while (true) {
		::getchar();
		logic.setTradeMarketPrice();
		logic.print();
	}

    return 0;
}

