//
//  BBGameDataManager.h
//  puzzle
//
//  Created by FengZi on 14-2-26.
//
//

#ifndef __puzzle__BBGameDataManager__
#define __puzzle__BBGameDataManager__

#include <iostream>
#include "cocos2d.h"
USING_NS_CC;

class BBGameDataManager {
public:
    static BBGameDataManager* getInstance();
    inline int getScreenScale(){return m_screenScale;}
    inline void setScreenScale(int scale){m_screenScale = scale;}
    
private:
    int m_screenScale;
    
private:
    void initData();
};
#endif /* defined(__puzzle__BBGameDataManager__) */
