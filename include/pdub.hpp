#ifndef PDUB_HPP
#define PDUB_HPP

#include <memory>
#include <iostream>

#include <QApplication>
#include <QDesktopWidget>
#include <QRect>

#include <common.hpp>
#include <windows/main_window.hpp>

class PDub : public QApplication
{
    Q_OBJECT

    public:
        PDub(int &argc, char *argv[]);

    private:
        Window_Main_t Window_Main;
};

#endif
