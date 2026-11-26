/////////////////////////////////////////////////////////////////////////////
// Name:        thememanager.hpp
// Purpose:     Apply custom light/dark theme
// Author:      Jan Buchholz
// Created:     2026-11-20
// Changed:     2026-11-26
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QWidget>
#include <QAction>
#include <QPointer>
#include <QStyleHints>
#include <QPainter>

class ThemeManager {
public:
    enum class Theme {
        Light,
        Dark
    };

    static Theme getTheme() {return m_theme;}
    static void setTheme(Theme th) {m_theme = th;}
    static const QPalette getPalette() {
        return (m_theme == Theme::Dark) ? m_darkPalette : m_lightPalette;
    }

    static QPalette createLightPalette() {
        QPalette p;
        // ---created by Google AI from KDE BreezeLight.colors---
        const QColor windowBg(239, 240, 241);      // [Colors:Window] BackgroundNormal
        const QColor windowText(35, 38, 41);       // [Colors:Window] ForegroundNormal
        const QColor baseBg(255, 255, 255);        // [Colors:View] BackgroundNormal
        const QColor alternateBg(247, 247, 247);   // [Colors:View] BackgroundAlternate
        const QColor text(35, 38, 41);             // [Colors:View] ForegroundNormal
        const QColor buttonBg(252, 252, 252);      // [Colors:Button] BackgroundNormal
        const QColor buttonText(35, 38, 41);       // [Colors:Button] ForegroundNormal
        const QColor highlight(61, 174, 233);      // [Colors:Selection] BackgroundNormal (Breeze Blue)
        const QColor highlightText(255, 255, 255); // [Colors:Selection] ForegroundNormal
        const QColor link(41, 128, 185);           // [Colors:View] ForegroundLink
        const QColor linkVisited(127, 140, 141);   // [Colors:View] ForegroundVisited
        const QColor tooltipBg(247, 247, 247);     // [Colors:Tooltip] BackgroundNormal
        const QColor tooltipText(35, 38, 41);      // [Colors:Tooltip] ForegroundNormal
        const QColor disabledText(160, 165, 170);  // ForegroundInactive
        const QColor midLight(227, 229, 231);
        const QColor mid(189, 195, 199);
        const QColor dark(127, 140, 141);
        // ---active & inactive---
        p.setColor(QPalette::Window, windowBg);
        p.setColor(QPalette::WindowText, windowText);
        p.setColor(QPalette::Base, baseBg);
        p.setColor(QPalette::AlternateBase, alternateBg);
        p.setColor(QPalette::Text, text);
        p.setColor(QPalette::PlaceholderText, disabledText);
        p.setColor(QPalette::Button, buttonBg);
        p.setColor(QPalette::ButtonText, buttonText);
        p.setColor(QPalette::BrightText, Qt::white);
        p.setColor(QPalette::Highlight, highlight);
        p.setColor(QPalette::HighlightedText, highlightText);
        p.setColor(QPalette::Link, link);
        p.setColor(QPalette::LinkVisited, linkVisited);
        p.setColor(QPalette::ToolTipBase, tooltipBg);
        p.setColor(QPalette::ToolTipText, tooltipText);
        // ---3D-Frames---
        p.setColor(QPalette::Light, Qt::white);
        p.setColor(QPalette::Midlight, midLight);
        p.setColor(QPalette::Mid, mid);
        p.setColor(QPalette::Dark, dark);
        p.setColor(QPalette::Shadow, QColor(71, 80, 87));
        // ---disabled---
        p.setColor(QPalette::Disabled, QPalette::WindowText, disabledText);
        p.setColor(QPalette::Disabled, QPalette::Text, disabledText);
        p.setColor(QPalette::Disabled, QPalette::ButtonText, disabledText);
        p.setColor(QPalette::Disabled, QPalette::Button, QColor(239, 240, 241));
        p.setColor(QPalette::Disabled, QPalette::Base, QColor(244, 245, 246));
        p.setColor(QPalette::Disabled, QPalette::Highlight, QColor(210, 214, 218));
        p.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledText);
        return p;
    }

    static QPalette createDarkPalette() {
        QPalette p;
        // ---created by Google AI from KDE BreezeDark.colors---
        const QColor windowBg(49, 54, 59);         // [Colors:Window] BackgroundNormal (#31363b)
        const QColor windowText(239, 240, 241);    // [Colors:Window] ForegroundNormal (#eff0f1)
        const QColor baseBg(35, 38, 41);           // [Colors:View] BackgroundNormal (#232629)
        const QColor alternateBg(42, 46, 50);      // [Colors:View] BackgroundAlternate (#2a2e32)
        const QColor text(239, 240, 241);          // [Colors:View] ForegroundNormal (#eff0f1)
        const QColor buttonBg(49, 54, 59);         // [Colors:Button] BackgroundNormal (#31363b)
        const QColor buttonText(239, 240, 241);    // [Colors:Button] ForegroundNormal (#eff0f1)
        const QColor highlight(61, 174, 233);      // [Colors:Selection] BackgroundNormal (#3daee9)
        const QColor highlightText(239, 240, 241); // [Colors:Selection] ForegroundNormal (#eff0f1)
        const QColor link(41, 128, 185);           // [Colors:View] ForegroundLink
        const QColor linkVisited(127, 140, 141);   // [Colors:View] ForegroundVisited
        const QColor tooltipBg(49, 54, 59);        // [Colors:Tooltip] BackgroundNormal
        const QColor tooltipText(239, 240, 241);   // [Colors:Tooltip] ForegroundNormal
        const QColor disabledText(110, 117, 124);  // ForegroundInactive
        const QColor light(68, 74, 80);
        const QColor midLight(58, 64, 70);
        const QColor mid(40, 44, 48);
        const QColor dark(27, 30, 32);
        // ---active & inactive---
        p.setColor(QPalette::Window, windowBg);
        p.setColor(QPalette::WindowText, windowText);
        p.setColor(QPalette::Base, baseBg);
        p.setColor(QPalette::AlternateBase, alternateBg);
        p.setColor(QPalette::Text, text);
        p.setColor(QPalette::PlaceholderText, disabledText);
        p.setColor(QPalette::Button, buttonBg);
        p.setColor(QPalette::ButtonText, buttonText);
        p.setColor(QPalette::BrightText, Qt::white);
        p.setColor(QPalette::Highlight, highlight);
        p.setColor(QPalette::HighlightedText, highlightText);
        p.setColor(QPalette::Link, link);
        p.setColor(QPalette::LinkVisited, linkVisited);
        p.setColor(QPalette::ToolTipBase, tooltipBg);
        p.setColor(QPalette::ToolTipText, tooltipText);
        // ---3D-Frames---
        p.setColor(QPalette::Light, light);
        p.setColor(QPalette::Midlight, midLight);
        p.setColor(QPalette::Mid, mid);
        p.setColor(QPalette::Dark, dark);
        p.setColor(QPalette::Shadow, QColor(18, 20, 22));
        // ---disabled---
        p.setColor(QPalette::Disabled, QPalette::WindowText, disabledText);
        p.setColor(QPalette::Disabled, QPalette::Text, disabledText);
        p.setColor(QPalette::Disabled, QPalette::ButtonText, disabledText);
        p.setColor(QPalette::Disabled, QPalette::Button, QColor(42, 46, 50));
        p.setColor(QPalette::Disabled, QPalette::Base, QColor(30, 33, 36));
        p.setColor(QPalette::Disabled, QPalette::Highlight, QColor(58, 64, 70));
        p.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledText);
        return p;
    }

    static void applyTheme(Theme theme) {
        m_theme = theme;
        // globally set palette
        const QPalette palette = getPalette();
        QApplication::setPalette(palette);
        // redraw everything
        const auto widgets = QApplication::allWidgets(); // because of: c++11 range-loop might detach Qt container
        for (QWidget* widget : widgets) {
            widget->setPalette(palette);
            widget->style()->unpolish(widget);
            widget->style()->polish(widget);
            widget->update();
        }
        refreshAllThemedIcons();
    }

    static void registerActionIcon(QAction *action, const QString &resourcePath) {
        m_themedIcons.append({action, resourcePath});
        action->setIcon(createThemedIcon(resourcePath));
    }

    static void refreshAllThemedIcons() {
        const auto icons = m_themedIcons; // because of: c++11 range-loop might detach Qt container
        for (const auto &entry : icons) {
            if (entry.action) {
                entry.action->setIcon(createThemedIcon(entry.resourcePath));
            }
        }
    }

    static QIcon createThemedIcon(const QString &resourcePath) {
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

private:
    inline static Theme m_theme = Theme::Light;
    struct ThemedIconEntry {
        QPointer<QAction> action;
        QString resourcePath;
    };
    inline static QList<ThemedIconEntry> m_themedIcons;
    inline static QPalette m_darkPalette = createDarkPalette();
    inline static QPalette m_lightPalette = createLightPalette();
};


