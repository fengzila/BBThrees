//
//  BBGameDataManager.cpp
//  puzzle
//
//  Created by FengZi on 14-2-26.
//
//

#include "BBGameDataManager.h"

static BBGameDataManager *instance = NULL;

BBGameDataManager* BBGameDataManager::getInstance()
{
    if (instance == NULL) {
        instance = new BBGameDataManager();
        instance->initData();
    }
    
    return instance;
}

void BBGameDataManager::initData()
{   
    m_screenScale = 1;
}