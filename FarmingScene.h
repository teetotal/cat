#ifndef __FARMING_SCENE_H__
#define __FARMING_SCENE_H__

#include "cocos2d.h"
#include "ui/ui.h"
#include "library/farming.h"
#include "MainScene.h"

#define EMPTY_PLANT_TAG -1
#define OPACITY_MAX 255
#define OPACITY_DIABLE 64
#define PLANT_COMPLETE_SEC 10
#define QUEST_CNT 4

class FarmingScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	virtual bool onTouchBegan(Touch* touch, Event* event);
	virtual bool onTouchEnded(Touch* touch, Event* event);
	virtual void onTouchMoved(Touch *touch, Event *event);		
    
       
    // implement the "static create()" method manually
    CREATE_FUNC(FarmingScene);
private:
#define PARTICLE_CLINK "particles/particle_clink.plist"

	gui mGui;
	Label * mPoint;

	enum Mode {
		Mode_Seed,
		Mode_Farming,
		Mode_Max
	};

	enum Plant_Status {
		Plant_Status_Crop,
		Plant_Status_Harvest,
		Plant_Status_Max
	};

	Mode mMode;
	Vec2 mCharacterInitPosition;

	struct seed {
		Layout * layout;
		MenuItemFont * label;
		int itemId;
		int itemQuantity;
	};

	bool mIsMove;
	Vec2 mTouchDownPosition;
	Size mGridSize;
	Sprite * mCharacter;
	ScrollView * mScrollView;
	Layout * mQuestLayer[QUEST_CNT];
	LayerColor * mPopupBackground, * mPopupLayer;

	int mThiefCnt;
	
	typedef std::vector<seed*> seedVector;	
	seedVector mSeedVector;

	int mCurrentNodeId;
    MainScene::field * mSelectedField;
    DrawNode * mSelectedFieldLayer;
    
    Layout * mMainLayoput;
    ui_deco mUIDeco;
    
    //funcs
    void initDeco();
    bool resetMode(bool isClearSelectedLayer = true);
    void selectField(MainScene::field * p);
    Vec2 getSpritePos(MainScene::field * p) {
        return Vec2(p->l->getPosition().x + (p->l->getContentSize().width / 2)
                             , p->l->getPosition().y + (p->l->getContentSize().height / 2));
    };
    float getScaleRatio(MainScene::field * p){
        return (p->l->getContentSize().height / p->sprite->getContentSize().height) * 0.8;
    };
    void setLabel(string sz, MainScene::field * p);
	
	void levelUp(MainScene::field * p);
	void clear(MainScene::field * p);
	void swap(MainScene::field* a, MainScene::field * b);
	
	void setOpacity();
	void clearOpacity();
	void createSeedMenu();
	//void addSeedMenu();
	void addSprite(MainScene::field * p, int seedId);

	void seedCallback(cocos2d::Ref* pSender, int seedId);
	void questCallback(cocos2d::Ref* pSender, int idx);
	void setQuest();

	void closeCallback(Ref * pSender) {
		Director::getInstance()->popScene();
	};
    void tillageCallback(Ref * pSender, MainScene::field * p);
	RepeatForever * getFarmingAnimation();
	void updateFarming(float f);

    
    void stopActionCharacter() {
        mCharacter->stopAllActions();
        mCharacter->setScale(1.f);
    };
    
	void stopAction(Sprite * p) {
		p->stopAllActions();
        p->setScale(mGridSize.height / p->getContentSize().height);
	};

	ParticleSystemQuad * createClinkParticle(Vec2 position) {
		return gui::inst()->createParticle(PARTICLE_CLINK, position);
	};
	//update point 
	void updatePoint();

	//show info
	void showInfo(MainScene::field * p);
	void closePopup(Ref * pSender);

	//thief
	void onActionFinished();
	Sequence * getThiefAnimate();

	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();
	virtual void onExit();
};

#endif // __FARMING_SCENE_H__
