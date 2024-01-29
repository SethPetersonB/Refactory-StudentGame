// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/DrawUtils.h"
#include "luabind/luabind.hpp"

namespace DrawUtils
{
  // Event for components for when the mouse enters them (shifts colors)
  template<typename T>
  void OnMouseEntered(T * sub, const Packet &)
  {
    glm::vec4 color = sub->getItem().getShade();

    sub->getItem().setShade(color - glm::vec4{ COLORSHIFT, COLORSHIFT, COLORSHIFT, 0 });
  }

  // Event for componetns for when the mouse exits them (shifts colors)
  template<typename T>
  void OnMouseExit(T * sub, const Packet &)
  {
    glm::vec4 color = sub->getItem().getShade();

    sub->getItem().setShade(color + glm::vec4{ COLORSHIFT, COLORSHIFT, COLORSHIFT, 0 });
  }

  template<typename T>
  void ElementLayerRequest(const T * sub, Packet & data)
  {
    data.setData<DrawLayer>(sub->getItem().getLayer());
  }

  template<typename T>
  void ElementMatrixRequest(const T * sub, Packet & data)
  {
    float ar = static_cast<float>(GSM::get().getRenderer().getViewWidth()) / GSM::get().getRenderer().getViewWidth();
    data.setData<glm::mat4>(sub->getItem().getMatrix(ar));
  }

  template<typename T>
  void ElementOrderRequest(const T * sub, Packet & data)
  {
    data.setData<size_t>(GetTokenDrawOrder(sub->getItem()));
  }
  // Request for component graphical element type
  //template<typename T>
  //void ItemTypeRequest(const T* item, Packet & data)
  //{
  //  data.setData(item->getType());
  //}

  // Request for graphical elment of an object
  template<typename T>
  void GraphicRequest(const T * member, Packet & data)
  {
    data.setData(member->getItem());
  }

  template<typename T>
  void TextureFrameRequest(const T * member, Packet & data)
  {
    data.setData(member->getTextureFrame());
  }

  template<typename T>
  void OnTextureFrameSet(T * member, const Packet & data)
  {
    member->setTextureFrame(data.getData<unsigned>());

  }

  template<typename T>
  static void SetTokenSprite(T * item, const std::string & name)
  {
    const Texture * tex = GSM::get().getRenderer().getTexture(name);

    item->getItem().setDrawSurface(tex);
  }

  template<typename T>
  using R_TOKEN_SET_FUNC = void(DrawToken::*)(T);
  
  template<typename T>
  using R_TOKEN_GET_FUNC = T(DrawToken::*)() const;

  template<typename T, typename DATA, R_TOKEN_SET_FUNC<DATA> FUNC>
  static void SetTokenProperty(T * item, const DATA & data)
  {
    DrawToken token = item->getItem();
    
    (token.*FUNC)(data);
  }


  template<typename T, typename DATA, R_TOKEN_GET_FUNC<DATA> FUNC>
  static DATA GetTokenProperty(T * item)
  {
    DrawToken token = item->getItem();

    return (token.*FUNC)();
  }

  template<typename T>
  void GetDrawTokenLuaRegisters(Stage * stage, const std::string & compName)
  {
    using namespace luabind;

    stage->registerLuaModule(
      class_<T, Component>(compName.c_str())
      .def("SetTexture",
        &SetTokenSprite<T>)

      .property("visible",
        &GetTokenProperty<T, bool, &DrawToken::isVisible>,
        &SetTokenProperty<T, bool, &DrawToken::setVisible>)

      .property("rotation",
        &GetTokenProperty<T, float, &DrawToken::getRotation>,
        &SetTokenProperty< T, float, &DrawToken::setRotation>)

      .property("frame",
        &GetTokenProperty<T, unsigned, &DrawToken::getFrame>,
        &SetTokenProperty<T, unsigned, &DrawToken::setFrame>)

      .property("color",
        &GetTokenProperty<T, glm::vec4, &DrawToken::getShade>,
        &SetTokenProperty<T, const glm::vec4 &, &DrawToken::setShade>)

      .property("depth",
        &GetTokenProperty<T, float, &DrawToken::getDepth>,
        &SetTokenProperty<T, float, &DrawToken::setDepth>)

      .property("position",
        &GetTokenProperty<T, glm::vec2, &DrawToken::getPosition>,
        &SetTokenProperty<T, const glm::vec2&, &DrawToken::setPosition>)
    );
  }
}
