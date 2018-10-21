
#ifndef PROJ_ANDROID_DECOSCENE_H
#define PROJ_ANDROID_DECOSCENE_H

#include "MainScene.h"
#include "cocos2d.h"
#include "ui/ui.h"
#include "ui/ui_deco.h"
#include "logics.h"

USING_NS_CC;

class DecoScene : public Scene {
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(DecoScene);

private:
	Layout * mMainLayoput;
    ui_deco mDeco;
    
    void closeCallback(Ref * pSender);
    
    float mTouchGap;
    Vec2 mTouchStart;

    vector<Touch*> mTouchVec;
    int getValidTouchCnt(){
        int ret = 0;
        for(int n =0; n< mTouchVec.size(); n++){
            if(mTouchVec[n])
                ret++;
        }
        return ret;
    };
    
    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
};

#endif 
