#pragma once

#include <map>

#include "DEvA/Reference.h"

class Bibliography {
	public:
		Bibliography();
		Reference getById(std::string identifier);
		Reference getByIndex(size_t index);
	private:
		std::vector<Reference> references;
		std::map<size_t, size_t> referenceIndices;
		std::map<std::string, size_t> referenceIdentifiers;

		size_t addReference(Reference);
		void sortReferences();
		void rebuildMaps();
};
