#pragma once

#include <QWidget>
#include <qlistwidget.h>

#include <BlackBox/System/IConsole.hpp>


class BasicConsoleWindow : public QListWidget, public IOutputPrintSink
{
	Q_OBJECT
public:
	BasicConsoleWindow(QWidget* parent)
		: QListWidget(parent)
	{
	} 

	void PostLoad()
	{
		gEnv->pConsole->AddOutputPrintSink(this);
	}

    // Inherited via IOutputPrintSink
    virtual void Print(const char* inszText) override;
	
    QStringList items;

};