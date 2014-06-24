//
//  BBHandleLayer.cpp
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//


#include "BBHandleLayer.h"
#include "GameCenterApi.h"
#include "BBGameDataManager.h"
#include "BBBlockSprite.h"

#define kBlockNum        4

const int rowMax = 4;
const int colMax = 4;
const int initBlockNum = 2;
int blockLength = 145;
//int grid[rowMax][colMax];
BBBlockSprite* grid[rowMax][colMax];
enum direction {
    kTop = 0,
    kRight = 1,
    kBottom = 2,
    kLeft = 3,
};

BBHandleLayer::BBHandleLayer()
{
    
}

BBHandleLayer::~BBHandleLayer()
{
    for (int i = 0; i < rowMax; i++) {
        for (int j = 0; j < colMax; j++) {
            resetBlock(Point(i, j), Point(i, j));
        }
    }
    
    m_randomNumArr->release();
    NotificationCenter::getInstance()->removeObserver(this, "kRefresh");
}

bool BBHandleLayer::init()
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
void BBHandleLayer::initData()
{
    m_visibleSize = Director::getInstance()->getVisibleSize();

    GameCenterApi::getInstance()->showAdBanner();
    
    if (BBGameDataManager::getInstance()->getScreenScale() == 1) {
        blockLength = 130;
        m_blockLength = 145;
    } else {
        blockLength = 290;
        m_blockLength = 326;
    }
    
    m_randomNumArr = __Array::create();
    m_randomNumArr->retain();
    
    resetData();
    
    createRandomValueArr();
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("block.plist", "block.png");
}

void BBHandleLayer::resetData()
{
    UserDefault* ud = UserDefault::getInstance();
    
    m_gameOver = false;
    m_canTouch = true;
    m_needNewBlock = true;
    maxValue = 6;
    createRandomValueArrTimes = 0;
    m_curScore = 0;
    m_maxScore = ud->getIntegerForKey("maxScore", 0);
    
    
    m_hasOverBestScore = false;
}

#pragma mark - initSprite
void BBHandleLayer::initSprite()
{
    auto bgLayer = LayerColor::create(Color4B(254, 247, 224, 255), m_visibleSize.width, m_visibleSize.height);
    addChild(bgLayer);
    
    createTopBanner();
    
    m_testLayer = LayerColor::create(Color4B(255, 0, 0, 255), 10, 10);
    
    createGrid();
    
    for (int i = 0; i < initBlockNum; i++) {
        createRandomBlock();
    }
    
//    addChild(m_testLayer);
    
    NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(BBHandleLayer::refresh), "kRefresh", NULL);
}

void BBHandleLayer::createTopBanner()
{
    m_logoSpt = Sprite::create("logo.png");
    addChild(m_logoSpt);
    
    Sprite *bestScore = Sprite::create("bg_bestScore.png");
    addChild(bestScore);
    
    m_bestScoreLabel = LabelTTF::create(__String::createWithFormat("%d", m_maxScore)->getCString(), "Helvetica-Bold", 40*BBGameDataManager::getInstance()->getScreenScale());
    m_bestScoreLabel->setColor(Color3B(255, 255, 255));
    m_bestScoreLabel->setPosition(Point(bestScore->getContentSize().width/2, bestScore->getContentSize().height/2 - 20));
    bestScore->addChild(m_bestScoreLabel);
    
    Sprite *curScore = Sprite::create("bg_score.png");
    addChild(curScore);
    
    m_curScoreLabel = LabelTTF::create("0", "Helvetica-Bold", 40*BBGameDataManager::getInstance()->getScreenScale());
    m_curScoreLabel->setColor(Color3B(255, 255, 255));
    m_curScoreLabel->setPosition(Point(curScore->getContentSize().width/2, curScore->getContentSize().height/2 - 20));
    curScore->addChild(m_curScoreLabel);
    
    if (BBGameDataManager::getInstance()->getScreenScale() == 1) {
        m_logoSpt->setPosition(Point(20 + m_logoSpt->getContentSize().width/2, m_visibleSize.height - m_logoSpt->getContentSize().height/2 - 20));
        bestScore->setPosition(Point(m_visibleSize.width - bestScore->getContentSize().width/2 - 90, m_visibleSize.height - bestScore->getContentSize().height/2 - 50));
        curScore->setPosition(Point(m_visibleSize.width - bestScore->getContentSize().width - curScore->getContentSize().width/2 - 90, m_visibleSize.height - curScore->getContentSize().height/2 - 50));
    } else {
        m_logoSpt->setPosition(Point(80 + m_logoSpt->getContentSize().width/2, m_visibleSize.height - m_logoSpt->getContentSize().height/2 - 40));
        bestScore->setPosition(Point(m_visibleSize.width - bestScore->getContentSize().width/2 - 220, m_visibleSize.height - bestScore->getContentSize().height/2 - 60));
        curScore->setPosition(Point(m_visibleSize.width - bestScore->getContentSize().width - curScore->getContentSize().width/2 - 230, m_visibleSize.height - curScore->getContentSize().height/2 - 60));
    }
}

void BBHandleLayer::createGrid()
{
    m_grid_bg = Sprite::create("bg_grid.png");
    m_grid_bg->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height - m_grid_bg->getContentSize().height/2 - m_logoSpt->getContentSize().height - 60*BBGameDataManager::getInstance()->getScreenScale()));
    addChild(m_grid_bg);
    
    m_relativeX = 20*BBGameDataManager::getInstance()->getScreenScale();
    m_relativeY = 20*BBGameDataManager::getInstance()->getScreenScale();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BBHandleLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BBHandleLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BBHandleLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_grid_bg);
    
//    for (int i = 0; i < rowMax; i++) {
//        for (int j = 0; j < colMax; j++) {
//            auto *label = LabelTTF::create(__String::createWithFormat("i:%dj:%d", i, j)->getCString(), "Arial", 30);
//            label->setColor(Color3B(255, 0, 0));
//            m_grid_bg->addChild(label);
//            
//            float blockX = m_relativeX + m_blockLength * i + blockLength/2;
//            float blockY = m_relativeY +m_blockLength * j + blockLength/2;
//            
//            label->setPosition(Point(blockX, blockY));
//        }
//    }
}

void BBHandleLayer::createRandomBlock()
{
    if (!m_needNewBlock) {
        return;
    }
    
    int occupiedNum = getOccupiedBlockNum();
    
    struct timeval psv;
    gettimeofday(&psv, NULL);
    unsigned long int rand_seed = psv.tv_sec*1000 + psv.tv_usec/1000;
    srand(rand_seed);
    
    int id = 0;
    int randIndex = arc4random()%(rowMax*colMax - occupiedNum);
    
    log("occupiedNum is %d, randIndex is %d", occupiedNum, randIndex);
    
    for (int i = 0; i < rowMax; i++) {
        for (int j = 0; j < colMax; j++) {
            if (grid[i][j] != NULL) {
                continue;
            }
            if (id == randIndex) {
                int randValue = getRandomValueFromArr();
                log("randValue is %d", randValue);
                
                BBBlockSprite *blockSpt = BBBlockSprite::create(randValue);
                blockSpt->setXY(i, j);
                m_grid_bg->addChild(blockSpt);

                float blockX = m_relativeX + m_blockLength * i + blockSpt->getContentSize().width/2;
                float blockY = m_relativeY +m_blockLength * j + blockSpt->getContentSize().height/2;

                blockSpt->setPosition(Point(blockX, blockY));
                
                grid[i][j] = blockSpt;
                
                return;
            }
            id++;
        }
    }
}

int BBHandleLayer::getOccupiedBlockNum()
{
    int occupiedNum = 0;
    
    for (int i = 0; i < rowMax; i++) {
        for (int j = 0; j < colMax; j++) {
            if (grid[i][j] == NULL) {
                continue;
            }
            BBBlockSprite* blockSpt = grid[i][j];
            if (blockSpt->numId() > 0) {
                occupiedNum++;
            }
        }
    }
    
    return occupiedNum;
}

void BBHandleLayer::createRandomValueArr()
{
    createRandomValueArrTimes++;
    
    m_randomNumArr->removeAllObjects();
    
    for (int i = 1; i <= 3; i++) {
        for (int j = 0; j < 4; j++) {
            m_randomNumArr->addObject(Integer::create(i));
        }
    }
}

int BBHandleLayer::getRandomValueFromArr()
{
    if (m_randomNumArr->count() <= 0) {
        // 当两轮12块随机取值结束后，
        createRandomValueArr();
        
        if (createRandomValueArrTimes % 2 == 0) {
            //
            if (maxValue <= 24) {
                return 3;
            }
            return getValueBy12Wave();
//            return 384;
        }
    }
    
    int randIndex = arc4random()%m_randomNumArr->count();
    int randValue = ((__Integer*)m_randomNumArr->getObjectAtIndex(randIndex))->getValue();
    m_randomNumArr->removeObjectAtIndex(randIndex);
    
    return randValue;
}

int BBHandleLayer::getValueBy12Wave()
{
    int randMax = floor(maxValue / 8);
    __Array *randomArr = __Array::create();
    randomArr->retain();
    int flagValue = 3;
    
    while (flagValue < randMax) {
        flagValue += flagValue;
        randomArr->addObject(Integer::create(flagValue));
    }
    
    int randIndex = arc4random()%randomArr->count();
    int randValue = ((__Integer*)randomArr->getObjectAtIndex(randIndex))->getValue();
    
    randomArr->release();
    
    return randValue;
}

bool BBHandleLayer::onTouchBegan(Touch *touch, Event * event)
{
    auto target = static_cast<Sprite*>(event->getCurrentTarget());//获取的当前触摸的目标
    Point locationInNode = target->convertToNodeSpace(touch->getLocation());
    Size s = target->getContentSize();
    Rect rect = Rect(0, 0, s.width, s.height);
    
    if (rect.containsPoint(locationInNode)) {
        m_firstPoint = touch->getLocation();
        
        m_canTouch = true;
        return true;
    } else {
        return false;
    }
}

void BBHandleLayer::refresh(Ref* obj)
{
    resetData();
    
    createRandomValueArr();
    
    for (int i = 0; i < rowMax; i++) {
        for (int j = 0; j < colMax; j++) {
            resetBlock(Point(i, j), Point(i, j));
        }
    }
    
    for (int i = 0; i < initBlockNum; i++) {
        createRandomBlock();
    }
    
    m_curScoreLabel->setString(__String::createWithFormat("%d", m_curScore)->getCString());
    m_bestScoreLabel->setString(__String::createWithFormat("%d", m_maxScore)->getCString());
}

void BBHandleLayer::resetBlock(Point curPoint, Point targetPoint)
{
    int curX = (int)curPoint.x;
    int curY = (int)curPoint.y;
    Point targetPointPos = computePosByXY((int)targetPoint.x, (int)targetPoint.y);
    float targetX = targetPointPos.x;
    float targetY = targetPointPos.y;
    BBBlockSprite* blockSpt = grid[curX][curY];
    if (blockSpt == NULL) {
        return;
    }
    grid[curX][curY] = NULL;
    blockSpt->runAction(Sequence::createWithTwoActions(Spawn::create(MoveTo::create(.15, Point(targetX, targetY)), FadeOut::create(.15), NULL), CallFuncN::create(CC_CALLBACK_1(BBHandleLayer::removeNode, this))));
}

void BBHandleLayer::removeNode(Node* node)
{
    node->stopAllActions();
    node->removeFromParentAndCleanup(true);
}

void BBHandleLayer::onTouchEnded(Touch *touch, Event * event)
{
    if (!isEffectiveTouchDistance(touch)) {
        // 手指滑动距离过短
        return;
    }
    
    update();
    createRandomBlock();
    checkOver();
}

bool BBHandleLayer::isEffectiveTouchDistance(Touch *touch)
{
    Point pos = touch->getLocation();
    
    log("touch move len is %f", sqrt(pow(pos.x - m_firstPoint.x, 2) + pow(pos.y - m_firstPoint.y, 2)));
    if (sqrt(pow(pos.x - m_firstPoint.x, 2) + pow(pos.y - m_firstPoint.y, 2)) < 35*BBGameDataManager::getInstance()->getScreenScale()) {
        // 手指滑动距离过短
        return false;
    }
    return true;
}

void BBHandleLayer::onTouchCancelled(Touch *touch, Event * event)
{
}

void BBHandleLayer::onTouchMoved(Touch *touch, Event * event)
{
    if (!m_canTouch) {
        return;
    }
    moveBlock(touch, event);
}

void BBHandleLayer::moveBlock(Touch *touch, Event * event)
{
    m_needNewBlock = false;
    
    Point pos = touch->getLocation();
    
    if (!isEffectiveTouchDistance(touch)) {
        // 手指滑动距离过短
        return;
    }
    
    double angle = atan2(pos.x - m_firstPoint.x, pos.y - m_firstPoint.y) * 180 / 3.14;
    
    m_needNewBlock = false;
    
    if (angle > -45 && angle < 45) {
        log("up");
        
        for (int i = 0; i < rowMax; i++) {
            for (int j = colMax - 1; j >= 0; j--) {
                mergeBlock(i, j, kTop);
                forwardBlock(i, j, kTop);
            }
        }
    } else if (angle > 45 && angle < 135) {
        log("right");
        
        for (int j = colMax - 1; j >= 0; j--) {
            for (int i = rowMax - 1; i >= 0; i--) {
                mergeBlock(i, j, kRight);
                forwardBlock(i, j, kRight);
            }
        }
    } else if ((angle > 135 && angle < 180) || (angle > -180 && angle < -135)) {
        log("down");
        for (int i = 0; i < rowMax; i++) {
            for (int j = 0; j < colMax; j++) {
                mergeBlock(i, j, kBottom);
                forwardBlock(i, j, kBottom);
            }
        }
    } else if (angle > -135 && angle < -45) {
        log("left");
        
        for (int j = 0; j < colMax; j++) {
            for (int i = 0; i < rowMax; i++) {
                mergeBlock(i, j, kLeft);
                forwardBlock(i, j, kLeft);
            }
        }
    }
    
    m_canTouch = false;
}

// 移动格子到最可能移动到的空位置
void BBHandleLayer::forwardBlock(int x, int y, int direction)
{
    if (grid[x][y] == NULL) {
        return;
    }
    
    Point nearestPoint = getNearestIdleBlock(x, y, direction);
    int nearestPointX = (int)nearestPoint.x;
    int nearestPointY = (int)nearestPoint.y;
    if (nearestPointX != -1 && nearestPointY != -1) {
        BBBlockSprite* blockSpt = grid[x][y];
        grid[nearestPointX][nearestPointY] = blockSpt;
        grid[x][y] = NULL;
        
        float blockX = m_relativeX + m_blockLength * nearestPointX + blockSpt->getContentSize().width/2;
        float blockY = m_relativeY +m_blockLength * nearestPointY + blockSpt->getContentSize().height/2;
        blockSpt->setXY(nearestPointX, nearestPointY);
        
        blockSpt->runAction(MoveTo::create(.15, Point(blockX, blockY)));
        
        m_needNewBlock = true;
    }
}

Point BBHandleLayer::getNearestIdleBlock(int x, int y, int direction)
{
    Point directionPoint;
    switch (direction) {
        case kTop:
            directionPoint = Point(0, 1);
            break;
        case kRight:
            directionPoint = Point(1, 0);
            break;
        case kBottom:
            directionPoint = Point(0, -1);
            break;
        case kLeft:
            directionPoint = Point(-1, 0);
            break;
            
        default:
            break;
    }
    
    Point nearestBlockPoint = Point(-1, -1);
    while (withinBounds(int(x + directionPoint.x), int(y + directionPoint.y)) && grid[int(x + directionPoint.x)][int(y + directionPoint.y)] == NULL) {
        x += directionPoint.x;
        y += directionPoint.y;
        nearestBlockPoint = Point(x, y);
    }
    return nearestBlockPoint;
}

Point BBHandleLayer::getNextBlock(int x, int y, int direction)
{
    Point nextBlockPoint = Point(-1, -1);
    switch (direction) {
        case kTop:
            for (int i = y - 1; i >= 0; i--) {
                if (!withinBounds(x, i)) {
                    continue;
                }
                if (grid[x][i] == NULL) {
                    continue;
                }
                nextBlockPoint = Point(x, i);
                break;
            }
            break;
        case kRight:
            for (int i = x - 1; i >= 0; i--) {
                if (!withinBounds(i, y)) {
                    continue;
                }
                if (grid[i][y] == NULL) {
                    continue;
                }
                nextBlockPoint = Point(i, y);
                break;
            }
            break;
        case kBottom:
            for (int i = y + 1; i < rowMax; i++) {
                if (!withinBounds(x, i)) {
                    continue;
                }
                if (grid[x][i] == NULL) {
                    continue;
                }
                nextBlockPoint = Point(x, i);
                break;
            }
            break;
        case kLeft:
            for (int i = x + 1; i < colMax; i++) {
                if (!withinBounds(i, y)) {
                    continue;
                }
                if (grid[i][y] == NULL) {
                    continue;
                }
                nextBlockPoint = Point(i, y);
                break;
            }
            break;
            
        default:
            break;
    }
    
    return nextBlockPoint;
}

Point BBHandleLayer::computePosByXY(int x, int y)
{
    float blockX = m_relativeX + m_blockLength * x + blockLength/2;
    float blockY = m_relativeY +m_blockLength * y + blockLength/2;
    
    return Point(blockX, blockY);
}

bool BBHandleLayer::withinBounds(int x, int y)
{
    return x >= 0 && x < rowMax && y >=0 && y < colMax;
}

void BBHandleLayer::mergeBlock(int x, int y, int direction)
{
    if (grid[x][y] == NULL) {
        return;
    }
    Point nextPoint = getNextBlock(x, y, direction);
    int nextPointX = (int)nextPoint.x;
    int nextPointY = (int)nextPoint.y;
    log("x is %x, y is %d, nextX is %d, nextY is %d", x, y, nextPointX, nextPointY);
    if (nextPoint.x != -1 && nextPoint.y != -1) {
        BBBlockSprite* nextBlockSpt = grid[nextPointX][nextPointY];
        BBBlockSprite* curBlockSpt = grid[x][y];
        if ((nextBlockSpt->numId() > 2 && nextBlockSpt->numId() == curBlockSpt->numId()) || nextBlockSpt->numId() + curBlockSpt->numId() == 3) {
            int newNumber = nextBlockSpt->numId() + curBlockSpt->numId();
            curBlockSpt->setNum(newNumber);
            resetBlock(Point(nextPointX, nextPointY), Point(x, y));
            
            if (newNumber > maxValue) {
                maxValue = newNumber;
            }
            
            m_needNewBlock = true;
        }
    }
}

bool BBHandleLayer::hasMergeBlock()
{
    for (int i = 0; i < rowMax; i++) {
        for (int j = 0; j < colMax; j++) {
            if (grid[i][j] == NULL) {
                continue;
            }
            if (j < colMax - 1) {
                BBBlockSprite* curBlockSpt = grid[i][j];
                BBBlockSprite* nextBlockSpt = grid[i][j+1];
                if ((curBlockSpt->numId() > 2 && curBlockSpt->numId() == nextBlockSpt->numId()) || curBlockSpt->numId() + nextBlockSpt->numId() == 3) {
                    return true;
                }
            }
        }
    }
    for (int j = 0; j < rowMax; j++) {
        for (int i = 0; i < colMax; i++) {
            if (grid[i][j] == NULL) {
                continue;
            }
            if (i < rowMax - 1) {
                BBBlockSprite* curBlockSpt = grid[i][j];
                BBBlockSprite* nextBlockSpt = grid[i+1][j];
                if ((curBlockSpt->numId() > 2 && curBlockSpt->numId() == nextBlockSpt->numId()) || curBlockSpt->numId() + nextBlockSpt->numId() == 3) {
                    return true;
                }
            }
        }
    }
    return false;
}

void BBHandleLayer::update()
{
    int totalScore = 0;
    for (int i = 0; i < rowMax; i++) {
        for (int j = 0; j < colMax; j++) {
            if (grid[i][j] == NULL) {
                continue;
            }
            BBBlockSprite* blockSpt = grid[i][j];
            if (blockSpt->numId() >= maxValue) {
                blockSpt->setMaxColor();
            } else {
                blockSpt->cancelMaxColor();
            }
            
            totalScore += computeScore(blockSpt->numId());
        }
    }
    if (m_curScore != totalScore) {
        m_curScore = totalScore;
        m_curScoreLabel->setString(__String::createWithFormat("%d", totalScore)->getCString());
        
        if (m_curScore >= m_maxScore) {
            // 破纪录
            m_curScoreLabel->runAction(Sequence::create(ScaleTo::create(.15, 1.2), ScaleTo::create(.15, .9), ScaleTo::create(.15, 1), NULL));
        }
    }
}

int BBHandleLayer::computeScore(int num)
{
    if (num < 3) {
        return 0;
    }
    log("computeScore: k is %d, score is %d", (int)(log(num/3)/log(2) + 1), (int)pow(3, (int)sqrt(num/3) + 1));
    return (int)pow(3, (int)(log(num/3)/log(2) + 1));
}

void BBHandleLayer::checkOver()
{
    if (m_gameOver) {
        return;
    }
    
    if (rowMax*colMax > getOccupiedBlockNum()) {
        return;
    }
    
    if (hasMergeBlock()) {
        return;
    }
    
    m_gameOver = true;
    
    m_resultLayer = Layer::create();
    getParent()->addChild(m_resultLayer);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event){return true;};
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [=](Touch* touch, Event* event){};
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_resultLayer);

    log("game over!");
    
    GameCenterApi::getInstance()->reportScore(m_curScore, "highScores");
    
    for (int i = 0; i < rowMax; i++) {
        for (int j = 0; j < colMax; j++) {
            if (grid[i][j] == NULL) {
                continue;
            }
            BBBlockSprite* blockSpt = grid[i][j];
            int score = computeScore(blockSpt->numId());
            if (blockSpt->numId() == maxValue) {
                blockSpt->showScore(score, true);
                createScoreLabel(score, Point(i, j), true, m_resultLayer);
            } else {
                blockSpt->showScore(score, false);
                createScoreLabel(score, Point(i, j), false, m_resultLayer);
            }
            
        }
    }
    
    if (m_curScore > m_maxScore) {
        // 破纪录
        m_hasOverBestScore = true;
        
        m_maxScore = m_curScore;
        
        UserDefault* ud = UserDefault::getInstance();
        ud->setIntegerForKey("maxScore", m_curScore);
        ud->flush();
    }
    
    runAction(Sequence::create(DelayTime::create(3.5), CallFunc::create([this](){
        showOverLayer();
    }), NULL));
}

void BBHandleLayer::createScoreLabel(int score, Point pos, bool isMaxScore, Layer* parentLayer)
{
    if (score <=0 ) {
        return;
    }
    auto *scoreLabel = LabelTTF::create(__String::createWithFormat("+%d", score)->getCString(), "Helvetica-Bold", 70*BBGameDataManager::getInstance()->getScreenScale());
    scoreLabel->enableShadow(Size(Point(2, -2)), 220, .5);
    if (isMaxScore) {
        scoreLabel->setColor(Color3B(192, 85, 104));
    } else {
        scoreLabel->setColor(Color3B(249, 199, 74));
    }
    
    Point curPos = computePosByXY(pos.x, pos.y);
    Point absolutePos = Point(curPos.x + m_grid_bg->getPositionX() - m_grid_bg->getContentSize().width/2, curPos.y + m_grid_bg->getPositionY() - m_grid_bg->getContentSize().height/2);
    scoreLabel->setPosition(Point(absolutePos.x, absolutePos.y));
    parentLayer->addChild(scoreLabel);
    scoreLabel->setOpacity(0);
    scoreLabel->setScale(0);
    
    float finalScale = 1.0f;
    if (score >= 1000 && score < 10000) {
        finalScale = .85;
    } else if (score >= 10000 && score < 100000) {
        finalScale = .75;
    } else if (score >= 100000 && score < 1000000) {
        finalScale = .65;
    }
    
    scoreLabel->runAction(Sequence::create(DelayTime::create(.45 + .25*(arc4random()%6)), ScaleTo::create(.15, 0, 1), ScaleTo::create(.15, 1), Spawn::create(MoveTo::create(.15, Point(absolutePos.x , absolutePos.y + m_blockLength/2)), FadeIn::create(.15), NULL), ScaleTo::create(.1, 1.1), ScaleTo::create(.1, finalScale), NULL));
}

void BBHandleLayer::showOverLayer()
{
    LayerColor *animLayer = LayerColor::create(Color4B(0, 0, 0, 50), m_visibleSize.width, m_visibleSize.height);
    m_resultLayer->addChild(animLayer);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BBHandleLayer::touchResultLayer, this);
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [=](Touch* touch, Event* event){};
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, animLayer);
    
    if (m_hasOverBestScore) {
        ParticleSystem *meteor1 = ParticleSystemQuad::create("boom.plist");
        meteor1->setPosition(Point(0, m_visibleSize.height));
        m_resultLayer->addChild(meteor1);
    
        ParticleSystem *meteor2 = ParticleSystemQuad::create("boom.plist");
        meteor2->setPosition(Point(m_visibleSize.width, m_visibleSize.height));
        m_resultLayer->addChild(meteor2);
    }

    auto *tipsTitleLabel = LabelTTF::create("Tap To Continue", "Arial", 40*BBGameDataManager::getInstance()->getScreenScale());
    tipsTitleLabel->setColor(Color3B(255, 255, 255));
    tipsTitleLabel->setPosition(Point(m_visibleSize.width/2, m_grid_bg->getPositionY() - m_grid_bg->getContentSize().height/2 - 30 * BBGameDataManager::getInstance()->getScreenScale()));
    m_resultLayer->addChild(tipsTitleLabel);
    
    tipsTitleLabel->runAction(RepeatForever::create(Sequence::create(FadeIn::create(.45), FadeOut::create(.65),DelayTime::create(.3), NULL)));
}

bool BBHandleLayer::touchResultLayer(Touch *touch, Event * event)
{
    NotificationCenter::getInstance()->postNotification("kRefresh");
    m_resultLayer->removeFromParentAndCleanup(true);
    m_resultLayer = NULL;
    
    return true;
}
