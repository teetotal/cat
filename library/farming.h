#pragma once
#include "pch.h"

class farming
{
public:
	farming() {};
	virtual ~farming() {};

	enum farming_status {
		farming_status_sprout = 0, //새싹
		farming_status_week, //돌봄 필요
		farming_status_good,	 //건강
		farming_status_decay,	 //썩음
		farming_status_grown,	//다 자람
		farming_status_max
	};

	struct seed { //씨앗
		int id;
		wstring name;	//이름		
		int outputMax;	//개당 최대 아웃풋		
		int timeGrow;	//성장에 필요한 시간 (초)		
		int cares;		//필요한 돌봄 회수
		int maxOvertime;	//완료 이후 상하지 않고 버틸 수 있는 시간
	};

	struct crop { //작물		
		int seedId;
		farming_status status;	//상태		
		time_t timePlant;	 //심은 시간
		int cntCare;	//돌본 횟수
		time_t timeLastGrow; //지난 돌봄 시간
		int boost;		// 추가 output

		crop() {
			seedId = 0;
			status = farming_status_max;	//상태				
			timePlant = 0;	 //심은 시간
			cntCare = 0;	//돌본 횟수
			timeLastGrow = 0; //지난 돌봄 시간
			boost = 0;		// 추가 output
		};

		void plant(int seedId) {
			this->seedId = seedId;
			status = farming_status_sprout;			
			timePlant = getNow();
			timeLastGrow++; //처음 심을때 한번은 돌본 걸로 설정
		}
	};	

	struct field : crop { //밭
		int id;		
		int x;
		int y;	

		field(int x, int y) {			
			this->x = x;
			this->y = y;
		}
	};
	//소유중인 밭
	typedef vector<field*> fields;

	/*
	functions
	*/
		
	void addField(int x, int y);	//밭 늘리기
	
	fields getFields() {			//밭 목록
		return mFields;
	};	
								
	void setStatus();				//농작물 상태 설정
	
	void setStatus(int fieldIdx);
	
	int harvest(int fieldIdx);		//수확
	
	bool plant(int fieldIdx, int seedId);	//심기
		
	void addSeed(int id, seed *s) {	//씨앗 등록
		mSeed[id] = s;
	};
private:	
	fields mFields;	

	map<int, seed*> mSeed;
};

