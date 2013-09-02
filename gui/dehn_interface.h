#ifndef DEHN_INTERFACE_H
#define DEHN_INTERFACE_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qtable.h>
#include <qpopupmenu.h>

#include "kernel.h"
#include  "organizer.h"

class Organizer;

class DehnInterface: public QWidget
{
	Q_OBJECT

public:
	DehnInterface(QString n, Triangulation *manifold, Organizer *o, QWidget *parent );
	~DehnInterface();

private:
	QString			name;
	Organizer		*organizer;
	Triangulation		*manifold;
	QCheckBox		*check;
	QGridLayout		*grid;
	QPushButton		*build,
				*close;
	QTable			*coordinates;

private slots:
	void			dehnSlot();
	void			updateValueSlot(int r, int c);
	void			kill();
};

#endif
