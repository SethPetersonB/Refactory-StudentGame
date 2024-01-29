//// Primary Author : Philip Nygard
//// 
//// Co-authors:
////    Kento Murawski (Rendering functions)
////    Dylan Weber (Texture rendering)
////
////© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
//// ---------------------------------------------------------------------------------
//#pragma once
//#include <vector>
//#include <memory>
//#include "glm/glm/vec2.hpp"
//#include "glm/glm/mat4x4.hpp"
//#include "glm/glm/glm.hpp"
//#include <GL/glew.h>
//#include <GL/gl.h>
//#include <SDL2/SDL.h>
//#include "Texture.h"
//#include "Mesh.h"
//#include "../include/audio_startup.h"
//
///* TODO */
///*
//Depth needs to be defiend on isometric objects to mean how many height units an isometric object is above it's Y value
//
//Implement HudSort propperly
//
//Replace std::sort in IsoSort with an insertion sort
//
//*/
//#define COLORSHIFT 128
//
//namespace Engine
//{
//  // Event for components for when the mouse enters them (shifts colors)
//  template<typename T>
//  void OnMouseEntered(T * sub, const Packet &)
//  {
//    glm::vec4 color = sub->getItem()->GetColor();
//
//    sub->getItem()->SetColor(color.r - COLORSHIFT, color.g - COLORSHIFT, color.b - COLORSHIFT, color.a);
//
//    //Audio_Engine* AEngine = GetAudioEngine();
//    //AEngine->PlaySounds("button_sound_fast.wav", Vector3());
//  }
//
//  // Event for componetns for when the mouse exits them (shifts colors)
//  template<typename T>
//  void OnMouseExit(T * sub, const Packet &)
//  {
//    glm::vec4 color = sub->getItem()->GetColor();
//
//    sub->getItem()->SetColor((GLubyte)color.r + COLORSHIFT, (GLubyte)color.g + COLORSHIFT, (GLubyte)color.b + COLORSHIFT, (GLubyte)color.a);
//  }
//
//  // Request for component graphical element type
//  template<typename T>
//  void ItemTypeRequest(const T* item, Packet & data)
//  {
//    data.setData(item->getType());
//  }
//
//  // Request for graphical elment of an object
//  template<typename T>
//  void GraphicRequest(const T * member, Packet & data)
//  {
//	  data.setData(member->getItem());
//  }
//
//  template<typename T>
//  void TextureFrameRequest(const T * member, Packet & data)
//  {
//    data.setData(member->getTextureFrame());
//  }
//
//  template<typename T>
//  void OnTextureFrameSet(T * member, const Packet & data)
//  {
//    member->setTextureFrame(data.getData<unsigned>());
//
//  }
//
//  class GraphicalElement;
//  struct ItemChain;
//
//  typedef std::shared_ptr<GraphicalElement> GRAPHICAL_ELEMENT_PTR;
//  typedef std::weak_ptr<GraphicalElement> GRAPHICAL_ELEMENT_LINK;
//  typedef std::unique_ptr<Texture> TEXTURE_PTR;
//  typedef std::unique_ptr<Mesh> MESH_PTR;
//  typedef std::function<void(ItemChain &)> CHAIN_SORTER;
//
//  //typedef std::shared_ptr<Texture> TEXTURE_LINk;
//  void FlushExpired(ItemChain &);
//  void IsometricSort(ItemChain &);
//  void HudSort(ItemChain &);
//  bool IsoLessThan(const GRAPHICAL_ELEMENT_LINK & lhs, const GRAPHICAL_ELEMENT_LINK & rhs);
//
//  // Types of graphical elements (ordered lowest back to front, starting at 0)
//  enum GraphicalElementType 
//  { 
//    MENU_ELEMENT = 4, 
//    HUD_ELEMENT = 3, 
//    WORLD_ELEMENT = 2, 
//    WORLD_TILE_ELEMENT = 1,
//    BACKGROUND_ELEMENT = 0
//  };
//
//  
//  struct Vertex
//  {
//    struct Position
//    {
//      Position(float _x, float _y) : x(_x), y(_y) {}
//      float x;
//      float y;
//      float z;
//    }position;
//
//    struct Color
//    {
//      GLubyte r;
//      GLubyte g;
//      GLubyte b;
//      GLubyte a;
//    }color;
//
//    struct TexCoords
//    {
//      TexCoords(float u, float v) : x(u), y(v) {}
//      float x;
//      float y;
//      float z;
//    }texcoords;
//
//    size_t frameCount;
//
//    Vertex(float x, float y, float u, float v) : position(x, y), texcoords(u, v) {}
//
//  };
//
//  /****************************************************************************/
//  /*!
//    \brief
//      Class for representation of all graphical items within the game.
//      Contains information such as positioning, element type, visibility,
//      color, texture, etc. that relate to how any object is drawn
//  */
//  /****************************************************************************/
//  class GraphicalElement
//  {
//  public:
//    enum GraphicScaleReference { XX, XY, YY };
//
//    GraphicalElement(GraphicalElementType type, GLubyte r, GLubyte g, GLubyte b, GLubyte a, const std::string & texture);
//    ~GraphicalElement();
//
//    bool DoesNeedUpdate() { return needsUpdate_; }
//
//	void setHeight(float h) { height_ = h; }
//    void SetPos(float x, float y);
//    void SetSize(float width, float height);
//    void SetRot(float rot);
//    void SetColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255);
//    void SetVisible(bool vis) { visible = vis; }
//    float SetDepth(float depth);
//    void SetScaleReference(GraphicScaleReference ref);
//    void SetFrame(size_t frame);
//    void UpdateMatrix();
//    void UpdateVertexDepth();
//
//    glm::vec2 GetPos() const { return glm::vec2(x_, y_); }
//    glm::vec2 GetScale() const { return glm::vec2(width_, height_); }
//    GraphicalElementType GetType() const { return type_; }
//    const std::string & GetTexture() { return textureLable; }
//    const glm::mat4 & GetMatrix() const { return transform_; }
//    GLuint GetVBO() const { return VBO_; }
//    float GetDepth() const { return z_; }
//    float GetNormalizedDepth() const;
//    float IsometricY() const { return iso_y_; }
//    size_t CurrentFrame() const { return currFrame_; }
//    GraphicScaleReference GetScaleReference() const { return scaleRef_; }
//    glm::vec4 GetColor() const { return color_; }
//    unsigned GetOrder() const;  // determines what the draw-order for the object is
//    bool IsVisible() { return visible; }
//
//    //static float TileWidth;
//
//  private:
//    bool visible;
//
//    GraphicalElement(const GraphicalElement &) = delete;
//    GraphicalElement & operator=(const GraphicalElement &) = delete;
//
//    bool needsUpdate_;
//
//    GraphicalElementType type_;
//
//    float rot_;
//    float x_;
//    float y_;
//    float iso_y_;
//    size_t currFrame_;
//    float z_;
//    float width_;
//    float height_;
//
//
//    GraphicScaleReference scaleRef_;
//    std::vector<Vertex> vertList_;
//    glm::mat4 transform_;
//    const std::string textureLable; // Lable of the texture
//    //Texture * mTexture; // instance of texture object for this sprite
//    GLuint VBO_;
//    glm::vec4 color_;
//  };
//
//  /****************************************************************************/
//  /*!
//    \brief
//      Item chain for creating multiple lists of object layers and keeping
//      the list fresh. Default constructor takes a function pointer for the
//      function to use for sorting it's items. The default sorter simply removes
//      expired items from the list, and any custom sorter given must do this
//      as well
//  */
//  /****************************************************************************/
//  struct ItemChain
//  {
//  public:
//    ItemChain(CHAIN_SORTER sorter = CHAIN_SORTER(FlushExpired), unsigned alloc = 1024);
//
//    const GRAPHICAL_ELEMENT_LINK & operator[](int index) const { return items_[index]; }
//    GRAPHICAL_ELEMENT_LINK & operator[](int index) { return items_[index]; }
//
//    unsigned size() const { return items_.size(); }
//    void push_back(GRAPHICAL_ELEMENT_LINK added) { items_.push_back(added); }
//    void remove(unsigned index) { items_.erase(items_.begin() + index); }
//    void Sort() { sortFunc_(*this); }
//
//    std::vector<GRAPHICAL_ELEMENT_LINK> & list() { return items_; }
//  private:
//    std::vector<GRAPHICAL_ELEMENT_LINK> items_;
//    CHAIN_SORTER sortFunc_;
//  };
//
//  /****************************************************************************/
//  /*!
//    \brief
//      Pipeline for drawing graphical elements. Deals with creation of new 
//      elementsand drawing elements in the propper order
//  */
//  /****************************************************************************/
//  class GFXPipe
//  {
//  public:
//    static GFXPipe & get();
//
//    GRAPHICAL_ELEMENT_PTR NewItem(GraphicalElementType type, 
//      const std::string & texture,
//      GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255);
//
//    void Upload();
//    Texture & LoadTexture(const std::string & path);
//    Texture & FetchTexture(const std::string & path);
//    void LoadTextures(const std::string & defpath);
//
//    Mesh & LoadMesh(const std::string & id, const std::vector<glm::vec3> & verts);
//    Mesh & GetMesh(const std::string & id) const;
//
//    void RotateIsometric(float rot);
//
//    float GetRot() const { return rotation_; }
//    static glm::mat4 IsoMatrix();
//
//    unsigned GetObjectOrder(GraphicalElement const & object);
//    unsigned GetObjectOrder(GRAPHICAL_ELEMENT_PTR const & object);
//
//    void SetLerpIsometric(float goal, float speed);
//
//    void UpdateRotLerp();
//
//  protected:
//
//    static void UploadItem(GRAPHICAL_ELEMENT_PTR item);
//
//  private:
//    float rotation_;
//    //float rotDelta_;
//    float rotStart_;
//    float rotGoal_;
//    float rotSpeed_;
//    bool isLerping_;
//    GFXPipe();
//
//    // Singleton, can't be coppied
//    GFXPipe(const GFXPipe &) = delete;
//    GFXPipe & operator=(const GFXPipe &) = delete;
//
//    //std::vector<GRAPHICAL_ELEMENT_LINK> items;
//    std::map<GraphicalElementType, ItemChain> items;
//    std::unordered_map<std::string, TEXTURE_PTR> loaded_textures;
//    std::unordered_map<std::string, MESH_PTR> loaded_meshes;
//  };
//
//}
