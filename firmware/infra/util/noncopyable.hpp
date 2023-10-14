/**
 * @author Blake Freer
 * @date 2023-10-14
 * @brief Provides a class which can be inherited to prevent copying objects
 * @note Taken from Christopher Kormanyos
**/

#ifndef UTIL_NONCOPYABLE
#define UTIL_NONCOPYABLE

namespace util {

/**
 * @brief Privately inherit `util::noncopyable` to prevent copy constructors and
 * reassignment.
*/
class noncopyable {
protected:
	noncopyable() = default;
	~noncopyable() = default;

private:
	noncopyable(const noncopyable&) = delete;
	noncopyable(noncopyable&&) = delete;

	auto operator=(const noncopyable&) -> noncopyable& = delete;
	auto operator=(noncopyable&&) -> noncopyable& = delete;
};

}

#endif