#pragma once

#include<random>

namespace Utils
{
  using RANDOM_ENGINE = std::mt19937;

  template<typename T>
  T random_uniform(const T & min, const T & max)
  {
    std::random_device generator;
    RANDOM_ENGINE gen(generator());
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(generator);
  }

  template<>
  inline float random_uniform(const float & min, const float & max)
  {
    std::random_device generator;
    RANDOM_ENGINE gen(generator());
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
  }

  template<>
  inline double random_uniform(const double & min, const double & max)
  {
    std::random_device generator;
    RANDOM_ENGINE gen(generator());
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
  }

  template<typename T>
  T random_normal(const T & mean, const T & deviation)
  {
    std::random_device generator;
    RANDOM_ENGINE gen(generator());
    std::normal_distribution<T> distribution(mean, deviation);
    return distribution(generator);
  }
}