/* CINEMA 4D SDK EXAMPLE CINEMA 4D SDK EXAMPLE CINEMA 4D SDK EXAMPLE
 *
 * File:    IrrlichtViewer.cpp
 * Desc:    Irrlicht 3D Engine Application Viewer
 * Version: 1.0
 * Author:  Ondrej Vanka '@aknavj' <ondrej.vanka@gmail.com>
 *
 * This file is part of the C4DIrrlichtDemo repository from
 * github.com/aknavj.
 *
 * This code serves as an example for Cinema 4D SDK C++ API
 * and belong to the community.
 *
 * Original author of the code @KBAR from <tools4polygons.blogspot.com>
 */
 // Changelog from @aknavj:
 // -----------------------
 // 2018:	All code was fixed and it's working correctly!
 //			No more missing stuff from bad HTML formating.
 //			Fixed missing or incomplete cycles and logic statements
 //
 //	2018:	Working under Maxon Cinema 4D R20 + Irrlicht 1.8.x 64-bit version
 // 2009:	@KBAR Irrlicht Viewer and Quake 3 BSP Map Importer plugin for Cinema 4D R11.5
 //			using Irrlicht 1.6.x 32-bit version.

#include "IrrlichtViewer.h"

/*
 */
Bool RunIrrlichtCommandData::Execute(BaseDocument *doc) {
	irrC4DThread.docToView->SetLink(doc);
	return irrC4DThread.Start();
}