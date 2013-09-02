#ifndef LIST_INTERFACE_H
#define LIST_INTERFACE_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qslider.h>
#include <qlabel.h>
#include <qtable.h>
#include <qhbox.h>
#include <qvbox.h>

#include "kernel.h"
#include  "organizer.h"


class ListInterface: public QWidget
{
	Q_OBJECT

public:
	ListInterface(QString n, Triangulation *manifold, DiagramCanvas *c,
				Organizer *o, QWidget *parent );
	~ListInterface();

private:
	DiagramCanvas		*canvas;
	QString			name;
	Organizer		*organizer;
	Triangulation		*manifold;
	QLabel			*num_r;
	QCheckBox		*checkBox;
	QSlider			*input_rand;
	int			size;
	QGridLayout		*grid;
	QPushButton		*build,
				*tabulate,
				*close;
	QTable			*data;
	Triangulation		**list;

private slots:
	void			warnSlot();
	void			tabulateSlot();
	void			triSlot();
	void			kill();
};

#endif
