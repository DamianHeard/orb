#ifndef ORGANIZER_H
#define ORGANIZER_H

#include <qdesktopwidget.h>
#include <qmainwindow.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qbutton.h>
#include <qiconset.h>
#include <qfiledialog.h>
#include <qworkspace.h>
#include <qtoolbar.h>
#include <vector>
#include "manifold_interface.h"
#include "diagram_window.h"
#include "clip_board.h"
#include "isometry_window.h"
#include "color.h"
#include "kernel.h"
using namespace std;

typedef class IsometryWindow IsometryWindow;

class Organizer: public QWidget
{
        Q_OBJECT

public:
        Organizer( QDesktopWidget *desk );
        ~Organizer();
	QStringList names();
	ManifoldInterface *window( int index );
	QString getPath();
	void setPath( QString s );
	QDesktopWidget *desktop();
	void loadOrbFile( const QString &s );

public slots:
	void    loadCover( Triangulation *m, QString name );
	void	loadDehn( Triangulation *dehn, QString n );
	void	loadFind( Triangulation *m, DiagramCanvas *new_canvas, QString name );
	void	loadPrune( Triangulation *prune, DiagramCanvas *new_canvas, QString name );
	void	loadDrill( Triangulation *drill, DiagramCanvas *new_canvas, QString name );

private slots:
	void		closeEvent( QCloseEvent *e );
	void		drawSlot();
	void		clipSlot();
	void		helpSlot();
	void		isomSlot();
	void		quitSlot();
	void		updateArrowSlot();
	void		importSlot( Triangulation *m,
					DiagramCanvas *d );
	void		importString( QString s );
	void		chooseOrbSlot();
	void		chooseSnapPeaSlot();
	void		loadOrbSlot( const QString &fileName );
	void		loadKnotSlot( const QString &fileName );
	void		loadSnapPeaSlot( const QString &fileName );
	void		closeSlot(ManifoldInterface* const);
	void		toggleDrawSlot();
	void		toggleClipSlot();
	void		toggleIsomSlot();

private:
	void				moveEvent( QMoveEvent * );
	QDesktopWidget			*desk;
	QString				path;
	int				cover_count,
					dehn_count,
					find_count,
					prune_count,
					drill_count,
					untitled_num,
					upper_bound;
	vector <ManifoldInterface *>	items; 
	QHBoxLayout				*box;
	QLabel				*blank;
	QPopupMenu			*arrowMenu;
        QToolButton			*open,
					*import,
					*draw,
					*clip,
					*arrow,
					*isom,
					*help,
					*quit;
	bool				loading;

	DiagramWindow			*diagram;
	ClipBoard			*clipBoard;
	IsometryWindow			*isomWindow;

	Triangulation			*readTriangulation(QTextStream &ts);
	DiagramCanvas			*readDiagram(QTextStream &ts);

signals:
	void				newWindow();
	
};

#endif
