#include "console.h"
#include "color.h"
#include "qfile.h"
#include <qapplication.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qpixmap.h>

#include "zoomin.xpm"
#include "zoomout.xpm"
#include "clear.xpm"

using namespace std;

extern "C" double	tetrahedron_volume( double angles[6], Boolean *ok );
extern "C" void		basic_simplification(Triangulation *manifold );
extern "C" void		randomize_triangulation(Triangulation *manifold );
extern "C" void		new_choose_generators(Triangulation *manifold, Boolean b );
extern "C" void		new_matrix_generators(Triangulation *manifold, O31Matrix *matrices );
extern "C" void		compute_reflection( int index, O31Matrix gen, GL4RMatrix basis );
extern "C" Boolean	contains_flat_tetrahedra( Triangulation *manifold );
extern "C" AbelianGroup *homology_from_fundamental_group( GroupPresentation *fg );
extern "C" void		symmetry_group_cusp_action(SymmetryGroup *sg, int i,int j,int *image,int ***map );
static double	edge_length( Tetrahedron *tet, int e );
static void print_group( QTextStream &stream, SymmetryGroup *sg );
static  void    print_abelian_group( QTextStream &stream, AbelianGroup *ag );
static  void    print_polyhedral_group( QTextStream &stream, SymmetryGroup *sg );

#define IDEAL_EPSILON		1e-4


Console::Console( Triangulation *m, QWidget *parent )
                  : QWidget( parent, "console" )
{
	QPixmap zoominIcon, zoomoutIcon, clearIcon;
	zoominIcon = QPixmap( zoomin_xpm );
	zoomoutIcon = QPixmap( zoomout_xpm );
	clearIcon = QPixmap( clear_xpm );
	QColorGroup cg;
	QPalette    p;
	QFont f( "Monaco", 13 );
	f.setUnderline( TRUE );
	f.setItalic( TRUE );
	manifold = m;
	cut_off = 2.000;
	tile_radius = 2.000;
	vertex_epsilon = 1e-6;

	grid = new QGridLayout( this, 2, 12, 0, 0, 0 );

        triButton = new QToolButton( this, "triangulation");
	triButton->setMaximumHeight( 25 );
        triButton->setTextLabel("tri");
        triButton->setUsesTextLabel( TRUE );
	triButton->setFont( f );
	triButton->setAccel(Key_Space);
        grid->addWidget( triButton,0,0,0);	
	grid->setColStretch( 0, 10 );

	triMenu = new QPopupMenu( this, "triMenu");
	triMenu->setItemEnabled( triMenu->insertItem( "Actions:", -1, -1 ), FALSE );
	triMenu->setAccel( Key_C,  triMenu->insertItem("Canonize", this, SLOT( can() ) ) );
	triMenu->setAccel( Key_R,  triMenu->insertItem("Randomize",this, SLOT(rnd() ) ) );
	triMenu->setAccel( SHIFT+Key_S,  triMenu->insertItem("Simplify",this, SLOT( sim() ) ) );
	triMenu->setAccel( SHIFT+Key_R, triMenu->insertItem("Remove finite vertices", this, SLOT( rem() ) ) );
	triMenu->insertSeparator(-1);
	triMenu->setItemEnabled( triMenu->insertItem( "Print:", -1, -1 ), FALSE );
	ae_id = triMenu->insertItem("Angle errors",this, SLOT( aeSlot() ) );
	v_id = triMenu->insertItem("Verbose", this, SLOT( vSlot() ) );
	triMenu->setAccel( Key_A, ae_id );
	triMenu->setAccel( Key_V, v_id );
	triMenu->setCheckable(TRUE);
	triButton->setPopup( triMenu );
	triButton->setPopupDelay( 0 );

	tetButton = new QToolButton( this, "tetrahedra");
	tetButton->setMaximumHeight( 25 );
	tetButton->setTextLabel("tet");
	tetButton->setUsesTextLabel( TRUE );
	tetButton->setFont( f );
	tetButton->setAccel(Key_T);
	grid->addWidget( tetButton,0,1,0);
	grid->setColStretch( 1, 10 );

	tetMenu = new QPopupMenu( this, "tetMenu");
	tetMenu->setCheckable( TRUE );
	tetMenu->setItemEnabled( tetMenu->insertItem( "Print:", -1, -1 ), FALSE );
	da_id = tetMenu->insertItem("Dihedral angles",this, SLOT(daSlot() ) );
	tetMenu->setAccel( Key_D,  da_id );
	el_id = tetMenu->insertItem("Edge lengths",this, SLOT( elSlot() ) );
	tetMenu->setAccel( Key_E,  el_id );
	nbr_id = tetMenu->insertItem("Neighbours", this, SLOT( nbrSlot() ) );
	tetMenu->setAccel( Key_N, nbr_id );
	vgm_id = tetMenu->insertItem("Vertex gram matrices",this, SLOT( vgmSlot() ) );
	tetMenu->setAccel( SHIFT+Key_G,  vgm_id );
	tetButton->setPopup( tetMenu );
	tetButton->setPopupDelay( 0 );

	symButton = new QToolButton( this, "symmetry group" );
	symButton->setMaximumHeight( 25 );
	symButton->setTextLabel("sym");
	symButton->setUsesTextLabel( TRUE );
	symButton->setFont( f );
	symButton->setAccel(Key_S);
	grid->addWidget( symButton,0,2,0);	
	grid->setColStretch( 2, 10 );

	symMenu = new QPopupMenu( this, "symMenu" );
	symMenu->setItemEnabled( symMenu->insertItem( "Print:", -1, -1 ), FALSE );
	sa_id = symMenu->insertItem("Coloured edge action", this, SLOT(saSlot()));
	symMenu->setAccel( SHIFT+Key_C,  sa_id );	
	ca_id = symMenu->insertItem("Vertex action", this, SLOT( caSlot()));
	symMenu->setAccel( SHIFT+Key_V,  ca_id );
	mt_id = symMenu->insertItem("Multiplication table", this,  SLOT( mtSlot()));
	symMenu->setAccel( SHIFT+Key_T,  mt_id );
	symMenu->setCheckable( TRUE );
	symButton->setPopup( symMenu );
	symButton->setPopupDelay( 0 );

	gtButton = new QToolButton( this, "graph type");
	gtButton->setMaximumHeight( 25 );
	gtButton->setTextLabel("gt");
	gtButton->setUsesTextLabel( TRUE );
	gtButton->setFont( f );
	gtButton->setAccel(Key_G);
	grid->addWidget( gtButton,0,3,0);
	grid->setColStretch( 3, 10 );	

	gtMenu = new QPopupMenu(this, "gtMenu" );
	gtMenu->setItemEnabled( gtMenu->insertItem( "Options:", -1, -1 ), FALSE );
	adjm_id = gtMenu->insertItem( "Matrix",this, SLOT(adjmSlot()));
	gtMenu->setAccel( SHIFT+Key_M, adjm_id );
	adjl_id = gtMenu->insertItem("List",this,SLOT(adjlSlot())); 
	gtMenu->setAccel( SHIFT+Key_L, adjl_id );
	gtMenu->setCheckable( TRUE );
	gtMenu->setItemChecked( adjm_id, TRUE );
	gtButton->setPopup( gtMenu );
	gtButton->setPopupDelay( 0 );


        fgButton = new QToolButton( this, "fundamental group");
	fgButton->setMaximumHeight( 25 );
        fgButton->setTextLabel("fg");
        fgButton->setUsesTextLabel( TRUE );
	fgButton->setFont( f );
	fgButton->setAccel(Key_F);
        grid->addWidget( fgButton,0,4,0);
	grid->setColStretch( 4, 10 );

	fgMenu = new QPopupMenu(this, "fgMenu" );
	fgMenu->setItemEnabled( fgMenu->insertItem( "Options:", -1, -1 ), FALSE );
	simp_id = fgMenu->insertItem( "Simplify presentation",this, SLOT( simpSlot()));
	few_id = fgMenu->insertItem("Few generators",this, SLOT( fewSlot()));
	fgMenu->insertSeparator( -1 );
	fgMenu->setItemEnabled( fgMenu->insertItem( "Print:", -1, -1 ), FALSE );
	hom_id = fgMenu->insertItem("Homology", this, SLOT( homSlot() ) );
	fgMenu->setAccel( Key_H, hom_id );
	fgMenu->insertSeparator( -1 );
	fgMenu->setItemEnabled( fgMenu->insertItem( "Actions:", -1, -1 ), FALSE );
	fgMenu->setAccel( SHIFT+Key_H,
		fgMenu->insertItem( "Covering Homology", this, SLOT( coverHomSlot() ) )); 
	fgMenu->setCheckable( TRUE );
	fgMenu->setItemChecked( hom_id, FALSE );
	fgMenu->setItemChecked( simp_id, TRUE );
	fgMenu->setItemChecked( few_id, TRUE );
	fgButton->setPopup( fgMenu );
	fgButton->setPopupDelay( 0 );

        mgButton = new QToolButton( this, "matrix generators");
	mgButton->setMaximumHeight( 25 );
        mgButton->setTextLabel("mg");
        mgButton->setUsesTextLabel( TRUE );
	mgButton->setFont( f );
	mgButton->setAccel(Key_M);
        grid->addWidget( mgButton,0,5,0);
	grid->setColStretch( 5, 10 );

	mgMenu = new QPopupMenu(this, "mgMenu" );
	mgMenu->setItemEnabled( mgMenu->insertItem( "Options:", -1, -1 ), FALSE );
	o31_id = mgMenu->insertItem( "O31",this, SLOT(o31Slot()));
	mgMenu->setAccel( Key_O, o31_id );
	psl_id = mgMenu->insertItem("PSL(2,C)",this,SLOT(pslSlot()));
	mgMenu->setAccel( Key_P, psl_id );
	mgMenu->insertSeparator( -1 );
	mgMenu->setItemEnabled( mgMenu->insertItem( "Print:", -1, -1 ), FALSE );
	b_id = mgMenu->insertItem("Boundary reflections",this,SLOT(bSlot()));
	sr_id = mgMenu->insertItem("Snap relations", this, SLOT(srSlot()));
	mgMenu->setAccel( Key_B, b_id );
	det_id = mgMenu->insertItem("Det(M)",this,SLOT(detSlot()));
	tr_id = mgMenu->insertItem("tr(M)",this,SLOT(trSlot()));
	trtr_id = mgMenu->insertItem("tr(M)^2",this,SLOT(trtrSlot()));
	hol_id = mgMenu->insertItem("holonomy",this,SLOT(holSlot()));
	mgMenu->setCheckable( TRUE );
	mgMenu->setItemChecked( o31_id, TRUE );
	mgButton->setPopup( mgMenu );
	mgButton->setPopupDelay( 0 );


        lsButton = new QToolButton( this, "length spectrum");
	lsButton->setMaximumHeight( 25 );
        lsButton->setTextLabel("ls");
        lsButton->setUsesTextLabel( TRUE );
	lsButton->setFont( f );
	lsButton->setAccel(Key_L);
        grid->addWidget( lsButton,0,6,0);
	grid->setColStretch( 6, 10 );

	grid->setColStretch( 7, 20 );

	lsMenu = new QPopupMenu(this, "lsMenu" );
	lsMenu->setItemEnabled( lsMenu->insertItem( "Variables:", -1, -1 ), FALSE );
	lsMenu->insertItem("Cut off length:", this, SLOT( cutOffSlot() ) );
	cut_id = lsMenu->insertItem(tr("\t%1").arg(cut_off), -1,-1 );
	lsMenu->setItemEnabled( cut_id, FALSE );	
	lsMenu->insertItem("Tile radius:", this, SLOT( tileRadiusSlot()) ); 
	tile_id = lsMenu->insertItem(tr("\t%1").arg(tile_radius), -1,-1 );
	lsMenu->setItemEnabled( tile_id, FALSE );
	lsMenu->insertItem("Vertex epsilon:",this, SLOT( vertexEpsilonSlot()));
	vertex_id = lsMenu->insertItem(tr("\t%1").arg(vertex_epsilon), -1,-1 );
	lsMenu->setItemEnabled( vertex_id, FALSE );
	lsMenu->insertSeparator( -1 );
	lsMenu->setItemEnabled( lsMenu->insertItem( "Options:", -1, -1 ), FALSE );
	fr_id = lsMenu->insertItem("Full rigor",this, SLOT( fullRigorSlot()));
	m_id = lsMenu->insertItem("Multiplicites",this, SLOT( multiplicitiesSlot()));
	lsMenu->setCheckable( TRUE );
	lsMenu->setItemChecked( fr_id, TRUE );
	lsMenu->setItemChecked( m_id, TRUE );
	lsButton->setPopup( lsMenu );
	lsButton->setPopupDelay( 0 );

	clrButton = new QToolButton( this, "clr");
	clrButton->setMaximumHeight( 25 );
	clrButton->setIconSet( clearIcon);
	clrButton->setAccel(Key_Backspace);
	grid->addWidget( clrButton, 0,11,0);
	grid->setColStretch( 11, 0 );

	inButton = new QToolButton( this, "in" );
	inButton->setMaximumHeight( 25 );
	inButton->setTextLabel("+");
	inButton->setIconSet(zoominIcon);
	inButton->setAccel(Key_Plus);
	grid->addWidget( inButton, 0,9,0);
	grid->setColStretch( 9, 0 );

	outButton = new QToolButton( this, "out");
	outButton->setMaximumHeight( 25 );
	outButton->setTextLabel("-");
	outButton->setAccel(Key_Minus);
	outButton->setIconSet(zoomoutIcon);
	grid->addWidget( outButton,0,10,0);
	grid->setColStretch( 10, 0 );

	outputBox = new QTextEdit( this, 0 );
	outputBox->setFont( QFont( "Monaco", 12 ));
	outputBox->setReadOnly( TRUE );
	outputBox->setVScrollBarMode(QScrollView::AlwaysOn );
	outputBox->setHScrollBarMode(QScrollView::AlwaysOn );
	outputBox->setWordWrap(QTextEdit::NoWrap);
	grid->addMultiCellWidget( outputBox, 1,1,0,11,0);
	outputBox->append("Click '?' for help.");

	p = outputBox->palette();
        cg = p.active();
        cg.setColor(QColorGroup::Base, BASE );
        cg.setColor(QColorGroup::Text, TEXT );
        cg.setColor(QColorGroup::Highlight, HIGHLIGHT );       
        p.setBrush(QColorGroup::Text, TEXT );
        p.setBrush(QColorGroup::Base, BASE );
        p.setActive(cg);
        outputBox->setPalette(p);
	outputBox->zoomOut();
	outputBox->zoomOut();

        connect( triButton, SIGNAL( clicked() ),
                this, SLOT( tri() ) );
	connect( tetButton, SIGNAL( clicked() ),
		this, SLOT( tet() ) );
	connect( symButton, SIGNAL( clicked() ),
		this, SLOT( sym() ) );
        connect( fgButton, SIGNAL( clicked() ),
                this, SLOT( fg() ) );
	connect( gtButton, SIGNAL( clicked() ),
		this, SLOT( gt() ) );
        connect( mgButton, SIGNAL( clicked() ),
                this, SLOT( mg() ) );
        connect( lsButton, SIGNAL( clicked() ),
                this, SLOT( ls() ) );

	connect( clrButton, SIGNAL( clicked() ),
		this, SLOT( clr() ) );
	connect( inButton, SIGNAL( clicked()),
		outputBox, SLOT( zoomIn() ) );
	connect( outButton, SIGNAL( clicked()),
		outputBox, SLOT( zoomOut() ) );

}

Console::~Console()
{
	triMenu->~QPopupMenu();
	triButton->~QToolButton();
	tetMenu->~QPopupMenu();
	tetButton->~QToolButton();
	symButton->~QToolButton();
	symMenu->~QPopupMenu();
	gtButton->~QToolButton();
	gtMenu->~QPopupMenu();
	fgMenu->~QPopupMenu();
	fgButton->~QToolButton();
	mgMenu->~QPopupMenu();
	mgButton->~QToolButton();
	lsButton->~QToolButton();
	lsMenu->~QPopupMenu();
	clrButton->~QToolButton();
	inButton->~QToolButton();
	outButton->~QToolButton();
	outputBox->~QTextEdit();
	grid->~QGridLayout();
}

void Console::setTriangulation( Triangulation *m )
{
	manifold = m;

	outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\nundo :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );
	printTri();
}

void Console::coverHomSlot()
{
	QString         output;
	QTextStream     stream(&output,IO_WriteOnly);

	outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\ncovering homology :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );

	for( EdgeClass *edge = manifold->edge_list_begin.next;
			edge!=&manifold->edge_list_end;
			edge = edge->next )
	if (edge->is_singular &&
		edge->singular_order != floor(edge->singular_order) || edge->singular_order == 0 )
	{
		stream << "Edges must have non-zero interger singular orders."	<< endl;
		outputBox->append( output );
		return;
	}

	bool ok;	
	int res = QInputDialog::getInteger(
			"Orb", "Examine up to how many sheets?", 2, 2, 9, 1,
			&ok, this );

	if (!ok)
	{
		stream << "Action aborted."  << endl;
		outputBox->append( output );
		return;
	}

	
	if (res>4)
		if ( QMessageBox::Cancel
			== QMessageBox::warning(this,"Orb",
				"Looking for a cover of this size risks crashing Orb.",
				QMessageBox::Cancel, QMessageBox::Ignore ) )
	{
		stream << "Action aborted."  << endl;
		outputBox->append( output );
		return;
	}

	for(int i=1;i<res+1;i++)
	{
		stream << endl << i << "-fold covers:" << endl;

		RepresentationList *repList = NULL;
		repList = find_representations( manifold, i , permutation_subgroup_Sn );	

		RepresentationIntoSn *rep;
		int r = 1;
		rep = repList->list;

		while (rep!=NULL)
		{
			stream << qSetW(4) << r << "   ";

			if (rep->covering_type == unknown_cover )
				stream << qSetW(4) << 'u';
	
			if (rep->covering_type == irregular_cover )
				stream << qSetW(4) << 'i';
	
			if (rep->covering_type == regular_cover )
				stream << qSetW(4) << 'r';
	
			if (rep->covering_type == cyclic_cover )
				stream << qSetW(4) << 'c';
	
			Triangulation *cover;
	
			cover = construct_cover( manifold, rep,
				repList->num_generators, repList->num_sheets );
	
			if (cover == NULL )
			{
				QMessageBox::critical(this,"Orb", "Error building cover.",
						QMessageBox::Ok, 0,0 );
				uFatalError("coverHomSlot","console");
			}
	
			stream << qSetW(4) << cover->num_cusps;
		
			GroupPresentation *fg =fundamental_group( cover, TRUE, TRUE, TRUE );
	
			if (fg==NULL)
			{
					QMessageBox::critical(this,"Orb",
					"Error calculating fundamental group.",
						QMessageBox::Ok, 0,0);
				uFatalError("coverHomSlot","console");
			}


			AbelianGroup *g = homology_from_fundamental_group( fg );

			if (g==NULL)
			{
				QMessageBox::critical(this,"Orb", "Error calculating homology.",
						QMessageBox::Ok, 0,0);
				uFatalError("coverHomSlot","console");
			}

			int *temp;
			temp  = new int[g->num_torsion_coefficients];

			int num_zeros = 0;
			int j;

			for(j=0;j<g->num_torsion_coefficients;j++)
			{
				int index = -1, k;
				long int min = -1;
				

				for(k=0;k<g->num_torsion_coefficients;k++)
				if (g->torsion_coefficients[k] != -1 &&
					(min == -1 || g->torsion_coefficients[k] < min))
				{
					min = g->torsion_coefficients[k];
					index = k;
				}

				g->torsion_coefficients[index] = -1;
				temp[j] = min;
			}	

			for(j=0;j<g->num_torsion_coefficients && temp[j] == 0;j++)
				num_zeros++;

			stream << qSetW(4) << num_zeros << ';';

			bool first = TRUE;
			for(;j<g->num_torsion_coefficients;j++)
			{
				if (first)
				{
					stream << temp[j];
					first = FALSE;
				}
				else	stream << ',' << temp[j];
			}

			delete[] temp;
			free_abelian_group(g);
			free_group_presentation( fg );

			free_triangulation( cover );
			stream << endl;	
			rep = rep->next;
			r++;
		}

		if (repList!=NULL)
			free_representation_list( repList );
	}

	outputBox->append( output );	
}
	
void Console::cutOffSlot()
{
	bool ok;
	double res = QInputDialog::getDouble(
		"Orb", "Enter length spectrum cut off length:", cut_off, 0,
				6, 3, &ok, this );
	if ( ok )
	{
		cut_off = res;
		lsMenu->changeItem( cut_id, tr("\t%1").arg(cut_off) );
	}
}

void Console::tileRadiusSlot()
{
        bool ok;
        double res = QInputDialog::getDouble(
                "Orb", "Enter user define tile radius:", tile_radius, 0,
                                6, 3, &ok, this );
        if ( ok )
	{
                tile_radius = res;
		lsMenu->changeItem( tile_id, tr("\t%1").arg(tile_radius) );
	}
}

void Console::vertexEpsilonSlot()
{
        bool ok;
        double res = QInputDialog::getDouble(
                "Orb", "Enter vertex epsilon:", vertex_epsilon, 0,
                                0.5, 16, &ok, this );
        if ( ok )
	{
                vertex_epsilon = res;
		lsMenu->changeItem( vertex_id, tr("\t%1").arg(vertex_epsilon) );	
	}

	
}

void Console::fullRigorSlot()
{
	if (lsMenu->isItemChecked( fr_id ) )
		lsMenu->setItemChecked( fr_id, FALSE );
	else	lsMenu->setItemChecked( fr_id, TRUE );

}

void Console::multiplicitiesSlot()
{

	if (lsMenu->isItemChecked( m_id ) )
		lsMenu->setItemChecked( m_id, FALSE );
	else    lsMenu->setItemChecked( m_id, TRUE );
}

void Console::aeSlot()
{
	if (triMenu->isItemChecked( ae_id ) )
		triMenu->setItemChecked( ae_id, FALSE );
	else	triMenu->setItemChecked( ae_id, TRUE );
}

void Console::vSlot()
{
	if (triMenu->isItemChecked( v_id ) )
		triMenu->setItemChecked( v_id, FALSE );
	else	triMenu->setItemChecked( v_id, TRUE );
}

void Console::homSlot()
{
	if (fgMenu->isItemChecked( hom_id ) )
		fgMenu->setItemChecked( hom_id, FALSE );
	else	fgMenu->setItemChecked( hom_id, TRUE );	
}

void Console::simpSlot()
{
	if (fgMenu->isItemChecked( simp_id ) )
		fgMenu->setItemChecked( simp_id, FALSE );
	else    fgMenu->setItemChecked( simp_id, TRUE );
}

void Console::fewSlot()
{
	if (fgMenu->isItemChecked( few_id ) )
		fgMenu->setItemChecked( few_id, FALSE );
	else	fgMenu->setItemChecked( few_id, TRUE );
}

void Console::adjlSlot()
{
	gtMenu->setItemChecked( adjl_id, TRUE );
	gtMenu->setItemChecked( adjm_id, FALSE );
}

void Console::adjmSlot()
{
	gtMenu->setItemChecked( adjm_id, TRUE );
	gtMenu->setItemChecked( adjl_id, FALSE );
}


void Console::o31Slot()
{
	mgMenu->setItemChecked( o31_id, TRUE );
	mgMenu->setItemChecked( psl_id, FALSE );
}

void Console::pslSlot()
{
	mgMenu->setItemChecked( o31_id, FALSE );
	mgMenu->setItemChecked( psl_id, TRUE );
}

void Console::bSlot()
{
	if (mgMenu->isItemChecked( b_id ) )
		mgMenu->setItemChecked( b_id, FALSE );
	else    mgMenu->setItemChecked( b_id, TRUE );
}

void Console::srSlot()
{
	if (mgMenu->isItemChecked( sr_id ) )
		mgMenu->setItemChecked( sr_id, FALSE );
	else    mgMenu->setItemChecked( sr_id, TRUE );
}


void Console::detSlot()
{
	if (mgMenu->isItemChecked( det_id ) )
		mgMenu->setItemChecked( det_id, FALSE );
	else	mgMenu->setItemChecked( det_id, TRUE );
}

void Console::trSlot()
{
	if (mgMenu->isItemChecked( tr_id ) )
		mgMenu->setItemChecked( tr_id, FALSE );
	else	mgMenu->setItemChecked( tr_id, TRUE );
}

void Console::trtrSlot()
{
	if (mgMenu->isItemChecked( trtr_id ) )
		mgMenu->setItemChecked( trtr_id, FALSE );
	else	mgMenu->setItemChecked( trtr_id, TRUE );
}

void Console::holSlot()
{
	if (mgMenu->isItemChecked( hol_id ) )
		mgMenu->setItemChecked( hol_id, FALSE );
	else	mgMenu->setItemChecked( hol_id, TRUE );
}

void Console::clr()
{
	outputBox->clear();
}


GroupPresentation *Console::computeFundamentalGroup()
{
	bool discrete = TRUE;
	double *orders;
	orders = new double[manifold->num_singular_arcs];

	int i = 0;
	for( EdgeClass *edge = manifold->edge_list_begin.next;
			edge!=&manifold->edge_list_end;
			edge = edge->next)
	if ( edge->is_singular )
	{
		orders[i++] = edge->singular_order;
		if (floor(edge->singular_order) != edge->singular_order )
			discrete = FALSE;
        }

	if (!discrete)
	for( EdgeClass *edge = manifold->edge_list_begin.next;
			edge!=&manifold->edge_list_end;
			edge = edge->next)
	if ( edge->is_singular )
		edge->singular_order = 0;

	GroupPresentation *fg = NULL;

	fg =fundamental_group( manifold,fgMenu->isItemChecked(simp_id)
				,TRUE,fgMenu->isItemChecked(few_id));

	i = 0;
	for( EdgeClass *edge = manifold->edge_list_begin.next;
			edge!=&manifold->edge_list_end;
			edge = edge->next)
	if ( edge->is_singular )
		edge->singular_order = orders[i++];

	delete[] orders;
	return fg;
}

void	Console::fg()
{
	outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\nfg :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );

	int		i;
        QString         output;
        QTextStream     stream(&output,IO_WriteOnly);
	GroupPresentation *fg = computeFundamentalGroup();
	bool		b;

	if (fg==NULL)
	{
		outputBox->append("Error calculating fundamental group.");
		return;
	}

	stream <<  "Generators:\t{";
	for( i = 0; i<fg_get_num_generators( fg );i++)
	{
		if (i!=0)
			stream << ',';
		stream << (char) (i+'a');
	}
	stream << "}\n";

	int num_rels = fg_get_num_relations( fg );

	b = FALSE;
	stream << "Relations :";
	for( i = 0; i<num_rels;i++)
	{
		int *rel;

		if (b == FALSE)	stream << "\t";
		else		stream << "\t\t";

		b = TRUE;
		rel = fg_get_relation( fg, i );

		int j =0;

		while (rel[j]!=0)
		{
			if (rel[j]>0) stream << (char) (rel[j]-1+'a');
			else          stream << (char) ('A'-rel[j]-1);

			j++;
		}
		stream << "\n";

		fg_free_relation( rel );
	}
	if (!b) stream << "\n";
	b = FALSE;

	num_rels = fg_get_num_parabolic_relations( fg );
	stream << "Parabolics:";
	for( i = 0; i<num_rels;i++)
	if ( fg_get_parabolic_relation( fg, i ) )
	{
		int *rel;
		if (b == FALSE)	stream << "\t";
		else		stream << "\t\t";

		b = TRUE;
		rel = fg_get_parabolic_relation( fg, i );

		int j =0;

		while (rel[j]!=0)
		{
			if (rel[j]>0) stream << (char) (rel[j]-1+'a');
			else          stream << (char) ('A'-rel[j]-1);

		j++;
		}
		stream << "\n";

		fg_free_relation( rel );
	}
	if (!b) stream << "\n";
        b = FALSE;

	num_rels = fg_get_num_cusps( fg );
	stream << "Merdians  :";
	for( i = 0; i<num_rels;i++)
	{
		int *rel;
		if (b == FALSE) stream << "\t";
		else            stream << "\t\t";

		b = TRUE;
		rel = fg_get_meridian( fg, i );

		int j =0;

		while (rel[j]!=0)
		{
			if (rel[j]>0) stream << (char) (rel[j]-1+'a');
			else          stream << (char) ('A'-rel[j]-1);

			j++;
		}
		stream << "\n";

		fg_free_relation( rel );
	}
	if (!b) stream << "\n";

	b = FALSE;

	num_rels = fg_get_num_cusps( fg );
	stream << "Longitudes:";
	for( i = 0; i<num_rels;i++)
	{
		int *rel;
		if (b == FALSE) stream << "\t";
		else	stream << "\t\t";

		b = TRUE;
		rel = fg_get_longitude( fg, i );

		int j =0;

		while (rel[j]!=0)
		{
			if (rel[j]>0) stream << (char) (rel[j]-1+'a');
			else	stream << (char) ('A'-rel[j]-1);

			j++;
		}
		stream << "\n";

		fg_free_relation( rel );
	}
	if (!b) stream << "\n";


	if (fgMenu->isItemChecked( hom_id ) )
	{
			AbelianGroup *g = homology_from_fundamental_group( fg );

			stream << "Homology  :\t";

			if (g==NULL)
				stream << "error calculating homology";
			else	for(i=0;i<g->num_torsion_coefficients;i++)
			{
				if (i!=0)
					stream << " + ";
				stream << 'Z';

				if (g->torsion_coefficients[i]>0)
					stream << '/' << g->torsion_coefficients[i];
			}

			stream << endl;

			free_abelian_group(g);
	}

	outputBox->append( output );

	free_group_presentation( fg );
}

void Console::gt()
{
	Cusp		*cusp;
	EdgeClass	*edge;
	Tetrahedron	*tet;
	int i,j, index;

	outputBox->removeSelection();
	outputBox->setColor( COMMAND );
	outputBox->setUnderline( TRUE );
	outputBox->setItalic( TRUE );
	outputBox->append("\ngt :");
	outputBox->setColor( TEXT );
	outputBox->setUnderline( FALSE );
	outputBox->setItalic( FALSE );

	QString	output;
	QTextStream     stream(&output,IO_WriteOnly);

        stream << endl;

	if (gtMenu->isItemChecked(adjl_id))
	{
		for ( i = 0; i < manifold->num_cusps; i++ )
/*
        	for( cusp = manifold->cusp_list_begin.next;
        		cusp!=&manifold->cusp_list_end;
        		cusp = cusp->next )
        	if (!cusp->is_finite)
*/
        	{
			cusp = find_cusp( manifold, i );
        		QString s;

        		stream << ' ' << qSetW(2) << cusp->index + 1;

        		stream << ' ' << qSetW(2) << cusp->euler_characteristic;

        		s.sprintf(" %6.3f", cusp->orbifold_euler_characteristic );
        		stream << qSetW(7) << s;

        		for( j = 0; j < cusp->num_cone_points; j++ )
        			stream << ' ' << qSetW(2) << cusp->cone_points[j] + 1;

        		stream << endl;
        	}
	}
	else
	{
                int **adj = new int*[manifold->num_cusps];

                for( i = 0; i<manifold->num_cusps; i++)
                        adj[i] = new int[manifold->num_cusps];


                for( i = 0; i<manifold->num_cusps; i++)
                        for(j = 0; j<manifold->num_cusps; j++ )
                                adj[i][j] = 0;

                for(    edge = manifold->edge_list_begin.next;
                        edge!=&manifold->edge_list_end;
                        edge = edge->next )
                if (    edge->is_singular )
                {

                        tet = edge->incident_tet;
                        index = edge->incident_edge_index;

                        i = tet->cusp[one_vertex_at_edge[index]]->index;
                        j = tet->cusp[other_vertex_at_edge[index]]->index;
                        if ( i < 0 || i >= manifold->num_cusps ||
                             j < 0 || j >= manifold->num_cusps )
                                uFatalError("gt","console");

                        if (i==j)
                                adj[i][i]++;
                        else
                        {
                                adj[i][j]++;
                                adj[j][i]++;
                        }
                }

                stream << "   ";
                for(i=0;i<manifold->num_cusps;i++)
                        stream << ' ' << qSetW(2) << i + 1;
                stream << endl;

                for(i=0;i<manifold->num_cusps;i++)
                {
                        stream << ' ' << qSetW(2) << i + 1;

                        for( j = 0; j<manifold->num_cusps;j++)
                        {
                                QString s;
                                s.sprintf("%2d", adj[i][j] );
                                stream << ' ' << qSetW(2) << s;
                        }
                        stream << endl;
                }
                stream << endl;

                for(i=0;i<manifold->num_cusps;i++)
                        delete[] adj[i];

                delete[] adj;
	}

	outputBox->append(output);
}



void Console::mg()
{
	outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\nmg :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );


	QString		output;
	QTextStream	stream(&output,IO_WriteOnly);

	if (    manifold->solution_type[complete] == not_attempted )  
	{
		outputBox->append("Compute structure first.");
		return;
	}

	if (    manifold->solution_type[complete] == step_failed ||
		manifold->solution_type[complete] == other_solution ||
		manifold->solution_type[complete] == no_solution )
	{
		outputBox->append("Invalid structure.\n");
		return;
	}

	GroupPresentation *fg = computeFundamentalGroup();


	int boundary = 0;
	if (mgMenu->isItemChecked( b_id ))
	for ( Cusp	*cusp = manifold->cusp_list_begin.next;
			 cusp!=&manifold->cusp_list_end;
			 cusp = cusp->next )
	if ( cusp->inner_product[ultimate] > 0.0001 )
		boundary++;

	stream << "\% Generators" << endl;
	stream << fg_get_num_generators(fg) + boundary << endl;
	stream << endl;

	for(int i=0;i<fg_get_num_generators(fg);i++)
	{
		MoebiusTransformation A;
		O31Matrix B;

		for(int j=0;j<4;j++)
			for(int k=0;k<4;k++)
				B[j][k] = get_matrix_entry(fg,i,j,k);

		if (mgMenu->isItemChecked(o31_id))
		{
			for(int j=0;j<4;j++)
			{
				for(int k=0;k<4;k++)
				{
					QString entry;
					entry.sprintf("%25.16f", B[j][k]);
					stream << qSetW(25) << entry;
				}
				stream << endl;
			}
			stream << endl;

			if (mgMenu->isItemChecked( det_id ))
			{
				QString det;
				det.sprintf("Det : %25.16f", gl4R_determinant( B ) );
				stream << det << endl;
			}

			if (mgMenu->isItemChecked( tr_id ))
			{
				QString tr;
				double trace = B[0][0]+B[1][1]+B[2][2]+B[3][3];
				tr.sprintf("Tr  : %25.16f", trace );
				stream << tr << endl;
			}

			if (mgMenu->isItemChecked( trtr_id ))
			{
				QString trtr;
				double trace = B[0][0]+B[1][1]+B[2][2]+B[3][3];
				trtr.sprintf("Tr^2: %25.16f", trace * trace );
				stream << trtr << endl;
			}

			if (mgMenu->isItemChecked( hol_id ))
			{
				Complex z = complex_length_o31(B);
				QString hol;
				hol.sprintf("Holonomy: %21.16f %c %18.16f i",
						z.real,
						(ABS(z.imag)==z.imag) ? '+' : '-',
						ABS(z.imag)  );
				stream << hol << endl;
			}

			if (	mgMenu->isItemChecked( det_id ) ||
				mgMenu->isItemChecked( tr_id )  ||
				mgMenu->isItemChecked( trtr_id )||
				mgMenu->isItemChecked( hol_id ) )
				stream << endl << endl;
		}
		else
		{
			O31_to_Moebius( B, &A );

			for(int j=0;j<2;j++)
			{
				for(int k=0;k<2;k++)
				{
					QString entry;
					entry.sprintf("%18.16f   %c %18.16f i",
						A.matrix[j][k].real,
						(A.matrix[j][k].imag==ABS(A.matrix[j][k].imag)) ? '+' : '-',
					 	ABS(A.matrix[j][k].imag) );
					stream << qSetW(48) << entry;
				}
				stream << endl;
			}
			stream << endl;

			if (mgMenu->isItemChecked( det_id ))
			{
				QString det;
				Complex d = sl2c_determinant(A.matrix);
				det.sprintf("Det :  %24.16f  %c %18.16f i",
					d.real,
					(d.imag==ABS(d.imag)) ? '+' : '-',
					ABS( d.imag ) );
				stream << det << endl;
			}

			if (mgMenu->isItemChecked( tr_id ))
			{
				QString tr;
				Complex trace = complex_plus(A.matrix[0][0], A.matrix[1][1]);
				tr.sprintf("Tr  :  %24.16f  %c %18.16f i",
					trace.real,
					(trace.imag==ABS(trace.imag)) ? '+' : '-',
					ABS(trace.imag) );

				stream << tr << endl;
                        }

			if (mgMenu->isItemChecked( trtr_id ))
			{
				QString trtr;
				Complex trace = complex_plus(A.matrix[0][0], A.matrix[1][1]);
				Complex trace_squared = complex_mult(trace,trace);
				trtr.sprintf("Tr^2:  %24.16f  %c %18.16f i",
					trace_squared.real,
					(trace_squared.imag==ABS(trace_squared.imag)) ? '+' : '-',
					ABS(trace_squared.imag) );

				stream << trtr << endl;
                        }

			if (mgMenu->isItemChecked( hol_id ))
			{
				Complex z = complex_length_mt(&A);
				QString hol;
				hol.sprintf("Holonomy: %21.16f  %c %18.16f i",
						z.real,
						(ABS(z.imag)==z.imag) ? '+' : '-',
						ABS(z.imag)  );
				stream << hol << endl;
                        }


			if (	mgMenu->isItemChecked( det_id ) ||
				mgMenu->isItemChecked( tr_id )  ||
				mgMenu->isItemChecked( trtr_id )||
				mgMenu->isItemChecked( hol_id ) )
				stream << endl << endl;
		}
	}

	if (mgMenu->isItemChecked( b_id ))
	for ( Cusp	*cusp = manifold->cusp_list_begin.next;
			 cusp!=&manifold->cusp_list_end;
			 cusp = cusp->next )
	if ( cusp->inner_product[ultimate] > 0.0001 )
	{
		bool found_boundary_gen = FALSE;
		Tetrahedron *tet;

		for(	tet = manifold->tet_list_begin.next;
			tet!= &manifold->tet_list_end &&
				!found_boundary_gen;
			tet = tet->next )
			for(int i = 0; i < 4 && !found_boundary_gen; i++)
			if (tet->cusp[i] == cusp)
			{
				found_boundary_gen = TRUE;
				O31Matrix       B;
				MoebiusTransformation A;

				compute_reflection( i, B, tet->basis );

				if (mgMenu->isItemChecked(o31_id))
				{
					for(int j=0;j<4;j++)
					{
						for(int k=0;k<4;k++)
						{
							QString entry;
							entry.sprintf("%25.16f", B[j][k]);
							stream << qSetW(25) << entry;
						}
						stream << endl;
					}
					stream << endl;

					if (mgMenu->isItemChecked( det_id ))
					{
						QString det;
						det.sprintf("Det : %25.16f", gl4R_determinant( B ) );
						stream << det << endl;
					}
	
					if (mgMenu->isItemChecked( tr_id ))
					{
						QString tr;
						double trace = B[0][0]+B[1][1]+B[2][2]+B[3][3];
						tr.sprintf("Tr  : %25.16f", trace );
						stream << tr << endl;
					}
	
					if (mgMenu->isItemChecked( trtr_id ))
					{
						QString trtr;
						double trace = B[0][0]+B[1][1]+B[2][2]+B[3][3];
						trtr.sprintf("Tr^2: %25.16f", trace * trace );
						stream << trtr << endl;
					}
	
					if (mgMenu->isItemChecked( hol_id ))
					{
						Complex z = complex_length_o31(B);
						QString hol;
						hol.sprintf("Holonomy: %21.16f %c %18.16f i",
						z.real,
						(ABS(z.imag)==z.imag) ? '+' : '-',
						ABS(z.imag)  );
						stream << hol << endl;
					}

					if (    mgMenu->isItemChecked( det_id ) ||
						mgMenu->isItemChecked( tr_id )  ||
						mgMenu->isItemChecked( trtr_id )||
						mgMenu->isItemChecked( hol_id ) )
						stream << endl << endl;
				}
				else
				{
					O31_to_Moebius( B, &A );

					for(int j=0;j<2;j++)
					{
						for(int k=0;k<2;k++)
						{
							QString entry;
							entry.sprintf("%18.16f   %c %18.16f i",
							A.matrix[j][k].real,
							(A.matrix[j][k].imag==ABS(A.matrix[j][k].imag)) ? '+' : '-',
							ABS(A.matrix[j][k].imag) );
							stream << qSetW(48) << entry;
						}
						stream << endl;
					}
					stream << endl;
	
					if (mgMenu->isItemChecked( det_id ))
					{
						QString det;
						Complex d = sl2c_determinant(A.matrix);
						det.sprintf("Det :  %24.16f  %c %18.16f i",
						d.real,
						(d.imag==ABS(d.imag)) ? '+' : '-',
						ABS( d.imag ) );
						stream << det << endl;
					}
	
					if (mgMenu->isItemChecked( tr_id ))
					{
						QString tr;
						Complex trace = complex_plus(A.matrix[0][0], A.matrix[1][1]);
						tr.sprintf("Tr  :  %24.16f  %c %18.16f i",
						trace.real,
						(trace.imag==ABS(trace.imag)) ? '+' : '-',
						ABS(trace.imag) );
	
						stream << tr << endl;
					}
	
					if (mgMenu->isItemChecked( trtr_id ))
					{
						QString trtr;
						Complex trace = complex_plus(A.matrix[0][0], A.matrix[1][1]);
						Complex trace_squared = complex_mult(trace,trace);
						trtr.sprintf("Tr^2:  %24.16f  %c %18.16f i",
						trace_squared.real,
							(trace_squared.imag==ABS(trace_squared.imag)) ? '+' : '-',
						ABS(trace_squared.imag) );
	
						stream << trtr << endl;
					}
	
					if (mgMenu->isItemChecked( hol_id ))
						{
						Complex z = complex_length_mt(&A);
						QString hol;
						hol.sprintf("Holonomy: %21.16f  %c %18.16f i",
								z.real,
									(ABS(z.imag)==z.imag) ? '+' : '-',
								ABS(z.imag)  );
						stream << hol << endl;
					}
	
					if (    mgMenu->isItemChecked( det_id ) ||
						mgMenu->isItemChecked( tr_id )  ||
						mgMenu->isItemChecked( trtr_id )||
						mgMenu->isItemChecked( hol_id ) )
						stream << endl << endl;
	
				}
			}
	}
	
	// Print out the fundament group relations for Snap.
	if (mgMenu->isItemChecked( sr_id ))
	{
		int num_rels = fg_get_num_relations( fg );
		
		stream << "% Relations\n";
		stream << num_rels;
		stream << "\n";
		
		for( int i = 0; i<num_rels;i++)
		{
			int *rel = fg_get_relation( fg, i );

			int j =0;
			while (rel[j]!=0)
			{
				if (rel[j]>0)	stream << (char) (rel[j]-1+'a');
				else			stream << (char) ('A'-rel[j]-1);

				j++;
			}
			stream << "\n";

			fg_free_relation( rel );
		}
		
		num_rels = fg_get_num_parabolic_relations( fg );
		int num_cusps = fg_get_num_cusps( fg );
		stream << "% Parabolics\n";
		stream << num_rels + 2 * num_cusps << "\n";
		
		for( int i = 0; i<num_rels;i++)
		if ( fg_get_parabolic_relation( fg, i ) )
		{
			int *rel;
			rel = fg_get_parabolic_relation( fg, i );

			int j =0;

			while (rel[j]!=0)
			{
				if (rel[j]>0) stream << (char) (rel[j]-1+'a');
				else          stream << (char) ('A'-rel[j]-1);
			j++;
			}
			stream << "\n";
			fg_free_relation( rel );
		}
		
		for( int i = 0; i<num_cusps;i++)
		{
			int *rel;
			rel = fg_get_meridian( fg, i );
			int j =0;
			while (rel[j]!=0)
			{
				if (rel[j]>0) stream << (char) (rel[j]-1+'a');
				else          stream << (char) ('A'-rel[j]-1);
				j++;
			}
			stream << "\n";
			fg_free_relation( rel );
			
			rel = fg_get_longitude( fg, i );
			j =0;
			while (rel[j]!=0)
			{
				if (rel[j]>0) stream << (char) (rel[j]-1+'a');
				else	stream << (char) ('A'-rel[j]-1);
				j++;
			}
			stream << "\n";
			fg_free_relation( rel );
		}
	}

	outputBox->append(output);

	free_group_presentation( fg );
}

void Console::ls()
{
	outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\nls :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );

	QString		output;
	QTextStream	stream(&output,IO_WriteOnly);
	WEPolyhedron	*domain = NULL;
	MultiLength	*spectrum;
	int		num_lengths;

	if (	manifold->solution_type[complete] == not_attempted )
	{
		outputBox->append("Compute structure first.");
		return;
	}

	if (	manifold->solution_type[complete] == step_failed ||
		manifold->solution_type[complete] == other_solution ||
		manifold->solution_type[complete] == no_solution )
	{
		outputBox->append("Invalid structure.\n");
		return;
	} 	

	GroupPresentation *fg = computeFundamentalGroup();

	if (fg == NULL)
	{
		outputBox->append("Error computing fundamental group.\n");
		return;
	}

        int boundary = 0;
        for ( Cusp      *cusp = manifold->cusp_list_begin.next;
                         cusp!=&manifold->cusp_list_end;
                         cusp = cusp->next )
        if ( cusp->inner_product[ultimate] > 0.0001 )
                boundary++;

	O31Matrix *gens = new O31Matrix[fg_get_num_generators(fg)+boundary];

        for(int i=0;i<fg_get_num_generators(fg);i++)
                for(int j=0;j<4;j++)
                        for(int k=0;k<4;k++)
                                gens[i][j][k] = get_matrix_entry(fg,i,j,k);


        for ( Cusp      *cusp = manifold->cusp_list_begin.next;
                         cusp!=&manifold->cusp_list_end;
                         cusp = cusp->next )
        if ( cusp->inner_product[ultimate] > 0.0001 )
        {
                bool found_boundary_gen = FALSE;
                Tetrahedron *tet;

                for(    tet = manifold->tet_list_begin.next;
                        tet!= &manifold->tet_list_end &&
                                !found_boundary_gen;
                        tet = tet->next )
                        for(int i = 0; i < 4 && !found_boundary_gen; i++)
                        if (tet->cusp[i] == cusp)
                        {
                                found_boundary_gen = TRUE;
                                O31Matrix       reflection;

                                compute_reflection( i, reflection, tet->basis );

                                for(int j = 0;j < 4; j++)
                                        for(int k = 0;k < 4; k++)
                                                gens[i+fg_get_num_generators(fg)][j][k]
									=reflection[j][k];
                        }
        }

	domain = Dirichlet_from_generators( gens,
					fg_get_num_generators(fg)+boundary,
					vertex_epsilon, Dirichlet_keep_going,
					TRUE );

	free_group_presentation( fg );

	if (domain == NULL)
	{
		outputBox->append("Error building Dirichlet domain.");
		return;
	}

	length_spectrum(domain,cut_off,lsMenu->isItemChecked(fr_id),
			lsMenu->isItemChecked(m_id),tile_radius,
			&spectrum,&num_lengths );

	stream << "multi\t"<< "length\n";

	for(int i = 0; i<num_lengths;i++)
	{
		stream << qSetW(5) << spectrum[i].multiplicity;

		QString num;
		num.sprintf("%04.9f", spectrum[i].length.real );
		stream << qSetW(15) << num;

		if (spectrum[i].length.imag >=0)
			stream << "   + ";
		else	stream << "   - ";

		num.sprintf("%04.9f i", fabs(spectrum[i].length.imag) );
		stream << qSetW(15) << num << endl;
	}

	free_Dirichlet_domain(domain);
	free_length_spectrum(spectrum);
	delete[] gens;
	outputBox->append(output);
}

void Console::can()
{
	outputBox->removeSelection();
	outputBox->setColor( COMMAND );
	outputBox->setUnderline( TRUE );
	outputBox->setItalic( TRUE );
	outputBox->append("\ncan :");
	outputBox->setColor( TEXT );
	outputBox->setUnderline( FALSE );
	outputBox->setItalic( FALSE );

	emit retriangulation();

	if ( canonize( manifold ) == func_failed )
		QMessageBox::information( this, "Orb", "Function canonize() failed.",  QMessageBox::Ok );
	else	printCanonizeInfo();

	emit triangulationChanged();
	printTri();
}

void Console::printCanonizeInfo()
{
        QString         output;
        QTextStream     stream(&output,IO_WriteOnly);

	bool		subdivided = FALSE;
	int		numPolyhedra = 0;
	Cusp		*cusp;
	Tetrahedron	*tet;
	EdgeClass	*edge;
	EdgeIndex	index;

	for(	cusp = manifold->cusp_list_begin.next;
		cusp!=&manifold->cusp_list_end;
		cusp = cusp->next )
	if (	cusp->is_finite )
	{
		subdivided = TRUE;
		numPolyhedra++;
	}

	if (!subdivided)
	{
		if (manifold->num_tetrahedra==1)
			stream << "Canonical Cell Decomposition consists of a single tetrahedron.\n\n";
		else	stream << "Canonical Cell Decomposition consists of " << manifold->num_tetrahedra << " tetrahedra.\n\n";

		outputBox->append(output);

		return;
	}


	if (numPolyhedra==1)
		stream << "Canonical Cell Decomposition consists of a single polyhedron.";
	else	stream << "Canonical Cell Decomposition consists of "<< numPolyhedra << " polyhedra.";

	numPolyhedra = 1;

        for(    cusp = manifold->cusp_list_begin.next;
                cusp!=&manifold->cusp_list_end;
                cusp = cusp->next )
        if (    cusp->is_finite )
        {
		stream << "\npoly " << numPolyhedra++ << " face orders: ";

		for(	edge = manifold->edge_list_begin.next;
			edge!=&manifold->edge_list_end;
			edge = edge->next )
		{
			tet = edge->incident_tet;
			index = edge->incident_edge_index;	

			if (tet->cusp[one_vertex_at_edge[index]]->is_finite && tet->cusp[other_vertex_at_edge[index]]->is_finite)
			{
				if (tet->cusp[one_vertex_at_edge[index]]==cusp)
					stream << qSetW(4) << edge->order;

				if (tet->cusp[other_vertex_at_edge[index]]==cusp)
					stream << qSetW(4) << edge->order;
			}
		}
        }

	stream << "\nThe decomposition was canonically subdivided.\n\n";
	outputBox->append(output);
}


void Console::rnd()
{
	outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\nrnd :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );

	emit retriangulation();
	remove_finite_vertices(manifold);
	randomize_triangulation(manifold);
	emit triangulationChanged();
	printTri();
}

void Console::sim()
{
	outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\nsim :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );

	emit retriangulation();
	basic_simplification(manifold);
	emit triangulationChanged();
	printTri();
}

void Console::rem()
{
        outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\nrem :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );

	Cusp *cusp;
	Boolean has_finite_vertices = FALSE;
	for( cusp = manifold->cusp_list_begin.next;
		cusp!=&manifold->cusp_list_end;
		cusp = cusp->next )
	if ( cusp->euler_characteristic== 2 && cusp->num_cone_points == 0 )
		has_finite_vertices = TRUE;

	if (!has_finite_vertices)
	{
		outputBox->append("\nTriangulation does not contain finite vertices.");
		return;
	}

        emit retriangulation();
        remove_finite_vertices( manifold );
        emit triangulationChanged();
        printTri();
}


void Console::daSlot()
{
	if (tetMenu->isItemChecked( da_id ) )
		tetMenu->setItemChecked( da_id, FALSE );
	else	tetMenu->setItemChecked( da_id, TRUE );
}

void Console::elSlot()
{
	if (tetMenu->isItemChecked( el_id ) )
		tetMenu->setItemChecked( el_id, FALSE );
	else	tetMenu->setItemChecked( el_id, TRUE );
}

void Console::vgmSlot()
{
	if (tetMenu->isItemChecked( vgm_id ) )
		tetMenu->setItemChecked( vgm_id, FALSE );
	else	tetMenu->setItemChecked( vgm_id, TRUE );
}

void Console::nbrSlot()
{
	if (tetMenu->isItemChecked( nbr_id ) )
		tetMenu->setItemChecked( nbr_id, FALSE );
	else	tetMenu->setItemChecked( nbr_id, TRUE );
}

void Console::saSlot()
{
	if (symMenu->isItemChecked( sa_id ) )
		symMenu->setItemChecked( sa_id, FALSE );
	else	symMenu->setItemChecked( sa_id, TRUE );
}

void Console::caSlot()
{
	if (symMenu->isItemChecked( ca_id ) )
		symMenu->setItemChecked( ca_id, FALSE );
	else    symMenu->setItemChecked( ca_id, TRUE );
}

void Console::mtSlot()
{
	if (symMenu->isItemChecked( mt_id ) )
		symMenu->setItemChecked( mt_id, FALSE );
	else    symMenu->setItemChecked( mt_id, TRUE );
}

void Console::tet()
{
	outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\ntet :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );

	Tetrahedron	*tet;
	double		total = 0;
	int		i,
			j,
			index;
	QString		output,
			edge;
	QTextStream	stream(&output,IO_WriteOnly);

        for(    tet = manifold->tet_list_begin.next, index = 0;
                tet!=&manifold->tet_list_end;
                tet = tet->next, index++ )
		tet->index = index;

	for(    tet = manifold->tet_list_begin.next;
		tet!=&manifold->tet_list_end;
		tet = tet->next )
	{
		double angles[6], tet_vol;
		Boolean ok = TRUE;

		for (i=0;i<6;i++)
			angles[i] = tet->dihedral_angle[ultimate][i];

		tet_vol = (tet->orientation_parameter[ultimate] >= 0)
			?  tetrahedron_volume(angles, &ok)
			: -tetrahedron_volume(angles, &ok);

		total += tet_vol;


		QString Vol;
		Vol.sprintf("%25.16f%c", tet_vol, ok ? ' ' : '*' );

		stream << endl << "tet" << qSetW(3) << tet->index + 1;
		if (	tetMenu->isItemChecked( vgm_id ) ||
			tetMenu->isItemChecked( da_id) ||
			tetMenu->isItemChecked( nbr_id ) ||
			tetMenu->isItemChecked( el_id) )
		{
			stream << "                                                             Volume:";
			stream << qSetW(26) << Vol << endl << endl; 
		}
		else	stream << "\t\tVolume: " << qSetW(26) << Vol << endl;

		if (	tetMenu->isItemChecked( da_id) ||
			tetMenu->isItemChecked( el_id) )
		{
			stream << "    ";

			for(i=0;i<6;i++)
			{
				edge.sprintf("%c%c", NL(one_face_at_edge[i]), NL(other_face_at_edge[i]) );
				stream << qSetW(16) << edge;
			}
			stream << endl;
		
			if (tetMenu->isItemChecked( da_id))
			{	
				stream << "da :";
				for( i=0;i<6;i++)
				{
					QString angle;
					angle.sprintf("%16.12f",tet->dihedral_angle[ultimate][i]);
					stream << qSetW(16) << angle;
				}
				stream << endl;
			}

			if (tetMenu->isItemChecked( el_id))
			{
				stream << "el :";
				for( i=0;i<6;i++)
				{
					QString length;
					length.sprintf("%16.12f",edge_length( tet, i) );
					stream << qSetW(16) << length;
				}
				stream << endl;
                        }
		}

                if (tetMenu->isItemChecked( nbr_id))
                {
                         stream << "nbr:";
                         for( i=0;i<4;i++)
                         {
                                QString length;

				if (i==0)
					length.sprintf("%11d    (",tet->neighbor[i]->index + 1 );
				else	length.sprintf("%15d    (",tet->neighbor[i]->index + 1 );
                                stream << length;

				for(j=0;j<4;j++)
				{
					char c = NL( EVALUATE(tet->gluing[i],j) );
					stream << c;
				}
				stream << ')';
                         }
                         stream << endl;
                }


		if (tetMenu->isItemChecked( vgm_id))
		{
			stream << "vgm:\n";

			for(i=0;i<4;i++)
			{
				for(int j=0;j<4;j++)
				if (i==j)
				{
					QString entry;
					entry.sprintf("%25.16f",
						tet->cusp[i]->inner_product[ultimate]);
						stream << qSetW(25) << entry;
				}
				else
				{
					QString entry;
					entry.sprintf("%25.16f",
						tet->edge_class[edge_between_vertices[i][j]]
							->inner_product[ultimate] );
				stream << qSetW(25) << entry;
				}
				stream << endl;
			}
		}

		if (	tetMenu->isItemChecked( da_id) ||
			tetMenu->isItemChecked( el_id) ||
			tetMenu->isItemChecked( vgm_id) )
		{
			stream << "---------------------------------------------------";
			stream << "-------------------------------------------------";
		}
	}

        QString s;
        s.sprintf("%25.16f", total );

        stream << endl << "Total volume : " << qSetW(26) << s << endl << endl;;

	outputBox->append(output);
}

void Console::sym()
{
        outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\nsym :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );

	QString		output;
	QTextStream     stream(&output,IO_WriteOnly);
	SymmetryGroup	*symmetry_group_of_manifold = NULL,
			*symmetry_group_of_link = NULL;
	Triangulation	*copy = NULL;
	int		i,j;

	copy_triangulation( manifold, &copy );

	stream << endl;

	if (compute_cusped_symmetry_group( manifold,
		&symmetry_group_of_manifold,
		&symmetry_group_of_link ) == func_OK)
	{
		print_group( stream, symmetry_group_of_manifold );

		stream << endl;

		Boolean chiral = TRUE;

                for(i=0;i<symmetry_group_of_manifold->order;i++)
                {
                                Isometry *isom = symmetry_group_of_manifold->symmetry_list->isometry[i];
                                if (manifold->orientability==oriented_manifold)
                                        if (parity[isom->tet_map[0]] != 0)
						chiral = FALSE;
                }

		if (chiral==TRUE)	stream << "chiral" << endl;
		else			stream << "amphicheiral" << endl;	

		if (symMenu->isItemChecked( mt_id ))
		{
			stream << endl << "mt :" << endl;

			for(i=0;i<symmetry_group_of_manifold->order;i++)
			{
				for(j=0;j<symmetry_group_of_manifold->order;j++)
					stream << qSetW(3) << symmetry_group_product(symmetry_group_of_manifold,i,j);
				stream << endl;
			}
		}

		if (symMenu->isItemChecked( ca_id ))
		{
			stream << endl << "va :"<< endl;
			for(i=0;i<symmetry_group_of_manifold->order;i++)
			{
				Isometry *isom = symmetry_group_of_manifold->symmetry_list->isometry[i];
				stream << qSetW(3) << i << ":";

				if (manifold->orientability==oriented_manifold)
				{
					if (parity[isom->tet_map[0]] == 0)
						stream << "   p";
					else	stream << "   r";
				}
			
				for(j=0;j<isom->num_cusps;j++)
					stream << qSetW(3) << isom->cusp_image[j] + 1;
				stream << endl;
			}
		}

                if (symMenu->isItemChecked( sa_id ))
                {
                        stream << endl << "ca :"<< endl;
                        for(i=0;i<symmetry_group_of_manifold->order;i++)
                        {
                                Isometry *isom = symmetry_group_of_manifold->symmetry_list->isometry[i];
                                stream << qSetW(3) << i << ":";

				if (manifold->orientability==oriented_manifold)
				{
					if (parity[isom->tet_map[0]] == 0)
						stream << "   p";
					else    stream << "   r";
				}

                                for(j=0;j<isom->num_singular_arcs;j++)
                                        stream << qSetW(4) << isom->singular_image[j];
                                stream << endl;
                        }
                }

		stream << endl;

		free_symmetry_group(symmetry_group_of_manifold);
		free_symmetry_group(symmetry_group_of_link);
	}
	else stream << "compute_cusped_symmetry_group() failed." << endl;
	
	outputBox->append(output);
	free_triangulation( copy );
}

static void print_group( QTextStream &stream, SymmetryGroup *sg )
{
        if (sg==NULL)
		uFatalError("print_group","console");

        if (sg->order==1)
                stream << "trivial ";

        else if (sg->is_dihedral && sg->order > 2 )
                stream << "D" << sg->order/2 << " ";

        else if (sg->is_abelian)
                print_abelian_group( stream, sg->abelian_description );

        else if (sg->is_polyhedral)
                print_polyhedral_group( stream, sg );

        else if (sg->is_S5 )
                stream <<"S5 ";

        else if (sg->is_direct_product)
        {
                print_group(stream,sg->factor[0]);
                stream << " x ";
                print_group(stream,sg->factor[1]);
                return;
        }

        else    stream << "nonabelian group of order " <<  sg->order;
}

static	void	print_abelian_group( QTextStream &stream, AbelianGroup *ag )
{
        int             i;

        /*
         *      Handle the case of a trivial group specially.
         */

        if (ag->num_torsion_coefficients == 0) {
                stream <<"trivial ";
                return;
        }

        for (i = 0; i < ag->num_torsion_coefficients; i++) {

                if (i != 0)
                        stream << "+ ";

                stream << "Z";

                if (ag->torsion_coefficients[i] != 0) {
                        stream << ag->torsion_coefficients[i];
                }
		stream << " ";
        }
}

static	void	print_polyhedral_group( QTextStream &stream, SymmetryGroup *sg )
{
        Boolean         is_binary_group;
        int             p,
                        q,
                        r;


        if (sg->is_polyhedral == FALSE)
		uFatalError("print_polyhedral_group","console");

        is_binary_group = sg->is_binary_group;
        p = sg->p;
        q = sg->q;
        r = sg->r;

        /*
         *      In principle there's nothing wrong with p != 2 (just so long as q or r is 2),
         *      but the SnapPea kernel should generate the (p,q,r) in ascending order.
         */
        if (p != 2)
                uFatalError("print_polyhedral_group", "console");

        switch (q) {

                case 2:

                        /*
                         *      We've got a <2,2,n> binary dihedral group.
                         */

                        if (is_binary_group == FALSE)
				uFatalError("print_polyhedral_group", "console");

                        stream << "binary dihedral group <2,2," << r << "> ";

                        break;
                case 3:

                        /*
                         *      We've got a (binary) tetrahedral, octahedral or icosahedral group.
                         */

                        if (is_binary_group == TRUE)
                                stream << "binary ";

                        switch (r) {

                                case 3:
                                        stream << "tetrahedral ";
                                        break;

                                case 4:
                                        stream << "octahedral ";
                                        break;

                                case 5:
                                        stream << "icosahedral ";
                                        break;

                                default:
					uFatalError("print_polyhedral_group", "console");
                        }

                        stream << "group ";

                        break;

                default:
			uFatalError("print_polyhedral_group", "console");
        }
}


void Console::tri()
{
	outputBox->removeSelection();
        outputBox->setColor( COMMAND );
        outputBox->setUnderline( TRUE );
        outputBox->setItalic( TRUE );
        outputBox->append("\ntri :");
        outputBox->setColor( TEXT );
        outputBox->setUnderline( FALSE );
        outputBox->setItalic( FALSE );
	printTri();
}

void Console::printTri()
{
	QString		output;
	QTextStream	stream(&output,IO_WriteOnly);

	saveTriangulation( stream, triMenu->isItemChecked( ae_id ),
			triMenu->isItemChecked( v_id ), FALSE );

	outputBox->append(output);
}


void Console::saveTriangulation( QTextStream &stream, bool ae, bool ex, bool curves )
{
	int		index;
	Tetrahedron	*tet;
	EdgeClass	*edge;
	PositionedTet	ptet0,
			ptet;

	for(	tet = manifold->tet_list_begin.next, index = 1;
		tet!=&manifold->tet_list_end;
		tet = tet->next, index++ )
		tet->index = index;

	if (ex)
	{
		if (manifold->solution_type[complete] == geometric_solution )
			if (contains_flat_tetrahedra(manifold)==TRUE)
				stream << "SolutionType partially_flat_solution\n";
			else	stream << "SolutionType geometric_solution\n";

		if (manifold->solution_type[complete] == nongeometric_solution )
			stream << "SolutionType nongeometric_solution\n";

		if (manifold->solution_type[complete] == not_attempted )
			stream << "SolutionType not_attempted\n";

		if (manifold->solution_type[complete] == other_solution )
			stream << "SolutionType other_solution\n";

		if (manifold->solution_type[complete] == step_failed )
			stream << "SolutionType step_failed\n";

		if (manifold->solution_type[complete] == no_solution )
			stream << "SolutionType no_solution\n";

		if (manifold->solution_type[complete] == invalid_solution )
			stream << "SolutionType invalid_solution\n";

		if (manifold->solution_type[complete] == degenerate_solution )
			stream << "SolutionType degenerate_solution\n";

		if (manifold->solution_type[complete] == flat_solution )
			stream << "SolutionType flat_solution\n";

		stream << "vertices_known\n\n";
	}

	for (	edge = manifold->edge_list_begin.next, index = 1;
		edge!=&manifold->edge_list_end;		
		edge = edge->next, index++)
	{
		QString s;

		stream << qSetW(3) << index;
		stream << ' ' << qSetW(2) << edge->singular_index + 1;

		s.sprintf(" %04.3f", edge->singular_order );

		stream << s;

                set_left_edge(edge,&ptet0);
                ptet = ptet0;

		if (ae)
		{
			double err =  (edge->singular_order==0)
				? 0
				: TWO_PI / edge->singular_order;

			do{
				err -= ptet.tet->dihedral_angle[ultimate]
						[edge_between_faces[ptet.near_face][ptet.left_face]];

				veer_left(&ptet);
			}while (!same_positioned_tet(&ptet, &ptet0));

			s.sprintf(" %21.16f", err );
			stream << s;
		}

		if (ex)
		{
			if (ptet.tet->cusp[remaining_face[ptet.left_face][ptet.near_face]]->index > -1 )
				s.sprintf(" %2d", ptet.tet->cusp[remaining_face[ptet.left_face][ptet.near_face]]->index + 1 );
			else	s.sprintf(" %2d", ptet.tet->cusp[remaining_face[ptet.left_face][ptet.near_face]]->index );
			stream << s;

			if (ptet.tet->cusp[remaining_face[ptet.near_face][ptet.left_face]]->index > -1 )
				s.sprintf(" %2d", ptet.tet->cusp[remaining_face[ptet.near_face][ptet.left_face]]->index + 1 );
			else    s.sprintf(" %2d", ptet.tet->cusp[remaining_face[ptet.near_face][ptet.left_face]]->index );
			stream << s;
		}

		do{
			char c = NL(ptet.left_face);
			char d = NL(ptet.near_face);

			s.sprintf(" %2d%c%c",ptet.tet->index,c,d);
			stream << s;
	
			veer_left(&ptet);

		}while (!same_positioned_tet(&ptet, &ptet0));

		stream << endl;
	}

	if (ex && manifold->solution_type[complete] != not_attempted )
	{
		stream << endl;

		for (   edge = manifold->edge_list_begin.next, index = 1;
			edge!=&manifold->edge_list_end;
			edge = edge->next, index++)
		{
			QString s;

			stream << qSetW(3) << index;

			set_left_edge(edge,&ptet0);
			ptet = ptet0;

			s.sprintf(" %21.16f", edge->inner_product[ultimate] );
			stream << qSetW(21) << s;

			int top = remaining_face[ptet.left_face][ptet.near_face];
                        s.sprintf(" %21.16f", ptet.tet->cusp[top]->inner_product[ultimate] );
                        stream << qSetW(21) << s;

                        int bottom = remaining_face[ptet.near_face][ptet.left_face];
                        s.sprintf(" %21.16f", ptet.tet->cusp[bottom]->inner_product[ultimate] );
                        stream << qSetW(21) << s;

			do
			{
				s.sprintf(" %21.16f", ptet.tet->dihedral_angle[ultimate]
						[edge_between_faces[ptet.near_face][ptet.left_face]]);
				stream << qSetW(21) << s;
				veer_left(&ptet);
			}while (!same_positioned_tet(&ptet, &ptet0));

			stream << endl;
		}
	}

	if (ex && curves )	
	{
                stream << endl;

                for (   edge = manifold->edge_list_begin.next, index = 1;
                        edge!=&manifold->edge_list_end;
                        edge = edge->next, index++)
                {
                        QString s;

                        stream << qSetW(3) << index;

                        set_left_edge(edge,&ptet0);
                        ptet = ptet0;

                        do
                        {
				int top = remaining_face[ptet.left_face][ptet.near_face];
				int bottom = remaining_face[ptet.near_face][ptet.left_face];

                                s.sprintf("   %2d %2d %2d %2d %2d %2d %2d %2d",
					ptet.tet->curve[0][0][top][bottom],
					ptet.tet->curve[0][0][bottom][top],
					ptet.tet->curve[0][1][top][bottom],
					ptet.tet->curve[0][1][bottom][top],
					ptet.tet->curve[1][0][top][bottom],
					ptet.tet->curve[1][0][bottom][top],
					ptet.tet->curve[1][1][top][bottom],
					ptet.tet->curve[1][1][bottom][top] );
                                stream << qSetW(21) << s;
                                veer_left(&ptet);
                        }while (!same_positioned_tet(&ptet, &ptet0));

                        stream << endl;
                }
	}
}


static double   edge_length( Tetrahedron *tet, int e )
{
	double chi1, chi2,i1,i2,temp;

	if (tet->edge_class[e]->singular_order == 0)
		return 0;

	chi1 = tet->cusp[one_vertex_at_edge[e]]->orbifold_euler_characteristic;
	chi2 = tet->cusp[other_vertex_at_edge[e]]->orbifold_euler_characteristic;

	if (chi2 < chi1 )
	{
		i1 = tet->cusp[other_vertex_at_edge[e]]->inner_product[ultimate];
		i2 = tet->cusp[one_vertex_at_edge[e]]->inner_product[ultimate];

		temp = chi2;
		chi2 = chi1;
		chi1 = temp;
	}
	else
	{
		i1 = tet->cusp[one_vertex_at_edge[e]]->inner_product[ultimate];
		i2 = tet->cusp[other_vertex_at_edge[e]]->inner_product[ultimate];
	}

	if ( ABS(chi1) < IDEAL_EPSILON && ABS(chi2) < IDEAL_EPSILON )
		return log( -tet->edge_class[e]->inner_product[ultimate] / 2 );  /* two ideal */

	if ( ABS(chi1) < IDEAL_EPSILON )
		return log( -tet->edge_class[e]->inner_product[ultimate]   /* one ideal and one finite */
						/  safe_sqrt(ABS(i2)) );

	if ( ABS(chi2) < IDEAL_EPSILON )		/* one ideal and one hyper */
		return log( -tet->edge_class[e]->inner_product[ultimate]
						/ safe_sqrt(ABS(i1)));

	if ( chi1 * chi2 > 0 )
		return acosh( -tet->edge_class[e]->inner_product[ultimate] /* cosh cases */
			/ (safe_sqrt(ABS(i1)) * safe_sqrt(ABS(i2)) ));	
	else	return asinh( -tet->edge_class[e]->inner_product[ultimate] /* one finite and one hyper */
			 / (safe_sqrt(ABS(i1)) * safe_sqrt(ABS(i2)) ));


	uFatalError("edge_length","console");	

	return 0;
}

