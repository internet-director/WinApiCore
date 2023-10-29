#pragma once
#include <core/config.h>
#include <core/wobf/wobf.h>
#include <core/thread/critical_section.h>

namespace core {
	class THREAD_EXPORT condition_variable
	{
		CONDITION_VARIABLE cv;
	public:
		condition_variable();
		~condition_variable() = default;
		
		void wait(critical_section& section);
		
		template<class Predicate>
		void wait(critical_section& section, Predicate stop_waiting) {
			while (!stop_waiting()) {
				wait(section);
			}
		}

		void notify_one();
		void notify_all();
	};
}
