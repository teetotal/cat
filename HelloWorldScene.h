/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/ui.h"
#include "library/farming.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	virtual bool onTouchBegan(Touch* touch, Event* event);
	virtual bool onTouchEnded(Touch* touch, Event* event);
	virtual void onTouchMoved(Touch *touch, Event *event);		
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
#define PARTICLE_CLINK "particles/particle_clink.plist"

	gui mGui;

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

	struct field {
		int tag;
		int plantTag;
		Vec2 position;
		Layout * l;
		Label * label;		
	};

	struct plant {
		int tag;
		int fieldTag;
		Vec2 position;
		Sprite * sprite;
		int type;
		int level;
		time_t start;		//심은 시각
		int accumulation;	//현재까지 누적 수확
		//bool isCropAction;		//Crop Action 중
		bool isHarvestAction;
		
		plant() {
			this->accumulation = 0;
			this->fieldTag = 0;
			this->level = 0;
			this->start = getNow();
			//this->isCropAction = false;
			this->isHarvestAction = false;
		};
	};

	bool mIsMove;
	Vec2 mTouchDownPosition;
	Size mGridSize;
	Sprite * mCharacter;

	typedef std::map<int, field *> fieldMap;
	typedef std::map<int, plant *> plantMap;
	fieldMap mMap;
	plantMap mPlantMap;

	int mCurrentNodeId;

	void levelUp(int tag);
	void clear(int tag);
	void swap(plant* a, plant * b);
	void setOpacity();
	void clearOpacity();
	int getPlantId();
	void createSeedMenu();
	void plantAnimation(plant * node, int cnt);

	void seedCallback(cocos2d::Ref* pSender, int seedId);
	void closeCallback(Ref * pSender) {
		Director::getInstance()->popScene();
	};
	RepeatForever * getFarmingAnimation();
	void updateFarming(float f);
	Plant_Status getStatus(plant * p, int &cnt);

	void stopAction(Sprite * p) {
		p->stopAllActions();
		p->setScale(1.f);
	};

	ParticleSystemQuad * createClinkParticle(Vec2 position) {
		auto particle = ParticleSystemQuad::create(PARTICLE_CLINK);
		particle->setPosition(position);

		return particle;
	};
};

#endif // __HELLOWORLD_SCENE_H__
