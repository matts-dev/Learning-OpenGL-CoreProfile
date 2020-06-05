#pragma once
#include <optional>
#include <string>

#include "../../AssetConfigs/SAConfigBase.h"
#include "../../../Tools/DataStructures/SATransform.h"

namespace SA
{
	const char* const TAG_GAMEMODE_CARRIER_TAKEDOWN = "Carrier Takedown";
	const char* const TAG_GAMEMODE_EXPLORE = "Linear Explore";

	class SpawnConfig;

	struct EnvironmentalBodyData
	{
		/** Optional fields that are not present will be randomized */
		std::optional<glm::vec3> dir;		//will be normalized
		std::optional<float> distance;		//how far the planet should be moved along its direction vector
	};
	struct PlanetData : public EnvironmentalBodyData
	{
		std::optional<std::string> texturePath;
		std::optional<bool> bHasCivilization; //ie should planet show city lights on dark side.
	};
	struct StarData : public EnvironmentalBodyData
	{
		std::optional<glm::vec3> color;
	};
	struct CarrierSpawnData
	{
		sp<SpawnConfig> shipSpawnConfig_carrier = nullptr;	//must be non-null or any fighters belonging to carrier will fail to spawn
		std::optional<glm::vec3> position;								//world location relative to center of map
		std::optional<glm::vec3> rotation_deg;							//used to construct a quaternion
		size_t numInitialFighters = 250;				//when game starts, how many fighters should be present around the carrier
		struct FighterSpawnData 
		{
			bool bEnableFighterRespawns = true;
			size_t maxNumberOwnedFighterShips = 1000;
			float respawnCooldownSec = 5.0f;
		};
		FighterSpawnData fighterSpawnData;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Class that holds the data for initializing a space level
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class SpaceLevelConfig : public ConfigBase
	{
		friend class SpaceLevelBase;
	public: //types
		/** Only relevant is gamemode tag directs to TAG_GAMEMODE_CARRIER_TAKEDOWN*/
		struct GameModeData_CarrierTakedown
		{
			struct TeamData
			{
				std::vector<CarrierSpawnData> carrierSpawnData; 
			};
			std::vector<TeamData> teams;
		};
	public:
		const std::optional<size_t>& getSeed() const { return seed; }

		/** Planets will be randomized until overridden*/
		void setNumPlanets(size_t number);
		bool overridePlanetData(size_t idx, const PlanetData& inData);
		const std::vector<PlanetData>& getPlanets() const { return planets; }

		/** Stars will be randomized until overridden*/
		void setNumStars(size_t number);
		bool overrideStarData(size_t idx, const StarData& inData);
		const std::vector<StarData>& getStars() const { return stars; }

		const std::string& getGamemodeTag() const { return gamemodeTag; }
		const GameModeData_CarrierTakedown& getGamemodeData_CarrierTakedown() const { return carrierGamemodeData; }
		void setGamemodeData_CarrierTakedown(const GameModeData_CarrierTakedown& inData) { carrierGamemodeData = inData; }
	public:
		virtual std::string getRepresentativeFilePath() override;
	protected:
		virtual void onSerialize(json& outData) override;
		virtual void onDeserialize(const json& inData) override;
	private:
		std::optional<size_t> seed;

		size_t numPlanets = 0;
		std::vector<PlanetData> planets;

		size_t numStars = 0;
		std::vector<StarData> stars;

		std::string gamemodeTag;
		GameModeData_CarrierTakedown carrierGamemodeData;

	};

}
