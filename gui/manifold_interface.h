#ifndef MANIFOLD_INTERFACE_H
#define MANIFOLD_INTERFACE_H

#include <qwidget.h>
#include <qworkspace.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qtable.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qcombobox.h>
#include <qwidgetstack.h> 
#include <qcheckbox.h>
#include <qpopupmenu.h>

#include "kernel.h"
#include "diagram_window.h"

class Console;
class DiagramCanvas;
class Organizer;

class MyTable : public QTable
{
public:
        MyTable(  int numRows, int numCols, QWidget * parent = 0, const char * name = 0 )
		: QTable(numRows,numCols,parent,name) {}

protected:
	void activateNextCell(){
				setCurrentCell((currentRow()+1) % numRows(),currentColumn());
				};
};

class ManifoldInterface: public QWidget
{
	Q_OBJECT

public:
	ManifoldInterface(int u, bool needsSave, QString path, Triangulation *manifold,
			DiagramCanvas *d, DiagramWindow *dw, Organizer *o );
	~ManifoldInterface();
	Triangulation *m();
	bool		isSaved();

private:
	QString		path, fileName;
	bool		saved;
	int		upper_bound;
	void            resizeEvent( QResizeEvent *e );
	Organizer	*organizer;
	Triangulation	*manifold, *undoManifold1, *undoManifold2;
	QGridLayout	*grid,
			*g;
	QPushButton	*compute;
	QLabel		*viewLabel,
			*solutionLabel;
	QComboBox	*viewCombo;	
	MyTable		*edges;
	QWidgetStack	*stack;
	Console		*console;
	QVBox		*frame1;
	QHBox		*f1,
			*f2;
	QLabel		*status;
	QToolButton	*viewButton,
			*draw,
			*saveButton,
			*optionsButton;

	QPopupMenu	*optionsMenu;

	DiagramWindow	*diagramWindow;
	DiagramCanvas	*canvas;
	QCheckBox	*checkBox1,
			*checkBox2,
			*manual;

private slots:
	void	pruneSlot();
	void	drillSlot();
	void	closeEvent( QCloseEvent *e );
	void	highlightChanged();
	void	canvasSelectionSlot( int arc_id );
	void	structureSlot();
	void	prepareForUndoSlot();
	void	retriangulationSlot();
	void	unattemptedSlot();
	void	updateValueSlot( int r, int c);
	void	updateLabelSlot();
	void	showSlot(const QString &front);
	void	updateStatusSlot( const QString &message);
	void	drawSlot();
	void	switchStackSlot();
	void	raiseConsoleSlot();
	void	raiseDiagramSlot();
	void	highlightDiagramEdgeSlot();
	void	coverSlot();
	void	dehnSlot();
	void	listSlot();
	void	undoSlot();
	void	saveAsSlot();
	void	saveSlot();
	void	SnapPeaSlot();

public slots:
	void	makeActiveSlot();
	bool	promptSaveSlot();
	void	chooseSaveSlot();

signals:
	void	structureChanged();
	void	closed(ManifoldInterface* const);
	void	nameChange();
};

#endif
