#include "oclint/UniqueResults.h"

#include <unordered_set>

#include "oclint/ResultCollector.h"
#include "oclint/RuleBase.h"
#include "oclint/Violation.h"

namespace
{

class ViolationHash
{
public:
    std::size_t operator()(const oclint::Violation& violation) const
    {
        std::size_t hash1 = std::hash<const oclint::RuleBase*>()(violation.rule);
        std::size_t hash2 = std::hash<std::string>()(violation.path);
        std::size_t hash3 = std::hash<std::string>()(violation.message);
        std::size_t hash4 = violation.startLine
            ^ (violation.startColumn << 2)
            ^ (violation.endLine << 4)
            ^ (violation.endColumn << 8);
        return hash1 ^ (hash2 << 1) ^ (hash2 << 2) ^ (hash3 << 3) ^ (hash4 << 4);
    }
};

} // end namespace


namespace oclint
{

UniqueResults::UniqueResults(const ResultCollector &resultCollector)
    : AbstractResults(resultCollector)
{
}

std::vector<oclint::Violation> UniqueResults::allViolations() const
{
    if (!_violations.empty())
    {
        return _violations;
    }

    std::unordered_set<oclint::Violation, ViolationHash> set;

    for (const auto& violationSet : _resultCollector.getCollection())
    {
        for (const auto& violation : violationSet->getViolations())
        {
            if (set.insert(violation).second)
            {
                //violation was successfully inserted into the set -> new violation
                _violations.push_back(violation);
            }
        }
    }
    return _violations;
}

const std::vector<Violation>& UniqueResults::allErrors() const
{
    if (!_errors.empty())
    {
        return _errors;
    }

    std::unordered_set<oclint::Violation, ViolationHash> set;

    for (const auto& violation : _resultCollector.getCompilerErrorSet()->getViolations())
    {
        if (set.insert(violation).second)
        {
            _errors.push_back(violation);
        }
    }
    return _errors;
}

const std::vector<Violation>& UniqueResults::allWarnings() const
{
    if (!_warnings.empty())
    {
        return _warnings;
    }

    std::unordered_set<oclint::Violation, ViolationHash> set;

    for (const auto& violation : _resultCollector.getCompilerWarningSet()->getViolations())
    {
        if (set.insert(violation).second)
        {
            _warnings.push_back(violation);
        }
    }
    return _warnings;
}

const std::vector<Violation>& UniqueResults::allCheckerBugs() const
{
    if (!_checkerBugs.empty())
    {
        return _checkerBugs;
    }

    std::unordered_set<oclint::Violation, ViolationHash> set;

    for (const auto& violation : _resultCollector.getClangStaticCheckerBugSet()->getViolations())
    {
        if (set.insert(violation).second)
        {
            _checkerBugs.push_back(violation);
        }
    }
    return _checkerBugs;
}

} // end namespace oclint
