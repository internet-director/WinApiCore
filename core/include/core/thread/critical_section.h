#pragma once
#include <core/config.h>

namespace core {
	class THREAD_EXPORT critical_section
	{
		CRITICAL_SECTION section;
	public:
		critical_section();
		~critical_section();

		void enter();
		void leave();
		PCRITICAL_SECTION operator*() noexcept;
	};
}
