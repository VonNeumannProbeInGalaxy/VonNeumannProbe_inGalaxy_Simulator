#pragma once

#include <random>
#include "Engine/Core/Base.h"

_NPGS_BEGIN

template <typename Ty = double>
class NPGS_API Distribution {
public:
    virtual ~Distribution() = default;
    virtual Ty Generate(std::mt19937& Engine) = 0;
};

template <typename Ty = double>
class NPGS_API UniformRealDistribution : public Distribution<Ty> {
public:
    UniformRealDistribution(Ty Min, Ty Max) : _Distribution(Min, Max) {}

    Ty Generate(std::mt19937& Engine) override {
        return _Distribution(Engine);
    }

private:
    std::uniform_real_distribution<Ty> _Distribution;
};

template <typename Ty = double>
class NPGS_API NormalDistribution : public Distribution<Ty> {
public:
    NormalDistribution(Ty Mean, Ty Sigma) : _Distribution(Mean, Sigma) {}

    Ty Generate(std::mt19937& Engine) override {
        return _Distribution(Engine);
    }

private:
    std::normal_distribution<Ty> _Distribution;
};

template <typename Ty = double>
class NPGS_API LogNormalDistribution : public Distribution<Ty> {
public:
    LogNormalDistribution(Ty Mean, Ty Sigma) : _Distribution(Mean, Sigma) {}

    Ty Generate(std::mt19937& Engine) override {
        return _Distribution(Engine);
    }

private:
    std::lognormal_distribution<Ty> _Distribution;
};

class NPGS_API BernoulliDistribution : public Distribution<double> {
public:
    BernoulliDistribution(double Probability) : _Distribution(Probability) {}

    double Generate(std::mt19937& Engine) override {
        return _Distribution(Engine);
    }

private:
    std::bernoulli_distribution _Distribution;
};

_NPGS_END
