#include "CutPlanApplier.h"

CutPlanApplier::CutPlanApplier(InventoryService &inventoryService)
    : inventoryService(inventoryService) {}

void CutPlanApplier::apply(const CutPlan &plan) const {
    for (const auto &barPlan : plan.bars) {
        double usedLength = barPlan.usedLength;

        if (usedLength <= 0.0) {
            double cutLoss =
                barPlan.stockBar.getProfile().getMaterial().getDefaultCutLoss();

            for (const auto &piece : barPlan.pieces) {
                usedLength += piece.getLength() + cutLoss;
            }
        }

        inventoryService.consumeBarLength(barPlan.stockBar.getId(),
                                          usedLength);
    }
}
