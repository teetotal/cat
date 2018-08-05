//
// Created by Jung, DaeCheon on 05/08/2018.
//

#ifndef PROJ_ANDROID_UI_CULTIVATION_H
#define PROJ_ANDROID_UI_CULTIVATION_H

#include "ui.h"

class ui_cultivation {
public:
    ui_cultivation() {
        
    };
    virtual ~ui_cultivation(){
        CCLOG("ui_cultivation released");
    };

	void addLevel(const string img, float maxLevel);
	void init(float duration, float maxOverTime, const string decayImg);
	void finalize();
	void start();
	
private:
	time_t mStart;
	float mDuration, mMaxOverTime;
	string mDecayImg;

	gui mUI;

};


#endif //PROJ_ANDROID_UI_CULTIVATION_H
