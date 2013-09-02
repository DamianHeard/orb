#include <stdlib.h>
#include <stdio.h>
#include <iostream> 
#include "organizer.h"
#include "casson.h"
#include <qmainwindow.h>
#include <qapplication.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qregion.h>
#include <qfont.h>
#include <qfileinfo.h>
#include <qdockarea.h>
#include <qimage.h>
#include <qpixmap.h>

#include "clip.xpm"
#include "compare.xpm"
#include "draw.xpm"
#include "import.xpm"
#include "help.xpm"
#include "open.xpm"
#include "orb32.xpm"
#include "switch.xpm"
#include "quit.xpm"

CassonFormat	*readCassonFormat( QTextStream &ts );
bool		verifyCassonFormat( CassonFormat *cf );
void		freeCassonFormat( CassonFormat *cf );
Triangulation	*cassonToTriangulation( CassonFormat *cf );
extern "C" Triangulation *get_triangulation( const char *file );
extern "C" Triangulation *graph2manifold(FILE *fp, double order, int *n );
extern "C" void identify_cusps( Triangulation *manifold );
extern "C" void my_tilts( Triangulation *manifold );
extern "C" void peripheral_curves_as_needed( Triangulation *manifold );

Organizer::Organizer( QDesktopWidget *d )
	:QWidget(0,0, Qt::WStyle_Tool | Qt::WDestructiveClose )
{
	QPixmap openIcon	= QPixmap(open_xpm );
	QPixmap drawIcon	= QPixmap(draw_xpm );
	QPixmap compareIcon	= QPixmap(compare_xpm );
	QPixmap clipIcon	= QPixmap(clip_xpm );
	QPixmap helpIcon	= QPixmap(help_xpm);
	QPixmap arrowIcon	= QPixmap(switch_xpm );
	QPixmap quitIcon	= QPixmap( quit_xpm );
	QPixmap importIcon	= QPixmap( import_xpm );

	untitled_num=0;
	cover_count = 0;
	dehn_count = 0;
	prune_count = 0;
	find_count = 0;
	drill_count = 0;
	path = "./examples/";
	loading = FALSE;
	desk = d;
	//setCaption("Orb toolbar");

	box = new QHBoxLayout( this );

        blank = new QLabel( this );
        blank->setPixmap(orb32_xpm);
	box->addWidget( blank );
	box->addStretch( 10 );

	open = new QToolButton( this );
	box->addWidget( open );

	//box->insertSpacing(-1,30);

        draw = new QToolButton( this );
	box->addWidget( draw );	

        isom = new QToolButton( this );
	box->addWidget( isom );

        clip = new QToolButton( this );
	box->addWidget( clip );
	//box->insertSpacing(-1,30);

	arrow = new QToolButton( this );
	box->addWidget( arrow );

	import = new QToolButton( this );
	box->addWidget( import );


	//box->insertSpacing(-1,30);

	help = new QToolButton( this );
	box->addWidget( help );

	quit = new QToolButton( this );
	box->addWidget(quit);

	open->setAutoRaise(TRUE);
	open->setAccel(Key_F1);
        open->setIconSet(openIcon);
        open->setTextLabel("Open");
        open->setToggleButton(FALSE);

	import->setAutoRaise(TRUE);
	import->setIconSet(importIcon);
	import->setTextLabel("Import SnapPea file");
	import->setToggleButton(FALSE);

	draw->setAutoRaise(TRUE);
	draw->setAccel(Key_F2);
	draw->setIconSet(drawIcon);
	draw->setTextLabel("Draw");
	draw->setToggleButton(TRUE);

	isom->setAutoRaise(TRUE);
	isom->setAccel(Key_F3);
	isom->setIconSet(compareIcon);
	isom->setTextLabel("Compare");
	isom->setToggleButton( TRUE );

	clip->setAutoRaise(TRUE);
	clip->setAccel(Key_F4);
        clip->setIconSet(clipIcon);
        clip->setTextLabel("Clipboard");
        clip->setToggleButton( TRUE );

	help->setAutoRaise(TRUE);
	help->setAccel(Key_F5);
	help->setIconSet(helpIcon);
	help->setTextLabel("Help");

        quit->setAutoRaise(TRUE);
	quit->setIconSet(quitIcon);
        quit->setTextLabel("Quit");

        arrow->setAutoRaise(TRUE);
        arrow->setIconSet(arrowIcon);
        arrow->setTextLabel("Switch window");
        arrowMenu = new QPopupMenu(this,"arrowMenu" );
	arrowMenu->setItemEnabled( arrowMenu->insertItem( "Windows:", -1, -1 ), FALSE );
        arrow->setPopup( arrowMenu );
        arrow->setPopupDelay( 1 );


        connect( open, SIGNAL( clicked() ),
                        this, SLOT( chooseOrbSlot() ) );
	connect( import, SIGNAL( clicked() ),
			this, SLOT(chooseSnapPeaSlot() ));
        connect( draw, SIGNAL( clicked() ),
                        this, SLOT( drawSlot() ) );
        connect( isom, SIGNAL( clicked() ),
                        this, SLOT( isomSlot() ) );
        connect( clip, SIGNAL( clicked() ),
                        this, SLOT( clipSlot() ) );
	connect( help, SIGNAL( clicked() ),
			this, SLOT( helpSlot() ) );

	connect( quit, SIGNAL( clicked() ),
			this, SLOT( quitSlot() ) );

        setGeometry(0,35,350,33);
        setFixedSize(350,33);
	upper_bound = y()+height();

	diagram = new DiagramWindow(this, upper_bound+35);
	diagram->hide();
	connect( diagram, SIGNAL( visibilityChanged() ),
		this, SLOT( toggleDrawSlot() ) );
	connect( diagram, SIGNAL( exporting( Triangulation *,DiagramCanvas *) ),
		this, SLOT( importSlot( Triangulation *,DiagramCanvas *) ));

	isomWindow = new IsometryWindow(upper_bound+35,this, items);
	isomWindow->hide();
	connect( isomWindow, SIGNAL( closed() ),
		this, SLOT( toggleIsomSlot() ) );

	connect( this, SIGNAL( newWindow() ),
		isomWindow, SLOT( updateListSlot() ));
	connect( this, SIGNAL( newWindow() ),
		this, SLOT( updateArrowSlot() ));

	clipBoard = new ClipBoard( this, upper_bound+35);
	clipBoard->hide();
	connect( clipBoard, SIGNAL( closed() ),
		this, SLOT( toggleClipSlot() ) );
	connect( clipBoard, SIGNAL( exportString(QString) ), this, SLOT( importString( QString ) ) );

}


Organizer::~Organizer()
{
	blank->~QLabel();
	open->~QToolButton();
	import->~QToolButton();
	draw->~QToolButton();
	isom->~QToolButton();
	clip->~QToolButton();	
	help->~QToolButton();
	quit->~QToolButton();
	arrow->~QToolButton();
	arrowMenu->~QPopupMenu();
	box->~QHBoxLayout();
	diagram->~DiagramWindow();
	isomWindow->~IsometryWindow();
	clipBoard->~ClipBoard();
}


QStringList Organizer::names()
{
	QStringList list;
	for( int i = 0; i<items.size();i++)
		list += items[i]->caption();

	return list;
}


ManifoldInterface *Organizer::window( int index )
{
	return items.at(index);
}

void Organizer::drawSlot()
{
	if (diagram->isHidden())
	{
		diagram->show();
		draw->setPaletteBackgroundColor( Qt::black );
	}
	else
	{
		diagram->hide();
		draw->setPaletteBackgroundColor( Qt::white );
	}
}

void Organizer::clipSlot()
{
	if (clipBoard->isHidden())
	{
		clipBoard->show();
		clip->setPaletteBackgroundColor( Qt::black );
	}
	else
	{
		clipBoard->hide();
		clip->setPaletteBackgroundColor( Qt::white );
	}
}

void Organizer::isomSlot()
{
	if (isomWindow->isHidden())
	{
		isomWindow->show();
		isom->setPaletteBackgroundColor( Qt::black );
	}
	else
	{
		isomWindow->hide();
		isom->setPaletteBackgroundColor( Qt::white );
	}
}

void Organizer::helpSlot()
{
    
    #ifdef Q_OS_MAC
	if (!QFileInfo("./Orb.app/Contents/Resources/OrbDocumentation.pdf").exists())
                QMessageBox::information( this, "Orb",
                "Could not find the file OrbDocumentation.pdf.\n");

 	if (system("open ./Orb.app/Contents/Resources/OrbDocumentation.pdf")==-1)
 		QMessageBox::information( this, "Orb",
     		"Unable to open OrbDocumentation.pdf.\n");
    #else
	if (!QFileInfo("./OrbDocumentation.pdf").exists())
                QMessageBox::information( this, "Orb",
                "Could not find the file OrbDocumentation.pdf.\n");

 	if (system("kfmclient openURL ./OrbDocumentation.pdf")==-1)
 		QMessageBox::information( this, "Orb",
     		"Unable to open OrbDocumentation.pdf.\n");        
    #endif
}

void Organizer::importSlot( Triangulation *manifold, DiagramCanvas *d )
{
	QString name = tr("untitled-%1").arg(untitled_num);

	if (manifold->name!=NULL)
		my_free(manifold->name);

	manifold->name = NEW_ARRAY( name.length() + 1, char );
	for( int i = 0; i < name.length() + 1; i++ )
		manifold->name[i] = name[i].latin1();

	ManifoldInterface *mi = new ManifoldInterface(upper_bound+35,TRUE,"",manifold,d,diagram, this );

	untitled_num++;
	connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
	connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
	connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

	mi->resize(770,360);
	mi->show();
	mi->setFocus();

	items.push_back( mi );
	diagram->hide();
	emit newWindow();
}

void Organizer::importString( QString s )
{
	QTextStream ts( &s, IO_ReadOnly );

	Triangulation *manifold = readTriangulation( ts );

	if ( manifold == NULL )
		return;

        QString name = tr("untitled-%1").arg(untitled_num);

        if (manifold->name!=NULL)
                my_free(manifold->name);

        manifold->name = NEW_ARRAY( name.length() + 1, char );
        for( int i = 0; i < name.length() + 1; i++ )
                manifold->name[i] = name[i].latin1();

	ManifoldInterface *mi = new ManifoldInterface(upper_bound+35,TRUE,"",manifold, NULL,diagram,this );
	untitled_num++;

	connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
	connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
	connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

	mi->resize(770,360);
	mi->show();
	mi->setFocus();
	items.push_back( mi );
	emit newWindow();
}

void Organizer::chooseOrbSlot()
{
	loading = TRUE;

	QString fn = QFileDialog::getOpenFileName(
			getPath(),
			"*.orb",
			this);

	if ( !fn.isEmpty() )
	{
		if (fn.endsWith( ".orb" ))
			loadOrbSlot( fn );

		path = fn.section('/',0,-2 );
	}

	loading = FALSE;
}

void Organizer::chooseSnapPeaSlot()
{
	loading = TRUE;

	QString fn = QFileDialog::getOpenFileName(
		getPath(),
		0,
		this);

	if ( !fn.isEmpty() )
	{
		loadSnapPeaSlot( fn );

		path = fn.section('/',0,-2 );
	}

	loading = FALSE;
}

void Organizer::loadSnapPeaSlot( const QString &fileName )
{
	Triangulation *manifold = NULL;

	manifold = get_triangulation( fileName.ascii() );

	if (manifold==NULL)
	{
		QMessageBox::information( this, "Orb",
			"Invalid file.");
		return;
	}

        QString name = tr("untitled-%1").arg(untitled_num);

        if (manifold->name!=NULL)
                my_free(manifold->name);

        manifold->name = NEW_ARRAY( name.length() + 1, char );
        for( int i = 0; i < name.length() + 1; i++ )
                manifold->name[i] = name[i].latin1();

	ManifoldInterface *mi =new ManifoldInterface(upper_bound+35,TRUE,"",manifold,
					NULL,diagram,this );
	connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
	connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
	connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

	untitled_num++;
	mi->show();
	mi->resize(770,360);

	items.push_back( mi );
	emit newWindow();
}

void Organizer::loadDehn( Triangulation *dehn, QString n )
{
	QString name = tr("%1-dehn%2").arg(n).arg(dehn_count);

	if (dehn->name!=NULL)
		my_free(dehn->name);

	dehn->name = NEW_ARRAY( name.length() + 1, char );
	for( int i = 0; i < name.length() + 1; i++ )
		dehn->name[i] = name[i].latin1();

	ManifoldInterface *mi = new ManifoldInterface(upper_bound+35,TRUE,"",dehn, NULL,diagram,this );
	connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
	connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
	connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

	mi->show();
	mi->resize(770,360);
	dehn_count++;
	items.push_back( mi );
	emit newWindow();
}

void Organizer::loadCover( Triangulation *cover, QString n )
{
	QString name = tr("%1-cover%2").arg(n).arg(cover_count);

        if (cover->name!=NULL)
                my_free(cover->name);

        cover->name = NEW_ARRAY( name.length() + 1, char );
        for( int i = 0; i < name.length() + 1; i++ )
                cover->name[i] = name[i].latin1();

	ManifoldInterface *mi = new ManifoldInterface(upper_bound+35,TRUE,"",cover, NULL,diagram,this );
	connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
	connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
	connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

	mi->show();
	mi->resize(770,360);
	cover_count++;
	items.push_back( mi );
	emit newWindow();
}

void Organizer::loadFind( Triangulation *find, DiagramCanvas *c, QString n )
{
	QString name = tr("%1-find%2").arg(n).arg(find_count);

	if (find->name!=NULL)
		my_free(find->name);

	find->name = NEW_ARRAY( name.length() + 1, char );
	for( int i = 0; i < name.length() + 1; i++ )
		find->name[i] = name[i].latin1();

	ManifoldInterface *mi = new ManifoldInterface(upper_bound+35,TRUE,"",find, c,diagram,this );
	connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
	connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
	connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

	mi->show();
	mi->resize(770,360);
	find_count++;
	items.push_back( mi );
	emit newWindow();
}

void Organizer::loadPrune( Triangulation *prune, DiagramCanvas *new_canvas, QString n )
{
        QString name = tr("%1-prune%2").arg(n).arg(prune_count);

        if (prune->name!=NULL)
                my_free(prune->name);
        
        prune->name = NEW_ARRAY( name.length() + 1, char );
        for( int i = 0; i < name.length() + 1; i++ )
                prune->name[i] = name[i].latin1();

	ManifoldInterface *mi = new ManifoldInterface(upper_bound+35,TRUE,"",prune, new_canvas,diagram,this );
	connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
	connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
	connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

	mi->show();
	mi->resize(770,360);
	prune_count++;
	items.push_back( mi );
	emit newWindow();
}

void Organizer::loadDrill( Triangulation *drill, DiagramCanvas *new_canvas, QString n )
{
        QString name = tr("%1-drill%2").arg(n).arg(prune_count);

        if (drill->name!=NULL)
                my_free(drill->name);

        drill->name = NEW_ARRAY( name.length() + 1, char );
        for( int i = 0; i < name.length() + 1; i++ )
                drill->name[i] = name[i].latin1();

        ManifoldInterface *mi = new ManifoldInterface(upper_bound+35,TRUE,"",drill, new_canvas,diagram,this );
        connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
        connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
	connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

        mi->show();
        mi->resize(770,360);
        drill_count++;
        items.push_back( mi );
        emit newWindow();
}

void Organizer::loadKnotSlot( const QString &fileName )
{
	FILE *fp;

	int val  = QMessageBox::question( this, "Orb", "Open as a complement or an orbifold?",
			"Complement", "Orbifold", QString::null, 0, 0 );


	if ((fp=fopen(fileName.latin1() ,"r"))==NULL)
	{
		QMessageBox::information( this, "Orb",
		"Failed to open file.");
		return;
	}

	int nv;
	Triangulation *manifold = graph2manifold( fp, (val==0) ? -1 : 0, &nv );

	if (manifold==NULL)
	{
		QMessageBox::information( this, "Orb",
		"Failed to load triangulation.");
		return;
	}

	ManifoldInterface *mi =new ManifoldInterface(upper_bound+35,TRUE,"",manifold,
				NULL,diagram, this );
	connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
	connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
	connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

	mi->show();
	mi->resize(770,360);

	items.push_back( mi );
	emit newWindow();
		
}

void Organizer::loadOrbFile( const QString &s )
{
	loadOrbSlot( s );
}

void Organizer::loadOrbSlot( const QString &fileName )
{
	QFile f( fileName );
	if (!f.open(IO_ReadOnly))
	{
		QMessageBox::information( this, "Orb",
		"Open cancelled." );
		return;
	}

	QTextStream ts( &f );
	QString line = ts.readLine();

	if (line == "% orb")
	{
		ts.readLine();

		Triangulation *manifold = readTriangulation( ts );

                if (manifold == NULL )
                {
                        QMessageBox::information( this, "Orb",
                                        "Failed to load triangulation." );
                        return;
                }

		QString name = fileName.section('/',-1);
		if (name.endsWith(".orb"))
			name.truncate( name.length() - 4 );

		manifold->name = NEW_ARRAY( name.length() + 1, char );
		int i = 0;
		for(i=0;i<name.length() + 1;i++)
			manifold->name[i] = name[i].latin1();

		DiagramCanvas *canvas = readDiagram( ts );

		QString temp = fileName.section('/',0,-2 );
		temp.append('/');

		ManifoldInterface *mi =new ManifoldInterface(upper_bound+35,FALSE,temp,manifold,
										canvas,diagram,this );
		connect( mi, SIGNAL(closed(ManifoldInterface* const)), this,SLOT(closeSlot(ManifoldInterface* const)));
		connect( mi, SIGNAL(nameChange()),isomWindow, SLOT( updateListSlot() ));
		connect( mi, SIGNAL(nameChange()), this, SLOT(updateArrowSlot() ));

		mi->show();
		mi->resize(770,360);

		items.push_back( mi );
		emit newWindow();
	}
	else	QMessageBox::information( this, "Orb",
				"Failed to open file." );

}

void Organizer::toggleDrawSlot()
{
	if (diagram->isHidden())
	{
		draw->setPaletteBackgroundColor( Qt::white );
		draw->setOn(FALSE);
	}
	else
	{
		draw->setPaletteBackgroundColor( Qt::black );
		draw->setOn(TRUE);
	}
}

void Organizer::toggleClipSlot() 
{
        clip->setOn(FALSE);
	clip->setPaletteBackgroundColor( Qt::white );
}

void Organizer::toggleIsomSlot() 
{
        isom->setOn(FALSE);
	isom->setPaletteBackgroundColor( Qt::white );
}

void Organizer::closeSlot(ManifoldInterface* const m)
 {
	int i;
	for( i = 0; i<items.size();i++)
	if (items[i] == m )
		break;
	items.at(i)->hide();	
	items.at(i)->~ManifoldInterface(); 
	items.erase(items.begin()+i);

	emit newWindow();

}


Triangulation *Organizer::readTriangulation( QTextStream &ts )
{
	Triangulation *manifold = NULL;
	CassonFormat	*cf = readCassonFormat( ts );

	if (verifyCassonFormat( cf ))
		manifold = cassonToTriangulation( cf );
	else	QMessageBox::information( this, "Orb",
			"Invalid file." );

	freeCassonFormat( cf );

	return manifold;
}

CassonFormat *readCassonFormat( QTextStream &ts )
{

        int             i;
        bool            ok = FALSE;
        CassonFormat    *cf = NULL;
        QString         line,
                        section;
        EdgeInfo        *nei = NULL,
                        *ei = NULL;
        TetEdgeInfo     *ntei = NULL,
                        *tei = NULL;

	cf = new CassonFormat;
	cf->head = NULL;
	cf->num_tet = 0;
	ts.skipWhiteSpace();

	line = ts.readLine();

	if (line =="SolutionType not_attempted")
	{
		cf->type = not_attempted;
		ok = TRUE;
	}

	if (line =="SolutionType geometric_solution" || line =="SolutionType partially_flat_solution" )
	{
		cf->type = geometric_solution;
		ok = TRUE;
	}

	if (line =="SolutionType nongeometric_solution")
	{
		cf->type = nongeometric_solution;
		ok = TRUE;
	}

	if (line =="SolutionType flat_solution")
	{
		cf->type = flat_solution;
		ok = TRUE;
	}

	if (line =="SolutionType degenerate_solution")
	{
		cf->type = degenerate_solution;
		ok = TRUE;
	}

	if (line =="SolutionType other_solution")
	{
			cf->type = other_solution;
			ok = TRUE;
	}

	if (line =="SolutionType no_solution")
	{
			cf->type = no_solution;
			ok = TRUE;
	}

	if (line =="SolutionType step_failed")
	{
			cf->type = step_failed;
			ok = TRUE;
	}

	if (line =="SolutionType invalid_solution")
	{
			cf->type = invalid_solution;
			ok = TRUE;
	}

	if (ok)
	{
		line = ts.readLine();
		if (line == "vertices_known")
		{
			cf->vertices_known = TRUE;
			ts.readLine();
			line = ts.readLine();
		}
		else
		{
			cf->vertices_known = FALSE;
			line = ts.readLine();
		}
	}
	else
	{
		cf->type = not_attempted;
		cf->vertices_known = FALSE;
	}
	
	while (!line.isEmpty())
	{
		line = line.simplifyWhiteSpace();

		nei = new EdgeInfo;

                if (cf->head==NULL)
                        cf->head = nei;
                else	ei->next = nei;

		nei->next = NULL;
		nei->head = NULL;

		ei = nei;

		ei->index = line.section(' ',0,0).toInt(&ok,10) - 1;
		if (!ok) return NULL;
		ei->singular_index = line.section(' ',1,1).toInt(&ok,10) - 1;
		if (!ok) return NULL;
		ei->singular_order = line.section(' ',2,2).toDouble(&ok);	
		if (!ok) return NULL;

		i = 3;

		if (cf->vertices_known)
		{
			ei->one_vertex = line.section(' ',i,i).toInt(&ok,10);
			if (!ok) return NULL;
			if (ei->one_vertex > 0 ) ei->one_vertex--;
			i++;
			ei->other_vertex = line.section(' ',i,i).toInt(&ok,10);
			if (!ok) return NULL;
			if (ei->other_vertex > 0 ) ei->other_vertex--;
			i++;
		}

		section = line.section(' ',i,i);

		while (!section.isEmpty())	
		{
			ntei = new TetEdgeInfo;
			for( int j = 0; j < 8; j++ )
				ntei->curves[j] = 0;

			if (ei->head==NULL)
				ei->head        = ntei;
			else
				tei->next       = ntei;

			ntei->next = NULL;
			tei = ntei;

			char c = section.at(section.length()-2).latin1();
			char d = section.at(section.length()-1).latin1();

			if ( c!= 'u' && c!='v' && c!='w' && c!='x' )
				return NULL;
				

			if ( d!= 'u' && d!='v' && d!='w' && d!='x' )
				return NULL;

			tei->f1 = LN(c);
			tei->f2 = LN(d);
			section.truncate(section.length()-2);
			tei->tet_index = section.toInt(&ok,10) - 1;
			if (!ok) return NULL;

			if (tei->tet_index + 1 > cf->num_tet)
				cf->num_tet = tei->tet_index + 1;

			i++;
			section = line.section(' ',i,i);

		}

		line = ts.readLine();
	}

	if (cf->type == not_attempted )
		return cf;

	ei = cf->head;	
	line = ts.readLine();

	while ( ei != NULL && !line.isEmpty() )
	{

		line = line.simplifyWhiteSpace();

		ei->e_inner_product  = line.section(' ',1,1).toDouble(&ok);
		if (!ok) return NULL;
		ei->v_inner_product1 = line.section(' ',2,2).toDouble(&ok);
		if (!ok) return NULL;
		ei->v_inner_product2 = line.section(' ',3,3).toDouble(&ok);
		if (!ok) return NULL;

		i = 4;
		tei = ei->head;

		while( tei != NULL )
		{
			section = line.section(' ', i,i );
			tei->dihedral_angle = section.toDouble(&ok);
			if (!ok) return NULL;
			i++;
			tei = tei->next;

		} 
		ei = ei->next;

		line = ts.readLine();

	}

        if (cf->vertices_known == FALSE )
                return cf;

	ei = cf->head; 
	line = ts.readLine();
	while ( ei != NULL && !line.isEmpty() )
	{
		line = line.simplifyWhiteSpace();
		i = 1;
		tei = ei->head;
		while( tei != NULL )
		{
			for( int j = 0; j < 8; j++ )
			{
				section = line.section(' ', i,i );
				tei->curves[j] = section.toInt(&ok);
				if (!ok) return NULL;
				i++;
			}
			tei = tei->next;
		}
		ei = ei->next;
		line = ts.readLine();
	}

	return cf;
}

bool verifyCassonFormat( CassonFormat *cf )
{
	int		i,j,k;
	bool		check[4][4];
	EdgeInfo	*ei;
	TetEdgeInfo	*tei;

	if (cf==NULL) return FALSE;

	for(i=0;i<cf->num_tet;i++)
	{
		for(j=0;j<4;j++)
			for(k=0;k<4;k++)
			if (j==k)
				check[j][k] = TRUE;
			else	check[j][k] = FALSE;

		ei = cf->head;

		if (ei == NULL)
			return FALSE;

		while(ei!=NULL)
		{
			tei = ei->head;

			if (tei == NULL)
				return FALSE;

			while(tei!=NULL)
			{
				if (tei->tet_index == i )
				{
					if (check[tei->f1][tei->f2])
						return TRUE;

					check[tei->f1][tei->f2] = TRUE;
					check[tei->f2][tei->f1] = TRUE;
				}
				tei = tei->next;
			}
			ei = ei->next;
		}

		for(j=0;j<4;j++)
			for(k=0;k<4;k++)
			if (check[j][k]==FALSE)
				return FALSE;
	}

	return TRUE;
}

Triangulation *cassonToTriangulation( CassonFormat *cf )
{
	Triangulation	*manifold;
	Tetrahedron	**tet_array;
	int		i,j,
			a1, a2, a3, a4,
			b1, b2, b3, b4,
			t1,t2;
	EdgeInfo	*ei;
	TetEdgeInfo	*tei1,
			*tei2;

	manifold = NEW_STRUCT(Triangulation);
	initialize_triangulation(manifold);

	manifold->num_tetrahedra                        = cf->num_tet;
        manifold->solution_type[complete]       = not_attempted;
        manifold->solution_type[ filled ]       = not_attempted;
	manifold->num_singular_arcs = 0;
	manifold->num_or_cusps = 0;
	manifold->num_nonor_cusps = 0;
	manifold->num_cusps = 0;

	tet_array = NEW_ARRAY(manifold->num_tetrahedra, Tetrahedron *);
	for (i = 0; i < manifold->num_tetrahedra; i++)
	{
		tet_array[i] = NEW_STRUCT(Tetrahedron);
		initialize_tetrahedron(tet_array[i]);
		INSERT_BEFORE(tet_array[i], &manifold->tet_list_end);
	}

	ei = cf->head;
	while (ei!=NULL)
	{
		tei1 = ei->head;
		while (tei1!=NULL)
		{
			if (tei1->next==NULL)
				tei2 = ei->head;
			else	tei2 = tei1->next;

			t1 = tei1->tet_index;
			a1 = tei1->f1;
			a2 = tei1->f2;
			a3 = vertex_at_faces[a1][a2];
			a4 = vertex_at_faces[a2][a1];


			t2 = tei2->tet_index;
			b1 = tei2->f1;
			b2 = tei2->f2;
			b3 = vertex_at_faces[b1][b2];
			b4 = vertex_at_faces[b2][b1];

			tet_array[t1]->dihedral_angle[ultimate][edge_between_faces[a1][a2]]
				= tei1->dihedral_angle;

			tet_array[t1]->neighbor[a1] = tet_array[t2];
			tet_array[t2]->neighbor[b2] = tet_array[t1];

			tet_array[t1]->gluing[a1] = CREATE_PERMUTATION(
                                a1, b2, a2, b1, a3, b3, a4, b4 );

			tet_array[t2]->gluing[b2] = CREATE_PERMUTATION(
				b1, a2, b2, a1, b3, a3, b4, a4 );

			tei1 = tei1->next;
		}
		ei = ei->next;
	}


        create_edge_classes(manifold);
	orient_edge_classes(manifold);

	create_cusps( manifold );
        count_cusps( manifold );


	ei = cf->head;

	while (ei!=NULL)
	{
		tei1 = ei->head;
		t1 = tei1->tet_index;
		a1 = tei1->f1;
		a2 = tei1->f2;

		int index = edge_between_faces[a1][a2];
		EdgeClass *edge = tet_array[t1]->edge_class[index];

		edge->inner_product[ultimate]	= ei->e_inner_product;
		edge->inner_product[penultimate]= ei->e_inner_product; 

		tet_array[t1]->cusp[remaining_face[a1][a2]]->inner_product[ultimate]
						= ei->v_inner_product1;
		tet_array[t1]->cusp[remaining_face[a1][a2]]->inner_product[penultimate]
						= ei->v_inner_product1;

		tet_array[t1]->cusp[remaining_face[a2][a1]]->inner_product[ultimate]
						= ei->v_inner_product2;
		tet_array[t1]->cusp[remaining_face[a2][a1]]->inner_product[penultimate]
						= ei->v_inner_product2;

		if (ei->singular_index  < 0 )
		{
			edge->is_singular = FALSE;
			edge->singular_order = 1;
			edge->old_singular_order = 1;
			edge->singular_index = -1;
		}
		else
		{
			edge->is_singular		= TRUE;
			manifold->num_singular_arcs++;
			edge->singular_order		= ei->singular_order;
			edge->old_singular_order	= ei->singular_order;
			edge->singular_index		= ei->singular_index;
		}

		ei = ei->next;
	}

	if (cf->type != not_attempted )
	for( Tetrahedron *tet = manifold->tet_list_begin.next;
			tet != &manifold->tet_list_end;
			tet = tet->next )
	{
		Boolean neg = FALSE;

		for(i=0;i<4;i++)
		for(j=0;j<4;j++)
		if (i!=j)
		{
			EdgeClass *edge = tet->edge_class[edge_between_vertices[i][j]];
			tet->Gram_matrix[i][j] = edge->inner_product[ultimate];

			if ( tet->dihedral_angle[ultimate][edge_between_vertices[i][j]] < -0.0001 ||
				tet->dihedral_angle[ultimate][edge_between_vertices[i][j]] > PI + 0.0001 )
				neg = TRUE;
		}
		else
		{
			Cusp *cusp = tet->cusp[i];
			tet->Gram_matrix[i][i] = cusp->inner_product[ultimate];
		}

		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				tet->inverse_Gram_matrix[i][j] = minor1( tet->Gram_matrix, i, j );

		tet->orientation_parameter[ultimate] = safe_sqrt( -gl4R_determinant( tet->Gram_matrix ) );
		if (neg) tet->orientation_parameter[ultimate] *= -1;

		for( i = 0; i<6;i++)
		if ( cos( tet->dihedral_angle[ultimate][i] ) * cos( tet->dihedral_angle[ultimate][i] ) < 1 / 2 )
			for(j=0;j<2;j++)
				tet->use_orientation_parameter[j][i] = FALSE;
		else
			for(j=0;j<2;j++)
				tet->use_orientation_parameter[j][i] = TRUE;
	}

	identify_cusps( manifold );

        ei = cf->head;

	if (cf->vertices_known)
        while (ei!=NULL)
        {
                tei1 = ei->head;
                t1 = tei1->tet_index;
                a1 = tei1->f1;
                a2 = tei1->f2;

		tet_array[t1]->cusp[remaining_face[a1][a2]]->index = ei->one_vertex;
		tet_array[t1]->cusp[remaining_face[a2][a1]]->index = ei->other_vertex;

                while (tei1!=NULL)
                {
                        t1 = tei1->tet_index;
                        a1 = tei1->f1;
                        a2 = tei1->f2;
                        int top		= remaining_face[a1][a2];
                        int bottom	= remaining_face[a2][a1];

                        tet_array[t1]->curve[0][0][top][bottom] = tei1->curves[0];
                        tet_array[t1]->curve[0][0][bottom][top] = tei1->curves[1];
                        tet_array[t1]->curve[0][1][top][bottom] = tei1->curves[2];
                        tet_array[t1]->curve[0][1][bottom][top] = tei1->curves[3];
                        tet_array[t1]->curve[1][0][top][bottom] = tei1->curves[4];
                        tet_array[t1]->curve[1][0][bottom][top] = tei1->curves[5];
                        tet_array[t1]->curve[1][1][top][bottom] = tei1->curves[6];
                        tet_array[t1]->curve[1][1][bottom][top] = tei1->curves[7];

                        tei1 = tei1->next;
                }

		ei = ei->next;
	}
	

	orient(manifold);
	my_free( tet_array );

	manifold->solution_type[complete] = cf->type;
	manifold->solution_type[filled] = cf->type;

	if (manifold->solution_type[complete] == geometric_solution )
		my_tilts( manifold );
	peripheral_curves_as_needed( manifold );

	return manifold;
}

void freeCassonFormat( CassonFormat *cf )
{
	EdgeInfo *e1, *e2;
	TetEdgeInfo *t1, *t2;

	if (cf == NULL)
		return;

	e1 = cf->head;

	while (e1!=NULL)
	{
		e2 = e1->next;
		t1 = e1->head;

		while (t1!=NULL)
		{
			t2 = t1->next;
			delete t1;
			t1 = t2;
		}

		delete e1;
		e1 = e2;
	}

	delete cf;
}


DiagramCanvas *Organizer::readDiagram( QTextStream &ts )
{
	DiagramCanvas *canvas = new DiagramCanvas(this,0,TRUE);

	canvas->readDiagram( ts );

	if (canvas->isUntouched())
	{
		canvas->~DiagramCanvas();
		canvas = NULL;
	}

	return canvas;
}

void Organizer::setPath( QString s )
{
	path = s;
}

QString Organizer::getPath()
{
	return path;	
}

void Organizer::moveEvent( QMoveEvent *e )
{
/*
	if (loading)
		return;

	if (pos().y()!=35 || pos().x() < 25 )
	{
		int x = (pos().x() < 25) ? 0 : pos().x();
		setGeometry( x, 35, width(), height() );
	}
*/
}

void Organizer::updateArrowSlot()
{
	arrowMenu->clear();
	arrowMenu->setItemEnabled( arrowMenu->insertItem( "Windows:", -1, -1 ), FALSE );
	for( int i = 0; i < items.size(); i++ )
		arrowMenu->insertItem(items[i]->caption(), items[i], SLOT(makeActiveSlot()));
}

void	Organizer::closeEvent( QCloseEvent *e )
{
	quitSlot();
}

void	Organizer::quitSlot()
{
	vector<ManifoldInterface *>::iterator mi_it; 
	mi_it = items.begin();

	int unsaved = 0;

        while ( mi_it < items.end() )
	{
                if ((*mi_it)->isSaved()==FALSE)
			unsaved++;

		mi_it++;
	}

	if (unsaved>0)
	{
		bool needSave = TRUE;

		if (unsaved>1)
                switch (QMessageBox::question( this,"Orb", tr("There are unsaved documents.\nHow do you wish to proceed?"),
                        tr("Examine..."), tr("Don't save"), tr("Cancel")))
                {
                        case 0: needSave = TRUE; break;
			case 1: needSave = FALSE; break;
                        default: return;
                }


        	mi_it = items.begin();

		while ( needSave && mi_it < items.end() )
		{
			if ((*mi_it)->isSaved()==FALSE)
				if ((*mi_it)->promptSaveSlot()==FALSE)
					return;

			(*mi_it)->~ManifoldInterface();
			items.erase(mi_it);

			isomWindow->updateListSlot();
			updateArrowSlot();
		}
	}

	this->~Organizer();
}

QDesktopWidget *Organizer::desktop()
{
	return desk;
}
