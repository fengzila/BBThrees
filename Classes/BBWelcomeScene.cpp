//
//  BBWelcomeScene.cpp
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//

#include "BBWelcomeScene.h"
#include "BBGameScene.h"
#include "BBNewGuideLayer.h"

BBWelcomeScene::BBWelcomeScene()
{
    
}

BBWelcomeScene::~BBWelcomeScene()
{
    
}

Scene* BBWelcomeScene::createScene()
{
    auto scene = Scene::create();
    auto layer = BBWelcomeScene::create();
    scene->addChild(layer);
    return scene;
}

bool BBWelcomeScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    initData();
    initSprite();
    return true;
}

#pragma mark - initData
void BBWelcomeScene::initData()
{
    // add your codes here...
    
}
#pragma mark - initSprite
void BBWelcomeScene::initSprite()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    // 添加背景层
    addChild(LayerColor::create(Color4B(40, 40, 40, 255), visibleSize.width, visibleSize.height));
    
    addChild(BBNewGuideLayer::create());
}