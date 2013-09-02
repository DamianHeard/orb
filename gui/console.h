#ifndef CONSOLE_H
#define CONSOLE_H

#include <qtextstream.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include "kernel.h"

#define NL(f)   (f==0) ? 'u' : ((f==1) ? 'v' : ((f==2) ? 'w' : 'x'))

class Console: public QWidget
{
	Q_OBJECT

public:
	Console(Triangulation *m, QWidget *parent);
	~Console();
	void			saveTriangulation( QTextStream &stream, bool ce, bool st, bool curves );
	void			setTriangulation( Triangulation *m );

private:
	Triangulation		*manifold,
				*undoManifold;
	QGridLayout		*grid;
	QTextEdit		*outputBox;
	QToolButton		*clrButton,
				*triButton,
				*tetButton,
				*symButton,
				*gtButton,
				*fgButton,
				*mgButton,
				*lsButton,
				*undoButton,
				*inButton,
				*outButton;
	QPopupMenu		*lsMenu,
				*fgMenu,
				*mgMenu,
				*symMenu,
				*gtMenu,
				*triMenu,
				*tetMenu;
	double			cut_off,
				tile_radius,
				vertex_epsilon;

	int			fr_id,
				m_id,
				hom_id,
				simp_id,
				few_id,
				adjm_id,
				adjl_id,
				o31_id,
				psl_id,
				b_id,
				det_id,
				tr_id,
				trtr_id,
				hol_id,
				cut_id,
				tile_id,
				vertex_id,
				da_id,
				el_id,
				vgm_id,
				nbr_id,
				ae_id,
				v_id,
				sa_id,
				mt_id,
				ca_id,
				sr_id;

	void			printTri();
	GroupPresentation	*computeFundamentalGroup();

private slots:
	void			clr();
	void			tri();
	void			tet();
	void			sym();
	void			can();
	void			rnd();
	void			sim();
	void			rem();
	void			daSlot();
	void			elSlot();
	void			vgmSlot();
	void			nbrSlot();
	void			saSlot();
	void			caSlot();
	void			mtSlot();
	void			aeSlot();
	void			vSlot();
	void			gt();
	void			fg();
	void			mg();
	void			ls();
	void			printCanonizeInfo();
	void			cutOffSlot();
	void			tileRadiusSlot();
	void			vertexEpsilonSlot();
	void			fullRigorSlot();
	void			multiplicitiesSlot();
	void			homSlot();
	void			coverHomSlot();
	void			simpSlot();
	void			fewSlot();
	void			adjmSlot();
	void			adjlSlot();
	void			o31Slot();
	void			pslSlot();
	void			bSlot();
	void			srSlot();
	void			detSlot();
	void			trSlot();
	void			trtrSlot();
	void			holSlot();

signals:
	void			triangulationChanged();
	void			retriangulation();

};

#endif
