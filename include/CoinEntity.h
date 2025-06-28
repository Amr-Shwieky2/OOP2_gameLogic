#pragma once
#include "CollectibleEntity.h"
#include "Transform.h"

class CoinEntity : public CollectibleEntity {
public:
    explicit CoinEntity(IdType id);

    void onCollect(Entity* collector) override;

    // You may want to override update/render if needed
};