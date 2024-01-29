// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include "Timer.h"
#include "glm/glm/glm.hpp"

namespace Engine
{
  class Particle : public Component
  {
  public:
    Particle(GameInstance* owner);
    Particle(GameInstance* owner, const ParsedObject& obj);
    virtual ~Particle() {};

    double GetLifeSpan() const { return lifespan_; }
    bool IsAlive() const { return alive_; }
    Timer& GetTimer() { return timer_; }

    void SetLifeSpan(double life) { lifespan_ = life; }
    void SetAlive(bool alive) { alive_ = alive; }

  private:
    friend class ParticleHandler;

    double lifespan_; // lifespan of particle
    bool alive_; // if particle is alive
    std::string type_; // type of particle
    glm::vec2 origin_; // particle origin point
    float speed_; // particle speed
    float angle_; // particle angle
    Timer timer_;

  };

  class ParticleHandler : public ComponentHandler
  {
  public:
    ParticleHandler(Stage* stage);
    virtual ~ParticleHandler() {};
    void update();
    void getLuaRegisters() override;

  protected:
    void ConnectEvents(Component* base_sub);
  };

	class ParticleEmitter : public Component
	{
	public:

		ParticleEmitter(GameInstance* owner);
    ParticleEmitter(GameInstance* owner, const ParsedObject & obj);

		virtual ~ParticleEmitter() {};
		bool isActive() const;
		int getNumParticles() const;

    double getLifespan() const;
    double getLifeDeviation() const { return particleLifeDeviation_; }
    double getMeanSpeed() const { return meanParticleSpeed_; }
    double getSpeedDeviation() const { return particleSpeedDeviation_; }

		void setNumParticles(int particles);
		void setActive(bool active);

		void createParticles(const std::string & type);

    // messaging functions
    void activeRequest(Packet& data);
    void numParticleRequest(Packet& data);

	private:
    friend class ParticleEmitterHandler;

		bool active_; // is object emitting particles?
		size_t num_particles_; // how many particles to emit
    
    double meanParticleLifespan_; // average particle lifespan
    double particleLifeDeviation_;
    
    double meanParticleSpeed_; // average particle speed
    double particleSpeedDeviation_;

    std::string type_; // type of particles to emit

	};

	class ParticleEmitterHandler : public ComponentHandler
	{
	public:
		ParticleEmitterHandler(Stage* stage);
		virtual ~ParticleEmitterHandler() {};
    void getLuaRegisters() override;
		void update();

	protected:
		void ConnectEvents(Component* base_sub);
	};
}
