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
        std::unique_ptr<Window_Control_t> Window_Control;
        std::unique_ptr<Window_Video_t> Window_Video;
        std::unique_ptr<Window_Editor_t> Window_Editor;
};

#endif
