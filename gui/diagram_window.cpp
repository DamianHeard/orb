#include "organizer.h"
#include "diagram_window.h"
#include <qlayout.h>
#include <qapplication.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qpixmap.h>

#include "drill.xpm"
#include "clear.xpm"
#include "draw.xpm" 
#include "examine.xpm"    

extern "C" void remove_finite_vertices( Triangulation *manifold );

DiagramWindow::DiagramWindow(Organizer *o, int u)
		: QMainWindow(0,0,Qt::WStyle_Tool)
{
	setCaption("Draw");
	upper_bound = u;
	QPixmap examineIcon, drillIcon, clearIcon;
	examineIcon = QPixmap( examine_xpm );
	drillIcon = QPixmap( drill_xpm );
	clearIcon = QPixmap( clear_xpm );

	box = new QVBox( this );
	setCentralWidget( box );
	tools = new QHBox( box, "tools" );

	blank = new QLabel( "",tools, 0 );
	blank->setPixmap( draw_xpm );

	blank1 = new QLabel("               ", tools, 0 );
	tools->setStretchFactor( blank1, 10 );

	exportButton = new QToolButton(tools,0);
	exportButton->setTextLabel("Examine...");
	exportButton->setIconSet(examineIcon);


        simpMenu = new QPopupMenu( this, "simpMenu");
	simpMenu->setItemEnabled( simpMenu->insertItem("Options:", -1, -1 ), FALSE );
        simp_id = simpMenu->insertItem("Simplify triangulation",this, SLOT(simpSlot()) );
        simpMenu->setCheckable(TRUE);
	simpMenu->setItemChecked( simp_id, TRUE );
        exportButton->setPopup( simpMenu );
        exportButton->setPopupDelay( 0 );


	drillButton = new QToolButton(tools,0);
	drillButton->setIconSet(drillIcon);
	drillButton->setTextLabel("Drill");
	drillButton->setToggleButton(TRUE);

	clearButton = new QToolButton(tools,0);
	clearButton->setIconSet(clearIcon);
	clearButton->setTextLabel("Clear");

	clearMenu = new QPopupMenu( this, 0 );
	clearButton->setPopup( clearMenu );
	clearButton->setPopupDelay( 1 );

	frame = new QHBox( box, 0 );
	frame->setLineWidth( 1 );
	frame->setMinimumHeight ( 170 );
	frame->setFrameStyle( QFrame::LineEditPanel | QFrame::Sunken ); 
	canvas = new DiagramCanvas( frame, "canvas", FALSE );
	frame->setFocus();
	box->setStretchFactor( frame, 10 );

	statusBar = new QLabel( box, 0 );

	connectCanvas();

	setMinimumWidth( 225 );
	setMinimumHeight( 245 );
        setMaximumWidth( o->desktop()->width()-50 );
        setMaximumHeight( o->desktop()->height()-upper_bound-50);
	
	setGeometry(0,90,250,245);
}

DiagramWindow::~DiagramWindow()
{
	simpMenu->~QPopupMenu();
	exportButton->~QToolButton();
	drillButton->~QToolButton();
	clearMenu->~QPopupMenu();
	clearButton->~QToolButton();
	blank->~QLabel();
	blank1->~QLabel();
	tools->~QHBox();
	canvas->~DiagramCanvas();
	frame->~QHBox();
	statusBar->~QLabel();
	box->~QVBox();
}

void DiagramWindow::exportingSlot( Triangulation *manifold )
{
	disconnectCanvas();
	canvas->setReadOnly();
	canvas->deselectEdges();

	if (simpMenu->isItemChecked( simp_id ))
		remove_finite_vertices( manifold );

	emit exporting( manifold,canvas);

	drillButton->setOn( FALSE );
	drillSlot();

	canvas = new DiagramCanvas(frame, "canvas", FALSE);

	canvas->show();
	connectCanvas();
}

void DiagramWindow::disconnectCanvas()
{
	exportButton->disconnect();
	drillButton->disconnect();
	clearMenu->clear();
	canvas->disconnect();
}

void DiagramWindow::connectCanvas()
{
	connect( exportButton, SIGNAL( clicked() ),
		canvas, SLOT( exportSlot() ) );

	connect( drillButton, SIGNAL( clicked() ),
		canvas, SLOT( drillToggle() ) );

        connect( drillButton, SIGNAL( clicked()),
                        this, SLOT( drillSlot()));

	clearMenu->insertItem( "clear selected",  canvas,
			SLOT(deleteSelectedEdges()), CTRL+Key_E );

	clearMenu->insertItem( "clear all",  canvas,
			SLOT(clearDiagram()), CTRL+Key_A );

	connect( canvas, SIGNAL( drillToggled() ),
		this, SLOT( toggleDrillSlot() ) );

	connect( canvas, SIGNAL( statusBarChanged(const QString &) ),
		this, SLOT( updateStatusSlot(const QString &) ) );

	connect( canvas, SIGNAL( exporting( Triangulation *)),
		this, SLOT( exportingSlot(Triangulation *) ) );
}

void DiagramWindow::hideEvent( QHideEvent *e )
{
        emit visibilityChanged();
}

void DiagramWindow::drillSlot()
{
        if (drillButton->isOn())
		drillButton->setPaletteBackgroundColor( Qt::black );
        else	drillButton->setPaletteBackgroundColor( Qt::white );
}

void DiagramWindow::updateStatusSlot(const QString &m)
{
	statusBar->setText( m );
}

void DiagramWindow::toggleDrillSlot()
{
	drillButton->toggle();
	drillSlot();
	canvas->drillToggle();
}


void DiagramWindow::loadDiagram( QTextStream &stream )
{
	canvas->clearDiagram();	
	canvas->readDiagram( stream );
	if (isHidden())
	{
		show();
		emit visibilityChanged();
	}
}	

void DiagramWindow::resizeEvent( QResizeEvent *e )
{
//	if (y() < upper_bound )
//	{
//		setGeometry(x(),upper_bound,width(),height());
//		repaint();
//	}
}

void DiagramWindow::moveEvent( QMoveEvent *e )
{
//	if (x() < 25 )
//		move( 0, y() );
}	

void DiagramWindow::simpSlot()
{

        if (simpMenu->isItemChecked( simp_id ) )
                simpMenu->setItemChecked( simp_id, FALSE );
        else    simpMenu->setItemChecked( simp_id, TRUE );

}

