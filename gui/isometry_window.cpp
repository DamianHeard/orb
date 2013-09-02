#include "isometry_window.h"
#include "color.h"
#include <qlayout.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qpixmap.h>

#include "compare.xpm"

using namespace std;
extern "C" void         randomize_triangulation(Triangulation *manifold );
extern "C" Boolean	same_triangulation( Triangulation *m1, Triangulation *m2, int *singular_map );


IsometryWindow::IsometryWindow( int u, Organizer *organ, vector <ManifoldInterface *> v)
	: QMainWindow(0,0,Qt::WStyle_Tool)
{
	setCaption("Compare");

	items = v;
	o = organ;
	upper_bound = u;
	box = new QVBox( this );
	setCentralWidget( box );

	top = new QHBox( box, "top");

	blank = new QLabel( "",top, 0 );
	blank->setPixmap( compare_xpm );
	top->setStretchFactor( blank, 10 );

	button = new QPushButton("Compare", top, 0 );
	connect( button, SIGNAL( clicked() ),
		this, SLOT( sameManifoldSlot() ) );

	lists = new QHBox( box, "lists" );

	list1 = new QListBox( lists );
        QPalette p = list1->palette();
        QColorGroup cg = p.active();
        cg.setColor(QColorGroup::Base, BASE );
        cg.setColor(QColorGroup::Text, TEXT );
        cg.setColor(QColorGroup::Highlight, HIGHLIGHT );
        p.setBrush(QColorGroup::Text, TEXT );
        p.setBrush(QColorGroup::Base, BASE );
        p.setActive(cg);
        list1->setPalette(p);

	list2 = new QListBox( lists );
        p = list1->palette();
        cg = p.active();
        cg.setColor(QColorGroup::Base, BASE );
        cg.setColor(QColorGroup::Text, TEXT );
        cg.setColor(QColorGroup::Highlight, HIGHLIGHT );
	p.setBrush(QColorGroup::Text, TEXT );
        p.setBrush(QColorGroup::Base, BASE );
	p.setActive(cg);
	list2->setPalette(p);


	buttonGroup = new QHButtonGroup( "Random matching:", box, "buttonGroup");
	checkBox = new QCheckBox("Use",buttonGroup, "checkBox");
	slider = new QSlider( 1, 1000, 100, 0, Qt::Horizontal, buttonGroup, "slider" );

	setMinimumWidth( 225 );
	setMaximumWidth( 500 );
	setMinimumHeight( 100  );
	setMaximumHeight( 500 );

        setGeometry(0,353,250,180);
}

IsometryWindow::~IsometryWindow()
{
	button->~QPushButton();
	blank->~QLabel();
	list1->~QListBox();
	list2->~QListBox();
	lists->~QHBox();
	top->~QHBox();
	buttonGroup->~QHButtonGroup();
	box->~QVBox();
}

void IsometryWindow::sameManifoldSlot()
{
	int i;
	if (list1->count()==0)
		return;

	if (list1->currentItem()==-1)
		list1->setCurrentItem(0);

	if (list2->currentItem()==-1)
		list2->setCurrentItem(0);

	Triangulation *manifold1 = NULL, *manifold2 = NULL;
	copy_triangulation( o->window(list1->currentItem())->m(), &manifold1 );
	copy_triangulation( o->window(list2->currentItem())->m(), &manifold2 );
	int *singular_map = NEW_ARRAY( manifold1->num_singular_arcs, int );

	if ( !checkBox->isChecked() )
	{
		if ( canonize( manifold1 ) == func_failed || canonize( manifold2 ) == func_failed )
		{
			QMessageBox::information( this, "Orb", "Function canonize() failed.",  QMessageBox::Ok );
			free_triangulation( manifold1 );
			free_triangulation( manifold2 );
			return;
		}

		if ( same_triangulation( manifold1, manifold2, singular_map ) )
		{
			if (manifold1->num_singular_arcs == 0 )
				QMessageBox::information( this, "Orb", "Matched.",  QMessageBox::Ok );
			else
			{
				        QString         output;
					QTextStream     stream(&output,IO_WriteOnly);

					stream << "\nSuccessful Coloured Edge Map: (";
					for (i=0;i<manifold1->num_singular_arcs;i++)
						if (i==0)
							stream << singular_map[i] + 1;
						else	stream << ',' << singular_map[i] + 1;
					stream << ").";

					QMessageBox::information( this, "Orb",output,QMessageBox::Ok );
			}
		}
		else	QMessageBox::information( this, "Orb", "Failed.",  QMessageBox::Ok );
	}
	else
	{
		bool matched = FALSE;
		for( int i = 0; i < slider->value(); i++ )
		if (same_triangulation( manifold1, manifold2, singular_map ))
		{
			if (manifold1->num_singular_arcs == 0 )
				QMessageBox::information( this, "Orb", "Matched.",  QMessageBox::Ok );
			else
			{
                                        QString         output;
                                        QTextStream     stream(&output,IO_WriteOnly);

                                        stream << "\nSuccessful Coloured Edge Map: (";
                                        for (i=0;i<manifold1->num_singular_arcs;i++)
                                                if (i==0)
                                                        stream << singular_map[i];
                                                else    stream << ',' << singular_map[i];
                                        stream << ").";

                                        QMessageBox::information( this, "Orb",output,QMessageBox::Ok );
			}
			matched = TRUE;
			break;
		}
		else randomize_triangulation( manifold1 );

		for( int i = 0; i < slider->value() && slider->value() != 1 && !matched; i++ )
		if (same_triangulation( manifold1, manifold2, singular_map ))
		{
			if (manifold1->num_singular_arcs == 0 )
				QMessageBox::information( this, "Orb", "Matched.",  QMessageBox::Ok );
			else
			{
                                        QString         output;
                                        QTextStream     stream(&output,IO_WriteOnly);

                                        stream << "\nSuccessful Coloured Edge Map: (";
                                        for (i=0;i<manifold1->num_singular_arcs;i++)
                                                if (i==0)
                                                        stream << singular_map[i];
                                                else    stream << ',' << singular_map[i];
                                        stream << ").";

                                        QMessageBox::information( this, "Orb",output,QMessageBox::Ok );
			}
			matched = TRUE;
			break;
		}
		else randomize_triangulation( manifold2 );

		if (!matched)
			QMessageBox::information( this, "Orb", "Failed.",  QMessageBox::Ok );
	}

	free_triangulation( manifold1 );
	free_triangulation( manifold2 );
	my_free(singular_map);
}

void IsometryWindow::hideEvent( QHideEvent *e )
{
        emit closed();
}

void IsometryWindow::updateListSlot()
{
	list1->clear();
	list2->clear();

	list1->insertStringList( o->names() );
	list2->insertStringList( o->names() );
}

void IsometryWindow::resizeEvent( QResizeEvent *e )
{
//         if (y() < upper_bound )
//         {
//                 setGeometry(x(),upper_bound,width(),height());
//                 repaint();
//         }
} 

void IsometryWindow::moveEvent( QMoveEvent *e )
{       
// 	if (x() < 25 )
// 		move( 0, y() );
} 

