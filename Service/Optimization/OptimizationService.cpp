#include "OptimizationService.h"
#include <algorithm>
#include <limits>

CutPlan
OptimizationService::optimize(const OptimizationProblem &problem) const {
    std::vector<CutPiece> pieces = problem.pieces;
    std::sort(pieces.begin(), pieces.end(),
              [](const CutPiece &left, const CutPiece &right) {
                  return left.getLength() > right.getLength();
              });

    std::vector<StockBar> bars = problem.stockBars;
    std::vector<BarCutPlan> barPlans;
    std::vector<CutPiece> unassignedPieces;

    for (const auto &piece : pieces) {
        int bestBarIndex = -1;
        double bestRemainingAfterCut = std::numeric_limits<double>::max();

        for (int i = 0; i < static_cast<int>(bars.size()); i++) {
            double cutLoss =
                bars[i].getProfile().getMaterial().getDefaultCutLoss();
            double requiredLength = piece.getLength() + cutLoss;

            if (!bars[i].canFit(requiredLength)) {
                continue;
            }

            double remainingAfterCut =
                bars[i].getRemainingLength() - requiredLength;
            if (remainingAfterCut < bestRemainingAfterCut) {
                bestBarIndex = i;
                bestRemainingAfterCut = remainingAfterCut;
            }
        }

        if (bestBarIndex == -1) {
            unassignedPieces.push_back(piece);
            continue;
        }

        double cutLoss =
            bars[bestBarIndex].getProfile().getMaterial().getDefaultCutLoss();
        double requiredLength = piece.getLength() + cutLoss;

        bars[bestBarIndex].consume(requiredLength);

        auto existingPlan = std::find_if(
            barPlans.begin(), barPlans.end(), [&](const BarCutPlan &plan) {
                return plan.stockBar.getId() == bars[bestBarIndex].getId();
            });

        if (existingPlan == barPlans.end()) {
            BarCutPlan plan;
            plan.stockBar = bars[bestBarIndex];
            plan.pieces.push_back(piece);
            plan.usedLength = requiredLength;
            plan.remainingLength = bars[bestBarIndex].getRemainingLength();
            barPlans.push_back(plan);
        } else {
            existingPlan->stockBar = bars[bestBarIndex];
            existingPlan->pieces.push_back(piece);
            existingPlan->usedLength += requiredLength;
            existingPlan->remainingLength =
                bars[bestBarIndex].getRemainingLength();
        }
    }

    double totalWaste = 0.0;
    for (const auto &plan : barPlans) {
        totalWaste += plan.remainingLength;
    }

    CutPlan result;
    result.bars = barPlans;
    result.unassignedPieces = unassignedPieces;
    result.totalWaste = totalWaste;
    return result;
}
