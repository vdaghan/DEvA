#include "DEvA/RandomNumberGenerator.h"

namespace DEvA {
	std::shared_ptr<RandomNumberGenerator> RandomNumberGenerator::singletonInstance{nullptr};

	std::shared_ptr<RandomNumberGenerator> RandomNumberGenerator::get() {
		if (!singletonInstance) {
			singletonInstance = std::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator());
		}
		return singletonInstance;
	}

	double RandomNumberGenerator::getDouble() {
		return doubleDistribution(generator);
	};

	RandomNumberGenerator::RandomNumberGenerator() : generator(randomDevice()), doubleDistribution{0.0, 1.0} {
	};
}
