// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include <list>
#include <set>
#include <functional>
#include "GameInstance.h"

#include "Script.h"
#include "GameInstance.h"
#include "ScriptSignal.h"
#include "grid.h"


namespace Engine
{
 // class Grid;
  //class ScriptRouter;
  class Grid;

  class Stage
  {
  public:
    typedef std::function<void(Stage *)> STAGE_RESET_FUNC;

    Stage(const Stage &) = delete;
    Stage & operator=(const Stage &) = delete;
    
    virtual ~Stage();

    virtual void update();

    void setStageRunning(bool running);
    void setStageOrder(unsigned order);
    void setStageRendered(bool rendered);
    void setStageReset(bool reset) { resetting_ = reset; }

    //static Grid & GetGrid();

    Grid& GetGrid() { return grid_; }
    void SetGrid(Grid grid) { grid_ = grid; }

    int GetGameObjectCount() { return gameInstanceList_.size(); }

    const std::string & getStageName() const;
    bool isStageRunning() const;
    bool isStageRendered() const;
    unsigned getStageOrder() const;
    bool isStageEnding() const;
    bool isStageResetting() const { return resetting_; }
    bool isStageToggling() const { return toggleRunning_; }
    void stageClear();

    void sendStageToBack();
    int bringStageToFront();

    void stageEnd();

    static Stage & GetStage(const std::string & stageName);
    static Stage & GetStage(const std::string & stageName, unsigned order);
    static Stage & New(const std::string & name,
      STAGE_RESET_FUNC reset  = STAGE_RESET_FUNC(),
      unsigned order = 0);

    static void SetResetFunc(Stage & stage, STAGE_RESET_FUNC res, bool resetNow = false);

    static void DestroyStage(const std::string & name, unsigned order);
    static void DestroyStage(Stage & stage);
    static void CleanStage(Stage & stage);
    static void ToggleRunning(Stage & stage);
    static void ResetStage(const std::string & name, unsigned order);
    //static void ResetStage(Stage & stage);
    static void ResetStage(Stage * stage);

    GameInstance & addGameInstance(const std::string type);
    GameInstance & addGameInstance();
    void removeGameInstance(unsigned long id);
    void removeGameInstance(GameInstance & inst);
    void flushInstanceList();
    GameInstance & getInstanceFromID(unsigned long id) const;
    GameInstance & getFirstInstanceByName(const std::string & name) const;
    std::ostream & printInstanceList(std::ostream & os) const;
    Messenger & getMessenger() { return mess_; }
    // This is a fatal exception. Please do not try to catch this.
    struct malformed_stage_list: public std::exception
    {
      const char * what() const throw();
    };

    struct stage_duplication: public std::exception
    {
      const char * what() const throw();
    };

    ComponentHandler * getHandler(const std::string & type) const;
    Component * createComponentFromType( GameInstance * owner, 
                                          const std::string & type);

    Component * createComponentFromType( GameInstance * owner, 
                                          const std::string & type,
                                          const ParsedObject & obj);

    ComponentHandler * fetchHandler(const std::string & type);

    void flushHandlers();
    void updateHandlers();
    void initHandlers();
    void addHandler(ComponentHandler * handler);

    // List of stages that exist, seperated out into their orders
    static std::map<unsigned, std::vector<Stage *> > StageList;
    static void FlushStageList();
    
    void initLuaSandbox(const std::string & sandbox = "scripts/sandbox.lua");
    void startLuaSandbox();

    SCRIPT_PTR loadScript(const std::string & script);
    SCRIPT_PTR loadScript(const std::string & script, GameInstance * owner);

    void unloadScript(SCRIPT_PTR script);

    void burstScripts(double dt);
    void registerLuaModule(luabind::scope & mod);
    void registerLuaModule(std::vector<luabind::scope> mods);
    ScriptRouter & getScriptEventRouter() { return event_Router_; }

    void unloadHierarchyInstance(GameInstance *); // Removes an instance from the lua hierarchy

    static Stage* lastRunning_; // the last running stage for quit confirmation

    protected:
      void addHierarchy(GameInstance * inst);
      void removeHierarchy(const std::string & type, unsigned long id);

  private:
    std::set<unsigned> removed_;
    std::map<unsigned long, GameInstance *> gameInstanceList_;

    std::vector<ComponentHandler*> handlers_;

    Stage(const std::string & name, STAGE_RESET_FUNC reset, unsigned order);
    static unsigned long AssignID();
    const std::string stageName_;

    bool isRunning_;
    bool isRendered_;
    int stageOrder_;
    bool stageEnding_;
    const unsigned long stageId_;
    Messenger mess_;
    int testVar;
    bool resetting_;
    bool toggleRunning_;

    std::unique_ptr<Sandbox> lua_Sandbox_;
    luabind::object hierarchy_;
    ScriptRouter event_Router_;

    STAGE_RESET_FUNC resFunc_;

    Grid grid_; // each stage should have its own grid
  };
}
