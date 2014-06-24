//
//  BBBlockSprite.h
//  puzzle
//
//  Created by FengZi on 14-1-23.
//
//

#ifndef __puzzle__BBBlockSprite__
#define __puzzle__BBBlockSprite__

#include "cocos2d.h"
USING_NS_CC;

class BBBlockSprite :public Sprite
{
public:
    BBBlockSprite();
    ~BBBlockSprite();
    static BBBlockSprite* create(int num);
    
public:
    void            setIndex(int index);
    int             getIndex();
    void            setNum(int num);
    int             numId();
    void            setXY(int x, int y);
    int             getX();
    int             getY();
    void            update();
    void            setMaxColor();
    void            cancelMaxColor();
    void            showScore(int score, bool isMaxScore);
    
private:
    int             m_numId;
    int             m_index;
    int             m_x;
    int             m_y;
    
    LabelTTF        *m_numSpt;
    
private:
    void            initData();
    void            initSprite(int num);
};

#endif /* defined(__puzzle__BBBlockSprite__) */
