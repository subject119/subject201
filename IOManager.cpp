#include "IOManager.h"
#include "GameManager.h"
#include "SolveWizard.h"
#include "Map.h"
#include "Cell.h"

USING_NS_CC;

bool IOManager::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }

    this->setTouchEnabled(true);
    this->setTouchMode(ccTouchesMode::kCCTouchesOneByOne);
}

MapData* IOManager::GetMapData()
{
    mapData = new MapData();
    mapData->height = 9;
    mapData->width = 9;
    return mapData;
}

bool IOManager::ccTouchBegan(CCTouch* touch, CCEvent  *event)
{
    CCPoint mapCoord = this->gameManager->map->convertTouchToNodeSpace(touch);
    this->selectedCell = NULL;
    
    int height = this->gameManager->map->GetHeight();
    int width = this->gameManager->map->GetWidth();
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (this->gameManager->map->cells[row][col]->boundingBox().containsPoint(mapCoord))
            {
                this->selectedCell = this->gameManager->map->cells[row][col];
                break;
            }
        }
        if (this->selectedCell != NULL) break;
    }
    
    return true;
}

void IOManager::ccTouchMoved(CCTouch* touch, CCEvent  *event)
{
    if (this->selectedCell == NULL) return;

    CCPoint mapCoord = this->gameManager->map->convertTouchToNodeSpace(touch);
	CCPoint selectedPoint = this->selectedCell->getPosition();
	CCPoint delta = CCPoint(mapCoord.x - selectedPoint.x,  mapCoord.y - selectedPoint.y);
    float dist = sqrt(delta.x * delta.x + delta.y * delta.y);
    float threshold = this->selectedCell->getContentSize().width / 2.0;
    if (dist < threshold) return;
    
    float radian = atan2f(delta.y, delta.x);
    float angle = CC_RADIANS_TO_DEGREES(radian);
    DIRECTION target;
    if (angle > 0 && angle < 60) 
    {
        target = DIRECTION::DIR3;
    }
    else if (angle > -180 && angle < -120) 
    {
        target = DIRECTION::DIR6;
    }
    else if (angle > 120 && angle < 180)
    {
        target = DIRECTION::DIR5;
    }
    else if (angle > -60 && angle < 0)
    {
        target = DIRECTION::DIR2;
    }
    else if (angle > 60 && angle < 120){
        target = DIRECTION::DIR4;
    }
    else if (angle > -120 && angle < -60){
        target = DIRECTION::DIR1;
    }

    this->gameManager->solveWizard->SolveBySwap(*this->selectedCell, target);
}

void IOManager::ccTouchEnded(CCTouch* touch, CCEvent  *event)
{
    //if (this->selectedCell == NULL) return;
    //this->selectedCell->setPosition(Map::CalcCellPositionByIndex(this->selectedCell->GetRow(), this->selectedCell->GetCol()));

    //Cell *targetCell = NULL;

    //Point mapCoord = this->gameManager->map->convertTouchToNodeSpace(touch);
    //int height = this->gameManager->map->GetHeight();
    //int width = this->gameManager->map->GetWidth();
    //for (int i = 0; i < height; i++)
    //{
    //    for (int j = 0; j < width; j++)
    //    {
    //        if (this->gameManager->map->cells[i][j]->getBoundingBox().containsPoint(mapCoord))
    //        {
    //            targetCell = this->gameManager->map->cells[i][j];
    //            break;
    //        }
    //    }
    //    if (targetCell != NULL) break;
    //}

    //if (targetCell != NULL)
    //{
    //    this->gameManager->solveWizard->SolveBySwap(*this->selectedCell, *targetCell);
    //}
}