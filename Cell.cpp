#include "Cell.h"
#include "Map.h"

USING_NS_CC;

int Cell::GetRow() const
{
    return this->row;
}

int Cell::GetCol() const
{
    return this->col;
}

DIRECTION Cell::GetDirection()
{
    return this->explodeDir;
}

void Cell::SetDirection(const DIRECTION dir)
{
    this->explodeDir = dir;
    switch (dir)
    {
    case DIRECTION::DIR1:
    case DIRECTION::DIR4:
        this->setRotation(0);
        break;
    case DIRECTION::DIR2:
    case DIRECTION::DIR5:
        this->setRotation(120);
        break;
    case DIRECTION::DIR3:
    case DIRECTION::DIR6:
        this->setRotation(60);
        break;
    }
}

void Cell::Explode(){
    if (this->gemType == GemType::Normal)
    {
        return;
    }

    switch (this->gemType)
    {
    case GemType::Straight4:
        {
            this->map->MarkResolvingInDirection(this, this->explodeDir);
            this->map->MarkResolvingInDirection(this, Map::OppositeDirection(this->explodeDir));
            this->exploded = true;
        }
        break;
    case GemType::Straight5:
        {
            if (this->map->S5TargetColor != GemColor::Vacant)
            {
                this->map->MarkResolvingColor(this->map->S5TargetColor);
                this->map->S5TargetColor = GemColor::Vacant;
            }
            else
            {
                this->map->MarkResolvingColor(RandomColor());
            }
            this->exploded = true;
        }
        break;
    case GemType::Cross2:
        {
            this->map->MarkResolvingSurrounding(this);
            this->exploded = true;
        }
        break;
    case GemType::Cross3:
        {
            this->SetColorGemTypeDir(this->color, GemType::Normal, DIRECTION::DIR1);
            this->exploded = true;
        }
        break;
    case GemType::Circle:
        {
            this->SetColorGemTypeDir(this->color, GemType::Normal, DIRECTION::DIR1);
            this->exploded = true;
        }
        break;
    case GemType::S4S4:
        {
            this->map->MarkResolvingInDirection(this, DIRECTION::DIR1);
            this->map->MarkResolvingInDirection(this, DIRECTION::DIR2);
            this->map->MarkResolvingInDirection(this, DIRECTION::DIR3);
            this->map->MarkResolvingInDirection(this, DIRECTION::DIR4);
            this->map->MarkResolvingInDirection(this, DIRECTION::DIR5);
            this->map->MarkResolvingInDirection(this, DIRECTION::DIR6);
            this->exploded = true;
        }
        break;
    case GemType::S4C2:
        {
            this->map->MarkResolvingWideInDirection(this, this->explodeDir);
            this->map->MarkResolvingWideInDirection(this, Map::OppositeDirection(this->explodeDir));
            this->exploded = true;
        }
        break;
    }
}

void Cell::Initialize(Map *map, const int r, const int c, const CellType t, const GemColor color)
{
    this->map = map;
    this->gemType = GemType::Normal;
    this->explodeDir = DIRECTION::DIR1;
    this->resolving = 0;
    this->falling = false;
    this->type = t;
    this->row = r;
    this->col = c;
    this->fallingTime = 0;
    this->color = color;
    this->exploded = false;
    this->initWithTexture(GetCellTexture(this->color, this->gemType));
}

Cell* Cell::createWithTexture(CCTexture2D *texture)
{
    return (Cell*)CCSprite::createWithTexture(texture);
}

void Cell::SetColorGemTypeDir(const GemColor color, const GemType type, const DIRECTION dir)
{
    this->color = color;
    this->gemType = type;
    CCTexture2D *pTexture = GetCellTexture(this->color, this->gemType);
    this->setTexture(pTexture);
    this->SetDirection(dir);
}

GemColor Cell::GetColor()
{
    return this->color;
}

GemType Cell::GetGemType()
{
    return this->gemType;
}

CCTexture2D* Cell::GetCellTexture(const GemColor color, const GemType type)
{
    std::string path = "";
    switch (color)
    {
    case GemColor::S5:
        path = "s5.png";
        break;
    case GemColor::Red:
        switch (type)
        {
        case GemType::Normal:
        case GemType::S4S4:
        case GemType::S4C2:
            path = "r.png";
            break;
        case GemType::Straight4:
            path = "rs4.png";
            break;
        case GemType::Cross2:
            path = "rc2.png";
            break;
        case GemType::Cross3:
            path = "c3.png";
            break;
        }
        break;
    case GemColor::Green:
        switch (type)
        {
        case GemType::Normal:
        case GemType::S4S4:
        case GemType::S4C2:
            path = "g.png";
            break;
        case GemType::Straight4:
            path = "gs4.png";
            break;
        case GemType::Cross2:
            path = "gc2.png";
            break;
        case GemType::Cross3:
            path = "c3.png";
            break;
        }
        break;
    case GemColor::Blue:
        switch (type)
        {
        case GemType::Normal:
        case GemType::S4S4:
        case GemType::S4C2:
            path = "b.png";
            break;
        case GemType::Straight4:
            path = "bs4.png";
            break;
        case GemType::Cross2:
            path = "bc2.png";
            break;
        case GemType::Cross3:
            path = "c3.png";
            break;
        }
        break;
    case GemColor::Vacant:
        path = "v.png";
        break;
    case GemColor::Purple:
        switch (type)
        {
        case GemType::Normal:
        case GemType::S4S4:
        case GemType::S4C2:
            path = "p.png";
            break;
        case GemType::Straight4:
            path = "ps4.png";
            break;
        case GemType::Cross2:
            path = "pc2.png";
            break;
        case GemType::Cross3:
            path = "c3.png";
            break;
        }
        break;
    case GemColor::Orange:
        switch (type)
        {
        case GemType::Normal:
        case GemType::S4S4:
        case GemType::S4C2:
            path = "o.png";
            break;
        case GemType::Straight4:
            path = "os4.png";
            break;
        case GemType::Cross2:
            path = "oc2.png";
            break;
        case GemType::Cross3:
            path = "c3.png";
            break;
        }
        break;
    case GemColor::Yellow:
        switch (type)
        {
        case GemType::Normal:
        case GemType::S4S4:
        case GemType::S4C2:
            path = "y.png";
            break;
        case GemType::Straight4:
            path = "ys4.png";
            break;
        case GemType::Cross2:
            path = "yc2.png";
            break;
        case GemType::Cross3:
            path = "c3.png";
            break;
        }
        break;
    }
    std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(path.c_str());
    return CCTextureCache::sharedTextureCache()->textureForKey(fullPath.c_str());
}

GemColor Cell::RandomColor()
{
    // don't generate Vacant, which is 0. so range is 1-6
    return (GemColor)((rand() % 6) + 1);
}

void Cell::CacheCellTexture()
{
    // TODO: temporary put it here
    CCTextureCache *cache = CCTextureCache::sharedTextureCache();
    cache->addImage("r.png");
    cache->addImage("g.png");
    cache->addImage("b.png");
    cache->addImage("v.png");
    cache->addImage("y.png");
    cache->addImage("o.png");
    cache->addImage("p.png");
    cache->addImage("rs4.png");
    cache->addImage("gs4.png");
    cache->addImage("bs4.png");
    cache->addImage("ys4.png");
    cache->addImage("os4.png");
    cache->addImage("ps4.png");
    cache->addImage("s5.png");
    cache->addImage("rc2.png");
    cache->addImage("gc2.png");
    cache->addImage("bc2.png");
    cache->addImage("yc2.png");
    cache->addImage("oc2.png");
    cache->addImage("pc2.png");
    cache->addImage("c3.png");
}
