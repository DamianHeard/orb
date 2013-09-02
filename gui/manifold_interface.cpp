#include "manifold_interface.h"
#include "cover_interface.h"
#include "dehn_interface.h"
#include "list_interface.h"
#include "color.h"
#include "console.h"
#include "diagram_canvas.h"
#include <qapplication.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qfont.h>
#include <iostream>
#include <qpixmap.h>

#include "orb32.xpm"
#include "view.xpm"
#include "disk.xpm"
#include "action.xpm"
#include "draw.xpm"

extern "C" void		WriteNewFileFormat(FILE *fp, TriangulationData *data);

using namespace std;


class TextItem : public QTableItem
{
public:
	TextItem( QTable *t) : QTableItem(t,OnTyping) {}
	void paint(QPainter * p, const QColorGroup & cg, const QRect & cr, bool 
selected);
	 void setColor(QColor t, QColor b ) {colorT=t;colorB=b;}


protected:
	QColor colorT;
	QColor colorB;
};


extern "C" SolutionType find_structure( Triangulation *, Boolean );
extern "C" double	my_volume(Triangulation *manifold, Boolean *ok );
extern "C" double	tetrahedron_volume( double angles[6], Boolean *ok );
extern "C" void		free_triangulation(Triangulation *manifold );
extern "C" Boolean	contains_flat_tetrahedra( Triangulation *manifold );
extern "C" void		remove_bad_vertices( Triangulation *manifold );

void TextItem::paint(QPainter * p, const QColorGroup & cg, const QRect & cr,
bool selected) {
        QColorGroup cg1(cg);
        cg1.setColor(QColorGroup::Text,colorT);
        cg1.setColor(QColorGroup::Base,colorB);
        cg1.setColor(QColorGroup::HighlightedText,white);

        QTableItem::paint(p,cg1,cr,selected);
}


ManifoldInterface::ManifoldInterface(int u, bool needsSave, QString p, Triangulation *m, DiagramCanvas *d,
			DiagramWindow *dw, Organizer *o)
		  : QWidget()
{
	QPixmap actionIcon, viewIcon, diskIcon, drawIcon, orbIcon;

	actionIcon = QPixmap( action_xpm );
	viewIcon = QPixmap( view_xpm );
	diskIcon = QPixmap( disk_xpm );
	drawIcon = QPixmap( draw_xpm );	
	orbIcon = QPixmap( orb32_xpm );

	setIcon( orbIcon );

	organizer = o;
	manifold = m;
	upper_bound = u;
	
	copy_triangulation( manifold, &undoManifold1 );
	copy_triangulation( manifold, &undoManifold2 );
	canvas = d;
	diagramWindow = dw;
	path = p;
	setCaption( manifold->name );

    QColor colorList[19];
colorList[0] = Qt::red;
colorList[1] = Qt::green;
colorList[2] = Qt::blue;
colorList[3] = Qt::cyan;
colorList[4] = Qt::magenta;
colorList[5] = QColor(255,180,15);

colorList[6] = QColor( 220, 100, 150 );
colorList[7] = QColor( 150, 220, 100 );
colorList[8] = QColor( 150, 100, 220  );
colorList[9] = QColor( 100, 150, 220 );
colorList[10] = QColor( 220, 150, 100  );
colorList[11] = QColor( 100, 220, 150 );

colorList[12] = QColor( 230, 60, 180 );
colorList[13] = QColor( 180, 230, 60 );
colorList[14] = QColor( 180, 60, 230 );
colorList[15] = QColor( 60, 180, 230 );
colorList[16] = QColor( 230, 180, 60 );
colorList[17] = QColor( 60, 230, 180 );

colorList[18] = Qt::lightGray;

	grid = new QGridLayout( this, 13, 4, 1, 0, 0 ); 
	solutionLabel = new QLabel(this);
	solutionLabel->setAlignment( Qt::AlignCenter );
	solutionLabel->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Sunken );
	solutionLabel->setLineWidth( 2 );
	solutionLabel->setMinimumWidth( 100 );
	grid->addMultiCellWidget( solutionLabel, 10, 10, 0, 2, 0);
	grid->setRowStretch( 10, 4 );
	grid->setRowStretch( 11, 1 );

	edges = new MyTable(manifold->num_singular_arcs,2,this,"table");
	edges->setSelectionMode( QTable::NoSelection );
	edges->setShowGrid(FALSE);
	connect( edges, SIGNAL( valueChanged(int,int) ),
		this, SLOT( updateValueSlot(int,int) ));
	edges->horizontalHeader()->setLabel( 0, "  " );
	edges->horizontalHeader()->setLabel( 1, "Labels" );
	edges->setVScrollBarMode(QScrollView::AlwaysOn );
	edges->setHScrollBarMode(QScrollView::AlwaysOff );
	edges->setColumnWidth( 0, 5 );
	edges->setMinimumHeight(45);
	edges->setMaximumWidth(120);
	edges->setColumnStretchable(1,TRUE);
	edges->setLineWidth( 3 );
	grid->addMultiCellWidget( edges, 0, 3, 0,2, 0 ); 
//	grid->setColSpacing(0,15);
//	grid->setColSpacing(1,15);
//	grid->setColSpacing(2,15);
	grid->setRowStretch( 1, 0 );
	if (manifold->num_singular_arcs==0)
		edges->hide();
	else	for(EdgeClass *edge = manifold->edge_list_begin.next;
				edge!=&manifold->edge_list_end;
				edge = edge->next )
		if (edge->singular_index > -1)
		{
			TextItem *ti=new TextItem(edges);
			edges->setItem(edge->singular_index,0,ti);
			ti->setColor( Qt::black, colorList[edge->singular_index % 18] );
			ti->setText("  ");
			ti->setEnabled(FALSE);

			ti=new TextItem(edges);
                        edges->setItem(edge->singular_index,1,ti);
                        ti->setColor( Qt::black, Qt::white);

			QString s;
			s.sprintf("%5.3f", edge->singular_order );
                        ti->setText(s);

                        edges->setRowStretchable( edge->singular_index, FALSE );
		}
	edges->setColumnReadOnly(0,TRUE);

	QPalette edge_box = edges->palette();

	QColorGroup cg = edge_box.active();
	cg.setColor(QColorGroup::Base, Qt::white );
	cg.setColor(QColorGroup::Text, Qt::black );
	cg.setColor(QColorGroup::Highlight, Qt::white );
	cg.setColor(QColorGroup::HighlightedText, Qt::black );
	edge_box.setBrush(QColorGroup::Text, TEXT );
	edge_box.setBrush(QColorGroup::Base, BASE );
	edge_box.setActive(cg);
	edges->setPalette(edge_box);

	checkBox2 = new QCheckBox( "Fix labels", this ,0 );
	checkBox2->setChecked( FALSE );
	checkBox2->setAccel(CTRL+Key_L);
	grid->addMultiCellWidget( checkBox2, 4,4,0,2,0);

	grid->setRowStretch( 5, /*10*/ 2 );

	grid->setRowStretch( 7, 0 );
	if (manifold->num_singular_arcs < 2 )
		checkBox2->hide();

	saveButton = new QToolButton( this, "save");
	saveButton->setTextLabel("save");
	saveButton->setIconSet( diskIcon);
	saveButton->setMaximumHeight( 33 );
	saveButton->setMaximumWidth( 33 );
	saveButton->setUsesBigPixmap(FALSE);
	saveButton->setAccel(CTRL+Key_S);
	grid->addWidget( saveButton, 12,0,0);

	viewButton = new QToolButton( this, "view");
	viewButton->setTextLabel("view");
	viewButton->setIconSet( viewIcon );
        viewButton->setMaximumHeight( 33 );
        viewButton->setMaximumWidth( 33 );
	viewButton->setAccel(Key_QuoteLeft);
	grid->addWidget( viewButton, 12,1,0);

	optionsButton = new QToolButton( this, "options");
	optionsButton->setIconSet( actionIcon );
        optionsButton->setMaximumHeight( 33 );
        optionsButton->setMaximumWidth( 33 );
	optionsButton->setTextLabel("action");
	grid->addWidget( optionsButton, 12,2,0);

	optionsMenu = new QPopupMenu(this,"optionsMenu" );
	optionsMenu->setAccel( CTRL+Key_B, optionsMenu->insertItem("Build cover..." ,this, SLOT(coverSlot())));
	optionsMenu->setAccel( CTRL+Key_D, optionsMenu->insertItem("Dehn filling...", this, SLOT(dehnSlot())));
	optionsMenu->setAccel( CTRL+Key_G, optionsMenu->insertItem("Drill graph", this, SLOT(drillSlot())));
	optionsMenu->setAccel( CTRL+Key_E, optionsMenu->insertItem("Export SnapPea file...", this, SLOT(SnapPeaSlot())));
	optionsMenu->setAccel( CTRL+Key_F, optionsMenu->insertItem("Find Triangulation...",this, SLOT(listSlot())));
	optionsMenu->setAccel( CTRL+Key_P, optionsMenu->insertItem("Prune graph", this, SLOT(pruneSlot())));
	optionsMenu->setAccel( CTRL+Key_Z, optionsMenu->insertItem("Undo", this, SLOT(undoSlot())));
	optionsButton->setPopup( optionsMenu );
	optionsButton->setPopupDelay( 1 );

	manual = new QCheckBox("Manual",this,0);
	manual->setAccel(CTRL+Key_M);
	manual->setChecked( FALSE );
	if (manifold->num_singular_arcs == 0 )
	{
		manual->setEnabled(FALSE);
		manual->hide();
	}
	grid->addMultiCellWidget( manual, 8,8,0,2,0);

	compute = new QPushButton( "Update", this, 0 );
	compute->setAccel(Key_U);
	connect( compute, SIGNAL( clicked() ),
		this, SLOT( structureSlot() ) );
	grid->addMultiCellWidget( compute, 7,7,0, 2, 0 );
	grid->setRowStretch(9,1);
	grid->setColStretch(0,0);
	grid->setColStretch(1,0);
	grid->setColStretch(2,0);

	stack = new QWidgetStack( this, "stack" );
	grid->addMultiCellWidget( stack, 0, 12, 3, 3, 0);
	grid->setColStretch( 3, 1 );
 
	console = new Console(m, this);
	connect( console, SIGNAL( retriangulation() ),
		this, SLOT(prepareForUndoSlot()));
	connect( console, SIGNAL( triangulationChanged() ),
		this, SLOT(retriangulationSlot()));
	stack->addWidget( console, -1 );

	Tetrahedron *tet = manifold->tet_list_begin.next;

	if ( manifold->solution_type[complete] == not_attempted )
	{
 		for(int i=0;i<4;i++)
   		for(int j=0;j<4;j++)
   		if (i!=j)
			tet->edge_class[edge_between_vertices[i][j]]
				->inner_product[ultimate] = -2;
   		else	
			tet->cusp[i]->inner_product[ultimate] = 0;
	}
	

	if (canvas!=NULL)
	{
		frame1 = new QVBox( this, 0 );
		frame1->setLineWidth( 0 );
		frame1->setMidLineWidth( 1 );
		stack->addWidget( frame1, -1 );

		f1 = new QHBox( frame1, 0 );
		status = new QLabel( f1, 0 );
		status->setFrameStyle( QFrame::Box  );
		//status->setPaletteForegroundColor( COMMAND );
		status->setLineWidth( 1 );
		status->setMidLineWidth( 0 );
		status->setMaximumHeight( 21 );
		f1->setStretchFactor( status, 10 );

		checkBox1 = new QCheckBox( "Highlight", f1 ,0 );
		checkBox1->setChecked( FALSE );

		connect( checkBox1, SIGNAL( stateChanged( int ) ),
			this, SLOT( highlightChanged() ) );	

		draw = new QToolButton( f1, 0 );
		draw->setMaximumHeight( 25 );
		draw->setIconSet( drawIcon);
		connect( draw, SIGNAL(clicked()),
			this, SLOT(drawSlot()) );

		f2 = new QHBox( frame1, 0 );
		f2->setLineWidth( 3 );
		f2->setFrameStyle( QFrame::LineEditPanel | QFrame::Sunken );
		frame1->setStretchFactor( f2, 10 );
		canvas->reparent( f2, 0, QPoint(0,0), TRUE );
		canvas->setMouseTracking( TRUE );
		canvas->update();
		connect( canvas, SIGNAL(statusBarChanged(const QString &)),
			this, SLOT(updateStatusSlot(const QString &)) );

		connect( canvas, SIGNAL( selectionChanged( int ) ),
			this, SLOT( canvasSelectionSlot( int ) ) );
	}
	else	viewButton->setEnabled(FALSE);

	connect(this, SIGNAL( structureChanged() ),
		this, SLOT( updateLabelSlot() ));
	connect(saveButton, SIGNAL( clicked() ),
		this, SLOT( chooseSaveSlot() ));

	connect( viewButton, SIGNAL( clicked() ),
		this,		SLOT( switchStackSlot() ));
        connect( edges, SIGNAL( currentChanged(int,int) ),
                this, SLOT( highlightDiagramEdgeSlot()));

	updateLabelSlot();
	setAutoMask( TRUE );

	setMaximumWidth( organizer->desktop()->width()-300 );
	setMaximumHeight( organizer->desktop()->height()-upper_bound-50);

        if (needsSave)	saved = FALSE;
        else		saved = TRUE;
}

ManifoldInterface::~ManifoldInterface()
{
	edges->~MyTable();
	compute->~QPushButton();
	manual->~QCheckBox();
	console->~Console();
	viewButton->~QToolButton();
	optionsMenu->~QPopupMenu();
	optionsButton->~QToolButton();
	saveButton->~QToolButton();

	if (canvas!=NULL)
	{
		status->~QLabel();
		draw->~QToolButton();
		checkBox1->~QCheckBox();
		canvas->~DiagramCanvas();
		f1->~QHBox();
		f2->~QHBox();
		frame1->~QVBox();	
	}

	stack->~QWidgetStack();
	solutionLabel->~QLabel();
	checkBox2->~QCheckBox();
	grid->~QGridLayout();
	free_triangulation( manifold );

	free_triangulation( undoManifold1 );
	free_triangulation( undoManifold2 );
}

Triangulation *ManifoldInterface::m()
{
	return manifold;
}

void ManifoldInterface::undoSlot()
{
	Triangulation *temp = NULL;

	temp = manifold;
	manifold = undoManifold1;
	undoManifold1 = temp;

	free_triangulation( undoManifold2 );
	copy_triangulation( manifold, &undoManifold2 );

	for( EdgeClass *edge = manifold->edge_list_begin.next;
		edge != &manifold->edge_list_end;
		edge = edge->next )
	if ( edge->singular_index > -1 )
	{
		QString s;
		s.sprintf("%5.3f",edge->singular_order);

		edges->item( edge->singular_index, 1)->setText(s);
		edges->updateCell( edge->singular_index, 1);
	}

	console->setTriangulation( manifold );
	emit structureChanged();	
}


void ManifoldInterface::dehnSlot()
{
	Cusp *cusp;
	bool ok = FALSE;

	for(	cusp = manifold->cusp_list_begin.next;
		cusp!=&manifold->cusp_list_end;
		cusp = cusp->next )
	if (	cusp->topology == torus_cusp )
		ok = TRUE;

	if (!ok)
	{
		QMessageBox::information( this, "Orb",
		"There are no torus cusps.");
		setActiveWindow();
		return;
	}

	if (manifold->orientability!=oriented_manifold)
	{
		QMessageBox::information( this, "Orb",
		"Orb can not Dehn fill nonorientable orbifolds yet.");
		setActiveWindow();
		return;
	}

	DehnInterface *dehn = new DehnInterface(caption(),manifold, organizer, this );
	dehn->show();
}

void ManifoldInterface::coverSlot()
{
	for( int i = 0; i < manifold->num_singular_arcs; i++)
	{
		bool ok;

		double d = edges->text(i,1).toDouble(&ok);

		if (!ok || floor(d) != d || d == 0 )
		{
			QMessageBox::information( this, "Orb",
				"Integer singular orders required.");
			setActiveWindow();
			return;
		}
	}

	CoverInterface *c = new CoverInterface(caption(),manifold, organizer, this );
	c->show();
}

void ManifoldInterface::listSlot()
{
	ListInterface *l = new ListInterface(caption(),manifold, canvas, organizer, this );
	l->show();
}

void ManifoldInterface::canvasSelectionSlot( int arc_id )
{
	edges->setCurrentCell( arc_id, 1 );
}

void ManifoldInterface::highlightDiagramEdgeSlot()
{

	if (canvas!=NULL && checkBox1->isChecked())
		canvas->highlightEdge(edges->currentRow());

}

void ManifoldInterface::highlightChanged()
{
	if (checkBox1->isChecked())
		canvas->highlightEdge(edges->currentRow());
	else	canvas->deselectEdges();
}

void ManifoldInterface::switchStackSlot()
{
	if (canvas==NULL)
		return;

	if (stack->visibleWidget() == frame1 )
		stack->raiseWidget( console );
	else	stack->raiseWidget( frame1 );

}

void ManifoldInterface::raiseDiagramSlot()
{
	stack->raiseWidget( frame1 );

	if (stack->isHidden())
		stack->show();
}

void ManifoldInterface::raiseConsoleSlot()
{
	stack->raiseWidget( console );

        if (stack->isHidden())
                stack->show();
}

void ManifoldInterface::closeEvent( QCloseEvent *e )
{
	if (!saved)
		if (promptSaveSlot()==FALSE)
			return;

	emit closed(this);
}

bool ManifoldInterface::promptSaveSlot()
{

		setActiveWindow();

		if (path.isEmpty())
		switch (QMessageBox::question( this,"Orb", tr("Save changes to %1 ?").arg(caption()),
			tr("Save as..."),tr("Don't save"), tr("Cancel")))
		{
			case 0: saveAsSlot(); setActiveWindow(); return TRUE;
			case 1: setActiveWindow(); return TRUE;
			default: setActiveWindow(); return FALSE;
		}
		else
		{
                	switch (QMessageBox::question( this,"Orb", tr("Save changes to %1 ?").arg(caption()),
                        	tr("Save"),tr("Don't save"), tr("Cancel")))
			{
				case 0: saveSlot(); setActiveWindow(); return TRUE;
				case 1: setActiveWindow(); return TRUE;
				default: setActiveWindow(); return FALSE;
			}
		}

		return FALSE;
}

void ManifoldInterface::chooseSaveSlot()
{

                setActiveWindow();

                if (path.isEmpty())
                switch (QMessageBox::question( this,"Orb", tr("Save changes to %1 ?").arg(caption()),
                        tr("Save as..."),tr("Cancel")))
                {
                        case 0: saveAsSlot(); break;
                        default: break;
                }
                else
                switch (QMessageBox::question( this,"Orb", tr("Save changes to %1 ?").arg(caption()),
                        tr("Save"),tr("Save as..."),tr("Cancel")))
                {
                        case 0: saveSlot(); break;
                        case 1: saveAsSlot(); break;
                        default: break;
                }

		setActiveWindow();
}

void ManifoldInterface::makeActiveSlot()
{
	setActiveWindow();
}

void ManifoldInterface::structureSlot()
{
	for( EdgeClass *edge = manifold->edge_list_begin.next;
			edge!=&manifold->edge_list_end;
			edge = edge->next )
	if (edge->is_singular)
	{
		for ( EdgeClass *edge1 = undoManifold2->edge_list_begin.next;
				edge1!=&undoManifold2->edge_list_end;
				edge1 = edge1->next )
			if (edge->singular_index == edge1->singular_index )
				edge->old_singular_order = edge1->singular_order;
	}

	find_structure( manifold, manual->isChecked() );

	free_triangulation( undoManifold1 );
	undoManifold1 = undoManifold2;
	copy_triangulation( manifold, &undoManifold2 );	

	emit structureChanged();
}

void ManifoldInterface::retriangulationSlot()
{
	manifold->solution_type[complete] = not_attempted;
	manifold->solution_type[filled] = not_attempted;

        for( EdgeClass *edge = manifold->edge_list_begin.next;
                        edge!=&manifold->edge_list_end;
                        edge = edge->next )
        if (edge->is_singular)
                                edge->old_singular_order = edge->singular_order;

	find_structure( manifold, manual->isChecked());

	emit structureChanged();
}

void ManifoldInterface::prepareForUndoSlot()
{
	free_triangulation( undoManifold1 );
	free_triangulation( undoManifold2 );
	copy_triangulation( manifold, &undoManifold1 );
	copy_triangulation( manifold, &undoManifold2 );
}


void ManifoldInterface::updateValueSlot(int r, int c)
{
	bool ok;

	double label = edges->text(r,c).toDouble(&ok);

	if (!ok)
	{
		QMessageBox::information( this, "Orb",
			"Singular edge order should be\n"
			"a real number" );
		edges->setText(r,c,"0.000");
		setActiveWindow();
	}

        QString s;
        s.sprintf("%5.3f",label);
	edges->setText(r,c,s);	

	if (checkBox2->isChecked())
		for(int i = 0; i < manifold->num_singular_arcs; i++ )
			edges->setText(i,1,s);

	for( EdgeClass *edge = manifold->edge_list_begin.next;
			edge!=&manifold->edge_list_end;
			edge = edge->next )
	if (edge->singular_index == r || (checkBox2->isChecked() && edge->singular_index > -1) )
		edge->singular_order = label;

	compute_cusp_euler_characteristics( manifold );

	unattemptedSlot();
}

void ManifoldInterface::unattemptedSlot()
{
	solutionLabel->setText("n/a\n ");
}

void ManifoldInterface::updateLabelSlot()
{
	Boolean ok;	

	saved = FALSE;

	switch( manifold->solution_type[complete])
	{
		case not_attempted:
			solutionLabel->setText("n/a\n ");	
			break;

		case geometric_solution:
			if ( contains_flat_tetrahedra(manifold) == TRUE )
				solutionLabel->setText( tr("partially flat\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			else
				solutionLabel->setText( tr("geometric\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			break;

		case nongeometric_solution:
			solutionLabel->setText( tr("nongeometric\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			break;

		case flat_solution:
			solutionLabel->setText( tr("flat\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			break;

		case degenerate_solution:
			solutionLabel->setText( tr("degenerate\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			break;

		case other_solution:
			solutionLabel->setText( tr("other\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			break;

		case no_solution:
			solutionLabel->setText( tr("no solution\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			break;

		case step_failed:
			solutionLabel->setText( tr("step failed\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			break;

		case invalid_solution:
			solutionLabel->setText( tr("invalid\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			break;

		default:
			solutionLabel->setText( tr("ERROR\n%1%2").
					arg(my_volume(manifold,&ok)).arg((ok) ? ' ' : '*'));
			break;	
	}
}

void ManifoldInterface::showSlot(const QString &front )
{
	if (front=="Console")
		raiseConsoleSlot();

	if (front=="Diagram")
		raiseDiagramSlot();

	if (front=="-None-")
		stack->hide();	
}

void ManifoldInterface::saveSlot()
{
        QString p = (path.isEmpty()) ? organizer->getPath() : path;
        p.append(tr("/%1.orb").arg(caption()));
	QFile file( p );

	if ( file.open( IO_WriteOnly ) )
	{
		QTextStream stream( &file );
		stream << "% orb\n";
		stream << manifold->name << "\n";
		console->saveTriangulation( stream, FALSE, TRUE, TRUE );

		if (canvas!=NULL)
			canvas->saveDiagram( stream );
	}

	saved = TRUE;
}

void ManifoldInterface::saveAsSlot()
{
	setActiveWindow();


	QString p = (path.isEmpty()) ? organizer->getPath() : path;
	p.append(tr("/%1.orb").arg(caption()));

	QString f = QFileDialog::getSaveFileName( p,"*.orb", this,"save file dialog",
                    "Choose a filename to save under" );


	if (f.isEmpty())
		return;

	QString temp = f.section('/',0,-2 );
	temp.append('/');
	organizer->setPath(temp);
	path = f;

	if (!f.endsWith(".orb"))
		f.append(".orb");

	if (QFileInfo(f).exists())
	{
		if (QMessageBox::warning( this, "Orb", "Save over existing file?",
			QMessageBox::Cancel, QMessageBox::Yes)== QMessageBox::Cancel)
		{
			setActiveWindow();
			return;
		}
		setActiveWindow();
	}

	if (manifold->name!=NULL)
		my_free( manifold->name );
	temp = f.section('/',-1);
	temp.truncate( temp.length() - 4 );

	manifold->name = NEW_ARRAY( temp.length() + 1, char );
	for( int i = 0; i < temp.length() + 1; i++ )
		manifold->name[i] = temp[i].latin1();	
	setCaption(manifold->name);

	QFile file( f );	
	if ( file.open( IO_WriteOnly ) )
	{
		QTextStream stream( &file );
		stream << "% orb\n";
		stream << caption() << "\n";
		console->saveTriangulation( stream, FALSE, TRUE, TRUE );

		if (canvas!=NULL)
			canvas->saveDiagram( stream );
	}
	else
	{
                if (QMessageBox::information( this, "Orb", "There was an error while saving the file.",
					QMessageBox::Ok) )
                setActiveWindow();
                return;
        }

	saved = TRUE;

	emit nameChange();
}


void ManifoldInterface::updateStatusSlot( const QString &message)
{
	status->setText(message);
}

void ManifoldInterface::drawSlot()
{
	QString         d;
	QTextStream     stream1(&d,IO_WriteOnly);

	canvas->saveDiagram( stream1 );

	QTextStream     stream2(&d,IO_ReadOnly);
	
	diagramWindow->loadDiagram( stream2 );
}

void ManifoldInterface::resizeEvent( QResizeEvent *e )
{
/*
        if (y() < upper_bound || x() < 200 )
        {
		int new_x = (x() < 250 ) ? 250 : x();
		int new_y = (y() < upper_bound) ? upper_bound : y();
                setGeometry(new_x,new_y,width(),height());
                repaint();
	}
*/
}   

bool ManifoldInterface::isSaved()
{
	return saved;
}

void ManifoldInterface::pruneSlot()
{
	int		index, *new_points, new_num, i, j, n;
	EdgeClass	*edge1, *edge2;
	Cusp		*cusp;
	Boolean		prunable = FALSE, *pruning, creates_bad_vertex = FALSE, go_again = TRUE;
	Triangulation	*copy;

	pruning = NEW_ARRAY( manifold->num_singular_arcs, Boolean );

	for(	edge1 = manifold->edge_list_begin.next;
		edge1!=&manifold->edge_list_end;
		edge1 = edge1->next )
	if (	edge1->is_singular && edge1->singular_order == 1 )
	{
		prunable = TRUE;
		pruning[edge1->singular_index] = TRUE;
	}
	else	pruning[edge1->singular_index] = FALSE;

	if (!prunable)
	{
		QMessageBox::information( this, "Orb","There are no edges for pruning.\n");
		my_free( pruning );
		setActiveWindow();
		return;
	}
	else	for(	cusp = manifold->cusp_list_begin.next;
			cusp!=&manifold->cusp_list_end;		/* need to check if we are creating a vertex of degree 1 */
			cusp = cusp->next )
		if (	cusp->num_cone_points > 0 )
		{
			n = 0;
			for( i = 0; i<cusp->num_cone_points; i++ )
			if ( pruning[cusp->cone_points[i]])
				n++;

			if (cusp->num_cone_points-n==1)
				creates_bad_vertex = TRUE;

		}
	
	my_free( pruning );

        if (creates_bad_vertex)
        {
                QMessageBox::information( this, "Orb","Pruning would create a vertex of degree  1.\n");
		setActiveWindow();
                return;
	}

	copy_triangulation( manifold, &copy );	

	while (go_again)
	{
		go_again = FALSE;

		for(	edge1 = copy->edge_list_begin.next;
			edge1!=&copy->edge_list_end;
			edge1 = edge1->next )
		if (	edge1->is_singular && edge1->singular_order == 1 )
		{
			go_again = TRUE;

			index = edge1->singular_index;
			edge1->is_singular = FALSE;
			edge1->singular_order = 1;
			edge1->singular_index = -1;

			for(	edge2 = copy->edge_list_begin.next;
				edge2!=&copy->edge_list_end;
				edge2 = edge2->next )
			if (    edge2->is_singular && edge2->singular_index > index )
				edge2->singular_index--;
				
			for(	cusp = copy->cusp_list_begin.next;
				cusp!=&copy->cusp_list_end;
				cusp = cusp->next )
			{
				new_num = 0;
				new_points = NULL;
				for( i = 0; i<cusp->num_cone_points;i++ )
				if (cusp->cone_points[i]!=index)
					new_num++;

				new_points = NEW_ARRAY( new_num, int );
				new_num=0;
				for( i = 0; i<cusp->num_cone_points;i++ )
				if( cusp->cone_points[i]!=index)
				{
					if (cusp->cone_points[i]<index)
						new_points[new_num++] = cusp->cone_points[i];
					else	new_points[new_num++] = cusp->cone_points[i]-1;
				}

				my_free(cusp->cone_points);
				cusp->cone_points = new_points;
				cusp->num_cone_points = new_num;

				if (	cusp->num_cone_points == 0 &&
					cusp->euler_characteristic == 0 )
					identify_one_cusp(copy,cusp);
			}

			copy->num_singular_arcs--;
			remove_bad_vertices(copy);
			break;
		}
	}

	basic_simplification( copy );
	compute_cusp_euler_characteristics(copy);
	peripheral_curves_as_needed( copy );

	DiagramCanvas	*new_canvas = NULL;

	if (canvas!=NULL)
	{
        	QString         d;
        	QTextStream     stream1(&d,IO_WriteOnly);
        	canvas->saveDiagram( stream1 );
        	QTextStream     stream2(&d,IO_ReadOnly);

		new_canvas = new DiagramCanvas( this, "canvas", TRUE );
		new_canvas->readDiagram( stream2 );

		j = 0;
		for (i = 0; i<manifold->num_singular_arcs;i++)
		for(	edge1 = manifold->edge_list_begin.next;
			edge1!=&manifold->edge_list_end;
			edge1 = edge1->next )
		if(	edge1->is_singular && edge1->singular_index == i )
		{
			if ( edge1->singular_order == 1 )
			{
				new_canvas->deleteArc( edge1->singular_index - j );
				j++;
			}
			break;
		}
	}

	organizer->loadPrune( copy, new_canvas, caption() );
}


void ManifoldInterface::SnapPeaSlot()
{
	Cusp *cusp;

	for(	cusp = manifold->cusp_list_begin.next;
		cusp!=&manifold->cusp_list_end;
		cusp = cusp->next )
	if (	cusp->topology!=torus_cusp && cusp->topology!=Klein_cusp )
	{
                QMessageBox::information( this, "Orb","Can not export for SnapPea.\n");
                setActiveWindow();
                return;
	}

        TriangulationData       *theTriangulationData;
        FILE                            *fp;

        QString p = organizer->getPath();
        p.append(tr("/"));

        QString f = QFileDialog::getSaveFileName( p,
                         "*.txt", this,"export file dialog",
                    "Choose a filename to export under" );

        if (f.isEmpty())
                return;

        if (QFileInfo(f).exists())
        {
                if (QMessageBox::warning( this, "Orb", "Save over existing file?",
                        QMessageBox::Cancel, QMessageBox::Yes)== QMessageBox::Cancel)
		{
			setActiveWindow();
                        return;
		}
		setActiveWindow();
        }

        QString temp = f.section('/',0,-2 );
        temp.append('/');
        organizer->setPath(temp);

        fp = fopen(f.ascii(), "w");
        if (fp == NULL)
        {
                QMessageBox::information( this, "Orb","Failed to export file.\n");
                setActiveWindow();
                return;
        }

        triangulation_to_data(manifold, &theTriangulationData);
        WriteNewFileFormat(fp, theTriangulationData);
        free_triangulation_data(theTriangulationData);

        fclose(fp);
}

void ManifoldInterface::drillSlot()
{
	EdgeClass	*edge;
	Boolean		drillable = FALSE, go_again;
	Triangulation	*copy;

	for(    edge = manifold->edge_list_begin.next;
		edge!=&manifold->edge_list_end;
		edge = edge->next )
		if (    edge->is_singular && edge->singular_order == 0 )
			drillable = TRUE;

	if (!drillable)
	{
		QMessageBox::information( this, "Orb","There are no edges for drilling.\n");
		setActiveWindow();
		return;
	}

	copy_triangulation( manifold, &copy );

        DiagramCanvas   *new_canvas = NULL;

        if (canvas!=NULL)
        {
                QString         d;
                QTextStream     stream1(&d,IO_WriteOnly);
                canvas->saveDiagram( stream1 );
                QTextStream     stream2(&d,IO_ReadOnly);

                new_canvas = new DiagramCanvas( this, "canvas", TRUE );
                new_canvas->readDiagram( stream2 );
        }

	go_again = TRUE;

	while(go_again)
	{
		go_again = FALSE;

		for(	edge = copy->edge_list_begin.next;
			edge!=&copy->edge_list_end;
			edge = edge->next )
			if (	edge->is_singular && edge->singular_order == 0 )	
		{
				if (new_canvas!=NULL)
					new_canvas->drillArc( edge->singular_index );
				my_drill_tube( copy, edge->singular_index );
				go_again = TRUE;
				break;
		}
	}

	basic_simplification( copy );


	organizer->loadDrill( copy, new_canvas, caption() );
}
