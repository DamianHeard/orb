#ifndef ISOMETRY_WINDOW_H
#define ISOMETRY_WINDOW_H

#include "organizer.h"
#include <qlabel.h>
#include <qmainwindow.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qhbuttongroup.h>
#include <qslider.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qprogressbar.h>
#include <qmessagebox.h>
using namespace std;

typedef class Organizer Organizer;
typedef class ManifoldInterface ManifoldInterface;

class IsometryWindow : public QMainWindow
{
    Q_OBJECT

public:
	IsometryWindow(int u,Organizer *organ, vector <ManifoldInterface *> v );
	~IsometryWindow();

private:
	void		moveEvent( QMoveEvent *e );
	int		upper_bound;
	vector <ManifoldInterface *> items;
	Organizer	*o;
	QLabel		*label,
			*blank;
	QGridLayout	*grid;
	QHBox		*lists,
			*top,
			*bottom;	
	QVBox		*box;
	QHButtonGroup	*buttonGroup;
	QCheckBox	*checkBox;
	QPushButton	*button;
	QRadioButton	*rbutton1,
			*rbutton2;
	QListBox	*list1,
			*list2;
	QSlider		*slider;
	void            resizeEvent( QResizeEvent *e );
public slots:
	void		updateListSlot();

private slots:
	void		sameManifoldSlot();
	void		hideEvent( QHideEvent *e );

signals:
	void		closed();    

};

#endif
