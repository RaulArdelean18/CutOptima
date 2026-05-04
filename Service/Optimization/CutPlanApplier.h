#pragma once

#include "Service/Inventory/InventoryService.h"
#include "Service/Optimization/OptimizationResult.h"

class CutPlanApplier {
  private:
    InventoryService &inventoryService;

  public:
    explicit CutPlanApplier(InventoryService &inventoryService);

    void apply(const CutPlan &plan) const;
};
