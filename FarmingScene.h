
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

	Label* mLabel;

private:
	void callback(Ref* pSender, SCENECODE type);
	void showItemSelect(int idx);
	void callbackSeed(Ref* pSender, int idx, int itemId);

	LayerColor * mPopupLayer, *mPopupLayerBackground;

	gui mGui;
	Layout * mFullLayer;
	MenuItemFont * mSeedButton[farmMaxField]; //심기 버튼 
	
	static FarmingScene * hInst;

	static void cultivationCB(int id);
	ui_cultivation mCultivation[farmMaxField];
	
};

#endif 