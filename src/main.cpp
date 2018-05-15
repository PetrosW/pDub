#include <QtDebug>
#include <common.hpp>
#include <pdub.hpp>

int main(int argc, char *argv[])
{
	try
	{
        //QtAV::Widgets::registerRenderers();
        qDebug() << "start";
		PDub Application(argc, argv);
		return Application.exec();
	}

	catch (std::bad_alloc &Error)
	{
		qDebug("Error: not enough memory\n");
		return PDubReturnValues::NO_MEMORY;
	}
}
