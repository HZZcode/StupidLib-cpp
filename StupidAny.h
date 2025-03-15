#pragma once

#include <string>
#include <exception>
#include <functional>

namespace StupidLib {
	class StupidBadAnyException :public std::exception {
	public:
		StupidBadAnyException() {}
		const char* what() const noexcept override {
			return "Bad Any Exception";
		}
	};
	class StupidNullAnyException :public std::exception {
	public:
		StupidNullAnyException() {}
		const char* what() const noexcept override {
			return "Null Any Exception";
		}
	};

	class StupidAny {
		struct NullAny {};
		using Func = std::function<void(void)>;
		Func data;

		template <typename T>
		struct Wrapper {
			T t;
		};

	public:
		inline static NullAny null_any;
		StupidAny() {
			data = [] {};
		}
		template <typename T>
		StupidAny(T value) {
			data = [value] {
				throw Wrapper{ value };
				};
		}
		template <typename T>
		StupidAny& operator=(T value) {
			data = [value] {
				throw Wrapper{ value };
				};
			return *this;
		}
		template <>
		StupidAny& operator=(NullAny) {
			data = [] {};
			return *this;
		}
		bool operator==(NullAny) {
			try {
				data();
			}
			catch (...) {
				return false;
			}
			return true;
		}
		template <typename T>
		T get() {
			try {
				data();
			}
			catch (Wrapper<T> value) {
				return value.t;
			}
			catch (...) {
				throw StupidBadAnyException();
			}
			throw StupidNullAnyException();
		}
	};
}