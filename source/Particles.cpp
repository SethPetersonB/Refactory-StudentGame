// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Particles.h"
#include "../include/GSM.h"
#include <cstdlib>
#include <random>
#include <luabind/luabind.hpp>

namespace Engine
{
  /****************************************************************************/
  /*!
  \brief
  ParticleEmitter constructor.

  \param owner
  Owner of the particle emitter component
  */
  /****************************************************************************/
  ParticleEmitter::ParticleEmitter(GameInstance* owner) : Component(owner, "ParticleEmitter"),
    active_(false),
    num_particles_(35),
    meanParticleLifespan_(0.25), particleLifeDeviation_(0.1),
    meanParticleSpeed_(500), particleSpeedDeviation_(250),
    type_("explosion")
  {
    
  }

  /****************************************************************************/
  /*!
  \brief
  ParticleEmitter constructor that loads from the json file.

  \param owner
  Owner of the particle emitter component

  \param obj
  The parsed object to read from
  */
  /****************************************************************************/
  ParticleEmitter::ParticleEmitter(GameInstance* owner, const ParsedObject & obj) : Component(owner, "ParticleEmitter"), active_(false)
  {
    num_particles_ = obj.getComponentProperty<unsigned>("ParticleEmitter", "numParticles");

    meanParticleLifespan_ = obj.getComponentProperty<double>("ParticleEmitter", "meanLifeSpan");
    particleLifeDeviation_ = obj.getComponentProperty<double>("ParticleEmitter", "lifespanDeviation");

    meanParticleSpeed_ = obj.getComponentProperty<double>("ParticleEmitter", "meanSpeed");
    particleSpeedDeviation_ = obj.getComponentProperty<double>("ParticleEmitter", "speedDeviation");

    type_ = obj.getComponentProperty<std::string>("ParticleEmitter", "type");

  }

  /****************************************************************************/
  /*!
  \brief
  Checks if a particle emitter is active

  \return
  If it is active
  */
  /****************************************************************************/
  bool ParticleEmitter::isActive() const
  {
    return active_;
  }

  /****************************************************************************/
  /*!
  \brief
  Sets if a particle emitter is active

  \param active
  The bool to set it to
  */
  /****************************************************************************/
  void ParticleEmitter::setActive(bool active)
  {
    active_ = active;
  }

  /****************************************************************************/
  /*!
  \brief
  Gets the number of particles the emitter has.

  \return
  The number of particles
  */
  /****************************************************************************/
  int ParticleEmitter::getNumParticles() const
  {
    return num_particles_;
  }

  /****************************************************************************/
  /*!
  \brief
  Sets the number of particles an emitter has.

  \param particles
  The number of particles to set
  */
  /****************************************************************************/
  void ParticleEmitter::setNumParticles(int particles)
  {
    num_particles_ = particles;
  }

  /****************************************************************************/
  /*!
  \brief
  Does the work of creating particles when the emitter is active.

  */
  /****************************************************************************/
  void ParticleEmitter::createParticles(const std::string & type)
  {
    getParent().PostMessage("CreateParticles", type);
  }

  /****************************************************************************/
  /*!
  \brief
  Messaging request

  \param data
  The data of the request
  */
  /****************************************************************************/
  void ParticleEmitter::activeRequest(Packet& data)
  {
    dynamic_cast<Message<bool>&>(data).data = isActive();
  }

  /****************************************************************************/
  /*!
  \brief
  Messaging request

  \param data
  The data of the request
  */
  /****************************************************************************/
  void ParticleEmitter::numParticleRequest(Packet& data)
  {
    dynamic_cast<Message<size_t>&>(data).data = getNumParticles();
  }

  /****************************************************************************/
  /*!
  \brief
  Constructor for the ParticleEmitterHandler.

  \param stage
  The stage it is on
  */
  /****************************************************************************/
  ParticleEmitterHandler::ParticleEmitterHandler(Stage* stage) : ComponentHandler(stage, "ParticleEmitter")
  {
    dependencies_ = { "Transform" };
  }

  /****************************************************************************/
  /*!
  \brief
   Connects messaging events for objects with particle emitters.

  \param base_sub
   The base object with this component that is subscribing
  */
  /****************************************************************************/
  void ParticleEmitterHandler::ConnectEvents(Component* base_sub)
  {
    ParticleEmitter* sub = dynamic_cast<ParticleEmitter *>(base_sub);

    using namespace std::placeholders;

    Messenger& objMessenger = sub->getParent().getMessenger();

    REQUEST_ACTION requestActive = std::bind(&ParticleEmitter::activeRequest, sub, std::placeholders::_1);
    REQUEST_ACTION particleRequest = std::bind(&ParticleEmitter::numParticleRequest, sub, std::placeholders::_1);

    ScriptRouter & router = getStage()->getScriptEventRouter();

    std::vector<std::shared_ptr<ScriptEvent>> events;

    events.push_back(router.newEvent<bool>(objMessenger, "SetParticlesActive"));
    events.push_back(router.newEvent<size_t>(objMessenger, "SetNumParticles"));
    events.push_back(router.newEvent<std::string>(objMessenger, "CreateParticles"));

    sub->getParent().registerScriptEvent(events);

    objMessenger.SetupRequest("ParticlesActive", requestActive);
    objMessenger.SetupRequest("NumParticles", particleRequest);
  }

  /****************************************************************************/
  /*!
  \brief
   Update function for the particle system.

  */
  /****************************************************************************/
  void ParticleEmitterHandler::update()
  {

  }

  /****************************************************************************/
  /*!
  \brief
   Gets the average lifespan of all the particles in an emitter.

  \return
   The mean lifespan.
  */
  /****************************************************************************/
  double ParticleEmitter::getLifespan() const
  {
    return meanParticleLifespan_;
  }

  void ParticleEmitterHandler::getLuaRegisters()
  {
    using namespace luabind;

    getStage()->registerLuaModule(
      class_<ParticleEmitter, Component>("ParticleEmitter")
      .def("Emit", &ParticleEmitter::createParticles)
      .def_readwrite("active", &ParticleEmitter::active_)
      .def_readwrite("num_particles", &ParticleEmitter::num_particles_)
      .def_readwrite("life_mean", &ParticleEmitter::meanParticleLifespan_)
      .def_readwrite("life_deviation", &ParticleEmitter::particleLifeDeviation_)
      .def_readwrite("speed_mean", &ParticleEmitter::meanParticleSpeed_)
      .def_readwrite("speed_deviation", &ParticleEmitter::particleSpeedDeviation_)
      .def_readwrite("type", &ParticleEmitter::type_)

      //.def("CreateParticles", &ParticleEmitter::createParticles)
      );
  }

  Particle::Particle(GameInstance* owner) : Component(owner, "Particle"), alive_(true), lifespan_(0.5)
  {
    
  }

  Particle::Particle(GameInstance* owner, const ParsedObject& obj) : Component(owner, "Particle"), alive_(true)
  {
    lifespan_ = obj.getComponentProperty<double>("Particle", "lifespan");
  }

  ParticleHandler::ParticleHandler(Stage* stage) : ComponentHandler(stage, "Particle")
  {
    dependencies_ = { "Physics", "Transform", "Sprite" };
  }

  void ParticleHandler::update()
  {
    // kill particles when they surpass their lifespan
    for (int i = 0; i < componentList_.size(); ++i)
    {
      Particle* particle = dynamic_cast<Particle*>(componentList_[i]);
      if (particle->GetTimer().ElapsedTime() >= particle->GetLifeSpan())
        getStage()->removeGameInstance(particle->getParent());
    }
  }

  void ParticleHandler::getLuaRegisters()
  {
    using namespace luabind;

    getStage()->registerLuaModule(
      class_<Particle, Component>("Particle")
      .def_readwrite("lifespan", &Particle::lifespan_)
      .def_readwrite("alive", &Particle::alive_)
      .def_readwrite("type", &Particle::type_)
      .def_readwrite("origin", &Particle::origin_)
      .def_readwrite("speed", &Particle::speed_)
      .def_readwrite("angle", &Particle::angle_)

      );
  }

  void ParticleHandler::ConnectEvents(Component* base_sub)
  {
    
  }

}

