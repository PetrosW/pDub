#ifndef COMMON_HPP
#define COMMON_HPP

#include <QDebug>
#include <QObject>
#include <cstdint>


class Window_Control_t;
class Window_Editor_t;
class Window_Video_t;

namespace PDubReturnValues {
	enum :std::uint8_t {
		NO_MEMORY = 1
	};
}

#endif
