#ifndef COVER_INTERFACE_H
#define COVER_INTERFACE_H

#include <qwidget.h>
#include <qworkspace.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtable.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qpopupmenu.h>

#include "kernel.h"
#include  "organizer.h"

class Organizer;

class CoverInterface: public QWidget
{
	Q_OBJECT

public:
	CoverInterface(QString n, Triangulation *manifold, Organizer *o, QWidget *parent );
	~CoverInterface();

private:
	QString			name;
	Organizer		*organizer;
	Triangulation		*manifold;
	QLabel			*sheets;
	QLineEdit		*input;
	QGridLayout		*grid;
	QPushButton		*build,
				*close;
	QTable			*covers;
	RepresentationList	*repList;

private slots:
	void			updateRepsSlot();
	void			coverSlot();
	void			kill();
};

#endif
