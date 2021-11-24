#ifndef SOURCE_UTILS_MATHUTILS_HPP
#define SOURCE_UTILS_MATHUTILS_HPP

#include <includes_pch.h>

namespace MathUtils {
	namespace {
		namespace MU = MathUtils;
	}

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

	template<typename T>
	inline constexpr T consteval_abs(T x) noexcept
	{
		static_assert(std::is_signed<T>::value);

		if constexpr (std::is_floating_point<T>::value)
		{
			if (x == T{}) // negative zero
				return T{};
		}
		return x < T{} ? -x : x;
	}

	inline constexpr float abs(float x) noexcept
	{
		return consteval_abs<float>(x);
	}

	inline constexpr double abs(double x) noexcept
	{
		return consteval_abs<double>(x);
	}

	inline constexpr int abs(int x) noexcept
	{
		return consteval_abs<int>(x);
	}

	inline constexpr long long abs(long long x) noexcept
	{
		return consteval_abs<long long>(x);
	}

	constexpr bool AreSame(float a, float b) noexcept
	{
		return MU::abs(a - b) < std::numeric_limits<float>::epsilon();
	}

	constexpr bool approximatelyEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon()) noexcept
	{
		return MU::abs(a - b) <= ((MU::abs(a) < MU::abs(b) ? MU::abs(b) : MU::abs(a)) * epsilon);
	}

	constexpr bool essentiallyEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon()) noexcept
	{
		return MU::abs(a - b) <= ((MU::abs(a) > MU::abs(b) ? MU::abs(b) : MU::abs(a)) * epsilon);
	}

	constexpr bool definitelyGreaterThan(float a, float b, float epsilon = std::numeric_limits<float>::epsilon()) noexcept
	{
		return (a - b) > ((MU::abs(a) < MU::abs(b) ? MU::abs(b) : MU::abs(a)) * epsilon);
	}

	constexpr bool definitelyLessThan(float a, float b, float epsilon = std::numeric_limits<float>::epsilon()) noexcept
	{
		return (b - a) > ((MU::abs(a) < MU::abs(b) ? MU::abs(b) : MU::abs(a)) * epsilon);
	}
}

#endif // !SOURCE_UTILS_MATHUTILS_HPP