//
//  BBHandleLayer.h
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//

#ifndef __puzzle__BBHandleLayer__
#define __puzzle__BBHandleLayer__

#include "cocos2d.h"

USING_NS_CC;

class BBHandleLayer : public Layer
{
public:
    BBHandleLayer();
    ~BBHandleLayer();
    virtual bool init();
    CREATE_FUNC(BBHandleLayer);
    
public:
    virtual bool        onTouchBegan(Touch *touch, Event * event);
    virtual void        onTouchEnded(Touch *touch, Event * event);
    virtual void        onTouchCancelled(Touch *touch, Event * event);
    virtual void        onTouchMoved(Touch *touch, Event * event);
    
private:
    Size                m_visibleSize;
    int                 m_blockLength;
    
    Sprite              *m_logoSpt;
    Sprite              *m_grid_bg;
    LabelTTF            *m_curScoreLabel;
    LabelTTF            *m_bestScoreLabel;
    __Array             *m_randomNumArr;
    float               m_relativeX;            // 所有格子的相对原点坐标
    float               m_relativeY;            // 所有格子的相对原点坐标
    int                 m_curScore;
    int                 m_maxScore;
    
    LayerColor          *m_testLayer;
    Layer               *m_resultLayer;
    
    bool                m_canTouch;
    Point               m_firstPoint;
    bool                m_needNewBlock;
    __Array             *m_needAppearAnim;
    int                 maxValue;
    int                 createRandomValueArrTimes;  // 创建随机数组的次数
    bool                m_gameOver;
    bool                m_hasOverBestScore;
    
private:
    void                initData();
    void                initSprite();
    void                resetData();
    
    void                createTopBanner();
    void                createGrid();
    void                refresh(Ref* obj);
    void                createRandomBlock();
    void                moveBlock(Touch *touch, Event * event);
    bool                withinBounds(int x, int y);
    Point               getNearestIdleBlock(int x, int y, int direction);
    Point               getNextBlock(int x, int y, int direction);
    void                mergeBlock(int x, int y, int direction);
    void                update();
    
    int                 getOccupiedBlockNum();
    void                resetBlock(Point curPoint, Point targetPoint);
    void                forwardBlock(int x, int y, int direction);
    void                removeNode(Node* node);
    Point               computePosByXY(int x, int y);
    int                 getRandomValueFromArr();
    void                createRandomValueArr();
    bool                hasMergeBlock();
    int                 getValueBy12Wave();
    bool                isEffectiveTouchDistance(Touch *touch);
    void                checkOver();
    int                 computeScore(int num);
    bool                touchResultLayer(Touch *touch, Event * event);
    void                showOverLayer();
    void                createScoreLabel(int score, Point pos, bool isMaxScore, Layer* parentLayer);
};

#endif /* defined(__puzzle__BBHandleLayer__) */
