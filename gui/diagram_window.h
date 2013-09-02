#ifndef DIAGRAM_WINDOW_H
#define DIAGRAM_WINDOW_H

#include <qlabel.h>
#include <qmainwindow.h>
#include <qvbox.h>
#include <qwidget.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qhbox.h>
#include "diagram_canvas.h"
using namespace std;

class Organizer;

class DiagramWindow : public QMainWindow 
{
    Q_OBJECT

public:
	DiagramWindow(Organizer *o, int u); 
	~DiagramWindow();
	void	loadDiagram( QTextStream &stream );

private:
	int		upper_bound, simp_id;
	void		resizeEvent( QResizeEvent *e );
	void		moveEvent( QMoveEvent *e );
	QLabel		*statusBar,
			*blank,
			*blank1;
	QHBox		*tools;
	QVBox		*box;
	QPopupMenu	*simpMenu;
	QToolButton	*exportButton,
			*drillButton,
			*clearButton;
	QPopupMenu	*clearMenu;
	QHBox		*frame;
	DiagramCanvas	*canvas;
	void		connectCanvas();
	void		disconnectCanvas();

private slots:
	void		hideEvent( QHideEvent *e );
	void		updateStatusSlot(const QString &m);
	void		toggleDrillSlot();
	void		drillSlot();
	void		simpSlot();
	void		exportingSlot( Triangulation *manifold );

signals:
	void		exporting( Triangulation *manifold,DiagramCanvas *canvas);
	void		visibilityChanged();

};

#endif
