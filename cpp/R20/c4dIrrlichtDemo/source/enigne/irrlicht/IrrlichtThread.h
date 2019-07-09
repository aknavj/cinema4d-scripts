/* CINEMA 4D SDK EXAMPLE CINEMA 4D SDK EXAMPLE CINEMA 4D SDK EXAMPLE
 *
 * File:    IrrlichtThread.h
 * Desc:    Irrlicht 3D Engine Thread
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

#ifndef __IRRLICHT_THREAD_H__
#define __IRRLICHT_THREAD_H__

#include <irrlicht.h>		// irrlicht include
#include <IEventReceiver.h>	// irrlicht include
#include "c4d_thread.h"		// c4d thread
#include "c4d.h"

/*
 */
//===========================================================================
// To run the Irrlicht scene we run it in our own thread.
// This also has an IEventReciever so that the user can press the Escape key to exit the scene
//===========================================================================
class IrrlichtThread : public C4DThread, public irr::IEventReceiver {

	public:
		IrrlichtThread() { }
		virtual ~IrrlichtThread() { }

		virtual void Main(void);
		virtual const Char *GetThreadName(void);

	private:
		virtual bool OnEvent(const irr::SEvent& event);

		void Init();
		void DoRecursion(BaseObject *op);
		void AddObject(BaseObject *obj);
		void AddLight(BaseObject *obj);
		void Run();

	public:
		AutoAlloc<BaseLink> docToView;

	private:
		void ViewObject(BaseObject *op);
		irr::IrrlichtDevice *device;
};

#endif /* __IRRLICHT_THREAD_H__ */