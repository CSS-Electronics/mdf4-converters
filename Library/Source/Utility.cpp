#include <algorithm>
#include "Utility.h"

namespace mdf {

  bool iequals(const std::string &a, const std::string &b) {
    // Based upon https://stackoverflow.com/a/4119881.

    return std::equal(a.begin(), a.end(),
      b.begin(), b.end(),
      [](char a, char b) {
        return tolower(a) == tolower(b);
      });
  }

  std::vector<std::shared_ptr<CNBlock>> getAllCNBlocks(std::shared_ptr<CGBlock> parent) {
    std::vector<std::shared_ptr<CNBlock>> result;

    std::shared_ptr<CNBlock> firstCNBlock = parent->getFirstCNBlock();

    if (firstCNBlock) {
      result = getAllCNBlocks(firstCNBlock);
    }

    return result;
  }

  std::vector<std::shared_ptr<CNBlock>> getAllCNBlocks(std::shared_ptr<CNBlock> parent) {
    std::vector<std::shared_ptr<CNBlock>> result;

    if (!parent) {
      return result;
    }

    // Add this to the result.
    result.push_back(parent);

    // First walk down composition channels.
    {
      std::shared_ptr<CNBlock> composition = parent->getCompositionBlock();
      std::vector<std::shared_ptr<CNBlock>> compositionResult = getAllCNBlocks(composition);

      // Merge the results.
      result.insert(std::end(result), std::begin(compositionResult), std::end(compositionResult));
    }

    // Next, walk down the channels.
    {
      std::shared_ptr<CNBlock> nextChannel = parent->getNextCNBlock();
      std::vector<std::shared_ptr<CNBlock>> nextChannelResult = getAllCNBlocks(nextChannel);

      // Merge the results.
      result.insert(std::end(result), std::begin(nextChannelResult), std::end(nextChannelResult));
    }

    // Remove any non-unique elements.
    result.erase(std::unique(std::begin(result), std::end(result)), std::end(result));

    return result;
  }

}
