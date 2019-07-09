/* CINEMA 4D SDK EXAMPLE CINEMA 4D SDK EXAMPLE CINEMA 4D SDK EXAMPLE
 *
 * File:    IrrlichtThread.cpp
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

#include "IrrlichtThread.h"

/*
 */
void IrrlichtThread::Main() {
	
	Init();

	BaseDocument *doc = (BaseDocument*)docToView->ForceGetLink();

	AutoAlloc<AtomArray> selection;
	doc->GetActiveObjects(selection, GETACTIVEOBJECTFLAGS::NONE);

	// add objects to scenetree
	for (Int32 i = 0; i < selection->GetCount(); i++) {
		C4DAtom *atom = selection->GetIndex(i);
		if (atom->IsInstanceOf(Obase)) {
			DoRecursion((BaseObject*)atom);
		}
		else if (atom->IsInstanceOf(Olight)) {
			BaseObject *op = (BaseObject*)atom;
			AddLight(op);
		}
	}
	
	// run a scene
	Run();
}

/*
 */
void IrrlichtThread::DoRecursion(BaseObject *op) {

	BaseObject *tp = op->GetDeformCache();
	if (tp) {
		DoRecursion(tp);
	}
	else {
		tp = op->GetCache(NULL);
		if (tp) {
			DoRecursion(tp);
		}
		else {
			if (!op->GetBit(BIT_CONTROLOBJECT)) {
				if (op->IsInstanceOf(Opolygon)) {
					AddObject(op);
				}
			}
		}
	}

	for (tp = op->GetDown(); tp; tp = tp->GetNext()) {
		DoRecursion(tp);
	}
}

/*
 */
void IrrlichtThread::AddLight(BaseObject *op) {
	// TODO
}

/*
 */
void IrrlichtThread::AddObject(BaseObject *op) {
	
	if (!op) return;

	// find the polygon object and the uv sets if they exits
	BaseDocument *doc = GetActiveDocument();
	PolygonObject *po = ToPoly(op);

	UVWTag *uvw_a = (UVWTag*)po->GetTag(Tuvw, 0);
	UVWTag *uvw_b = (UVWTag*)po->GetTag(Tuvw, 1);

	// setup irrlicht scene manager and video driver
	irr::scene::ISceneManager *scene_manager = device->getSceneManager();
	irr::video::IVideoDriver *driver = device->getVideoDriver();
	
	// create mesh in irrlicht
	irr::scene::SMesh *mesh = new irr::scene::SMesh();
	mesh->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, FALSE);

	// if there are 2 uv sets then we should have 2 materials and 2 textures
	// first texture is diffuse color
	// second texture is lightmap
	if (uvw_a && uvw_b) {
		
		irr::scene::SMeshBufferLightMap *sbuffer = new irr::scene::SMeshBufferLightMap();
		
		const Vector *vp = po->GetPointR();
		const CPolygon *cp = po->GetPolygonR();

		// count how many do we have polygons indicies / points
		Int32 indices_count = 0;
		for (Int32 index = 0; index < po->GetPolygonCount(); index++) {
			const CPolygon &p = cp[index];

			if (p.c == p.d) {
				indices_count += 3;
			} else {
				indices_count += 4;
			}
		}

		// reallocate sbuffer containters
		sbuffer->Indices.reallocate(indices_count);
		sbuffer->Vertices.reallocate(indices_count);

		// get normal tag if it exists. If doesn't exist we will create normals ourselfs
		VariableTag *normal_tag = (VariableTag*)po->GetTag(Tnormal);
		//SWORD *normalData = NULL

		if (normal_tag) {
			//normalData = (SWORD*)normal_tag->GetDataAdressW();
		}

		// add all polygons to the scene
		// make sure to cut up any quads into triangles as we do so
		Vector vA, vB, vC, cD;
		Vector normalA, normalB, normalC, normalD;

		irr::u16 vert_count = 0;
		for (Int32 index = 0; index < po->GetPolygonCount(); ++index) {
			const CPolygon &p = cp[index];

			// push vertex points
			sbuffer->Indices.push_back(vert_count++);
			sbuffer->Indices.push_back(vert_count++);
			sbuffer->Indices.push_back(vert_count++);

			// set uvs
			UVWStruct uvs_a;
			if (uvw_a) {
				uvs_a = uvw_a->GetSlow(index);
			}

			UVWStruct uvs_b;
			if (uvw_b) {
				uvs_b = uvw_b->GetSlow(index);
			}

			/*
			if (normalData) {
				GetNormal(normalData+12*a+0,normalA);
				GetNormal(normalData+12*a+3,normalB);
				GetNormal(normalData+12*a+6,normalC);
				GetNormal(normalData+12*a+9,normalD);
			} else {
				Vector normal = (vp[p.b] - vp[p.a]) % (vp[p.c] - vp[p.a]);
				normal.Normalize();
				normalA = normalB = normalC = normal;
			}
			*/

			// Push A polygon, A normal, A + B UVW
			sbuffer->Vertices.push_back(
				irr::video::S3DVertex2TCoords
				(
					vp[p.a].x, vp[p.a].y, vp[p.a].z,
					normalA.x, normalA.y, normalA.z,
					irr::video::SColor(255, 255, 255, 255),
					uvs_a.a.x, uvs_a.a.y,
					uvs_b.a.x, uvs_b.a.y
				)
			);

			// Push B polygon, B normal, A + B UVW
			sbuffer->Vertices.push_back(
				irr::video::S3DVertex2TCoords
				(
					vp[p.b].x, vp[p.b].y, vp[p.b].z,
					normalB.x, normalB.y, normalB.z,
					irr::video::SColor(255, 255, 255, 255),
					uvs_a.b.x, uvs_a.b.y,
					uvs_b.b.x, uvs_b.b.y
				)
			);

			// Push C polygon, C normal, A + B UVW
			sbuffer->Vertices.push_back(
				irr::video::S3DVertex2TCoords
				(
					vp[p.c].x, vp[p.c].y, vp[p.c].z,
					normalC.x, normalC.y, normalC.z,
					irr::video::SColor(255, 255, 255, 255),
					uvs_a.c.x, uvs_a.c.y,
					uvs_b.c.x, uvs_b.c.y
				)
			);

			
		} 

		sbuffer->recalculateBoundingBox();
		mesh->addMeshBuffer(sbuffer);
		sbuffer->drop();
	} 

	// no second UV set. Virtually the same code as above
	else if (uvw_a && !uvw_b) {

		irr::scene::SMeshBuffer *sbuffer = new irr::scene::SMeshBuffer();
		const Vector *vp = po->GetPointR();
		const CPolygon *cp = po->GetPolygonR();

		Int32 indices_count = 0;
		for (Int32 index = 0; index < po->GetPolygonCount(); ++index) {
			const CPolygon &p = cp[index];

			if (p.c == p.d) {
				indices_count += 3;
			}
			else {
				indices_count += 4;
			}
		}

		// reallocate sbuffer containters
		sbuffer->Indices.reallocate(indices_count);
		sbuffer->Vertices.reallocate(indices_count);

		VariableTag *normal_tag = (VariableTag*)po->GetTag(Tnormal);
		// SWORD *normalData = NULL
		if (normal_tag) {
			//normalData = (SWORD*)normal_tag->GetDataAddressW();
		}

		// add all polygons to the scene
		// make sure to cut up any quads into triangles as we do so
		Vector vA, vB, vC, vD;
		Vector normalA, normalB, normalC, normalD;

		irr::u16 vert_count = 0;
		for (Int32 index = 0; index < po->GetPolygonCount(); ++index) {
			const CPolygon &p = cp[index];

			// push vertex points
			sbuffer->Indices.push_back(vert_count++);
			sbuffer->Indices.push_back(vert_count++);
			sbuffer->Indices.push_back(vert_count++);

			// set uvs
			UVWStruct uvs_a;
			if (uvw_a) {
				uvs_a = uvw_a->GetSlow(index);
			}

			UVWStruct uvs_b;
			if (uvw_b) {
				uvs_b = uvw_b->GetSlow(index);
			}

			/*
			if (normalData) {
				GetNormal(normalData+12*a+0,normalA);
				GetNormal(normalData+12*a+3,normalB);
				GetNormal(normalData+12*a+6,normalC);
				GetNormal(normalData+12*a+9,normalD);
			} else {
				Vector normal = (vp[p.b] - vp[p.a]) % (vp[p.c] - vp[p.a]);
				normal.Normalize();
				normalA = normalB = normalC = normal;
			}
			*/

			// Push A polygon, A normal, A + B UVW
			sbuffer->Vertices.push_back(
				irr::video::S3DVertex
				(
					vp[p.a].x, vp[p.a].y, vp[p.a].z,
					normalA.x, normalA.y, normalA.z,
					irr::video::SColor(255, 255, 255, 255),
					uvs_a.a.x, uvs_a.a.y
				)
			);

			// Push B polygon, B normal, A + B UVW
			sbuffer->Vertices.push_back(
				irr::video::S3DVertex
				(
					vp[p.b].x, vp[p.b].y, vp[p.b].z,
					normalB.x, normalB.y, normalB.z,
					irr::video::SColor(255, 255, 255, 255),
					uvs_a.b.x, uvs_a.b.y
				)
			);

			// Push C polygon, C normal, A + B UVW
			sbuffer->Vertices.push_back(
				irr::video::S3DVertex
				(
					vp[p.c].x, vp[p.c].y, vp[p.c].z,
					normalC.x, normalC.y, normalC.z,
					irr::video::SColor(255, 255, 255, 255),
					uvs_a.c.x, uvs_a.c.y
				)
			);


			// if we have a quad we need to export a second triangle
			if (p.c != p.d) {
				/*
				if (!normalData) {
					Vector normal = (vp[p.c] - vp[p.a]) % (vp[p.d] - vp[p.a]);
					normal.Normalize();
					normalA = normalC = normalD = normal;
				}
				*/

				sbuffer->Indices.push_back(vert_count++);
				sbuffer->Indices.push_back(vert_count++);
				sbuffer->Indices.push_back(vert_count++);

				// Push A polygon, A normal, A + B UVW
				sbuffer->Vertices.push_back(
					irr::video::S3DVertex
					(
						vp[p.a].x, vp[p.a].y, vp[p.a].z,
						normalA.x, normalA.y, normalA.z,
						irr::video::SColor(255, 255, 255, 255),
						uvs_a.a.x, uvs_a.a.y
					)
				);

				// Push C polygon, C normal, A + B UVW
				sbuffer->Vertices.push_back(
					irr::video::S3DVertex
					(
						vp[p.c].x, vp[p.c].y, vp[p.c].z,
						normalC.x, normalC.y, normalC.z,
						irr::video::SColor(255, 255, 255, 255),
						uvs_a.c.x, uvs_a.c.y
					)
				);

				// Push D polygon, D normal, A + B UVW
				sbuffer->Vertices.push_back(
					irr::video::S3DVertex
					(
						vp[p.d].x, vp[p.d].y, vp[p.d].z,
						normalD.x, normalD.y, normalD.z,
						irr::video::SColor(255, 255, 255, 255),
						uvs_a.d.x, uvs_a.d.y
					)
				);
			}
		}

		sbuffer->recalculateBoundingBox();
		mesh->addMeshBuffer(sbuffer);
		sbuffer->drop();
	}

	// setup the mes as a scene node and add it to scene manager in irrlicht
	mesh->recalculateBoundingBox();
	irr::scene::IMeshSceneNode *node = scene_manager->addMeshSceneNode(mesh);

	// setup position of the mesh in the scene
	Vector pos = op->GetAbsPos(); /* *Vector(op->GetUpMg().off.x, op->GetUpMg().off.y, op->GetUpMg().off.z);*/
	node->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));

	Vector rot = op->GetAbsRot(); /* *Vector(op->GetUpMg().off.x, op->GetUpMg().off.y, op->GetUpMg().off.z);*/
	node->setRotation(irr::core::vector3df(
		rot.x, rot.y, rot.z
	));

	Vector scale = op->GetAbsScale();/* * Vector(op->GetUpMg().off.x, op->GetUpMg().off.y, op->GetUpMg().off.z);*/
	node->setScale(irr::core::vector3df(scale.x, scale.y, scale.z));

	// tets its material type if it has a light map or not
	if (uvw_a && !uvw_b) {
		node->setMaterialType(irr::video::EMT_SOLID);
	}
	else if (uvw_a && uvw_b) {
		node->setMaterialType(irr::video::EMT_LIGHTMAP_M4);
	}

	// finally we copy all textures
	// TODO: optimise a texture duplicates (not checking if we used a texture once)
	if (node) {
		Filename doc_path = GetActiveDocument()->GetDocumentPath();

		Int32 tex_count = 0;

		TextureTag *tex = (TextureTag*)op->GetTag(Ttexture, tex_count);
		while (tex) {
			BaseMaterial *mat = tex->GetMaterial();
			if (mat) {
				BaseChannel *diffuse_color = mat->GetChannel(CHANNEL_COLOR);
				if (diffuse_color) {
					PaintTexture *theTexture = PaintTexture::GetPaintTextureOfBaseChannel(doc, diffuse_color);
					if (theTexture) {
						PaintLayer *layer = theTexture->GetFirstLayer();
						PaintLayerBmp *paintBmp = (PaintLayerBmp*)layer;

						irr::video::IImage *image = driver->createImage(irr::video::ECF_R8G8B8, irr::core::dimension2d<irr::u32>(paintBmp->GetBw(), paintBmp->GetBh()));

						Int32 x, y;
						for (x = 0; x < paintBmp->GetBw(); x++) {
							for (y = 0; y < paintBmp->GetBh(); y++) {
								PIX pix_buff[3];
								paintBmp->GetPixelCnt(x, y, 1, pix_buff, COLORMODE::RGB, PIXELCNT::NONE);
								image->setPixel(x, y, irr::video::SColor(255, pix_buff[0], pix_buff[1], pix_buff[2]));
							}
						}

						irr::video::ITexture *newTex = driver->addTexture(mat->GetName().GetCStringCopy(), image);
						node->setMaterialTexture(tex_count, newTex);
					}
				}
			}

			tex = (TextureTag*)op->GetTag(Ttexture, ++tex_count);
		}

	}
}

/*
 */
void IrrlichtThread::Init() {

	device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, false, this);
	if (!device) return;

	device->setWindowCaption(L"Cinema 4D Irrlicht Demo");

	irr::video::IVideoDriver *driver = device->getVideoDriver();
	irr::scene::ISceneManager *scene_manager = device->getSceneManager();

	// HACK: turn off all clipping
	for (int i = 0; i < 6; i++) {
		driver->enableClipPlane(i, false);
	}

	irr::scene::ICameraSceneNode *camera_node = scene_manager->addCameraSceneNodeMaya();
	camera_node->setTarget(irr::core::vector3df(0, 0, 0));
	camera_node->setPosition(irr::core::vector3df(200,200,200));

	scene_manager->setAmbientLight(irr::video::SColorf(1.0f, 1.0f, 1.0f));
	device->getCursorControl()->setVisible(true);
}

/*
 */
void IrrlichtThread::Run() {
	
	if (!device) return;

	irr::video::IVideoDriver *driver = device->getVideoDriver();
	irr::scene::ISceneManager *scene_manager = device->getSceneManager();
	irr::gui::IGUIEnvironment *gui = device->getGUIEnvironment();

	while (device->run()) {
		if (device->isWindowActive()) {
			driver->beginScene(true, true, irr::video::SColor(255, 100, 101, 140));
				scene_manager->drawAll();
				gui->drawAll();
			driver->endScene();
		}
		else {
			device->yield();
		}

		device->sleep(1);
	}

	device->drop();
}

/*
 */
bool IrrlichtThread::OnEvent(const irr::SEvent& event) {
	if ((event.EventType == irr::EET_KEY_INPUT_EVENT) /*&& (event.KeyInput.PressedDown() == false)*/) {
		if (event.KeyInput.Key == irr::KEY_ESCAPE) {
			if (device) {
				device->closeDevice();
				return true;
			}
		}
	}

	return false;
}

/*
 */
const Char* IrrlichtThread::GetThreadName() {
	return "C4DIrrlichtDemo Thread";
}