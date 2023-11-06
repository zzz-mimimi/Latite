#pragma once
#include <cstdint>
#include <functional>
#include "Version.h"
#include "Util/Meta.h"

namespace SDK {
	namespace detail {
	}

	template <typename ... PossibleTypes>
	class Unknown {
	public:
		template <typename AsType>
		[[nodiscard]] inline AsType* _as() noexcept {
			return reinterpret_cast<AsType*>(this);
		}

		// consistent with MVCLASS_FIELD: 1. latest, 2. 1.18.12, 3: 1.19.51
		[[nodiscard]] inline void _do(std::function<void(Unknown<PossibleTypes...>)> func) noexcept {
		}
	};
}

namespace util {
	template <typename TreatAs, typename Pointer>
	extern TreatAs& directAccess(Pointer ptr, size_t offset) {
		return *reinterpret_cast<TreatAs*>(reinterpret_cast<uintptr_t>(ptr) + offset);
	}
}

namespace SDK {
	template <int offs_1_20_40, int offs_1_20_30, int offs_1_18_12, int offs_1_19_51>
	extern inline const int mvGetOffset() {
		switch (SDK::internalVers) {
		case SDK::V1_20_40:
			return offs_1_20_40;
		case SDK::V1_20_30:
			return offs_1_20_30;
		case SDK::V1_19_51:
			return offs_1_19_51;
			break;
		case SDK::V1_18_12:
			return offs_1_18_12;
		default:
			// Don't want to do anything in release mode so we don't bloat the compiled code too much
#ifdef LATITE_DEBUG
			__debugbreak();
#endif
			return 0;
		}
	}

	// TODO: keep note of this
	template <int offs_1_20_30, int offs_1_18_12, int offs_1_19_51>
	extern inline const int mvGetOffset() {
		switch (SDK::internalVers) {
		case SDK::VLATEST:
		case SDK::V1_20_30:
			return offs_1_20_30;
			break;
		case SDK::V1_19_51:
			return offs_1_19_51;
			break;
		case SDK::V1_18_12:
			return offs_1_18_12;
			break;
		default:
			// Don't want to do anything in release mode so we don't bloat the compiled code too much
#ifdef LATITE_DEBUG
			__debugbreak();
#endif
			return 0;
		}
	}
}

#define CLASS_FIELD(type, name, offset)                                                                      \
    __declspec(property(get = __get_field_##name, put = __set_field_##name)) type name;                             \
    type &__get_field_##name() const { return util::directAccess<type>(this, offset); }                                    \
    template<typename T> void __set_field_##name(const T &value) { util::directAccess<type>(this, offset) = value; }

#define MVCLASS_FIELD(type, name, ...) __declspec(property(get = __get_field_##name, put = __set_field_##name)) type name;                             \
	type& __get_field_##name() const { return util::directAccess<type>(this, SDK::mvGetOffset<__VA_ARGS__>()); }                                    \
	template<typename T> void __set_field_##name(const T& value) { util::directAccess<type>(this, (SDK::mvGetOffset<__VA_ARGS__>())) = value; }