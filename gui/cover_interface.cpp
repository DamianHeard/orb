#include "cover_interface.h"

#include <qmessagebox.h>
#include <qapplication.h>

using namespace std;


CoverInterface::CoverInterface(QString n, Triangulation *m, Organizer *o, QWidget *parent )
		  : QWidget()
{
	copy_triangulation( m, &manifold );
	name = n;
	organizer = o;
	setCaption( tr("%1 - covers ").arg(name) );
	repList = NULL;

	connect( parent, SIGNAL(destroyed()),
		this, SLOT(kill()));

	grid = new QGridLayout( this, 4, 2, 4, 4, 0 ); 

	build = new QPushButton( "Build", this, 0 );
	build->setAccel(Key_B);
	connect( build, SIGNAL( clicked() ),
		this, SLOT( coverSlot() ));
	grid->addWidget( build, 3,0, 0 );

	close = new QPushButton( "Close", this, 0 );
	connect( close, SIGNAL( clicked() ), this, SLOT( close() ) );
	close->setAccel(Key_C);
	grid->addWidget( close, 3,1, 0 );

	sheets = new QLabel( "Number of sheets: (1-9) ",this,0,0);
	grid->addWidget( sheets, 0,0, 0 );

	input = new QLineEdit("0","9",this,0 );
	connect( input, SIGNAL(textChanged(const QString & ) ),
			this, SLOT( updateRepsSlot()  ) );
	grid->addWidget( input, 0,1, 0 );

	covers = new QTable(0,4,this,"table");
	QStringList headings;
	headings += "Type";
	headings += "Vertices";
	headings += "Coloured Edges";
	headings += "Homology";
	covers->setColumnLabels( headings );
	covers->setSelectionMode( QTable::SingleRow );
	covers->setReadOnly( TRUE );
	covers->setShowGrid( FALSE );
	covers->setColumnWidth( 0, 35 );
	covers->setColumnWidth( 1, 50 );
	covers->setColumnWidth( 2, 60 );
	covers->setColumnStretchable( 3, TRUE ); 
	covers->setVScrollBarMode( QScrollView::Auto );
	covers->setHScrollBarMode( QScrollView::Auto );
	grid->addMultiCellWidget( covers, 1,1,0,2, 0 ); 
	grid->setColStretch(0,10);
}

CoverInterface::~CoverInterface()
{
	covers->~QTable();
	close->~QPushButton();
	build->~QPushButton();
	sheets->~QLabel();
	input->~QLineEdit();
	grid->~QGridLayout();
	
	if (repList!=NULL)
		free_representation_list( repList );

	free_triangulation( manifold );
}

void CoverInterface::kill()
{
	this->~CoverInterface();
}

void CoverInterface::coverSlot()
{
	if (repList==NULL || covers->currentRow() < 0 )
		return;

	RepresentationIntoSn *rep;	

	if (repList->list == NULL)
	{
		QMessageBox::critical( this, "Orb", "Failed to build cover.", QMessageBox::Ok, 0 );
		return;
	}

	rep = repList->list;

	for(int i = 0; i < covers->currentRow(); i++)
	{
		if (rep->next==NULL)
		{
			QMessageBox::critical( this, "Orb", "Failed to build cover.", QMessageBox::Ok, 0 );
			return;
		}

		rep = rep->next;
	}

	Triangulation *cover = NULL;

	cover = construct_cover( manifold, rep, repList->num_generators, repList->num_sheets );

	if (cover == NULL)
	{
		QMessageBox::critical( this, "Orb", "Failed to build cover.", QMessageBox::Ok, 0 );
		return;
	}

	organizer->loadCover( cover,  name );
}

void CoverInterface::updateRepsSlot()
{
	if (input->text().isEmpty() || input->text() == "0" )
		return;

	if (input->text().toInt()>4)
		if ( QMessageBox::Cancel
			== QMessageBox::warning(this,"Orb", "Looking for a cover of this size risks crashing Orb.",
				QMessageBox::Cancel, QMessageBox::Ignore ) )
				input->setText("1");

	if (repList!=NULL)
		free_representation_list( repList );

	repList = NULL;
	repList = find_representations( manifold, input->text().toInt() , permutation_subgroup_Sn );	

	RepresentationIntoSn *rep;
	int r = 0;
	rep = repList->list;
	covers->setNumRows(0);
	while (rep!=NULL)
	{
		covers->insertRows( r );	

		QString s;

		if (rep->covering_type == unknown_cover )
			s = "    u";

		if (rep->covering_type == irregular_cover )
			s = "    i";

		if (rep->covering_type == regular_cover )
			s = "    r";

		if (rep->covering_type == cyclic_cover )
			s = "    c";

		covers->setText( r, 0, s );

		Triangulation *cover;

		cover = construct_cover( manifold, rep, repList->num_generators, repList->num_sheets );

		if (cover == NULL )
		{
			QMessageBox::critical(this,"Orb", "Error building cover.",
				QMessageBox::Ok, 0,0 );
			uFatalError("","");
		}

		covers->setText( r, 1, tr("  %1").arg(cover->num_cusps ));	
		covers->setText( r, 2, tr("  %1").arg(cover->num_singular_arcs ));

		GroupPresentation *fg =fundamental_group( cover, TRUE, TRUE, TRUE );

		if (fg==NULL)
		{
			QMessageBox::critical(this,"Orb", "Error calculating fundamental group.",
					QMessageBox::Ok, 0,0);
			uFatalError("","");
		}


		AbelianGroup *g = homology_from_fundamental_group( fg );

		if (g==NULL)
		{
			QMessageBox::critical(this,"Orb", "Error calculating homology.",
					QMessageBox::Ok, 0,0);
			uFatalError("","");
		}

		s = "     ";
		for(int i=0;i<g->num_torsion_coefficients;i++)
		{
			if (i!=0)
				s += " + ";
			s += "Z";

			if (g->torsion_coefficients[i]>0)
			{
				s += "/";
				s += tr("%1").arg(g->torsion_coefficients[i]);
			}
		}

		free_abelian_group(g);
		free_group_presentation( fg );

		covers->setText( r, 3, s );

		free_triangulation( cover );
		
		rep = rep->next;
		r++;
	}
}

