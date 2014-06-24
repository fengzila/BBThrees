//
//  BBBlockSprite.cpp
//  puzzle
//
//  Created by FengZi on 14-1-23.
//
//

#include "BBBlockSprite.h"
#include "BBGameDataManager.h"

BBBlockSprite::BBBlockSprite()
{
    
}

BBBlockSprite::~BBBlockSprite()
{
    
}

BBBlockSprite* BBBlockSprite::create(int num)
{
    BBBlockSprite *sprite = new BBBlockSprite();
    if (sprite && sprite->initWithSpriteFrameName("blue.png"))
    {
        sprite->initData();
        sprite->initSprite(num);
        sprite->autorelease();
        sprite->setCascadeOpacityEnabled(true);
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void BBBlockSprite::initData()
{
    m_numId = 0;
}

void BBBlockSprite::initSprite(int num)
{
    m_numSpt = LabelTTF::create(__String::createWithFormat("%d", num)->getCString(), "GillSans-Bold", 90*BBGameDataManager::getInstance()->getScreenScale());
    m_numSpt->setPosition(Point(getContentSize().width/2, getContentSize().height/2));
    addChild(m_numSpt);
    
    m_numId = num;
    
    update();
    
    setScale(0);
    runAction(ScaleTo::create(.15, 1));
}

void BBBlockSprite::setIndex(int index)
{
    m_index = index;
}

int BBBlockSprite::getIndex()
{
    return m_index;
}

void BBBlockSprite::setNum(int num)
{
    m_numId = num;
    runAction(Sequence::create(ScaleTo::create(.15, 1.2), ScaleTo::create(.15, 1), NULL));
    update();
}

int BBBlockSprite::numId()
{
    return m_numId;
}

void BBBlockSprite::setXY(int x, int y)
{
    m_x = x;
    m_y = y;
}

int BBBlockSprite::getX()
{
    return m_x;
}

int BBBlockSprite::getY()
{
    return m_y;
}

void BBBlockSprite::update()
{
    if (m_numId == 0) {
        return;
    }
    if (m_numId == 1) {
        SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("blue.png");
        setDisplayFrame(frame);
        m_numSpt->setColor(Color3B(255, 255, 255));
    } else if (m_numId == 2) {
        SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("red.png");
        setDisplayFrame(frame);
        m_numSpt->setColor(Color3B(255, 255, 255));
    } else if (m_numId >= 3)
    {
        SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("white.png");
        setDisplayFrame(frame);
        m_numSpt->setColor(Color3B(0, 0, 0));
    }
    
    if (m_numId >= 100 && m_numId < 1000) {
        m_numSpt->setScale(.85);
    } else if (m_numId >= 1000 && m_numId < 10000)
    {
        m_numSpt->setScale(.65);
    } else if (m_numId >= 10000){
        m_numSpt->setScale(.45);
    }
    m_numSpt->setString(__String::createWithFormat("%d", m_numId)->getCString());
}

void BBBlockSprite::setMaxColor()
{
    m_numSpt->setColor(Color3B(253, 91, 128));
}

void BBBlockSprite::cancelMaxColor()
{
    update();
}

void BBBlockSprite::showScore(int score, bool isMaxScore)
{
    SpriteFrame *frame;
    switch (m_numId) {
        case 1:
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("gray_1.png");
            setDisplayFrame(frame);
            break;
        case 2:
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("gray_2.png");
            setDisplayFrame(frame);
            break;
            
        default:
            break;
    }
}