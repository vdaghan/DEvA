#pragma once

#include <concepts>
#include <utility>
#include <ranges>
#include <type_traits>

namespace DEvA {
	template <typename T> concept CSpecificationTypes =
	requires {
		typename T::Genotype;
		typename T::GenotypeProxy;
		typename T::Phenotype;
		typename T::PhenotypeProxy;
		typename T::Distance;
		typename T::IndividualParameters;
	};
	template <typename T> concept CHasDistance = requires { typename T::Distance; };
	template <typename T> concept CNoDistance = not CHasDistance<T>;
	template <typename T> concept CSpecification = 
	CSpecificationTypes<T> and
	requires () {
		true;
		//{ T::genotypeFromProxy(T::GenotypeProxy()) };
		//{T::phenotypeFromProxy(PP)} -> std::same_as<T::Phenotype>;
		//requires std::same_as<T::Genotype, T::genotypeFromProxy(GP)>;
	};

	template <typename F>
	concept CCallableVoid = requires (F f) { f(); };

	template <typename F, typename Arg>
	concept CCallableWith = std::invocable < F, Arg >;
	
	template <typename F, typename... Args>
	concept CCallable = CCallableWith<F, Args...> || CCallableVoid<F>;

	//TODO: Assumes the single argument of G can be {}-constructible. It shouldn't.
	template <typename F, typename G>
	concept CComposable = requires(F f, G g) {
		requires std::invocable<F, decltype(g({}))>;
	};
	
	//TODO: Assumes the single argument of F can be {}-constructible. It shouldn't.
	template <typename F, typename T>
	concept CReturns = requires(F f, T t) {
		{f({})} -> std::same_as<T>;
	};

	template <typename G, typename T>
	concept CGenotype = std::convertible_to<G, T>;

	template <typename T>
	concept CContainer = std::ranges::range<T>;

	template <typename T> using TypeOfElements = std::ranges::range_value_t<T>;
	template <typename T, typename... Args>
	concept CContainerOfCallables =
		CContainer<T>
		&& CCallable<TypeOfElements<T>, Args...>;

	template <typename G, typename T>
	concept CGenotypeContainer = 
		not CGenotype<G, T>
		&& CContainer<T>
		&& CGenotype<G, decltype(*T().begin())>;

	auto toFunction(CContainer auto containerOfCallables) {
		return [&, containerOfCallables](CContainer auto container) {
			decltype(container) retVal;
			auto itCallable = containerOfCallables.begin();
			auto itContainer = container.begin();
			while (itCallable != containerOfCallables.end() and itContainer != container.end()) {
				retVal.emplace_back((*itCallable)(*itContainer));
				++itCallable;
				++itContainer;
			}
			return retVal;
		};
	};
}
