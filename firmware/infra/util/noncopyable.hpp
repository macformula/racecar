/**
 * @author Blake Freer
 * @date 2023-10-14
 * @brief Provides a class which can be inherited to prevent copying objects
 * @note Taken from Christopher Kormanyos
**/

#ifndef UTIL_NONCOPYABLE
#define UTIL_NONCOPYABLE

namespace util {

class noncopyable {
protected:
	noncopyable() = default;
	~noncopyable() = default;

private:
	// Prevent copy constructor and assignment for both references and rvalues
	noncopyable(const noncopyable&) = delete;
	noncopyable(noncopyable&&) = delete;

	auto operator=(const noncopyable&) -> noncopyable& = delete;
	auto operator=(noncopyable&&) -> noncopyable& = delete;
};

}

#endif