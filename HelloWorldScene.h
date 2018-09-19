#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/ui.h"
#include "library/farming.h"
#include "MainScene.h"

#define EMPTY_PLANT_TAG -1
#define OPACITY_MAX 255
#define OPACITY_DIABLE 64
#define PLANT_COMPLETE_SEC 10
#define QUEST_CNT 3

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	virtual bool onTouchBegan(Touch* touch, Event* event);
	virtual bool onTouchEnded(Touch* touch, Event* event);
	virtual void onTouchMoved(Touch *touch, Event *event);		
    
       
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
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
	
	typedef std::vector<seed*> seedVector;	
	seedVector mSeedVector;

	int mCurrentNodeId;
	
	void levelUp(MainScene::field * p);
	void clear(MainScene::field * p);
	void swap(MainScene::field* a, MainScene::field * b);
	
	void setOpacity();
	void clearOpacity();
	void createSeedMenu();
	void addSeedMenu();
	void addSprite(MainScene::field * p, int seedId);

	void seedCallback(cocos2d::Ref* pSender, int seedId);
	void questCallback(cocos2d::Ref* pSender, int idx);
	void setQuest();

	void closeCallback(Ref * pSender) {
		Director::getInstance()->popScene();
	};
	RepeatForever * getFarmingAnimation();
	void updateFarming(float f);

	void stopAction(Sprite * p) {
		p->stopAllActions();
		p->setScale(1.f);
	};

	ParticleSystemQuad * createClinkParticle(Vec2 position) {
		auto particle = ParticleSystemQuad::create(PARTICLE_CLINK);
		particle->setPosition(position);

		return particle;
	};
	//update point 
	void updatePoint();
};

#endif // __HELLOWORLD_SCENE_H__
