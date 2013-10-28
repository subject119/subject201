#include "UIManager.h"
#include "GameManager.h"

#include "AppMacros.h"

USING_NS_CC;

void UIManager::InitializeUI()
{
    InitializeScoreBoard();
}

void UIManager::InitializeScoreBoard()
{
    auto visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    auto origin = CCDirector::sharedDirector()->getVisibleOrigin();
    this->scoreBoardLabel = CCLabelTTF::create("Score: 0", "Arial", 16);
    this->scoreBoardLabel->setPosition(CCPoint(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - this->scoreBoardLabel->getContentSize().height));
    this->addChild(this->scoreBoardLabel, 1);
}

void UIManager::SetScore(const int score)
{
    std::string display= "Score: ";
    display += std::to_string(score);
    this->scoreBoardLabel->setString(display.c_str());
}