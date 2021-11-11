#include <type_traits>

namespace peval {
	template<long long n>
	struct factorial :
		std::integral_constant<long long, n* factorial<n - 1>::value> {};

	template<>
	struct factorial<0> :
		std::integral_constant<long long, 1> {};

	// Equation of possible combinations
	constexpr size_t nChoosek(size_t n, size_t k)
	{
		if (k > n) return 0;
		if (k * 2 > n) k = n - k;
		if (k == 0) return 1;

		int result = n;
		for (int i = 2; i <= k; ++i) {
			result *= (n - i + 1);
			result /= i;
		}
		return result;
	}
}