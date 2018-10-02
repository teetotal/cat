
#ifndef PROJ_ANDROID_ALERTSCENE_H
#define PROJ_ANDROID_ALERTSCENE_H

#include "MainScene.h"
#include "cocos2d.h"
#include "ui/ui.h"
#include "logics.h"

USING_NS_CC;

class AlertScene : public Scene {
public:
	static Scene* createScene(errorCode err);

	bool init(errorCode err);
	
	static AlertScene * create(errorCode err)
	{ 
		AlertScene *pRet = new(std::nothrow) AlertScene(); 
		if (pRet && pRet->init(err)) 
		{ 
			pRet->autorelease(); 
			return pRet; 
		} 
		else 
		{ 
			delete pRet; 
			pRet = nullptr; 
			return nullptr; 
		} 
	};

private:
	void callback(Ref* pSender, SCENECODE type);
	void invokeItem(Ref* pSender, int id);	
	void closeScene() {
		Director::getInstance()->popScene();
	};

	Layout * mLayer;
};

#endif 