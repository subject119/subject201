
#include <time.h>

#include "SolveWizard.h"
#include "GameManager.h"
#include "ScoreManager.h"
#include "UIManager.h"
#include "IOManager.h"

USING_NS_CC;

SolveWizard::SolveWizard()
{
    this->fallingCount = 0;
    this->resolvingCount = 0;
    this->gameState = GameStates::Start;
    this->swapDir = DIRECTION::DIR1;
}

void SolveWizard::onEnter()
{
    CCNode::onEnter();

    this->scheduleUpdate();
}

void SolveWizard::SolveBySwap(Cell &cellA, DIRECTION dir)
{
    Cell *target = this->gameManager->map->Neighbor(cellA, dir);
    if (target != NULL)
    {
        this->swapCells[0] = &cellA;
        this->swapCells[1] = target;
        this->swapDir = dir;
        GemColor tempColor = cellA.GetColor();
        GemType tempType = cellA.GetGemType();
        DIRECTION tempDir = cellA.GetDirection();
        // modify both color and type
        cellA.SetColorGemTypeDir(target->GetColor(), target->GetGemType(), target->GetDirection());
        target->SetColorGemTypeDir(tempColor, tempType, tempDir);

        if (ExplodeNowBySwap(&cellA, target)) 
        {
            this->gameState = GameStates::ExplodingHighGems;
            return;
        }
        
        if (QuickTestSolvable())
        {
            this->gameState = GameStates::Start;
        }
        else
        {
            // if swap doesn't have effect, swap cells back
            target->SetColorGemTypeDir(cellA.GetColor(), cellA.GetGemType(), cellA.GetDirection());
            cellA.SetColorGemTypeDir(tempColor, tempType, tempDir);
        }
    }
    else
    {
        this->swapCells[0] = NULL;
        this->swapCells[1] = NULL;
        this->swapDir = DIRECTION::DIR1;
    }
}

void SolveWizard::update(float dt)
{
    switch (this->gameState)
    {
    case GameStates::Start:
        {
            this->gameManager->iOManager->setTouchEnabled(false);
            MarkStraights();
            if (this->resolvingCells.empty())
            {
                this->gameManager->iOManager->setTouchEnabled(true);
                this->gameState = GameStates::WaitingForUserInput;
            }
            else
            {
                this->gameState = GameStates::ExplodingHighGems;
            }
        }
        break;
    case GameStates::ExplodingHighGems:
        {
            this->gameManager->iOManager->setTouchEnabled(false);
            if (this->explosiveHighGems.empty())
            {
                this->gameState = GameStates::Resolving;
                this->Resolve();
            }
            else
            {
                ExplodeExplosiveHighGems();
            }
        }
        break;
    case GameStates::Resolving:
        {
            if (this->resolvingCount > 0)
            {
                return;
            }
            else
            {
                this->StartToFall(DIRECTION::DIR4);
                if (this->fallingCount > 0)
                {
                    return;
                }
                else
                {
                    this->gameState = GameStates::Start;
                }
            }
        }
        break;
    }
}

void SolveWizard::MarkStraights()
{
    this->resolvingCells.clear();
    MarkResolvableByDirection(DIRECTION::DIR2);
    MarkResolvableByDirection(DIRECTION::DIR3);
    MarkResolvableByDirection(DIRECTION::DIR4);
}

void SolveWizard::Resolve()
{
    GenerateNewHighGem();
    int resolved = ResolveGems();

    // TODO, temp, update score
    this->gameManager->scoreManager->AddToScore(resolved);
    this->gameManager->UIManager->SetScore(this->gameManager->scoreManager->GetScore());
}

void SolveWizard::GenerateNewHighGem()
{
    for (auto straight : this->resolvingCells)
    {
        for (auto cs : straight.gems)
        {
            // generate C2
            if (cs->resolving == 2)
            {
                cs->SetColorGemTypeDir(cs->GetColor(),  GemType::Cross2, DIRECTION::DIR1);
                cs->resolving = 0;
                cs->exploded = false;
            }
            // generate C3
            else if (cs->resolving == 3)
            {
                cs->SetColorGemTypeDir(cs->GetColor(),  GemType::Cross3, DIRECTION::DIR1);
                cs->resolving = 0;
                cs->exploded = false;
            }
        }

        if (straight.gems.size() < 4) continue;

        GemType tc = GemType::Normal;
        Cell *cc = GetCause(straight);
        // Don't forget setting direction for type Straight4
        // straight is not casued by user swapping, turn the first gem of the straight
        if (cc == NULL) 
        {
            cc = straight.gems.front();
            cc->SetColorGemTypeDir(cc->GetColor(), tc, straight.direction);
        }
        // generate S4 type high gem
        if (straight.gems.size() == 4) 
        {    
            tc = GemType::Straight4;
            cc->SetColorGemTypeDir(cc->GetColor(), tc, this->swapDir);
        }
        if (straight.gems.size() == 5)
        {
            tc = GemType::Straight5;
            // S5 has unique color
            cc->SetColorGemTypeDir(GemColor::S5, tc, this->swapDir);
        }
        
        cc->resolving = 0;
        cc->exploded = false;
    }
}

// only mark Cell::resolving flag, don't change color or type, color changes in Resolving()
bool SolveWizard::MarkResolvableByDirection(const DIRECTION dir)
{
    std::vector<Cell*> heads;
    GenerateHeads(dir, heads);

    for (int i = 0; i < heads.size(); i++)
    {
        Cell *begin = heads[i];
        Cell *end = begin;
        int len = 0;
        while (end != NULL && end->type != CellType::Outspace)
        {
            if (begin->GetColor() == end->GetColor())
            {
                end = this->gameManager->map->Neighbor(*end, dir);
                ++len;
            }
            else
            {
                if (len > 2)
                {
                    this->resolvingCells.push_back(Resolvable());
                    this->resolvingCells.back().direction = dir;
                    while (begin->GetColor() != end->GetColor())
                    {
                        this->resolvingCells.back().gems.push_back(begin);
                        begin->resolving++;
                        if (begin->GetGemType() != GemType::Normal)
                        {
                            this->explosiveHighGems.push(begin);
                        }
                        begin = this->gameManager->map->Neighbor(*begin, dir);
                    }
                }
                begin = end;
                len = 0;
            }
        }
        if (len > 2)
        {
            this->resolvingCells.push_back(Resolvable());
            this->resolvingCells.back().direction = dir;
            while (begin != NULL)
            {
                this->resolvingCells.back().gems.push_back(begin);
                begin->resolving++;
                if (begin->GetGemType() != GemType::Normal)
                {
                    this->explosiveHighGems.push(begin);
                }
                begin = this->gameManager->map->Neighbor(*begin, dir);
            }
        }
    }

    return this->resolvingCells.size();
}

// mark Cell::resolving flag, change type afterwards, don't change color
// high gems are released here
void SolveWizard::ExplodeExplosiveHighGems()
{
    while (!this->explosiveHighGems.empty()) 
    {
        //assert(this->explosiveHighGems.front()->GetGemType() != GemType::Normal);
        this->explosiveHighGems.front()->Explode();
        this->explosiveHighGems.pop();
    }
}

// Resolve high/normal gem
int SolveWizard::ResolveGems()
{
    float animDuration = 0.15f;
    int height = this->gameManager->map->GetHeight();
    int width = this->gameManager->map->GetWidth();
    int resolved = 0;

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            Cell *cell = this->gameManager->map->cells[row][col];
            if (cell->resolving > 0)
            {
                ResolveWithAnim(cell, animDuration);
                resolved++;
            }
        }
    }

    return resolved;
}

Cell* SolveWizard::GetCause(Resolvable &straight)
{
    auto cell = std::find(straight.gems.begin(), straight.gems.end(), this->swapCells[0]);
    if (cell != straight.gems.end())
    {
        return *cell;
    }
    else
    {
        cell = std::find(straight.gems.begin(), straight.gems.end(), this->swapCells[1]);
        if (cell != straight.gems.end())
        {
            return *cell;
        }
    }

    return NULL;
}

// reset all gemss (including high gem) color, gemtype and direction
void SolveWizard::ActionResolveEnds(Cell* cell)
{
    this->resolvingCount--;
    // reset both color and type
    cell->SetColorGemTypeDir(GemColor::Vacant, GemType::Normal, DIRECTION::DIR1);
    cell->resolving = 0;
    cell->setScale(1.0f);
    cell->setOpacity(255.0f);
}

// anim only for normal type gem resolving
void SolveWizard::ResolveWithAnim(Cell *cell, const float animDuration)
{
    this->resolvingCount++;
    auto *fadeOut = CCFadeOut::create(animDuration);
    auto *scale = CCScaleTo::create(animDuration, 0.25f);
    auto actionResolveEnds = CCCallFuncO::create(this, callfuncO_selector(SolveWizard::ActionResolveEnds), cell);
    CCSequence *seq = CCSequence::create(scale, actionResolveEnds, NULL);
    cell->runAction(fadeOut);
    cell->runAction(seq);
}

void SolveWizard::StartToFall(const DIRECTION dir)
{
    unsigned int fallingTime = this->gameManager->map->GetTimer();
    for (int col = 0; col < this->gameManager->map->GetWidth(); col++)
    {
        int offset = 0; // offset == (newPos - pos) * height
        Cell *pos = this->gameManager->map->cells[0][col];
        Cell *newPos = pos;

        // find the lowest vacant cell
        while (pos != NULL && pos->GetColor() != GemColor::Vacant)
        {
            pos = this->gameManager->map->Neighbor(*pos, dir);
        }
        // now, pos->GetColor() == GemColor::Vacant

        newPos = pos;
        while (newPos != NULL)
        {
            // find the next non-vacant cell newPos
            if (newPos->GetColor() != GemColor::Vacant)
            {
                // 1. move cell pos to the cell newPos' position
                pos->setPosition(this->gameManager->map->GetCellOriginalPos(*newPos));
                // 2. set cell pos' color to the cell newPos', newPos's to vacant
                // change both color and type
                pos->SetColorGemTypeDir(newPos->GetColor(), newPos->GetGemType(), newPos->GetDirection());
                newPos->SetColorGemTypeDir(GemColor::Vacant, GemType::Normal, DIRECTION::DIR1);
                // 3. set pos as falling
                pos->falling = true; 
                pos->fallingTime = fallingTime;
                this->fallingCount++;
                // move pos and newPos to next cell
                pos = this->gameManager->map->Neighbor(*pos, dir);
                newPos = this->gameManager->map->Neighbor(*newPos, dir);
            }
            else
            {
                offset++; // pos + offset = newPos
                newPos = this->gameManager->map->Neighbor(*newPos, dir);
            }
        }
        // if all upper cells above pos are vacant
        while (pos != NULL)
        {
            // 1. move pos according to offset
            pos->setPosition(Map::CalcCellPositionByIndex(pos->GetRow() + offset, pos->GetCol()));
            // 2. assgin random color to pos
            pos->SetColorGemTypeDir(Cell::RandomColor(), GemType::Normal, DIRECTION::DIR1);
            // 3. set pos as falling
            pos->falling = true;
            pos->fallingTime = fallingTime;
            this->fallingCount++;
            // move pos to next
            pos = this->gameManager->map->Neighbor(*pos, dir);
        }
    }
}

bool SolveWizard::QuickTestSolvable()
{
    bool resolved = MarkResolvableByDirection(DIRECTION::DIR2) || 
        MarkResolvableByDirection(DIRECTION::DIR3) || 
        MarkResolvableByDirection(DIRECTION::DIR4);
    ClearResolvingFlags();
    this->resolvingCells.clear();
    return resolved;
}

void SolveWizard::ClearResolvingFlags()
{
    int height = this->gameManager->map->GetHeight();
    int width = this->gameManager->map->GetWidth();
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            this->gameManager->map->cells[row][col]->resolving = 0;
        }
    }
}

void SolveWizard::GenerateHeads(const DIRECTION dir, std::vector<Cell*> &heads)
{
    switch (dir)
    {
    case DIRECTION::DIR2:
        {
            int i = 0;
            for (; i < this->gameManager->map->GetHeight(); i++)
            {
                heads.push_back(this->gameManager->map->cells[i][0]);
            }
            for (int j = 1; j < this->gameManager->map->GetWidth(); j++)
            {
                if (Map::IsOdd(j))
                {
                    heads.push_back(this->gameManager->map->cells[this->gameManager->map->GetHeight() - 1][j]);
                }
            }
            break;
        }
    case DIRECTION::DIR3:
        {
            int i = 0;
            for (; i < this->gameManager->map->GetHeight(); i++)
            {
                heads.push_back(this->gameManager->map->cells[i][0]);
            }
            for (int j = 1; j < this->gameManager->map->GetWidth(); j++)
            {
                if (!Map::IsOdd(j))
                {
                    heads.push_back(this->gameManager->map->cells[0][j]);
                }
            }
            break;
        }
    case DIRECTION::DIR4:
        {
            for (int i = 0; i < this->gameManager->map->GetWidth(); i++)
            {
                heads.push_back(this->gameManager->map->cells[0][i]);
            }
            break;
        }
    }
}

bool SolveWizard::ExplodeNowBySwap(Cell *cellA, Cell *cellB)
{
    Cell *higher = HigherGem(cellA, cellB);
    Cell *lower = LowerGem(cellA, cellB);
    switch (higher->GetGemType())
    {
    case GemType::Straight5:
        {
            switch (lower->GetGemType())
            {
            case GemType::Normal:
                {
                    this->explosiveHighGems.push(higher);
                    this->gameManager->map->S5TargetColor = lower->GetColor();
                    higher->resolving++;
                }
                break;
            }
        }
        return true;
    case GemType::Straight4:
        {
            DIRECTION exploDir = higher->GetDirection();
            switch (lower->GetGemType())
            {
                //S4S4
            case GemType::Straight4:
                {
                    Cell *source = this->swapCells[0];
                    Cell *target = this->swapCells[1];
                    source->SetColorGemTypeDir(source->GetColor(), GemType::Normal, DIRECTION::DIR1);
                    target->SetColorGemTypeDir(target->GetColor(), GemType::S4S4, DIRECTION::DIR1);
                    this->explosiveHighGems.push(target);
                    source->resolving++;
                    target->resolving++;
                }
                return true;
                //S4C2
            case GemType::Cross2:
                {
                    Cell *source = this->swapCells[0];
                    Cell *target = this->swapCells[1];
                    source->SetColorGemTypeDir(source->GetColor(), GemType::Normal, DIRECTION::DIR1);
                    target->SetColorGemTypeDir(target->GetColor(), GemType::S4C2, exploDir);
                    this->explosiveHighGems.push(target);
                    source->resolving++;
                    target->resolving++;
                }
                return true;
            }
        }
        break;
    }

    return false;
}

Cell* SolveWizard::LowerGem(Cell *cellA, Cell *cellB)
{
    return cellA->GetGemType() < cellB->GetGemType() ? cellA : cellB;
}

Cell* SolveWizard::HigherGem(Cell *cellA, Cell *cellB)
{
    return cellA->GetGemType() >= cellB->GetGemType() ? cellA : cellB;
}