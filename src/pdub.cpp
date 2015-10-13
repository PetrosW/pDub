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

//    QDesktopWidget Monitor;
//    QSize Resolution = Monitor.availableGeometry(Monitor.primaryScreen() ).size();

    QScreen *Screen = QGuiApplication::primaryScreen();
    QSize AvSize = Screen->availableSize();
    qDebug() << AvSize.width();
    qDebug() << AvSize.height();


    Window_Control.move(AvSize.width()/2, 0);
    Window_Video.move(0, 0);
    Window_Editor.move(0, AvSize.height()/2);

    Window_Control.resize(200,200);

    //QRect *Window_Control_Geometry = &Window_Control.frameGeometry();

    qDebug() << Window_Control.frameGeometry().height();
    qDebug() << Window_Control.frameGeometry().width();

    int Frame_Height = Window_Control.frameGeometry().height() - Window_Control.height();
    int Frame_Width = Window_Control.frameGeometry().width() - Window_Control.width();

    qDebug() << Frame_Height;
    qDebug() << Frame_Width;

    //Window_Control.resize(AvSize.width()/2, AvSize.height()/2);
    //Window_Video.resize(AvSize.width()/2, AvSize.height()/2);
    //Window_Editor.resize(AvSize.width(), AvSize.height()/2);

    Window_Control.resize(AvSize.width()/2 - Frame_Width, AvSize.height()/2 - Frame_Height);
    Window_Video.resize(AvSize.width()/2 - Frame_Width, AvSize.height()/2 - Frame_Height);
    Window_Editor.resize(AvSize.width() - Frame_Width, AvSize.height()/2 - Frame_Height);

    Window_Control.setDeafaultMicrophone();

}
