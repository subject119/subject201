#include "GameManager.h"
#include "AppMacros.h"

#include "Map.h"
#include "SolveWizard.h"
#include "IOManager.h"
#include "Cell.h"
#include "ScoreManager.h"
#include "UIManager.h"

USING_NS_CC;

CCScene* GameManager::scene()
{
    // 'scene' is an autorelease object
    auto scene = CCScene::create();
    auto *gameManagerlayer = GameManager::create();
    scene->addChild(gameManagerlayer);

    return scene;
}

// on "init" you need to initialize your instance
bool GameManager::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    auto visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    auto origin = CCDirector::sharedDirector()->getVisibleOrigin();

    this->solveWizard = new SolveWizard();
    this->addChild(static_cast<CCNode *>(this->solveWizard));
    this->solveWizard->gameManager = this;

    this->scoreManager = new ScoreManager();
    this->scoreManager->gameManager = this;

    this->map = Map::create();
    this->map->gameManager = this;
    this->addChild(this->map);
    this->map->setPosition(CCPoint(origin.x + 150, origin.y + 60));

    this->iOManager = IOManager::create();
    this->iOManager->gameManager = this;
    this->addChild(this->iOManager);

    this->UIManager = UIManager::create();
    this->UIManager->gameManager = this;
    this->addChild(this->UIManager);
    this->UIManager->setPosition(CCPoint(0, 0));

    srand (time(NULL));
    Cell::CacheCellTexture();
    this->UIManager->InitializeUI();
    this->map->Reset(*(this->iOManager->GetMapData())); 
    this->scoreManager->StartRecording();

    return true;
}

long GameManager::GetTime()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec * 1000 + now.tv_sec / 1000);
}