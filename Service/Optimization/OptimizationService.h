#pragma once

#include "Service/Optimization/OptimizationPreparationService.h"
#include "Service/Optimization/OptimizationResult.h"

class OptimizationService {
  public:
    CutPlan optimize(const OptimizationProblem &problem) const;
};
