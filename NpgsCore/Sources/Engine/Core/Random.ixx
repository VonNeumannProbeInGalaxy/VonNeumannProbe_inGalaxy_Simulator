module;

#include "Engine/Core/Base.h"

export module Core.Random;

import <random>;

_NPGS_BEGIN

export template <typename Ty = float, typename RandomEngine = std::mt19937>
class Distribution {
public:
    virtual ~Distribution() = default;
    virtual Ty operator()(RandomEngine& Engine) = 0;
    virtual Ty Generate(RandomEngine& Engine) = 0;
};

export template <typename Ty = int, typename RandomEngine = std::mt19937>
class UniformIntDistribution : public Distribution<Ty> {
public:
    UniformIntDistribution(Ty Min, Ty Max) : _Distribution(Min, Max) {}

    Ty operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    Ty Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::uniform_int_distribution<Ty> _Distribution;
};

export template <typename Ty = float, typename RandomEngine = std::mt19937>
class UniformRealDistribution : public Distribution<Ty, RandomEngine> {
public:
    UniformRealDistribution(Ty Min, Ty Max) : _Distribution(Min, Max) {}

    Ty operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    Ty Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::uniform_real_distribution<Ty> _Distribution;
};

export template <typename Ty = float, typename RandomEngine = std::mt19937>
class NormalDistribution : public Distribution<Ty, RandomEngine> {
public:
    NormalDistribution(Ty Mean, Ty Sigma) : _Distribution(Mean, Sigma) {}

    Ty operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    Ty Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::normal_distribution<Ty> _Distribution;
};

export template <typename Ty = float, typename RandomEngine = std::mt19937>
class LogNormalDistribution : public Distribution<Ty, RandomEngine> {
public:
    LogNormalDistribution(Ty Mean, Ty Sigma) : _Distribution(Mean, Sigma) {}

    Ty operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    Ty Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::lognormal_distribution<Ty> _Distribution;
};

export template <typename RandomEngine = std::mt19937>
class BernoulliDistribution : public Distribution<double, RandomEngine> {
public:
    BernoulliDistribution(double Probability) : _Distribution(Probability) {}

    double operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    double Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::bernoulli_distribution _Distribution;
};

_NPGS_END
