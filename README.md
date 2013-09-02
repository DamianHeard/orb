orb
---

Orb is a computer program that can find hyperbolic structures on a large class of hyperbolic 3-orbifolds and 3-manifolds. It can start with a projection of a graph embedded in the 3-sphere, and produce and simplify a triangulation with some prescribed subgraph as part of the 1-skeleton and the remainder of the graph drilled out. It enables computation of hyperbolic structures on knot complements, graph complements and orbifolds whose underlying space is the 3-sphere minus a finite number of points.

The code was created by modifying Jeff Weeks' computer program SnapPea.

Compiling
---------
Before you can compile Orb you needed to ensure Qt (version 3.2.x or later) is installed. You can then compile Orb by going to the directory

```
$ # Assuming you start in the orb directory
$ cd snappea/code
$ make
```

This will build the 'Orb' library.  As the directory names suggests this is just a modified SnapPea kernel.  The code in here is pretty hacky and a lot of it is redundant.  You will probably receive a lot of warnings like "unused variable" but nothing major.  Now go to the directory

```
$ cd ../..
$ qmake -o Makefile Orb.pro
$ # This should build a Makefile for Orb's GUI
$ make
```

This builds the GUI for Orb. (This uses the Qt library so if Qt isn't installed properly you'll find out here.)  If all goes well the build will be complete and you can run Orb entering

```
$ open Orb.app
```
You can avoid using the terminal to run Orb by keeping its icon in the dock.

Funding
-------
This work was partially supported by grants from the Australian Research Council and an
Australian Postgraduate Award.
