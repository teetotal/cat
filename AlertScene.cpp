//
// Created by Jung, DaeChun on 24/08/2018.
//

#include "AlertScene.h"
#include "SimpleAudioEngine.h"

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

	auto layer = gui::inst()->createLayout(Size(300, 200), "", true);
	layer->setPosition(
		Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y)
	);
	layer->setAnchorPoint(Vec2(0.5, 0.5));
	
	int idx = 0;
	const Size grid = Size(5, 3);
	string msg = wstring_to_utf8(logics::hInst->getErrorMessage(err));
	gui::inst()->addLabelAutoDimension(2, idx++, msg, layer, 0, ALIGNMENT_CENTER, Color3B::BLACK, grid, Size::ZERO, Size::ZERO);

	SCENECODE sceneCode1 = SCENECODE_NONE;
	SCENECODE sceneCode2 = SCENECODE_NONE;

	if (err == error_not_enough_hp) {
		sceneCode1 = SCENECODE_HP_ADVERTISEMENT;
		sceneCode2 = SCENECODE_HP_SHOP;
	}
	else if (err == error_not_enough_point) {
		sceneCode1 = SCENECODE_POINT_ADVERTISEMENT;
		sceneCode2 = SCENECODE_POINT_SHOP;
	}

	if (sceneCode1 != SCENECODE_NONE) {
		gui::inst()->addTextButtonAutoDimension(1, idx, wstring_to_utf8(L"광고 보기"), layer
			, CC_CALLBACK_1(AlertScene::callback, this, sceneCode1)
			, 0
			, ALIGNMENT_CENTER
			, Color3B::ORANGE
			, grid
			, Size::ZERO
			, Size::ZERO
		);

		gui::inst()->addTextButtonAutoDimension(3, idx++, wstring_to_utf8(L"충전 하기"), layer
			, CC_CALLBACK_1(AlertScene::callback, this, sceneCode2)
			, 0
			, ALIGNMENT_CENTER
			, Color3B::ORANGE
			, grid
			, Size::ZERO
			, Size::ZERO
		);
	}


	gui::inst()->addTextButtonAutoDimension(2, 2, "CLOSE", layer
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
		logics::hInst->increasePoint(500);
		Director::getInstance()->popScene();
		break;
	case SCENECODE_HP_ADVERTISEMENT://HP 광고
	case SCENECODE_HP_SHOP://HP 충전
						   //임시
		logics::hInst->increaseHP(10);
		Director::getInstance()->popScene();
		break;	
	default:
		break;
	}
}
