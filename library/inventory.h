#pragma once
#include "pch.h"
#define INVENTORY_INVALID_CATEGORY -1
class inventory
{
public:
	inventory() {};
	virtual ~inventory() {};
	void pushItem(int category, int itemId, int quantity);
	bool popItem(int category, int itemId, int quantity);
	int getItemQuantuty(int category, int itemId);
	bool checkItemQuantity(int category, int itemId, int quantity);
	void getWarehouse(vector<intPair> &vec, int category = INVENTORY_INVALID_CATEGORY);
private:
	vector<intMap> mWarehouses;
	intMap mCategory;

	void getWarehouseByCategory(vector<intPair> &vec, int category);
};

