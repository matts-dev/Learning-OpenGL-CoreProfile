#pragma once
#include "SAGameEntity.h"

namespace SA
{
	//#TODO this probably needs to exist separately in another header, so  player can do pattern of having protected member return a key
	/** Special key to allows brains (ai/player) to access methods 
		Only friend classes can construct these, giving the friend
		classes unique access to methods requiring an instance as
		a parameter.*/
	class BrainKey
	{
		friend class PlayerBase;
		friend class AIBrain;
	};


	class AIBrain : public GameEntity
	{
	public:
		bool awaken();
		void sleep();
		bool isActive(){ return bActive; }

	protected:
		/** return true if successful */
		virtual bool onAwaken() = 0;
		virtual void onSleep() = 0;

	protected:
		/** Give subclasses easy access to a BrainKey; copy elision should occur*/
		inline BrainKey getBrainKey() { return BrainKey{}; }

	private:
		bool bActive = false;
	};


	class BehaviorTree
	{

	};

	/*
		this is a rough approximation of what I hope to accomplish, subject to change
		behavior tree syntax, top priority is higher up 

		bt = {
			selector
			{
				sequence //attack
				{
					task_findEnemey {},
					selector
					{
						task_moveToEnemy{ },
						task_attack {}
					}
				}
				sequence //investigate
				{
					task_moveToLastLoc{}
					selector{
						//service? evaluate if investigating long enough
						task_setLastLocToRandomNearby
					}
				}
				sequence //guard
				{
					task_aquireNextPoint{}
					task_moveToNextPoint{}
				}
			}
		}
	*/
}