/*****
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****/


#include "granny/grntype.h"
#include "granny/grannyfile.h"
#include "granny/GrannyTextures.h"
#include "Config.h"
#include "Debug.h"
#include <iostream>
#include <cassert>

using namespace std;

// Utility Function
void calculateBoneRests (Bone * bone);


cGrannyFile::cGrannyFile ()
{
  texture = 0;
  m_initialized = false;
  m_texfilename = "";
  left_hand_bone = -1;
  right_hand_bone = -1;
  color_array = NULL;
}

cGrannyFile::~cGrannyFile ()
{
  // Destroy the Texture we're binding
  //delete texture;
  if (pGrannyTextureLoader)
    pGrannyTextureLoader->FreeTexture (m_texfilename);
  texture = NULL;
  if (color_array)
    free (color_array);
}

void cGrannyFile::load (std::string filename, std::string basepath)
{
  if (color_array)
    free (color_array);
  color_array = NULL;

  std::fstream * file =
    new fstream (filename.c_str (), ios::in | ios::binary);

  if (!file->is_open ())
      {
        pDebug.Log ("Error: File not found: " + filename);
        return;
      }


  stream = new cGrannyStream (file, filename);
  file->close ();
  delete file;

  glPushMatrix ();
  glLoadIdentity ();

  stream->seekg (0x40);         // skip header (Could be FileType magic)

  dword chunk = stream->readDword ();
  switch (chunk)
      {
      case 0xCA5E0000:
        mainChunk ();
        break;
      default:
        hex (cerr);
        cerr << "Unknown main chunk: " << chunk << endl;
        exit (1);
      }

  if (getTextureName () != "")
    loadTexture (basepath.c_str ());

  Bones & bones = getBones ();
  if (bones.bones.size () > 0)
      {
        calculateBoneRests (bones.bones[0]);
      }

  delete stream;
  stream = NULL;
  m_initialized = true;

  Meshes & meshes = getMeshes ();
  list < Mesh >::iterator imesh;
  imesh = meshes.meshes.begin ();

  if (imesh != meshes.meshes.end ())
      {
        color_array =
          (float *) malloc (imesh->points.size () * 3 * sizeof (float));
      }

  glPopMatrix ();
}

void cGrannyFile::addTime (float t)
{
}

string cGrannyFile::getTextureName ()
{
  return object.getTextureName ();
}

Meshes & cGrannyFile::getMeshes ()
{
  return object.getMeshes ();
}

Bones & cGrannyFile::getBones ()
{
  return object.getBones ();
}

BoneTies & cGrannyFile::getTies ()
{
  return object.getTies ();
}

Textures & cGrannyFile::getTextures ()
{
  return object.getTextures ();
}

BoneAnim & cGrannyFile::getBoneAnim (dword id)
{
  return object.getBoneAnim (id);
}

Animations & cGrannyFile::getAnimations ()
{
  return object.getAnimations ();
}

dword cGrannyFile::findString (string str)
{
  return object.findString (str);
}

dword cGrannyFile::getValue (dword obj, dword key)
{
  return object.getValue (obj, key);
}

dword cGrannyFile::findValue (dword key, dword value)
{
  return object.findValue (key, value);
}

std::string cGrannyFile::findID (dword id)
{
  return object.findID (id);
}

void cGrannyFile::mainChunk ()
{
  dword children = stream->readDword ();

  for (int i = 0; i < 6; ++i)
    stream->readDword ();       // CRC?

  for (dword child = 0; child < children; ++child)
      {
        // Chunk Header
        dword chunk = stream->readDword ();

        switch (chunk)
            {
              // Final Chunk (End-of-File?)
            case 0xCA5E0101:
              final.load (stream);
              break;

              // Copyright Chunk
            case 0xCA5E0102:
              copyright.load (stream);
              break;

              // Object Chunk
            case 0xCA5E0103:
              object.load (stream);
              break;
            default:
              hex (cerr);
              cerr << "Unknown GRN Chunk: " << chunk << endl;
              exit (1);
            }
      }
}

bool cGrannyFile::loadTexture (const char *basepath)
{
  std::string filename;
//    filename = basepath;
//    //filename += getTextureName();
  //filename += ".tga";

  // Try to load our Texture
/*	texture = new Texture;
	if( !texture->LoadFromFile( filename.c_str() ) )
	{
		delete texture;
		texture = 0;
		return false;
	}*/
  if (texture && pGrannyTextureLoader)
    pGrannyTextureLoader->FreeTexture (m_texfilename);

  texture = NULL;
  m_texfilename = getTextureName ();

  if (pGrannyTextureLoader)
    texture = pGrannyTextureLoader->LoadTexture (m_texfilename);

  return (texture);
}

void calculateBoneRests (Bone * bone)
{


  float X, Y, Z, x, y, z, w;
  X = bone->translate.points[0];
  Y = bone->translate.points[1];
  Z = bone->translate.points[2];

  x = bone->quaternion.points[0];
  y = bone->quaternion.points[1];
  z = bone->quaternion.points[2];
  w = bone->quaternion.points[3];

  GrnMatrix matrix;
  float x2 = x + x;
  float y2 = y + y;
  float z2 = z + z;
  float xx = x * x2;
  float xy = x2 * y;
  float xz = x2 * z;
  float xw = x2 * w;
  float yy = y * y2;
  float yz = y2 * z;
  float yw = y2 * w;
  float zz = z * z2;
  float zw = z2 * w;

  matrix[0] = 1 - yy - zz;
  matrix[1] = xy + zw;
  matrix[2] = xz - yw;
  matrix[3] = 0;
  matrix[4] = xy - zw;
  matrix[5] = 1 - xx - zz;
  matrix[6] = yz + xw;
  matrix[7] = 0;
  matrix[8] = xz + yw;
  matrix[9] = yz - xw;
  matrix[10] = 1 - xx - yy;
  matrix[11] = 0;
  matrix[12] = X;
  matrix[13] = Y;
  matrix[14] = Z;
  matrix[15] = 1;

  glMultMatrixf (matrix.matrix);
  glGetFloatv (GL_MODELVIEW_MATRIX, matrix.matrix);

  bone->matrix = matrix;
  bone->matrix.invert ();
  bone->curMatrix = matrix;
  bone->curMatrix *= bone->matrix;

  vector < Bone * >::iterator ibone;
  for (ibone = bone->children.begin ();
       ibone != bone->children.end (); ibone++)
      {
        glPushMatrix ();
        calculateBoneRests (*ibone);
        glPopMatrix ();
      }
}


int cGrannyFile::getFrame (cGrannyFile * animation, float &curTime)
{
  if (animation)
      {
        if (curTime >= animation->length ())
          curTime = 0.0f;
        if (animation->length () > 0.0f)
          return (int) (curTime / animation->length () * 25.0f);
      }
  return 0;
}

cDeformedArray *cGrannyFile::createDeformed (cGrannyFile * animation,
                                             float time,
                                             list < Mesh >::iterator imesh)
{
  cDeformedArray *deformed = NULL;

  //Meshes &meshes = getMeshes();
  Bones & bones = getBones ();
  //Textures &textures = getTextures();
  BoneTies & boneTies = getTies ();

  glPushMatrix ();
  glLoadIdentity ();
  if (animation)
    animation->getSkeleton (bones.bones[0], time);
  glPopMatrix ();

  vector < BoneWeight >::iterator iwt;
  dword pnt = 0;
  if (imesh->weights.size () > 0)
      {
        deformed = new cDeformedArray (imesh->weights.size ());
        float *data = deformed->data ();

        for (iwt = imesh->weights.begin (); iwt != imesh->weights.end ();
             ++iwt, ++pnt)
            {
              *data = 0;
              *(data + 1) = 0;
              *(data + 2) = 0;
              for (unsigned int wt = 0; wt < iwt->numWeights; ++wt)
                  {
                    Point post;
                    float weight = iwt->weights[wt];
                    dword bone = boneTies.boneties[iwt->bones[wt]]->bone;
                    post = bones.bones[bone]->curMatrix * imesh->points[pnt];
                    (*data) += (post.points[0] * weight);
                    *(data + 1) += (post.points[1] * weight);
                    *(data + 2) += (post.points[2] * weight);
                  }
              data += 3;
            }
      }
  else if (imesh->points.size () > 0)
      {
        deformed = new cDeformedArray (imesh->points.size ());
        float *data = deformed->data ();

        for (unsigned int i = 0; i < imesh->points.size (); i++)
            {
              Point post;
              float weight = 1.0f;
              post = bones.bones[1]->curMatrix * imesh->points[i];
              *data = post.points[0] * weight;
              data++;
              *data = post.points[1] * weight;
              data++;
              *data = post.points[2] * weight;
              data++;
            }
      }

  if (animation)
      {
        deformed->matrix_left_hand = animation->matrix_left_hand;
        deformed->matrix_right_hand = animation->matrix_right_hand;
      }

  return deformed;
}

void cGrannyFile::Render (cGrannyFile * animation, float &curTime,
                          cCharacterLight * character_light, float r, float g,
                          float b, float alpha, bool is_corpse)
{
  if (!initialized ())
    return;

  cDeformedArray *deformed = NULL;
  int frame;
  if (is_corpse)
    frame = 24;
  else
    frame = getFrame (animation, curTime);
  //int frame = getFrame(animation, curTime);

  float time = 0.0f;

  if (animation)
      {
        time = ((float) frame) / 25.0f * animation->length ();
        deformed = animation->getDeformed (frame);
      }


  Meshes & meshes = getMeshes ();
  //Bones &bones = getBones();
  Textures & textures = getTextures ();
  //BoneTies &boneTies = getTies();
  list < Mesh >::iterator imesh;
  if (meshes.meshes.size () != 1)
      {
//             printf("Warning: More than one Granny Mesh is not supported!\n");
        //return;
      }

  imesh = meshes.meshes.begin ();


  if (!deformed)
      {
        deformed = createDeformed (animation, time, imesh);
        if (animation)
          animation->addDeformed (deformed, frame);
      }
  else
      {
        if (animation)
            {
              animation->matrix_left_hand = deformed->matrix_left_hand;
              animation->matrix_right_hand = deformed->matrix_right_hand;
            }

      }



  // we would need to recalculate normals here.. screw that dude.
  // I doubt OSI does (although since I have yet to actually run UO:TD,
  // I don't know for sure).

  if (deformed)
      {

        float *data = deformed->data ();

        if (imesh->normals.size () && color_array
            && (imesh->points.size () <= imesh->normals.size ()))
            {
              for (unsigned int i = 0; i < imesh->points.size (); i++)
                  {
//                    float *vertex = data + (i * 3);

/*  depreciated
                    int v_x = (int) (vertex[0] * 2.0f + 1.0f);
                    int v_y = (int) (vertex[1] * 2.0f + 1.0f);
                    int v_z = (int) (vertex[2] * 1.0f); */
                    Point & p = imesh->normals[i];
                    character_light->CalcColor (0, 0, 0, p.points,
                                                color_array + i * 3);
                  }
            }
        if (alpha < 1.0f)
          glDisable (GL_ALPHA_TEST);
        dword poly = 0;
        vector < gPolygon >::iterator polygon;
        //
        glBindTexture (GL_TEXTURE_2D, getTexture ());
        glBegin (GL_TRIANGLES);

        assert (character_light);

        for (polygon = imesh->polygons.begin ();
             polygon != imesh->polygons.end (); polygon++, poly++)
            {
              for (int i = 0; i < 3; ++i)
                  {

                    // Do we have texture-map information?
                    if (imesh->textureMap.size ())
                        {
                          Point & p =
                            imesh->textureMap[textures.polygons[poly].
                                              nodes[i + 1]];
                          glTexCoord2fv (p.points);
                        }

                    float *vertex = data + (polygon->nodes[i] * 3);

                    if (color_array)
                        {
                          float *color = color_array + polygon->nodes[i] * 3;
                          //glColor3f(color[0] * r, color[1] * g, color[2] * b);

                          if (alpha < 1.0f)
                            glColor4f (color[0] * r, color[1] * g,
                                       color[2] * b, alpha);
                          else
                            glColor3f (color[0] * r, color[1] * g,
                                       color[2] * b);


//                                    printf("%.2f %.2f %.2f %.2f\n", p.points[0], p.points[1], p.points[2], p.points[0]*p.points[0]+ p.points[1]* p.points[1]+ p.points[2]* p.points[2]);
                          //glNormal3fv( p.points);
                        }
                    //printf("%i %i %i\n", v_x, v_y, v_z);
                    //
                    //printf("%.2f %.2f %.2f\n", *vertex, *(vertex + 1), *(vertex + 2));

                    glVertex3fv (vertex);

                  }
            }
        if (alpha < 1.0f)
          glEnable (GL_ALPHA_TEST);
        glEnd ();

        if (!animation)
          delete deformed;
      }
}

void cGrannyFile::getSkeleton (Bone * bone, float &curTime)
{
}

cGrannyAnimation::cGrannyAnimation ()
{
  cGrannyFile::cGrannyFile ();
  animBones = NULL;
  m_length = 0.0f;
}

cGrannyAnimation::~cGrannyAnimation ()
{
  if (animBones)
    delete animBones;
  std::map < int, cDeformedArray * >::iterator iter;
  for (iter = cache.begin (); iter != cache.end (); iter++)
    delete iter->second;
  cache.clear ();
}


void cGrannyAnimation::Assign (cGrannyFile * model)
{
  if (!initialized () || !model->initialized ())
    return;

  left_hand_bone = model->left_hand_bone;
  right_hand_bone = model->right_hand_bone;

  BoneTies & boneTies = getTies ();
  BoneTies & boneLodTies = model->getTies ();
  Animations & anims = getAnimations ();

  vector < BoneAnim >::iterator ibn;
  dword anmNm = findString ("__ObjectName");
  dword lodNm = model->findString ("__ObjectName");

  animBones = new dword[100];
  for (int i = 0; i < 100; i++)
    animBones[i] = (dword)-1;

  dword abone = 0;
  for (ibn = anims.bones.begin (); ibn != anims.bones.end (); ibn++, abone++)
      {
        dword boneId;
        dword animId = ibn->id;
        dword id = boneTies.boneObjects[animId - 1];
        dword textId = getValue (id, anmNm);
        std::string boneStr = findID (textId);
        for (unsigned int j = 0; j < boneStr.length (); j++)
          boneStr[j] = tolower (boneStr[j]);

        if (boneStr.find ("mesh") != boneStr.npos
            || boneStr.find ("master") != boneStr.npos)
          boneId = abone;
        else
            {
              textId = model->findString (boneStr);
              dword obj = model->findValue (lodNm, textId);
              id = boneLodTies.findID (obj);
              boneId = boneLodTies.findBone (id);
            }

        animBones[boneId] = abone;
      }

  m_length = object.getAnimLength ();
}

dword *cGrannyAnimation::GetAnimBones (void)
{
  return animBones;
}

void cGrannyAnimation::getSkeleton (Bone * bone, float &curTime)
{
  float X, Y, Z, x, y, z, w;
  dword rid, mid;
  GrnMatrix matrix;

  if (!animBones)
    return;

  if ((left_hand_bone != -1) && ((int) bone->id == left_hand_bone))
      {
        glPushMatrix ();
        glRotatef (-135.0f, 1.0f, 0.0f, 0.0f);
        glRotatef (-45.0f, 0.0f, 1.0f, 0.0f);
        //glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        glGetFloatv (GL_MODELVIEW_MATRIX, matrix_left_hand.matrix);
        glPopMatrix ();
      }

  if ((right_hand_bone != -1) && ((int) bone->id == right_hand_bone))
      {
        glPushMatrix ();
        glRotatef (90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef (135.0f, 0.0f, 1.0f, 0.0f);
        //glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        glGetFloatv (GL_MODELVIEW_MATRIX, matrix_right_hand.matrix);
        glPopMatrix ();
      }

  rid = mid = 0;
  if ((animBones[bone->id] == (dword)-1))
      {
        return;
      }
  else
      {
        BoneAnim & boneAnim = getBoneAnim (animBones[bone->id]);
        vector < float >::iterator movi, roti;

        // Problem here, i suppose that some animations are looped back/forward
        if (curTime > boneAnim.length)
            {
              curTime = 0.0f;
            }

        for (roti = boneAnim.quaternionTimeline.begin ();
             roti != boneAnim.quaternionTimeline.end () && (*roti) < curTime;
             roti++, rid++);

        for (movi = boneAnim.translateTimeline.begin ();
             movi != boneAnim.translateTimeline.end () && (*movi) < curTime;
             movi++, mid++);

        X = boneAnim.translates[mid].points[0];
        Y = boneAnim.translates[mid].points[1];
        Z = boneAnim.translates[mid].points[2];

        // Interpolate when between Keyframes
        if (curTime != (*movi))
            {
              float F1 = *movi;
              movi++;
              if (movi != boneAnim.translateTimeline.end ())
                  {
                    float F2 = *movi;
                    float x2 = boneAnim.translates[mid + 1].points[0];
                    float y2 = boneAnim.translates[mid + 1].points[1];
                    float z2 = boneAnim.translates[mid + 1].points[2];
                    float t = (curTime - F1) / (F2 - F1);
                    x = X + t * (x2 - X);
                    y = Y + t * (y2 - Y);
                    z = Z + t * (z2 - Z);
                    X = x;
                    Y = y;
                    Z = z;
                  }
            }

        x = boneAnim.quaternions[rid].points[0];
        y = boneAnim.quaternions[rid].points[1];
        z = boneAnim.quaternions[rid].points[2];
        w = boneAnim.quaternions[rid].points[3];
        //}

        float x2 = x + x;
        float y2 = y + y;
        float z2 = z + z;
        float xx = x * x2;
        float xy = x2 * y;
        float xz = x2 * z;
        float xw = x2 * w;
        float yy = y * y2;
        float yz = y2 * z;
        float yw = y2 * w;
        float zz = z * z2;
        float zw = z2 * w;

        matrix[0] = 1 - yy - zz;
        matrix[1] = xy + zw;
        matrix[2] = xz - yw;
        matrix[3] = 0;
        matrix[4] = xy - zw;
        matrix[5] = 1 - xx - zz;
        matrix[6] = yz + xw;
        matrix[7] = 0;
        matrix[8] = xz + yw;
        matrix[9] = yz - xw;
        matrix[10] = 1 - xx - yy;
        matrix[11] = 0;
        matrix[12] = X;
        matrix[13] = Y;
        matrix[14] = Z;
        matrix[15] = 1;

        glMultMatrixf (matrix.matrix);
        glGetFloatv (GL_MODELVIEW_MATRIX, matrix.matrix);
        bone->curMatrix = matrix;
        bone->curMatrix *= bone->matrix;

      }



  vector < Bone * >::iterator ibone;
  for (ibone = bone->children.begin (); ibone != bone->children.end ();
       ++ibone)
      {
        glPushMatrix ();
        getSkeleton (*ibone, curTime);
        glPopMatrix ();
      }
}

cDeformedArray *cGrannyAnimation::getDeformed (int index)
{
  std::map < int, cDeformedArray * >::iterator iter = cache.find (index);
  if (iter != cache.end ())
    return iter->second;
  return NULL;
}

void cGrannyAnimation::addDeformed (cDeformedArray * deformed, int index)
{
  std::map < int, cDeformedArray * >::iterator iter = cache.find (index);
  if (iter != cache.end ())
      {
        delete iter->second;
        cache.erase (index);
      }

  cache.insert (make_pair (index, deformed));

}

cDeformedArray::cDeformedArray (int size)
{
  assert (size > 0);
  m_data = new float[size * 3];
}


cDeformedArray::~cDeformedArray ()
{
  delete (m_data);
}


float *cDeformedArray::data ()
{
  return m_data;
}
