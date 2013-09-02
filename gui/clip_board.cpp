#include "clip_board.h"
#include "organizer.h"
#include "color.h"
#include <qlayout.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qpixmap.h>

#include "examine.xpm"
#include "clip.xpm"
#include "disk.xpm"
#include "zoomin.xpm"
#include "clear.xpm"
#include "zoomout.xpm"

ClipBoard::ClipBoard( Organizer *o, int u )
		: QMainWindow(0,0,Qt::WStyle_Tool)
{
	setCaption("Clipboard");
	upper_bound = u;
	saving = FALSE;

	QPixmap  diskIcon, zoominIcon, zoomoutIcon, clearIcon, examineIcon;
	examineIcon = QPixmap( examine_xpm );
	diskIcon = QPixmap( disk_xpm );
	zoominIcon = QPixmap( zoomin_xpm );
	zoomoutIcon = QPixmap( zoomout_xpm );
	clearIcon = QPixmap( clear_xpm );

	box = new QVBox( this );
	setCentralWidget( box );

	tools = new QHBox( box, "tools" );

	blank = new QLabel( "",tools, 0 );
	blank->setPixmap( clip_xpm );
	tools->setStretchFactor( blank, 10 );

	exportButton = new QToolButton( tools,0);
	exportButton->setIconSet(examineIcon);
	exportButton->setTextLabel("Examine...");

	saveButton = new QToolButton(tools,0);
	saveButton->setIconSet(diskIcon);
	saveButton->setTextLabel("Save");

	zoomInButton = new QToolButton(tools,0);
	zoomInButton->setIconSet(zoominIcon);
	
	zoomInButton->setTextLabel("Zoom in");

	zoomOutButton = new QToolButton(tools,0);
	zoomOutButton->setIconSet(zoomoutIcon);
	zoomOutButton->setTextLabel("Zoom out");

	clearButton = new QToolButton(tools,0);
	clearButton->setIconSet(clearIcon);
	clearButton->setTextLabel("Clear");

	edit = new QTextEdit( box, "edit" );
	edit->setVScrollBarMode(QScrollView::AlwaysOn );
	edit->setHScrollBarMode(QScrollView::AlwaysOn );
	edit->setFont( QFont( "Monaco", 12 ));
	edit->setWordWrap( QTextEdit::NoWrap );
	edit->setTextFormat(PlainText);

        QPalette edit_box = edit->palette();

        QColorGroup cg = edit_box.active();
        cg.setColor(QColorGroup::Base, BASE );
        cg.setColor(QColorGroup::Text, TEXT );
        cg.setColor(QColorGroup::Highlight, HIGHLIGHT );
        edit_box.setBrush(QColorGroup::Text, TEXT );
        edit_box.setBrush(QColorGroup::Base, BASE );
        edit_box.setActive(cg);
        edit->setPalette(edit_box);

	setMinimumWidth( 225 );
	setMinimumHeight( 200 );

        setMaximumWidth( o->desktop()->width()-50 );
        setMaximumHeight( o->desktop()->height()-upper_bound-50);

        setGeometry(0,550,250,200);

	connect( exportButton, SIGNAL( clicked() ),
		this, SLOT( exportText()) );
	connect( saveButton, SIGNAL( clicked() ),
		this, SLOT( save() ) );
	connect( clearButton, SIGNAL( clicked() ),
		edit, SLOT( clear() ) );
	connect( zoomInButton, SIGNAL( clicked()),
		edit, SLOT( zoomIn() ) );
	connect( zoomOutButton, SIGNAL( clicked()),
		edit, SLOT( zoomOut() ) );
}

ClipBoard::~ClipBoard()
{
	exportButton->~QToolButton();
	saveButton->~QToolButton();
	zoomInButton->~QToolButton();
	zoomOutButton->~QToolButton();
	clearButton->~QToolButton();
	blank->~QLabel();
	tools->~QHBox();
	edit->~QTextEdit();
	box->~QVBox();
}

void ClipBoard::exportText()
{
	if (edit->text().isEmpty())
		return;

	emit exportString( edit->text() );	
}

void ClipBoard::save()
{
	saving = TRUE;

	QString f = QFileDialog::getSaveFileName(
		"Clipboard", QString::null, this );

	if (f.isEmpty())
	{
		QMessageBox::information( this, "Orb", "Save cancelled.");
		return;
	}

	if (!f.endsWith(".txt"))
		f.append(".txt");


	QFile file( f );
	if ( file.open( IO_WriteOnly ) )
	{
		QTextStream stream( &file );
		stream << edit->text();
	}
	else	QMessageBox::information( this, "Orb",
			"Save failed." );

	saving = FALSE;
}

void ClipBoard::hideEvent( QHideEvent *e )
{
        emit closed();
}

void ClipBoard::resizeEvent( QResizeEvent *e )
{
//         if (y() < upper_bound )
//         {
//                 setGeometry(x(),upper_bound,width(),height() );
//                 repaint();
//         }
} 


void ClipBoard::moveEvent( QMoveEvent *e )
{
	if (saving)
		return;

// 	if (x() < 25 )
// 		move( 0, y() );
}  
