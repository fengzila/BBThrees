//
//  BBWelcomeScene.h
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//

#ifndef __puzzle__BBWelcomeScene__
#define __puzzle__BBWelcomeScene__

#include "cocos2d.h"
USING_NS_CC;

class BBWelcomeScene : public Layer
{
public:
    BBWelcomeScene();
    ~BBWelcomeScene();
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BBWelcomeScene);
    
private:
    void initData();
    void initSprite();
};

#endif /* defined(__puzzle__BBWelcomeScene__) */
