#pragma once
#include "utils.h"
#include <vector>
#include <utility>

EdgeList gomoryHuTree(const EdgeList& graph, std::vector<std::pair<int, int>> *trace = nullptr);

