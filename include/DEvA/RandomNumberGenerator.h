#pragma once

#include <algorithm>
#include <memory>
#include <random>
#include <ranges>

namespace DEvA {
	class RandomNumberGenerator {
		public:
			static std::shared_ptr<RandomNumberGenerator> get();

			template <typename T> T getIntBetween(T a, T b) {
				std::uniform_int_distribution<T> dist(a, b);
				return dist(generator);
			};
			template <typename T> T getRealBetween(T a, T b) {
				std::uniform_real_distribution<T> dist(a, b);
				return dist(generator);
			};
			template <typename T> void shuffle(T & container) {
				std::ranges::shuffle(container, std::mt19937_64{randomDevice()});
			};
			double getDouble();
			unsigned int rand() { return randomDevice(); };
		private:
			static std::shared_ptr<RandomNumberGenerator> singletonInstance;
			RandomNumberGenerator();

			std::random_device randomDevice;
			std::mt19937_64 generator;
			std::uniform_real_distribution<double> doubleDistribution;
	};
}
