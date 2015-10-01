#ifndef PDUB_HPP
#define PDUB_HPP

#include <memory>
#include <iostream>

#include <QApplication>
#include <QDesktopWidget>
#include <QRect>

#include <windows/control_window.hpp>
#include <windows/video_window.hpp>
#include <windows/editor_window.hpp>

class PDub : public QApplication
{
    Q_OBJECT

    public:
        PDub(int &argc, char *argv[]);

    private:
        Window_Control_t Window_Control;
        Window_Video_t Window_Video;
        Window_Editor_t Window_Editor;
};

#endif
