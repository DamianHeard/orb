#include "list_interface.h"

#include <qmessagebox.h>
#include <qapplication.h>

using namespace std;

extern "C" int find_triangulation( Triangulation *manifold, Triangulation ***list, int num_tri, int num_rand );
extern "C" void free_triangulation_list( Triangulation **list, int size );
extern "C" Boolean      contains_flat_tetrahedra( Triangulation *manifold );

static double distance_to_solution( Triangulation *manifold );

ListInterface::ListInterface(QString n, Triangulation *m, DiagramCanvas *c, Organizer *o, QWidget *parent )
		  : QWidget()
{
	copy_triangulation( m, &manifold );
	name = n;
	organizer = o;
	setCaption( tr("%1 - representatives ").arg(name) );
	size = 0;
	list = NULL;
	canvas = c;


	connect( parent, SIGNAL(destroyed()),
		this, SLOT(kill()));

	grid = new QGridLayout( this, 4, 4, 4, 4, 0 ); 
	grid->setColStretch( 0, 0 );
	grid->setColStretch( 1, 10 );
	grid->setColStretch( 2, 10 );
	grid->setColStretch( 3, 0 );

	tabulate = new QPushButton( "Tabulate", this, 0 );
	connect( tabulate, SIGNAL(clicked()), this, SLOT( tabulateSlot() ) );
	grid->addWidget(tabulate,3,0,0);

	build = new QPushButton( "Build", this, 0 );
	connect( build, SIGNAL( clicked() ),
		this, SLOT( triSlot() ));
	grid->addMultiCellWidget(build,3,3,1,2,0);

	close = new QPushButton( "Close", this, 0 );
	connect( close, SIGNAL( clicked() ), this, SLOT( close() ) );
	grid->addWidget(close,3,3,0);

	num_r = new QLabel( "Randomizations:",this,0,0);
	grid->addMultiCellWidget( num_r,1,1,0,0,0);

	input_rand = new QSlider( 1, 100, 100, 0, Qt::Horizontal, this, "slider" );
	grid->addMultiCellWidget( input_rand,1,1,1,3,0);

	checkBox = new QCheckBox( "Attempt to find structures", this ,0 );
	connect( checkBox, SIGNAL( clicked()), this, SLOT(warnSlot()) );
	grid->addMultiCellWidget( checkBox, 2,2,0,3,0 );

	data = new QTable(0,6,this,"table");
	QStringList headings;
	headings += "No. tet";
	headings += "No. edges";
	headings += "Coloured edge orders";
	headings += "Non-coloured edge orders";
	headings += "Solution type";
	headings += "Distance to solution";
	data->setColumnWidth( 0, 45 );
	data->setColumnWidth( 1, 60 );
	data->setColumnWidth( 2, 130 );
	data->setColumnWidth( 3, 160 );
	data->setColumnWidth( 4, 100 );
	data->setColumnWidth( 5, 160 );
	data->setColumnStretchable( 5, TRUE );
	data->setColumnLabels( headings );
	data->setSelectionMode( QTable::SingleRow );
	data->setReadOnly( TRUE );
	data->setShowGrid( FALSE );
	data->setVScrollBarMode( QScrollView::Auto );
	data->setHScrollBarMode( QScrollView::Auto );
	grid->addMultiCellWidget( data, 0,0,0,3, 0 ); 
	grid->setColStretch(0,10);

	setMinimumWidth( 670 );
}

ListInterface::~ListInterface()
{
	data->~QTable();
	tabulate->~QPushButton();
	close->~QPushButton();
	build->~QPushButton();
	num_r->~QLabel();
	input_rand->~QSlider();
	checkBox->~QCheckBox();
	grid->~QGridLayout();

	if (list!=NULL)	
		free_triangulation_list( list, size );

	free_triangulation( manifold );
}

void ListInterface::warnSlot()
{
	if (checkBox->isChecked())
	{
		if (	QMessageBox::warning(this, "Orb",
			"Find structures on large list can take a long time.",
			QMessageBox::Ok, QMessageBox::Cancel ) == QMessageBox::Cancel )
			checkBox->setChecked(FALSE);
	}
}

void ListInterface::kill()
{
	this->~ListInterface();
}

void ListInterface::tabulateSlot()
{
	EdgeClass *edge;
	int num_edges;

	if (list!=NULL)
		free_triangulation_list( list, size );
	list = NULL;

	size = find_triangulation( manifold, &list, input_rand->value(), input_rand->value() );

	data->setNumRows(size);

	for( int i = 0; i<size; i++ )
	{
		data->setText(i,0,tr("   %1   ").arg(list[i]->num_tetrahedra));

		num_edges = 0;
		int ce[8]={0,0,0,0,0,0,0,0}, nce[8]={0,0,0,0,0,0,0,0};

		for(	edge = list[i]->edge_list_begin.next;
			edge!=&list[i]->edge_list_end;
			edge = edge->next )
		{
			if (edge->is_singular)
			{
				if (edge->order < 9)
					ce[edge->order]++;
			}
			else	if (edge->order < 9)
					nce[edge->order]++;

			num_edges++;
		}

		QString s1, s2;
		s1.sprintf("   %d,%d,%d,%d,%d,%d,%d,%d  ",ce[0],ce[1],ce[2],ce[3],ce[4],ce[5],ce[6],ce[7]);
		s2.sprintf("   %d,%d,%d,%d,%d,%d,%d,%d  ",nce[0],nce[1],nce[2],nce[3],nce[4],nce[5],nce[6],nce[7]);


		data->setText(i,1,tr("   %1   ").arg(num_edges));
		data->setText(i,2,s1);
		data->setText(i,3,s2);

		if (checkBox->isChecked())
			find_structure(list[i],FALSE);

		if (checkBox->isChecked())
		switch (list[i]->solution_type[complete])
		{
			case not_attempted:
				data->setText(i,4,"n/a");
				break;

			case geometric_solution:
				if ( contains_flat_tetrahedra(list[i]) == TRUE )
					data->setText(i,4,"partially flat");
				else	data->setText(i,4,"geometric");
				break;

			case nongeometric_solution:
				data->setText(i,4,"nongeometric");
				break;

			case flat_solution:
				data->setText(i,4,"flat");
				break;

			case degenerate_solution:
				data->setText(i,4,"degenerate");
				break;

			case other_solution:
				data->setText(i,4,"other");
				break;

			case no_solution:
				data->setText(i,4,"no solution");
				break;

			case step_failed:
				data->setText(i,4,"step failed");
				break;

			case invalid_solution:
				data->setText(i,4,"invalid");
				break;

			default:
				data->setText(i,4,"ERROR");
				break;
		}

		if (checkBox->isChecked())
		{
			QString s;
			s.sprintf("  %18.16f  ",distance_to_solution(list[i]) );
			data->setText(i,5,s);
		}
	}
}

void ListInterface::triSlot()
{
	if (list==NULL || data->currentRow() < 0 )
		return;

	if (list[data->currentRow()]==NULL)
	{
		QMessageBox::critical( this, "Orb", "Failed to build triangulation.", QMessageBox::Ok, 0 );
		return;
	}

	DiagramCanvas *new_canvas = NULL;

	if (canvas!=NULL)
	{
		QString         d;
		QTextStream     stream1(&d,IO_WriteOnly);
		canvas->saveDiagram( stream1 );
		QTextStream     stream2(&d,IO_ReadOnly);

		new_canvas = new DiagramCanvas( this, "canvas", TRUE );
		new_canvas->readDiagram( stream2 );
	}

	Triangulation *copy = NULL;
	copy_triangulation( list[data->currentRow()], &copy );
	organizer->loadFind( copy, new_canvas, name );
}

static double distance_to_solution( Triangulation *manifold )
{
	EdgeClass *edge;
	PositionedTet ptet, ptet0;
	double total = 0;

	for(	edge = manifold->edge_list_begin.next;
		edge!=&manifold->edge_list_end;
		edge = edge->next )
	{
		set_left_edge(edge,&ptet0);
		ptet = ptet0;

		double err =  (edge->singular_order==0)
				? 0
				: TWO_PI / edge->singular_order;

		do{
			err -= ptet.tet->dihedral_angle[ultimate]
					[edge_between_faces[ptet.near_face][ptet.left_face]];

			veer_left(&ptet);
		}while (!same_positioned_tet(&ptet, &ptet0));

		total += err *err;
	}

	return sqrt(total);
}

