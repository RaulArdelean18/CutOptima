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
            if (!bars[i].canFit(piece.getLength())) {
                continue;
            }

            double remainingAfterCut =
                bars[i].getRemainingLength() - piece.getLength();
            if (remainingAfterCut < bestRemainingAfterCut) {
                bestBarIndex = i;
                bestRemainingAfterCut = remainingAfterCut;
            }
        }

        if (bestBarIndex == -1) {
            unassignedPieces.push_back(piece);
            continue;
        }

        bars[bestBarIndex].consume(piece.getLength());

        auto existingPlan = std::find_if(
            barPlans.begin(), barPlans.end(), [&](const BarCutPlan &plan) {
                return plan.stockBar.getId() == bars[bestBarIndex].getId();
            });

        if (existingPlan == barPlans.end()) {
            BarCutPlan plan;
            plan.stockBar = bars[bestBarIndex];
            plan.pieces.push_back(piece);
            plan.remainingLength = bars[bestBarIndex].getRemainingLength();
            barPlans.push_back(plan);
        } else {
            existingPlan->stockBar = bars[bestBarIndex];
            existingPlan->pieces.push_back(piece);
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
