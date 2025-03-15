#pragma once

#include <functional>
#include <exception>
#include <string>
#include <unordered_map>
#include <utility>

namespace StupidLib {
	class StupidMapException :public std::exception {
		std::string message;
	public:
		StupidMapException(const std::string& msg) : message(msg) {}
		const char* what() const noexcept override {
			return message.c_str();
		}
	};

	template <typename T1, typename T2>
	class StupidMap {
		using MapFunc = std::function<T2(T1)>;
		MapFunc map;
		class StupidMapAccessor {
			T1 key;
			T2 value;
			StupidMap<T1, T2>* parent;
		public:
			StupidMapAccessor() :key(), value(), parent(nullptr) {}
			StupidMapAccessor(StupidMap<T1, T2>* parent, T1 key, T2 value) :key(key), value(value), parent(parent) {}
			operator T2() const {
				return value;
			}
			StupidMapAccessor operator=(T2 value) {
				this->value = value;
				MapFunc oldMap = parent->map;
				parent->map = [this, oldMap](T1 key)->T2 {
					if (key == this->key) return this->value;
					else return oldMap(key);
					};
				return *this;
			}
		};
		std::unordered_map<T1, StupidMapAccessor> accessors{};
	public:
		StupidMap() {
			this->map = [](T1 key)->T2 {throw StupidMapException("Key not found"); };
		}
		const StupidMapAccessor at(T1 key) const {
			T2 value = map(key);
			return StupidMapAccessor(this, key, value);
		}
		const StupidMapAccessor operator[](T1 key) const {
			return at(key);
		}
		StupidMapAccessor& operator[](T1 key) {
			T2 value = map(key);
			accessors[key] = StupidMapAccessor(this, key, value);
			return accessors[key];
		}
		void insert(std::pair<T1, T2> key_value) {
			T1 key_(key_value.first);
			T2 value_(key_value.second);
			MapFunc oldMap = map;
			map = [key_, value_, oldMap](T1 key)->T2 {
				if (key == key_) return value_;
				else return oldMap(key);
				};
		}
		void erase(T1 key_) {
			MapFunc oldMap = map;
			map = [key_, oldMap](T1 key)->T2 {
				if (key == key_) throw StupidMapException("Key not found");
				else return oldMap(key);
				};
		}
	};
}