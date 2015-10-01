#include <pdub.hpp>

PDub::PDub(int &argc, char *argv[])
    : QApplication(argc, argv),
      Window_Control(),
      Window_Video(&Window_Control, &Window_Control),
      Window_Editor(&Window_Control, &Window_Control)
{
    Window_Control.setWindowEditorPtr(&Window_Editor);
    Window_Control.setWindowVideoPtr(&Window_Video);

    Window_Editor.setWindowVideoPtr(&Window_Video);

    Window_Video.setWindowEditorPtr(&Window_Editor);

    QDesktopWidget Monitor;
    QSize Resolution = Monitor.availableGeometry(Monitor.primaryScreen() ).size();

    /*uint16_t Width = Resolution.width() >> 1;
    uint16_t Height = Resolution.height() * 0.6;

    Window_Control->setGeometry(0, 0, Width, Height);
    Window_Control->move(0, 0);
    //Window_Control->resize(Resolution.width() >> 1, Resolution.height() * 0.6);

    Window_Video->move(0, 0);
    Window_Video->setGeometry(Width, 0, Width, Height);

    std::cout << Window_Control->width() << std::endl << Window_Control->height() << std::endl;*/
}
