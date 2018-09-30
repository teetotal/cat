//
// Created by Jung, DaeChun on 24/08/2018.
//

#include "AlertScene.h"
#include "SimpleAudioEngine.h"

#define REWARD_POINT_ADVERTISEMENT 500
#define REWARD_HP_ADVERTISEMENT 10

Scene* AlertScene::createScene(errorCode err)
{
	return AlertScene::create(err);
}

bool AlertScene::init(errorCode err)
{	
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	auto bg = Sprite::create(BG_FARM);
	bg->setContentSize(Director::getInstance()->getVisibleSize());
	bg->setAnchorPoint(Vec2(0, 0));
	//bg->setOpacity(200);
	bg->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(bg);
	
	gui::inst()->addTextButton(8, 0, "BACK", this, CC_CALLBACK_1(AlertScene::callback, this, SCENECODE_CLOSESCENE), 0, ALIGNMENT_CENTER, Color3B::RED);

	auto layer = gui::inst()->createLayout(DEFAULT_LAYER_SIZE, "", true, Color3B::GRAY);
	layer->setPosition(
		Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y)
	);
	layer->setAnchorPoint(Vec2(0.5, 0.5));

	//gui::inst()->drawGrid(layer, layer->getContentSize(), Size(-1, -1), Size::ZERO, Size::ZERO);
		
	const Size grid = Size(-1, -1);
	string msg = wstring_to_utf8(logics::hInst->getErrorMessage(err));
	gui::inst()->addLabelAutoDimension(4, 1, msg, layer, 0, ALIGNMENT_CENTER, Color3B::BLACK, grid, Size::ZERO, Size::ZERO);

	SCENECODE sceneCode1 = SCENECODE_NONE;
	SCENECODE sceneCode2 = SCENECODE_NONE;

	if (err == error_not_enough_hp) {
		sceneCode1 = SCENECODE_HP_ADVERTISEMENT;
		sceneCode2 = SCENECODE_HP_SHOP;
		
		vector<intPair> vec;
		logics::hInst->getActor()->inven.getWarehouse(vec, (int)inventoryType_HP);

		if (vec.size() == 0) {
			gui::inst()->addTextButtonAutoDimension(4, 3, wstring_to_utf8(L"광고 보기"), layer
				, CC_CALLBACK_1(AlertScene::callback, this, sceneCode1)
				, 0
				, ALIGNMENT_CENTER
				, Color3B::YELLOW
				, grid
				, Size::ZERO
				, Size::ZERO
			);
		}
		else {
			POPUP_LIST(layer
				, Size(3, 5)
				, 2
				, Vec2(0, 7)
				, Vec2(9, 3)
				, Size::ZERO
				, 4
				, Size(120, 70)
				, (int n = 0; n < (int)vec.size(); n++)
				,
				, MainScene::getItemImg(vec[n].key)
				, CC_CALLBACK_1(AlertScene::invokeItem, this, vec[n].key)
				, "Lv." + to_string(logics::hInst->getItem(vec[n].key).grade)
				, wstring_to_utf8(logics::hInst->getItem(vec[n].key).name)				
				, "x " + to_string(vec[n].val)
				, "+HP " + to_string(logics::hInst->getItem(vec[n].key).value)
				, gui::inst()->EmptyString
			)
		}
		
		
	}
	else if (err == error_not_enough_point) {
		sceneCode1 = SCENECODE_POINT_ADVERTISEMENT;
		sceneCode2 = SCENECODE_POINT_SHOP;

		gui::inst()->addTextButtonAutoDimension(2, 3, wstring_to_utf8(L"광고 보기"), layer
			, CC_CALLBACK_1(AlertScene::callback, this, sceneCode1)
			, 0
			, ALIGNMENT_CENTER
			, Color3B::YELLOW
			, grid
			, Size::ZERO
			, Size::ZERO
		);

		gui::inst()->addTextButtonAutoDimension(6, 3, wstring_to_utf8(L"충전 하기"), layer
			, CC_CALLBACK_1(AlertScene::callback, this, sceneCode2)
			, 0
			, ALIGNMENT_CENTER
			, Color3B::YELLOW
			, grid
			, Size::ZERO
			, Size::ZERO
		);
	}

	gui::inst()->addTextButtonAutoDimension(4, 6, "CLOSE", layer
		, CC_CALLBACK_1(AlertScene::callback, this, SCENECODE_CLOSESCENE)
		, 0
		, ALIGNMENT_CENTER
		, Color3B::BLUE
		, grid
		, Size::ZERO
		, Size::ZERO
	);

	this->addChild(layer);
	return true;
}


void AlertScene::callback(Ref* pSender, SCENECODE type) {

	switch (type)
	{
	case SCENECODE_CLOSESCENE:
		Director::getInstance()->popScene();
		break;
	case SCENECODE_POINT_ADVERTISEMENT://포인트 광고 
	case SCENECODE_POINT_SHOP://포인트 충전
							  //임시
		this->addChild(gui::inst()->createParticle("particles/particle_clink.plist", 4, 3));
		logics::hInst->increasePoint(REWARD_POINT_ADVERTISEMENT);
		gui::inst()->addLabel(4, 3, COIN + to_string(REWARD_POINT_ADVERTISEMENT), this, 0, ALIGNMENT_CENTER, Color3B::ORANGE)
			->runAction(
			Sequence::create(
				EaseIn::create(ScaleBy::create(0.5, 3), 0.4)
				, EaseOut::create(ScaleBy::create(0.5, 1/3), 0.4)
				, CallFunc::create([this]() { closeScene();	})
				, NULL
			)
		);
		break;
	case SCENECODE_HP_ADVERTISEMENT://HP 광고
	case SCENECODE_HP_SHOP://HP 충전
						   //임시
		logics::hInst->increaseHP(REWARD_HP_ADVERTISEMENT);
		this->addChild(gui::inst()->createParticle("particles/particle_clink.plist", 4, 3));
		gui::inst()->addLabel(4, 3, "+HP" + to_string(REWARD_HP_ADVERTISEMENT), this, 0, ALIGNMENT_CENTER, Color3B::ORANGE)
			->runAction(
				Sequence::create(
					EaseIn::create(ScaleBy::create(0.5, 3), 0.4)
					, EaseOut::create(ScaleBy::create(0.5, 1 / 3), 0.4)
					, CallFunc::create([this]() { closeScene();	})
					, NULL
				)
			);
		break;	
	default:
		break;
	}
}

void AlertScene::invokeItem(Ref* pSender, int id) {
	errorCode err = logics::hInst->runRecharge(id, 1);
	if (err != error_success && err != error_levelup) {
		//alert(err);
	}
	else {
		this->addChild(gui::inst()->createParticle("particles/particle_clink.plist", 4, 3));
		gui::inst()->addLabel(4, 3, "+HP" + to_string(logics::hInst->getItem(id).value), this, 0, ALIGNMENT_CENTER, Color3B::ORANGE)
			->runAction(
				Sequence::create(
					EaseIn::create(ScaleBy::create(0.5, 3), 0.4)
					, EaseOut::create(ScaleBy::create(0.5, 1 / 3), 0.4)
					, CallFunc::create([this]() { closeScene();	})
					, NULL
				)
			);
	}
}