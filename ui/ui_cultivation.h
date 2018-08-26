//
// Created by Jung, DaeCheon on 05/08/2018.
//

#ifndef PROJ_ANDROID_UI_CULTIVATION_H
#define PROJ_ANDROID_UI_CULTIVATION_H

#include "ui.h"

#define FONT_MIN_SIZE 6

typedef void(*cultivationCB)(int id);

class ui_cultivation {
public:
    ui_cultivation() : mIsDecay(false){
		mSize = Size(0, 0);
    };
    virtual ~ui_cultivation(){
        CCLOG("ui_cultivation released");
    };

	//void addLevel(const string img, float maxLevel, const ccMenuCallback& callback, const string text = "");
    void addLevel(const string img, float maxLevel, const string text = "");

    Layout * init(
            int id
            , cultivationCB cb
            , float fontSize
			//, const string seedImg
            , const string decayImg
            , const string progressBarImg
            , Node * p
            , Vec2 position
            , bool isAutoDimension
            , Size dimension = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size grid = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size origin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE)
            , Size margin = Size(GRID_INVALID_VALUE, GRID_INVALID_VALUE));

	void finalize();
	void update(float percent, int remain, const string comment);
	float getPercent(){
        return mCurrentPercent;
    }
    void setDecay(bool b);

    int mId;
private:
    struct imgLevel{
        string img;
        float maxLevel;
        //ccMenuCallback callback;
        string text;
    };
    vector<imgLevel> mVec;

    Sprite * mImg;
    LoadingBar * mProgressBar;
    Label *mRemain, * mLabel, * mComment;

    float mFontSize;
    int mCurrentIdx;
    float mCurrentPercent;
    bool mIsDecay;

    Size mSize;
	string mDecayImg;

	gui mUI;
};


#endif //PROJ_ANDROID_UI_CULTIVATION_H
