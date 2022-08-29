#include <algorithm>
#include <array>
#include <list>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"

using Matrix = std::vector<std::vector<bool>>;
using Matrices = std::array<Matrix, 3>;

int main() {
	std::vector<std::pair<size_t, size_t>> matrixSizes;
	std::random_device randDev;
	std::default_random_engine randGen(randDev());
	std::uniform_int_distribution<size_t> distributionDim(5, 20);
	size_t minRows = std::numeric_limits<size_t>::max();
	size_t maxRows = std::numeric_limits<size_t>::min();
	size_t minCols = std::numeric_limits<size_t>::max();
	size_t maxCols = std::numeric_limits<size_t>::min();
	for (size_t i(0); i != 3; ++i) {
		size_t numRows = distributionDim(randGen);
		size_t numCols = distributionDim(randGen);
		matrixSizes.emplace_back(std::make_pair(numRows, numCols));
		minRows = std::min({minRows, numRows});
		maxRows = std::max({maxRows, numRows});
		minCols = std::min({minCols, numCols});
		maxCols = std::max({maxCols, numCols});
	}
	std::uniform_int_distribution<size_t> distributionRow(1, minRows);
	std::uniform_int_distribution<size_t> distributionCol(1, minCols);
	size_t r = distributionRow(randGen);
	size_t c = distributionCol(randGen);

	std::cout << "r = " << r << ", c = " << c << std::endl;
	for (size_t i(0); i != 3; ++i) {
		std::cout << "M" << i << " dimensions: " << matrixSizes[i].first << "x" << matrixSizes[i].second << std::endl;
	}

	using Genotype = Matrices;
	using Phenotype = Matrices;
	using Fitness = size_t;
	using Spec = DEvA::Specialisation<Genotype, Phenotype, Fitness>;
	DEvA::EvolutionaryAlgorithm<Spec> ea;

	Spec::FGenesis fgenesis = [&](){
		Spec::Generation generation;
		std::uniform_int_distribution<size_t> distributionVal(0, 1023);
		for (size_t n(0); n != 100; ++n) {
			Spec::GenotypePtr M = std::make_shared<Spec::Genotype>();
			for (size_t m(0); m != 3; ++m) {
				auto & matrix = M->at(m);
				matrix.resize(matrixSizes[m].first);
				for (size_t i(0); i != matrixSizes[m].first; ++i) {
					matrix[i].resize(matrixSizes[m].second);
					for (size_t j(0); j != matrixSizes[m].second; ++j) {
						double val = distributionVal(randGen);
						matrix[i][j] = (val >= 512);
					}
				}
			}
			generation.emplace_back(std::make_shared<Spec::SIndividual>(M));
		}
		return generation;
	};

	Spec::FEvaluate fevaluate = [&](Spec::PhenotypePtr pptr) -> Spec::Fitness {
		size_t fitness(0);

		for (size_t m(0); m != 3; ++m) {
			auto & matrix = pptr->at(m);
			size_t matrixDimRows = matrix.size();
			size_t matrixDimCols = matrix[0].size();
			std::vector<size_t> numOnesInRow(matrixDimRows);
			std::vector<size_t> numOnesInCol(matrixDimCols);

			for (size_t i(0); i != matrix.size(); ++i) {
				for (size_t j(0); j != matrix[i].size(); ++j) {
					numOnesInCol[j] += size_t(matrix[i][j]);
					numOnesInRow[i] += size_t(matrix[i][j]);
				}
			}

			for (size_t i(0); i != matrix.size(); ++i) {
				for (size_t j(0); j != matrix[i].size(); ++j) {
					size_t maxCol(std::max({numOnesInCol[j], c}));
					size_t minCol(std::min({numOnesInCol[j], c}));
					fitness += maxCol-minCol;
				}
				size_t maxRow(std::max({numOnesInRow[i], r}));
				size_t minRow(std::min({numOnesInRow[i], r}));
				fitness += maxRow - minRow;
			}
		}

		return fitness;
	};

	Spec::FVariation customCutAndCrosfill = [matrixSizes](Spec::GenotypePtrs gptrs) -> Spec::GenotypePtrs {
		Spec::GenotypePtr firstParent = std::make_shared<Genotype>(*(gptrs.front()));
		Spec::GenotypePtr secondParent = std::make_shared<Genotype>(*(gptrs.back()));
		Spec::GenotypePtr firstOffspring = std::make_shared<Genotype>();
		Spec::GenotypePtr secondOffspring = std::make_shared<Genotype>();

		// Resize offsprings. We need a library function for this
		for (size_t m(0); m != 3; ++m) {
			std::pair<size_t, size_t> matrixSize = matrixSizes[m];
			firstOffspring->at(m).resize(matrixSize.first);
			secondOffspring->at(m).resize(matrixSize.first);
			for (size_t i(0); i != matrixSize.first; ++i) {
				firstOffspring->at(m).at(i).resize(matrixSize.second);
				secondOffspring->at(m).at(i).resize(matrixSize.second);
			}
		}

		std::random_device randDev;
		std::default_random_engine randGen(randDev());
		std::uniform_int_distribution<size_t> distributionMatrix(0, 3);
		for (size_t m(0); m != 3; ++m) {
			std::pair<size_t, size_t> matrixSize = matrixSizes[m];
			std::uniform_int_distribution<size_t> distributionRowOrCol(0, 1);
			size_t axisIndex(distributionRowOrCol(randGen));
			size_t distributionLimit(axisIndex == 0 ? matrixSize.first : matrixSize.second);
			std::uniform_int_distribution<size_t> distribution(0, distributionLimit);
			size_t crossoverIndex = distribution(randGen);

			if (0 == axisIndex) {
				auto itCrossover1 = DEvA::Common::iteratorForNthListElement<Spec>(firstParent->at(m), crossoverIndex);
				auto itCrossover2 = DEvA::Common::iteratorForNthListElement<Spec>(secondParent->at(m), crossoverIndex);
				auto it1 = firstParent->at(m).begin();
				auto it2 = secondParent->at(m).begin();
				size_t ind1(0);
				size_t ind2(0);

				while (ind1 <= crossoverIndex) {
					firstOffspring->at(m).push_back(*it1);
					secondOffspring->at(m).push_back(*it2);
					++it1;
					++it2;
					++ind1;
					++ind2;
				}

				for (auto it = secondParent->at(m).begin(); it != secondParent->at(m).end(); ++it) {
					if (firstOffspring->at(m).end() == std::find(firstOffspring->at(m).begin(), firstOffspring->at(m).end(), *it)) {
						firstOffspring->at(m).push_back(*it);
					}
				}

				for (auto it = firstParent->at(m).begin(); it != firstParent->at(m).end(); ++it) {
					if (secondOffspring->at(m).end() == std::find(secondOffspring->at(m).begin(), secondOffspring->at(m).end(), *it)) {
						secondOffspring->at(m).push_back(*it);
					}
				}
			} else {
				auto itCrossover1 = DEvA::Common::iteratorForNthListElement<Spec>(firstParent->at(m), crossoverIndex);
				auto itCrossover2 = DEvA::Common::iteratorForNthListElement<Spec>(secondParent->at(m), crossoverIndex);
				for (size_t i(0); i != matrixSize.first; ++i) {
					auto it1 = firstParent->at(m).at(i).begin();
					auto it2 = secondParent->at(m).at(i).begin();
					size_t ind1(0);
					size_t ind2(0);

					while (ind1 <= crossoverIndex) {
						firstOffspring->at(m).at(i).push_back(*it1);
						secondOffspring->at(m).at(i).push_back(*it2);
						++it1;
						++it2;
						++ind1;
						++ind2;
					}

					for (auto it = secondParent->at(m).at(i).begin(); it != secondParent->at(m).at(i).end(); ++it) {
						if (firstOffspring->at(m).at(i).end() == std::find(firstOffspring->at(m).at(i).begin(), firstOffspring->at(m).at(i).end(), *it)) {
							firstOffspring->at(m).at(i).push_back(*it);
						}
					}

					for (auto it = firstParent->at(m).at(i).begin(); it != firstParent->at(m).at(i).end(); ++it) {
						if (secondOffspring->at(m).at(i).end() == std::find(secondOffspring->at(m).at(i).begin(), secondOffspring->at(m).at(i).end(), *it)) {
							secondOffspring->at(m).at(i).push_back(*it);
						}
					}
				}
			}
			
		}

		Spec::GenotypePtrs retList = {firstOffspring, secondOffspring};
		return retList;
	};

	Spec::FVariation fvariation = [&](Spec::GenotypePtrs gptrs) -> Spec::GenotypePtrs {
		Spec::GenotypePtrs offsprings = customCutAndCrosfill(gptrs);
		for (auto & gptr : offsprings) {
			std::default_random_engine randGen;
			std::uniform_int_distribution<int> distribution(0, 100);
			size_t swapProbability = distribution(randGen);
			if (swapProbability <= 80) {
				gptr = DEvA::StandardVariations<Spec>::swap(gptr);
			}
		}
		return offsprings;
	};

	ea.setGenesisFunction(fgenesis);
	ea.setTransformFunction(DEvA::StandardTransforms<Spec>::copy);
	ea.setEvaluationFunction(fevaluate);
	ea.setParentSelectionFunction(DEvA::StandardParentSelectors<Spec>::bestNofM<2, 5>);
	ea.setVariationFunction(fvariation);
	ea.setSurvivorSelectionFunction(DEvA::StandardSurvivorSelectors<Spec>::clamp<100>);
	ea.setConvergenceCheckFunction(DEvA::StandardConvergenceCheckers<Spec>::equalTo<0>);

	auto result = ea.search(10000);
	std::cout << "Best genotype: [";
	for (auto it = ea.bestGenotype->begin(); it != ea.bestGenotype->end(); ++it) {
		//std::cout << *it << " ";
	}
	std::cout << "]\nFitness: " << ea.bestFitness << "\n";
	if (DEvA::StepResult::Convergence == result) {
		std::cout << "Converged.\n";
	} else {
		std::cout << "Step limit reached.\n";
	}

	return 0;
}
