// Primary Author : Philip Nygard
// 
// Co-authors:
//    Kento Murawski (Basic rendering functions in UploadItem)
//    Dylan Weber (Texture rendering functions in UploadItem)
//
//© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
// ---------------------------------------------------------------------------------
#include "../include/GraphicsPipeline.h"
#include "../include/Input.h"
#include "../include/GSM.h"
#include "../include/Logger.h"
#include <algorithm>
#include <cmath>

#define EPSILON 0.00001

static float Clamp2Pi(float val)
{
  float  pi2 = glm::pi<float>() * 2;

  val = (fmod(val, pi2));

  return (val > 0) ? val : (2 * glm::pi<float>()) - val;
}

namespace Engine
{
  // Constructor for graphical elements. Private, called by factory
  GraphicalElement::GraphicalElement(GraphicalElementType type, GLubyte r, GLubyte g, GLubyte b, GLubyte a, const std::string & texture) :
    type_(type), x_(0), y_(0), width_(1), height_(1), rot_(0), textureLable(texture), z_(0.0f), scaleRef_(XY), visible(true), currFrame_(0)
  {
    GFXPipe::get().FetchTexture(texture);

    glGenBuffers(1, &VBO_);

    // Temprorary code until dynamic mesh loading. Creates a white standard square mesh
    // First tri
    vertList_.push_back(Vertex(0.5, 0.5, 1.0, 1.0));
    //vertexData[0].texcoords.x = 1.0;
    //vertexData[0].texcoords.y = 1.0;

    vertList_.push_back(Vertex(-0.5, 0.5, 0.0, 1.0));
    /*vertexData[1].texcoords.x = 0.0;
    vertexData[1].texcoords.y = 1.0;*/

    vertList_.push_back(Vertex(-0.5, -0.5, 0.0, 0.0));
    /*vertexData[2].texcoords.x = 0.0;
    vertexData[2].texcoords.y = 0.0;*/

    //vertList_.push_back(Vertex(0.5, 0.5));
        
    // Second tri
    vertList_.push_back(Vertex(-0.5,  -0.5, 0.0, 0.0 ));
    /*vertexData[3].texcoords.x = 0.0;
    vertexData[3].texcoords.y = 0.0;*/
    

    vertList_.push_back(Vertex(0.5, -0.5, 1.0, 0.0));
    /*vertexData[4].texcoords.x = 1.0;
    vertexData[4].texcoords.y = 0.0;*/
    

    vertList_.push_back(Vertex(0.5, 0.5, 1.0, 1.0));
    /*vertexData[5].texcoords.x = 1.0;
    vertexData[5].texcoords.y = 1.0;*/


    SetColor(r, g, b, a);

    //UpdateVertexDepth();

    UpdateMatrix();
  }

  // Clears VBO buffers for the object
  GraphicalElement::~GraphicalElement()
  {
    if (VBO_ != 0)
    {
      glDeleteBuffers(1, &VBO_);
    }
  }

  // Sets X and Y coordinates of the graphical element
  void GraphicalElement::SetPos(float x, float y)
  {
    x_ = x;
    y_ = y;
    needsUpdate_ = true;
  }

  // Sets the scale of the graphical element
  void GraphicalElement::SetSize(float width, float height)
  {
    width_ = width;
    height_ = height;
    needsUpdate_ = true;
  }

  // Sets rotation of the graphical element
  void GraphicalElement::SetRot(float rot)
  {
    rot_ = rot;
    needsUpdate_ = true;
  }

  // Sets RGBA color of the graphical element
  void GraphicalElement::SetColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
  {
    color_ = glm::vec4(r, g, b, a);

    for (unsigned i = 0; i < vertList_.size(); i++)
    {
      vertList_[i].color.r = r;
      vertList_[i].color.g = g;
      vertList_[i].color.b = b;
      vertList_[i].color.a = a;
      vertList_[i].position.z = 0;
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertList_.size(), &vertList_.front(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // No update needed for color change
  }
 
  // Sets the depth of the graphical lelement. Used to determine height of isometric objects
  float GraphicalElement::SetDepth(float depth)
  {
    if (depth < 0.0f)
      z_ = 0.0f;
    //else if (depth > 1.0f)
     // z_ = 1.0f;
    else
      z_ = depth;

    needsUpdate_ = true;

    return z_;
  }
  
  // Not used anymore
  float GraphicalElement::GetNormalizedDepth() const
  {
    switch (type_)
    {
    case Engine::HUD_ELEMENT:
      return (z_ / 4.0f) + 0.75f;
      break;
    case Engine::MENU_ELEMENT:
      return (z_ / 4.0f) + 0.5f;
      break;
    case Engine::WORLD_TILE_ELEMENT:
    case Engine::WORLD_ELEMENT:
      return z_ / 2.0f;
      break;
    case Engine::BACKGROUND_ELEMENT:
      return z_ / 4.0f;
    default:
      return 0;
    }
  }

  // Sets X/Y scale reference for UI elements
  void GraphicalElement::SetScaleReference(GraphicScaleReference ref)
  { 
    scaleRef_ = ref; 
    needsUpdate_ = true;
  }

  // Updates the transformation matrix of objects
  void GraphicalElement::UpdateMatrix()
  {
    float scaledWidth = width_;
    float scaledHeight_ = height_;

    // Get scaled ratio for screen elements
    if (scaleRef_ != XY && 
      (type_ != WORLD_TILE_ELEMENT && type_ != WORLD_ELEMENT))
    {
      Display & display = GSM::get().getDisplay();
      float dispWidth = (float)display.GetViewWidth();
      float dispHeight = (float)display.GetViewHeight();
      float ar = dispWidth / dispHeight;

      switch (scaleRef_)
      {
      case Engine::GraphicalElement::XX:
      {
        scaledHeight_ *= ar;
      }break;
      case Engine::GraphicalElement::YY:
      {

        scaledWidth *= (1 / ar);
      } break;

      default:
        break;
      }
    }

    glm::mat4 scaleMat = glm::scale(glm::vec3(scaledWidth, scaledHeight_, 1.0));
    glm::mat4 rotmat = glm::rotate(rot_, glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 transMat = glm::translate(glm::vec3(x_, y_, GetNormalizedDepth()));

    transform_ = transMat * rotmat * scaleMat;

  

    if (type_ == WORLD_TILE_ELEMENT || type_ == WORLD_ELEMENT)
    {
      /*
      Isometric transformation:
      Scale Y to  86.602
      Shear X 30 degrees
      Rotate -30 degrees along Z
      */

      // These are static so they don't have to be calculated every time
      static glm::mat4 isoTrans(GFXPipe::IsoMatrix());
      
      glm::mat4 worldRot = glm::rotate(GFXPipe::get().GetRot(), glm::vec3(0, 0, 1));



      //glm::mat4 isoProjection = isoTrans * transform_;

      // World objects
      if (type_ == WORLD_ELEMENT)
      {
        /*
        Isometric sprite transformations:

        Isometric projection on base
        Determine width of sprite:
        width = magnitued(base left - base right)

        Sprite centered on base
        Height adjusted as needed
        */

        //Width
        float width;

        //bottom left -> top right

        glm::vec4 scale(width_, height_, 0, 0);

        scale = isoTrans * scale;

        width = sqrt(scale.x * scale.x + scale.y * scale.y) ;
        
        // Height-width ratio of isometric cubes
        float height = width * 2 / glm::tan(glm::radians(60.0f));

        // Scale
        glm::mat4 isoScaleMat = glm::scale(glm::vec3(width, height, 1.0f));
        glm::vec4 pos = isoTrans * worldRot * glm::vec4(x_, y_, 0, 1);

        iso_y_ = pos.y;

        pos = pos + glm::vec4(0, (height / 2) * z_, 0, 0);
        glm::mat4 isoTransMat = glm::translate(glm::vec3(pos.x, pos.y + height / 4, pos.z));

        transform_ = isoTransMat * isoScaleMat;
      }
      // Grid tiles
      else if (type_ == WORLD_TILE_ELEMENT)
      {
       /* Camera & cam = GSM::get().getCamera();
        glm::mat4 inv = glm::inverse(transform_);
        glm::vec4 pos(x_, y_, 0, 1);
        pos = inv * pos;
        if ((pos.x) <= 0.5f && (pos.y) <= 0.5f)
          SetColor(0, 0, 0);
        else
          SetColor(255, 255, 255);*/

        transform_ = isoTrans * worldRot * transform_;

      }
    }

    needsUpdate_ = false;
  }

  // Not used anymore
  void GraphicalElement::UpdateVertexDepth()
  {
    float norm = GetNormalizedDepth();

    for (unsigned i = 0; i < vertList_.size(); i++)
    {
      vertList_[i].position.z = norm;
    }
  }

  // Sets texture frame for an object
  void GraphicalElement::SetFrame(size_t frame)
  {
    Texture & tex = GFXPipe::get().FetchTexture(textureLable);

    currFrame_ = frame % tex.FrameCount() ;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the order of the object on the GFXPipe's draw order and which
      ItemChain it is drawn on

    \return
      The order the object is in the draw chain
      The lower a value is, the earlier it is drawn. Higher objects appear
      in front of lower objects
  
  */
  /****************************************************************************/
  unsigned GraphicalElement::GetOrder() const
  {
    return GFXPipe::get().GetObjectOrder(*this);
  }

  // GFXPipe

  // Constructor for GFXPipe
  GFXPipe::GFXPipe():
    rotation_(0), rotSpeed_(1)
  {
    items[BACKGROUND_ELEMENT] = ItemChain();
    items[WORLD_TILE_ELEMENT] = ItemChain();
    items[WORLD_ELEMENT] = ItemChain(IsometricSort);
    items[HUD_ELEMENT] = ItemChain(HudSort);
    items[MENU_ELEMENT] = ItemChain(HudSort);

    // Default square mesh
    std::vector<glm::vec3> squareMesh =
    {
      glm::vec3(0.5, 0.5, 1),
      glm::vec3(0.5, -0.5, 1),
      glm::vec3(-0.5, -0.5, 1),
      glm::vec3(-0.5, 0.5, 1),
    };

    // Default hex mesh
    std::vector<glm::vec3> hexMesh =
    {

      glm::vec3(0, 0.5, 1),       // 1
      glm::vec3(0.5, 0.25, 1),    // 3
      glm::vec3(0.5, -0.25, 1),   // 4
      glm::vec3(0, -0.5, 1),      // 6
      glm::vec3(-0.5, -0.25, 1),  // 8
      glm::vec3(-0.5, 0.25, 1),   // 9
    };

    // Load default meshes
    LoadMesh("Square", squareMesh);
    LoadMesh("Hexagon", hexMesh);
  }

  // Gets the GFXPipe singleton
  GFXPipe & GFXPipe::get()
  {
    static GFXPipe gp;

    return gp;
  }

  // Rotates the world by a specified amount
  void GFXPipe::RotateIsometric(float rot)
  {
    rotation_ = Clamp2Pi(rotation_ + rot);
    //rotGoal_ = rotation_;
  }

  // Sets how much the camera should Slerp and and what speed
  void GFXPipe::SetLerpIsometric(float goal, float speed)
  {
    if (!isLerping_)
    {
      rotGoal_ = (rotation_ + goal);
    }
    else
      rotGoal_ = (rotGoal_ + goal);

      //rotDelta_ = Clamp2Pi(rotDelta_ + goal);
      rotSpeed_ = speed;
      isLerping_ = true;
  }

  // Updates the slerp of the current rotation
  void GFXPipe::UpdateRotLerp()
  {
    if (isLerping_)
    {
      float pi = glm::pi<float>();
      float pi2 = 2 * pi;
      float pi3 = 3 * pi;
      float shortest_angle = (fmod((fmod((rotGoal_ - rotation_), pi2) + pi3), pi2) - pi);

      if (abs(shortest_angle) <= EPSILON)
      {
        RotateIsometric(shortest_angle);
        isLerping_ = false;
      }

      RotateIsometric(shortest_angle * rotSpeed_ * Display::GetFrameTime());
    }
  }

  // Factory for graphical elements. Takes a type, texture, and RGBA values
  GRAPHICAL_ELEMENT_PTR GFXPipe::NewItem(GraphicalElementType type, const std::string & texture, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
  {
    GRAPHICAL_ELEMENT_PTR ret(new GraphicalElement(type, r, g, b, a, texture));
    items[type].push_back(ret);
    return ret;
  }

  // Loads a texture with the given path into the texture list. Overwrites existing textures
  Texture & GFXPipe::LoadTexture(const std::string & path)
  {
    loaded_textures[path].reset(new Texture(path));
    return *loaded_textures[path];
  }

  // Fetches a texture from the list, loading it in if it does not exist
  Texture & GFXPipe::FetchTexture(const std::string & path)
  {
    if (loaded_textures.find(path) == loaded_textures.end())
    {
      LoadTexture(path);
    }

    return *loaded_textures[path];
  }

  // Loads a mesh with teh given ID and verts into the mesh list. Overwrites existing meshes
  Mesh & GFXPipe::LoadMesh(const std::string & id, const std::vector<glm::vec3> & verts)
  {

    loaded_meshes[id].reset(new Mesh(verts));
    return *loaded_meshes[id];
  }

  // Retrieves a mesh with the given ID from the loaded meshes. Throws an exception if it does not exist
  Mesh & GFXPipe::GetMesh(const std::string & id) const
  {
    return *loaded_meshes.at(id);
  }

  // Uploads all active graphical elements to the gpu
  void GFXPipe::Upload()
  {
    unsigned i = 0;

    UpdateRotLerp();

    for(auto & link_chain : items)
    {
      link_chain.second.Sort();

      for(i = 0; i < link_chain.second.size(); i++)
      {
        UploadItem(link_chain.second[i].lock());
      }
    }
  }

  // Matrix to use for iso transformations
  glm::mat4 GFXPipe::IsoMatrix()
  {
    glm::mat4 isoScale = glm::scale(glm::vec3(1.0f, glm::tan(glm::radians(30.0f)), 1.0f));
    glm::mat4 isoRot = glm::rotate(glm::radians(-45.0f), glm::vec3(0, 0, 1));
    glm::mat4 isoShear;

    return isoScale * isoRot;
  }

  // Uploads a specific item to the GPU
  void GFXPipe::UploadItem(GRAPHICAL_ELEMENT_PTR item)
  {
    if (item->DoesNeedUpdate())
      item->UpdateMatrix();

    if (!item->IsVisible())
      return;

    glm::mat4 trans; //= item->GetMatrix();
    Camera & cam = GSM::get().getCamera();

    switch (item->GetType())
    {
    case WORLD_TILE_ELEMENT:
    case WORLD_ELEMENT:
      trans = cam.CameraToNDC() * cam.WorldToCamera() * item->GetMatrix();
      break;

    default:
      trans = item->GetMatrix();
      break;
    }
    Texture & itemTexture = get().FetchTexture(item->GetTexture());

    GLint transformLocation = GSM::get().getColorShader().getUniformLocation("transform");
    GLint currFrameLocation = GSM::get().getColorShader().getUniformLocation("currFrame");
    GLint frameCountLocation = GSM::get().getColorShader().getUniformLocation("frameCount");

    glUniform1ui(frameCountLocation, itemTexture.FrameCount());
    glUniform1ui(currFrameLocation, item->CurrentFrame());
    glUniformMatrix4fv(transformLocation, 1, false, &trans[0][0]);

    // Bind Buffer
      glBindBuffer(GL_ARRAY_BUFFER, item->GetVBO());
      // Enable the vertex array, at 0 index
      glEnableVertexAttribArray(0);

      // Send position data to GPU
      glVertexAttribPointer(0,                                  // Which index to use ( We enabled Attrib Array # 0 )
                            3,                                  // We have 3 data points (x and y and z)
                            GL_FLOAT,                           // Which are floats
                            GL_FALSE,                           // We don't want points to be normalized
                            sizeof(Vertex),                     // We send data a vertex at a time (3 floats and 4 GLuints)
                            (void*)offsetof(Vertex, position)); // We send the offset before the data points used (position) within the Vertex struct
      
      // Send color data GPU
      glVertexAttribPointer(1,                                  // Which index to use ( We enabled Attrib Array # 1 )
                            4,                                  // We have 4 data points (r, g, b, and a)
                            GL_UNSIGNED_BYTE,                   // Which are GL unsigned bytes
                            GL_TRUE,                            // We want points to be normalized
                            sizeof(Vertex),                     // We send data a vertex at a time (3 floats and 4 GLuints)
                            (void*)offsetof(Vertex, color));    // We send the offset before the data points used (color) within the Vertex struct

      itemTexture.Bind();

                       // Send texture data to GPU
      glVertexAttribPointer(2,                                  // Which index to use ( We enabled Attrib Array # 2 )
        3,                                  // We have 3 data points (x and y and z)
        GL_FLOAT,                           // Which are floats
        GL_FALSE,                           // We don't want points to be normalized
        sizeof(Vertex),                     // We send data a vertex at a time (3 floats and 4 GLuints)
        (void*)offsetof(Vertex, texcoords)); // We send the offset before the data points used (texcoords) within the Vertex struct


      glDrawArrays(GL_TRIANGLES, 0, 6);


      // Make sure to diable vertex array once drawing is done
      glDisableVertexAttribArray(0);
      // Unbind buffer when drawing is done
      glBindBuffer(GL_ARRAY_BUFFER, 0);

  }

  /****************************************************************************/
  /*!
    \brief
      Gets the order of a graphical element in the draw order

    \return
      The order the object is drawn at
      The lower a value is, the earlier it is drawn. Higher objects appear
      in front of lower objects
  
  */
  /****************************************************************************/
  unsigned GFXPipe::GetObjectOrder(GraphicalElement const & object)
  {
    // Determine which item chain the object is on (iterator)
    auto iter_chain = items.find(object.GetType());

    // Determine the number of objects on lower item chains
    unsigned lowerItems = 0;

    /* Map of chains is ordered, so we can loop through them to 
      determine the amount of items that are drawn earlier than 
      the current object */
    for (auto iter = items.begin(); iter != iter_chain; iter++)
    {
      lowerItems += iter->second.size();
    }

    // Determine the object's position in its item chain
    for (auto & list_obj : iter_chain->second.list())
    {
      /* Ugly syntax here, but basically terermining if the address of an 
       object on the item chain is equal to the itterator's current position */
      if (&object == list_obj.lock().get())
        return lowerItems;

      lowerItems++;
    }

    return lowerItems;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the order of a graphical element in the draw order

    \return
      The order the object is drawn at
      The lower a value is, the earlier it is drawn. Higher objects appear
      in front of lower objects
  
  */
  /****************************************************************************/
  unsigned GFXPipe::GetObjectOrder(GRAPHICAL_ELEMENT_PTR const & object)
  {
    return GetObjectOrder(*object);
  }

  // ItemChain

  // Creates an item chain to use for ordering items. Takes a sorter functor and how many spaces to pre-allocate
  ItemChain::ItemChain(CHAIN_SORTER sorter, unsigned alloc) : sortFunc_(sorter)
  {
    items_.reserve(alloc);
  }

  // Flushes expired items from an item chain
  void FlushExpired(ItemChain & chain)
  {
    std::vector<GRAPHICAL_ELEMENT_LINK> expired;
    unsigned i = 0;

    while (i < chain.size())
    {
      if (chain[i].expired())
        chain.remove(i);
      else
        ++i;
    }
  }

  // Not used
  template<typename T>
  void InsertionSort(std::vector<T> & list, unsigned startPos = 0, std::function<bool(T &, T &)> compare = T::operator<)
  {
    
  }

  // Sorts isometric objects by their Y coordinate and height
  void IsometricSort(ItemChain & chain)
  {
    FlushExpired(chain);

    /* TODO */
    // Replace std::sort with insertion sort. For a list of items like this (not many items chaning position often),
    // insertion sort is likely to be faster
    std::sort(chain.list().begin(), chain.list().end(), IsoLessThan);
  }

  // Sorts hud elements
  void HudSort(ItemChain & chain)
  {
    FlushExpired(chain);
  }

  // Comparator for two graphical elements
  bool IsoLessThan(const GRAPHICAL_ELEMENT_LINK & lhs, const GRAPHICAL_ELEMENT_LINK &  rhs)
  {
    GRAPHICAL_ELEMENT_PTR lhs_l = lhs.lock();
    GRAPHICAL_ELEMENT_PTR rhs_l = rhs.lock();

    //items with a higher Y value are smaller, items with the same Y value are sorted by depth
    if (abs(lhs_l->IsometricY() - rhs_l->IsometricY()) <= EPSILON)
      return lhs_l->GetDepth() < rhs_l->GetDepth();

    return lhs_l->IsometricY() > rhs_l->IsometricY();
  }

}

