#pragma once

#include <any>
#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "Common.h"
#include "Individual.h"

class Generation {
	public:
		Generation(size_t);

		IndividualPtrList getIndividuals();
		void addIndividual(IndividualPtr);

		std::optional<std::any> getParameter(std::string) const;
		Parameters getParameters() const;
		void setParameter(std::string, std::any);

		size_t getEpoch() const { return epoch; };
		size_t getSize() const;
	private:
		IndividualPtrList individuals;
		Parameters parameters;
		size_t epoch;

};

typedef std::shared_ptr<Generation> GenerationPtr;
