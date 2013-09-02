#ifndef CLIP_BOARD_H
#define CLIP_BOARD_H

#include <qlabel.h>
#include <qmainwindow.h>
#include <qwidget.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qtextedit.h>
using namespace std;

class Organizer;


class ClipBoard : public QMainWindow
{
    Q_OBJECT

public:
	ClipBoard(Organizer *o, int u); 
	~ClipBoard();

private:
	bool		saving;
	void		moveEvent( QMoveEvent *e );
	int		upper_bound;
	QLabel		*blank;
	QHBox		*tools;
	QVBox		*box;
	QToolButton	*exportButton,
			*saveButton,
			*zoomInButton,
			*zoomOutButton,
			*clearButton;
	QTextEdit	*edit;
	void            resizeEvent( QResizeEvent *e );

private slots:
	void		exportText();
	void		save();
	void		hideEvent( QHideEvent *e );

signals:
	void		closed();    
	void		exportString( QString );

};

#endif
