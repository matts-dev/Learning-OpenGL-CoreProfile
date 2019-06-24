#pragma once
#include <set>

#include "SAGameEntity.h"
#include "..\Tools\RemoveSpecialMemberFunctionUtils.h"
#include "..\Tools\DataStructures\MultiDelegate.h"

namespace SA
{
	//forward declarations
	class SystemBase;
	class WindowSystem;
	class AssetSystem;
	class LevelSystem;
	class PlayerSystem;

	class Window;

	//////////////////////////////////////////////////////////////////////////////////////

	/** 
		The Game Base class; this is the root to the game systems and has a static getter. 
		Understanding this class should be an early step in understanding the systems of
		this engine. 
	*/
	class GameBase : public GameEntity, public RemoveCopies, public RemoveMoves
	{

	//////////////////////////////////////////////////////////////////////////////////////
	//  Construction
	/////////////////////////////////////////////////////////////////////////////////////
	public:
		GameBase();


	//////////////////////////////////////////////////////////////////////////////////////
	//  Base Class Singleton
	/////////////////////////////////////////////////////////////////////////////////////
	public:
		static GameBase& get();

	private:
		/** It is preferred to make a static getter in the implemented subclass, but this
		    exists to avoid having systems know about concrete class types*/
		static GameBase* RegisteredSingleton;

	//////////////////////////////////////////////////////////////////////////////////////
	//  START UP / SHUT DOWN
	/////////////////////////////////////////////////////////////////////////////////////
	public: //starting system
		/** Starts up systems and begins game loop */
		void start();
	protected:
		/** Child game classes should set up pre-gameloop state here.
			#return value Provide an initial primary window on startup.	*/
		virtual sp<Window> startUp() = 0;
		virtual void shutDown() = 0;
	private: //starting systems
		bool bStarted = false;
		bool bExitGame = false;

	//////////////////////////////////////////////////////////////////////////////////////
	//  GAME LOOP
	//////////////////////////////////////////////////////////////////////////////////////
	public:
		MultiDelegate<float /*deltaSec*/> PreGameloopTick;
		MultiDelegate<float /*deltaSec*/> PostGameloopTick;

		void startShutdown();
	private: 
		void TickGameloop_GameBase();
	protected:
		virtual void tickGameLoop(float deltaTimeSecs) = 0;
		virtual void renderLoop(float deltaTimeSecs) = 0;

	//////////////////////////////////////////////////////////////////////////////////////
	//  SYSTEMS 
	//////////////////////////////////////////////////////////////////////////////////////
	public:
		//System getters (to prevent circular dependencies, be sure to use forward declared references)
		inline WindowSystem& getWindowSystem() { return *windowSys; }
		inline AssetSystem& getAssetSystem() { return *assetSys; }
		inline LevelSystem& getLevelSystem() { return *levelSys; }
		inline PlayerSystem& getPlayerSystem() { return *playerSys; }

		/** this isn't as encapsulated as I'd like, but will not likely be an issue */
		void SubscribePostRender(const sp<SystemBase>& system);
	private:
		bool bCustomSystemRegistrationAllowedTimeWindow = false;
	protected:
		virtual void onRegisterCustomSystem() {};
		void RegisterCustomSystem(const sp<SystemBase>& system);

	private: //systems
		sp<WindowSystem> windowSys;
		sp<AssetSystem> assetSys;
		sp<LevelSystem> levelSys;
		sp<PlayerSystem> playerSys;
		std::set< sp<SystemBase> > systems;
		std::set< sp<SystemBase> > postRenderNotifys;

	//////////////////////////////////////////////////////////////////////////////////////
	//  Time
	//////////////////////////////////////////////////////////////////////////////////////

	private: //time management 
		/** Time management needs to be separate from systems since their tick relies on its results. */
		void updateTime();
		float currentTime = 0; 
		float lastFrameTime = 0;
		float rawDeltaTimeSecs = 0;
		float deltaTimeSecs = 0.f;
		float MAX_DELTA_TIME_SECS = 0.5f;
	};

}