/////////////////////////////////////////////////////////////////////////////
// Name:        jbstyles.hpp
// Purpose:     Stylesheets and SVG icon color change
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-11-05
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QString>

#include <QString>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QGuiApplication>
#include <QPalette>

inline QIcon createThemedIcon(const QString &resourcePath) {
    QPixmap pixmap(resourcePath);
    if (pixmap.isNull()) return QIcon();
    // ---get text color from system theme---
    QColor iconColor = QGuiApplication::palette().color(QPalette::ButtonText);
    QPainter painter(&pixmap);
    // ---re-color SVG (non-transparent parts)---
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), iconColor);
    painter.end();
    return QIcon(pixmap);
}

inline QString styleToolBar = R"(
        QToolBar > QToolButton {
            border-radius: 3px;
            padding: 2px 2px;
        }
        QToolBar > QToolButton:hover {
            background-color: gray;
            border-color: auto;
        }
        QToolBar > QToolButton:pressed {
            background-color: darkGray;
            border-color: auto;
        }
        QToolBar > QToolButton:checked {
            background-color: darkGray;
            border-color: auto;
        }
        QToolBar {
            border: 0px;
            padding: 1px 1px;
        }
)";

inline QString styleStatusBar = R"(
        QStatusBar {
            border: 0px;
            padding: 1px 1px;
        }
)";

inline QString styleSplitterHandle = R"(
        QSplitter::handle:horizontal {
            width: 1px;
        }
        QSplitter::handle:vertical {
            height: 1px;
        }
)";
