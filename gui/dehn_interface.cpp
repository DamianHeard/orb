#include "dehn_interface.h"

#include <qmessagebox.h>
#include <qapplication.h>

using namespace std;

extern "C" FuncResult attach_handle( Triangulation *manifold, Boolean singular_cores );

DehnInterface::DehnInterface(QString n, Triangulation *m, Organizer *o, QWidget *parent )
		  : QWidget()
{
	copy_triangulation( m, &manifold );
	name = n;
	organizer = o;
	setCaption( tr("%1 - Dehn filling ").arg(name) );

	connect( parent, SIGNAL(destroyed()),
		this, SLOT(kill()));

	grid = new QGridLayout( this, 4, 2, 4, 4, 0 ); 
	grid->setRowStretch( 1, 4 );

	check = new QCheckBox("Coloured disk cores",this);
	grid->addMultiCellWidget(check, 2,2,0,1,0);

	build = new QPushButton( "Build", this, 0 );
	build->setAccel(Key_B);
	connect( build, SIGNAL( clicked() ),
		this, SLOT( dehnSlot() ));
	grid->addWidget( build, 3,0, 0 );

	close = new QPushButton( "Close", this, 0 );
	connect( close, SIGNAL( clicked() ), this, SLOT( close() ) );
	close->setAccel(Key_C);
	grid->addWidget( close, 3,1, 0 );

	coordinates = new QTable(0,2,this,"table");
	QStringList headings;
	headings += "Meridians";
	headings += "Longitudes";
	coordinates->setColumnLabels( headings );
	coordinates->setSelectionMode(QTable::NoSelection);
	coordinates->setColumnStretchable( 0, TRUE );
	coordinates->setColumnStretchable( 1, TRUE );
	coordinates->setShowGrid( FALSE );
	coordinates->setVScrollBarMode( QScrollView::Auto );
	coordinates->setHScrollBarMode( QScrollView::Auto );
	grid->addMultiCellWidget( coordinates, 0,0,0,1, 0 ); 

	for( int i = manifold->num_cusps - 1; i >= 0; i-- )
	{
		int r = 0;
		Cusp *cusp = find_cusp( manifold, i );
		if (cusp->topology == torus_cusp )
		{
			coordinates->insertRows( r );
			coordinates->verticalHeader()->
					setLabel( r, tr("Vertex %1").arg(cusp->index + 1));
			coordinates->setText( r, 0, "" );
			coordinates->setText( r, 1, "" );
			r++;
		}
	}
	connect( coordinates, SIGNAL( valueChanged(int,int) ),
		this, SLOT( updateValueSlot(int,int) ));

	setMaximumWidth( 210 );
	resize( 210, 150 );
}

DehnInterface::~DehnInterface()
{
	coordinates->~QTable();
	check->~QCheckBox();
	close->~QPushButton();
	build->~QPushButton();
	grid->~QGridLayout();
	
	free_triangulation( manifold );
}

void DehnInterface::kill()
{
	this->~DehnInterface();
}

void DehnInterface::dehnSlot()
{
	int r = 0;
	Cusp *cusp;
	Triangulation *copy;

	copy_triangulation( manifold, &copy );

	for(	cusp = copy->cusp_list_begin.next;
		cusp!=&copy->cusp_list_end;
		cusp = cusp->next )
	if (cusp->topology == torus_cusp )
	{
		bool ok;

		cusp->m = 0;

		ok = TRUE;
		if (coordinates->text(r,0).isEmpty()==FALSE)
			cusp->m = (double) coordinates->text(r,0).toInt(&ok);

		if (!ok)
		{
			QMessageBox::critical( this, "Orb", "Error in dehnSlot()",
				QMessageBox::Ok, 0 );
			return;
		}

		cusp->l = 0;
		ok = TRUE;
		if (coordinates->text(r,1).isEmpty()==FALSE)
			cusp->l = (double) coordinates->text(r,1).toInt(&ok);
		if (!ok)
		{
			QMessageBox::critical( this, "Orb", "Error in dehnSlot().",
				QMessageBox::Ok, 0 );
			return;
		}

		if (cusp->m != 0 ||  cusp->l != 0 )
			cusp->is_complete = FALSE;
		else	cusp->is_complete = TRUE;

		r++;
	}

        if (attach_handle( copy, check->isChecked() )==func_failed)
        {
                QMessageBox::critical( this, "Orb", "Failed to produce Dehn filling.",
					QMessageBox::Ok, 0 );
                return;
        }

        organizer->loadDehn( copy,  name );
}

void DehnInterface::updateValueSlot(int r, int c)
{
	bool ok = TRUE;

	if (coordinates->text(r,c).isEmpty()==FALSE)
		coordinates->text(r,c).toInt(&ok);

	if (!ok)
	{
		QMessageBox::information( this, "Orb",
			"Enter integer values only.\n" );
		coordinates->setText(r,c,"");
		setActiveWindow();
	}
}
