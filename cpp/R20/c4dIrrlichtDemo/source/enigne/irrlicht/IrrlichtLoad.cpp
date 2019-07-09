/* CINEMA 4D SDK EXAMPLE CINEMA 4D SDK EXAMPLE CINEMA 4D SDK EXAMPLE
 *
 * File:    IrrlichtLoad.cpp
 * Desc:    Irrlicht Quake 3 BSP Loader
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

#include "IrrlichtLoad.h"	// plugin include
#include <irrlicht.h>		// irrlicht include
#include <IVideoDriver.h>	// irrlicht include
#include <ISceneManager.h>	// irrlicht include

/*
 */
Bool IrrlichtLoadBSP(BaseDocument *doc, Filename fn, String pk3, String bsp_map) {

	// init Irrlicht video device
	irr::IrrlichtDevice *device = irr::createDevice(
		irr::video::EDT_NULL, 
		irr::core::dimension2d<irr::u32>(640, 480), 
		16, 
		false, 
		false, 
		false
	);
	if (!device) return false;

	// get video driver
	irr::video::IVideoDriver *driver = device->getVideoDriver();
	
	// get scene manager
	irr::scene::ISceneManager *scene_manager = device->getSceneManager();

	// get gui env
	irr::gui::IGUIEnvironment *gui = device->getGUIEnvironment();

	// private media directory for filesystem path
	//Filename fn = GeGetPluginPath() + Filename(String("media"));

	char path[1024];
	fn.GetString().GetCString(path, 1024);

	// setup filesystem init for irrlicht (where to lookup for data)
	device->getFileSystem()->addFolderFileArchive(path);

	// To display the Quake 3 map, we first need to load it.Quake 3 maps
	// are packed into.pk3 files, which are nothing other than.zip files.
	// So we add the.pk3 file to our FileSystem.After it was added,
	// we are able to read from the files in that archive as they would
	// directly be stored on disk.

	device->getFileSystem()->addZipFileArchive(pk3.GetCStringCopy());
	irr::scene::IQ3LevelMesh *mesh = (irr::scene::IQ3LevelMesh*)scene_manager->getMesh(bsp_map.GetCStringCopy());
	
	irr::scene::ISceneNode *node = NULL;
	if (mesh) {

		irr::scene::IMesh *geometry = mesh->getMesh(irr::scene::quake3::E_Q3_MESH_GEOMETRY);

		for (irr::u32 index = 0; index < geometry->getMeshBufferCount(); index++) {
			
			irr::scene::IMeshBuffer *m_buff = geometry->getMeshBuffer(index);
			irr::u32 num_verts = m_buff->getVertexCount();
			irr::u32 num_index = m_buff->getIndexCount();

			// find polygon count
			irr::u32 num_poly = num_index / 3;

			// create polygon object
			PolygonObject *po = (PolygonObject*)BaseObject::Alloc(Opolygon);
			po->ResizeObject(num_verts, num_poly);

			Vector *vp = po->GetPointW();
			CPolygon *cp = po->GetPolygonW();

			// create normal tags
			//VariableTag *nt = NULL;
			
			// create materials and textures
			if (m_buff) {
				UVWTag *uvw_a = NULL;
				UVWTag *uvw_b = NULL;

				Vector tex_trans;
				Vector tex_rot;
				Vector tex_scl;

				irr::video::SMaterial &material = m_buff->getMaterial();
				Int32 mat_index = 0;

				for (mat_index = irr::video::MATERIAL_MAX_TEXTURES -1; mat_index >= 0; mat_index--) {

					irr::video::ITexture *ptr_tex = material.getTexture(mat_index);
					if (ptr_tex) {
						Material *mat = Material::Alloc();
						doc->InsertMaterial(mat);
						
						if (mat_index == 0) {
							// first material will use UVs from the first UV Tag
							uvw_a = (UVWTag*)po->MakeVariableTag(Tuvw, num_poly);
						} else if (mat_index == 1 && m_buff->getVertexType() == irr::video::EVT_2TCOORDS) {
							// second material will use UVs from the second UV Tag
							uvw_b = (UVWTag*)po->MakeVariableTag(Tuvw, num_poly);
						}
						
						const irr::core::dimension2d<irr::u32> size = ptr_tex->getOriginalSize();
						const irr::core::stringc &name = ptr_tex->getName();

						// Set the data for the texture
						BaseContainer bc;
						bc.SetInt32(TEXTURE_FILEFORMAT, FILTER_BMP);
						bc.SetInt32(TEXTURE_WIDTH, size.Width);
						bc.SetInt32(TEXTURE_HEIGHT, size.Height);
						bc.SetBool(TEXTURE_SAVE_IMMEDIATLY, 0);
						bc.SetInt32(TEXTURE_MODE, (Int32)COLORMODE::RGBw); // COLORMODE::RGB

						// create a new material in Cinema 4D
						mat->SetChannelState(CHANNEL_COLOR, TRUE);
						mat->SetName(Filename(name.c_str()).GetString());
						
						// get the color channel for the new material
						BaseChannel *color_channel = mat->GetChannel(CHANNEL_COLOR);

						// assign the texture name to the material so the material can reference the image
						BaseContainer bla;
						bla.SetString(BASECHANNEL_TEXTURE, Filename(name.c_str()).GetString());
						color_channel->SetData(bla);

						// see if we have already loaded this texture
						PaintTexture *newTexture = NULL;
						GeListHead *head = PaintTexture::GetPaintTextureHead();
						if (head) {
							PaintTexture *headTexture = (PaintTexture*)head->GetFirst();
							while (headTexture) {
								if (headTexture->GetFilename() == Filename(name.c_str())) {
									newTexture = headTexture;
									break;
								}
								headTexture = (PaintTexture*)headTexture->GetNext();
							}
						}

						if (!newTexture) {
							newTexture = PaintTexture::CreateNewTexture(name.c_str(), bc);

							irr::core::matrix4 &texlMatrix = material.getTextureMatrix(mat_index);
							irr::core::vector3df t = texlMatrix.getTranslation();
							irr::core::vector3df r = texlMatrix.getRotationDegrees();
							irr::core::vector3df s = texlMatrix.getScale();

							tex_trans.x = t.X;
							tex_trans.y = t.Y;
							tex_trans.z = t.Z;

							tex_rot.x = RadToDeg(Float32(r.X));
							tex_rot.y = RadToDeg(Float32(r.Y));
							tex_rot.z = RadToDeg(Float32(r.Z));

							tex_scl.x = s.X;
							tex_scl.y = s.Y;
							tex_scl.z = s.Z;

							irr::u32 bytesPerRow = ptr_tex->getPitch();
							irr::video::ECOLOR_FORMAT format = ptr_tex->getColorFormat();

							unsigned char *pixeldata = (unsigned char*)ptr_tex->lock();

							// use pointer for the data but dont delete it whet the image is dropped
							irr::video::IImage *tmpImage = driver->createImageFromData(format, size, pixeldata, true, false);

							// get the first layer for the new image
							PaintLayerBmp *paintbmp = NULL;
							PaintLayer *layer = newTexture->GetFirstLayer();
							paintbmp = (PaintLayerBmp*)layer;
							paintbmp->SetName(String(name.c_str()));

							// write the pixels from the Irrlicht image to the C4D PaintLayerBmp
							irr::u32 x, y;
							for (x = 0; x < (irr::u32)size.Width; x++) {
								for (y = 0; y < (irr::u32)size.Height; y++) {
									irr::video::SColor c = tmpImage->getPixel(x, y);

									PIX pix_buf[3];
									pix_buf[0] = (UChar)c.getRed();
									pix_buf[1] = (UChar)c.getGreen();
									pix_buf[2] = (UChar)c.getBlue();

									paintbmp->SetPixelCnt(x, y, 1, pix_buf, 3, COLORMODE::RGB, PIXELCNT::NONE);
								}
							}

							ptr_tex->unlock();
						}

						// create the texture tag and add it to the polygon object.
						// tell the texture to MIX the color if it has a lightmap (which is the secound material in our case)
						{
							TextureTag *texTag = (TextureTag*)po->MakeTag(Ttexture);
							texTag->SetMaterial(mat);

							BaseContainer t_bc;
							t_bc.SetFloat(TEXTURETAG_SIDE, TEXTURETAG_SIDE_FRONTANDBACK);
							t_bc.SetBool(TEXTURETAG_TILE, TRUE);
							t_bc.SetVector(TEXTURETAG_POSITION, tex_trans);
							t_bc.SetVector(TEXTURETAG_SIZE, tex_scl);
							t_bc.SetVector(TEXTURETAG_ROTATION, tex_rot);
							t_bc.SetInt32(TEXTURETAG_PROJECTION, TEXTURETAG_PROJECTION_UVW);

							if (mat_index == 1) {
								t_bc.SetBool(TEXTURETAG_MIX, true);
							}

							texTag->SetData(t_bc);
						}

						mat->Message(MSG_UPDATE);
						mat->Update(true, true);
					}
				}

				// copy accross the vertex data to the polygon object
				if (m_buff->getVertexType() == irr::video::EVT_STANDARD) {
					irr::video::S3DVertex *vertices = (irr::video::S3DVertex*)m_buff->getVertices();
					
					for (irr::u32 index = 0; index < m_buff->getVertexCount(); index++) {
						const irr::video::S3DVertex &v = vertices[index];
						vp[index].x = v.Pos.X;
						vp[index].y = v.Pos.Y;
						vp[index].z = v.Pos.Z;
					}
				}
				else if (m_buff->getVertexType() == irr::video::EVT_2TCOORDS) {
					irr::video::S3DVertex2TCoords *vertices = (irr::video::S3DVertex2TCoords*)m_buff->getVertices();

					for (irr::u32 index = 0; index < m_buff->getVertexCount(); index++) {
						const irr::video::S3DVertex2TCoords &v = vertices[index];
						vp[index].x = v.Pos.X;
						vp[index].y = v.Pos.Y;
						vp[index].z = v.Pos.Z;
					}
				}
				else if (m_buff->getVertexType() == irr::video::EVT_TANGENTS) {
					irr::video::S3DVertexTangents *vertices = (irr::video::S3DVertexTangents*)m_buff->getVertices();

					for (irr::u32 index = 0; index < m_buff->getVertexCount(); index++) {
						const irr::video::S3DVertexTangents &v = vertices[index];
						vp[index].x = v.Pos.X;
						vp[index].y = v.Pos.Y;
						vp[index].z = v.Pos.Z;
					}
				}

				// 
				switch (m_buff->getIndexType()) {
					case irr::video::EIT_16BIT:
					{
						irr::u16 *indices = (irr::u16*)m_buff->getIndices();
						irr::u32 index;
						irr::u32 num_poly = 0; // poly index

						// some index shenanigans
						for (index = 0; index < m_buff->getIndexCount(); index+=3) {
							cp[num_poly].a = indices[index];
							cp[num_poly].b = indices[index + 1];
							cp[num_poly].d = cp[num_poly].c = indices[index + 2];

							// if its the standart type then we need to cast to S3DVertex
							if (m_buff->getVertexType() == irr::video::EVT_STANDARD && uvw_a) {
								
								irr::video::S3DVertex *vertices = (irr::video::S3DVertex*)m_buff->getVertices();
								UVWStruct uv;

								const irr::video::S3DVertex &va = vertices[indices[index]];
								const irr::video::S3DVertex &vb = vertices[indices[index + 1]];
								const irr::video::S3DVertex &vc = vertices[indices[index + 2]];

								// set the UVs for the first UV set
								uv.a.x = va.TCoords.X;
								uv.a.y = va.TCoords.Y;

								uv.b.x = vb.TCoords.X;
								uv.b.y = vb.TCoords.Y;

								uv.c.x = vc.TCoords.X;
								uv.c.y = vc.TCoords.Y;

								uvw_a->SetSlow(num_poly, uv);

								// Set normals
								// TODO
								// TODO
							}
							else if (m_buff->getVertexType() == irr::video::EVT_2TCOORDS) {
								
								irr::video::S3DVertex2TCoords *vertices = (irr::video::S3DVertex2TCoords*)m_buff->getVertices();
								UVWStruct uv;

								const irr::video::S3DVertex2TCoords &va = vertices[indices[index]];
								const irr::video::S3DVertex2TCoords &vb = vertices[indices[index + 1]];
								const irr::video::S3DVertex2TCoords &vc = vertices[indices[index + 2]];

								// UV A used by Diffuse color
								if (uvw_a) {
									uv.a.x = va.TCoords.X;
									uv.a.y = va.TCoords.Y;

									uv.b.x = vb.TCoords.X;
									uv.b.y = vb.TCoords.Y;

									uv.c.x = vc.TCoords.X;
									uv.c.y = vc.TCoords.Y;

									uvw_a->SetSlow(num_poly, uv);
								}

								// UV B used by lightmap
								if (uvw_b) {
									uv.a.x = va.TCoords2.X;
									uv.a.y = va.TCoords2.Y;

									uv.b.x = vb.TCoords2.X;
									uv.b.y = vb.TCoords2.Y;

									uv.c.x = vc.TCoords2.X;
									uv.c.y = vc.TCoords2.Y;

									uvw_b->SetSlow(num_poly, uv);
								}

								// Set normals
								// TODO
								// TODO
							}
						
							// increment polycount
							num_poly++;
						}

						break;
					}
					case irr::video::EIT_32BIT:
					{
						irr::u32 *indices = (irr::u32*)m_buff->getIndices();
						break;
					}
					default:
					{
						break;
					}
				}

				doc->InsertObject(po, 0, 0);
				po->Message(MSG_UPDATE);
			}
		}
	}

	device->drop();

	EventAdd();

	return TRUE;

}

/*
 */
Bool IrrlichtLoadMD3(BaseDocument *doc, Filename fn) {
	return true;
}

/*
 */
Bool IrrlichtLoadCommand::Execute(BaseDocument *doc) {
	return IrrlichtLoadBSP(doc, GeGetPluginPath() + Filename(String("media")), "map-20kdm2.pk3", "maps/20kdm2.bsp");
}