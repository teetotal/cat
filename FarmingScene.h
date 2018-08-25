
#ifndef PROJ_ANDROID_FARMINGSCENE_H
#define PROJ_ANDROID_FARMINGSCENE_H

#include "MainScene.h"
#include "cocos2d.h"
#include "ui/ui.h"
#include "logics.h"

USING_NS_CC;

class FarmingScene : public Scene {
public:
	static Scene* createScene();

	virtual bool init();
	
	CREATE_FUNC(FarmingScene);

	Sequence * getFarmingAnimation(CallFuncN * cb);
	Sequence * getFarmingMistakeAnimation(CallFuncN * cb);

private:
	void callback(Ref* pSender, SCENECODE type);
	void callbackActionAnimation(Ref* pSender);
	void showItemSelect(int idx);
	void callbackSeed(Ref* pSender, int idx, int itemId);
	void updateFarming(float f); // ��Ȯ���� ������Ʈ
	LayerColor * mPopupLayer, *mPopupLayerBackground;

	void messagePopup(string msg);
	void closePopup() {
		if (mPopupLayerBackground != NULL) {
			mPopupLayerBackground->removeChild(mPopupLayer);
			this->removeChild(mPopupLayerBackground);

			delete mPopupLayer;
			delete mPopupLayerBackground;
		}

		mPopupLayer = NULL;
		mPopupLayerBackground = NULL;
	};

	Sprite * mCharacter;
	gui mGui;
	Label * mLabel; // ���ĸ����� �޼��� ������
	Layout * mFullLayer;
	MenuItemFont * mSeedButton[farmMaxField]; //�ɱ� ��ư 
	
	static FarmingScene * hInst;

	static void cultivationCB(int id);
	ui_cultivation mCultivation[farmMaxField];
	int mSleepCnt;
	queue<intPair> mHarvestQ;
};

#endif 