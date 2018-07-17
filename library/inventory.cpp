#include "inventory.h"

void inventory::pushItem(int category, int itemId, int quantity) {
	if (mCategory.find(category) == mCategory.end()) {
		intMap w;
		mWarehouses.push_back(w);
		mCategory[category] = mWarehouses.size() - 1;
	}

	if (mWarehouses[mCategory[category]].find(itemId) == mWarehouses[mCategory[category]].end()) {
		mWarehouses[mCategory[category]][itemId] = quantity;
	}
	else {
		mWarehouses[mCategory[category]][itemId] += quantity;
	}
}

bool inventory::popItem(int category, int itemId, int quantity) {
	if (!checkItemQuantity(category, itemId, quantity)) {
		return false;
	}

	int q = mWarehouses[mCategory[category]][itemId];
	if (q > quantity) {
		mWarehouses[mCategory[category]][itemId] -= quantity;
	} else if (q == quantity) {
		mWarehouses[mCategory[category]].erase(itemId);
	} 

	return true;
}

bool inventory::checkItemQuantity(int category, int itemId, int quantity) {
	if (mCategory.find(category) == mCategory.end()) {
		return false;
	}

	if (mWarehouses[mCategory[category]].find(itemId) == mWarehouses[mCategory[category]].end()) {
		return false;
	}
	else if (mWarehouses[mCategory[category]][itemId] < quantity) {
		return false;
	}

	return true;
}

void inventory::getWarehouse( vector<intPair> &vec, int category) {
	if (category != INVENTORY_INVALID_CATEGORY) {
		return getWarehouseByCategory(vec, category);
	}

	for (intMap::iterator it = mCategory.begin(); it != mCategory.end(); ++it) {
		getWarehouseByCategory(vec, it->first);
	}

}

void inventory::getWarehouseByCategory(vector<intPair> &vec, int category) {
	if (mCategory.find(category) != mCategory.end()) {
		int categoryId = mCategory[category];
		for (intMap::iterator it = mWarehouses[categoryId].begin(); it != mWarehouses[categoryId].end(); ++it) {
			
			intPair pair;
			pair.key = it->first;
			pair.val = it->second;

			vec.push_back(pair);
		}
	}
}