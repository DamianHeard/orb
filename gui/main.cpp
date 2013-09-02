#include <qapplication.h>
#include "organizer.h"

int main(
        int     argc,
        char    **argv )
{
        QApplication a( argc, argv );

        Organizer *o = new Organizer(a.desktop());
        a.setMainWidget(o);
        o->showMaximized();
        a.connect( o, SIGNAL(destroyed()), &a, SLOT(quit()) );

        return a.exec();
}

