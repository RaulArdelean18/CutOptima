#pragma once

#include "Domain/CutRequest/CutRequestItem.h"
#include <string>
#include <unordered_map>
#include <vector>

class RequestPartitioner {
  public:
    bool canPartitionByMaterial(const std::vector<CutRequestItem> &items) const;

    std::unordered_map<std::string, std::vector<CutRequestItem>>
    partitionByMaterial(const std::vector<CutRequestItem> &items) const;
};
