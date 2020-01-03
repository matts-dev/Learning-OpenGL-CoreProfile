#pragma once
#include "../../GameFramework/SALevel.h"

namespace SA
{
	class ProjectileSystem;
	class TeamCommander;
	class StarField;

	class SpaceLevelBase : public LevelBase
	{

	public:
		SpaceLevelBase();

		virtual void render(float dt_sec, const glm::mat4& view, const glm::mat4& projection) override;
		
		TeamCommander* getTeamCommander(size_t teamIdx);

	protected:
		virtual void startLevel_v() override;
		virtual void endLevel_v() override;
		virtual void postConstruct() override;
		virtual sp<StarField> onGenerateStarField();

	protected: //debug
		sp<StarField> getStarField();

	protected:
		sp<SA::Shader> forwardShadedModelShader;
		size_t numTeams = 2;

	private:
		sp<StarField> starField;
		std::vector<sp<TeamCommander>> commanders;
	};
}