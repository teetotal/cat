
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
	void updateFarming(float f); // 수확상태 업데이트
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
	Label * mLabel; // 훔쳐먹은거 메세지 때문에
	Layout * mFullLayer;
	MenuItemFont * mSeedButton[farmMaxField]; //심기 버튼 
	
	static FarmingScene * hInst;

	static void cultivationCB(int id);
	ui_cultivation mCultivation[farmMaxField];
	int mSleepCnt;
	queue<intPair> mHarvestQ;
};

#endif 