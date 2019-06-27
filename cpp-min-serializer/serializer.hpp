#pragma once
#include <vector>
#include <type_traits>
#include <cstdio>
#include <cstdint>

template <typename T>
class HasSerialize
{
	typedef uint8_t Yes;
	typedef uint16_t No;

	template <typename U> static Yes&	test(decltype(&U::serialize));
	template <typename U> static No&	test(...);

public:
	enum { value = sizeof(test<T>(0)) == sizeof(Yes)  };
};

struct Serializer
{
	std::vector<uint8_t> type;

	typedef std::vector<uint8_t>(*serialize_func_t)();

	template <typename T, typename = typename std::enable_if<
		HasSerialize<T>::value>::type
	>
	Serializer(T& obj)
	{
		printf("Called viable class constructor\n");

		type = obj.serialize();
	}

	template <typename T, typename... U>
	Serializer(T& obj, U&... ob2)
	{
		printf("Called non viable class constructor.\n");

		constexpr size_t obj_size = sizeof(obj);
		uint8_t* obj_ptr = reinterpret_cast<uint8_t*>(&obj);
		type.resize(obj_size, 0);

		for (size_t i = 0; i < obj_size; ++i)
		{
			type.at(i) = *(uint8_t*)(obj_ptr + i);
		}
	}
};
