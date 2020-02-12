#include "SASpaceLevelBase.h"
#include "../../Rendering/BuiltInShaders.h"
#include "../../GameFramework/SAPlayerSystem.h"
#include "../../GameFramework/SAGameBase.h"
#include "../../Tools/DataStructures/SATransform.h"
#include "../../GameFramework/SAPlayerBase.h"
#include "../../Rendering/Camera/SACameraBase.h"
#include "../SAProjectileSystem.h"
#include "../Team/Commanders.h"
#include "../Environment/StarField.h"
#include "../Environment/Star.h"
#include "../../Rendering/OpenGLHelpers.h"
#include "../Environment/Planet.h"
#include "../../Rendering/RenderData.h"
#include "../../GameFramework/SARenderSystem.h"
#include "../../Tools/Algorithms/SphereAvoidance/AvoidanceSphere.h"

namespace SA
{
	void SpaceLevelBase::render(float dt_sec, const glm::mat4& view, const glm::mat4& projection)
	{
		using glm::vec3; using glm::mat4;

		GameBase& game = GameBase::get();

		const sp<PlayerBase>& zeroPlayer = game.getPlayerSystem().getPlayer(0);
		const RenderData* FRD = game.getRenderSystem().getFrameRenderData_Read(game.getFrameNumber());

		if (zeroPlayer && FRD)
		{
			const sp<CameraBase>& camera = zeroPlayer->getCamera();

			if (starField)
			{
				starField->render(dt_sec, FRD->view, FRD->projection);
			}

			//stars may be overlapping each other, so only clear depth once we've rendered all the solar system stars.
			ec(glClear(GL_DEPTH_BUFFER_BIT));
			for (const sp<Star>& star : localStars)
			{
				star->render(dt_sec, FRD->view, FRD->projection);
			}

			for (const sp<Planet>& planet : planets)
			{
				planet->render(dt_sec, FRD->view, FRD->projection);
			}
			ec(glClear(GL_DEPTH_BUFFER_BIT));

			//#todo a proper system for renderables should be set up; these uniforms only need to be set up front, not during each draw. It may also be advantageous to avoid virtual calls.
			forwardShadedModelShader->use();
			forwardShadedModelShader->setUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(FRD->view));
			forwardShadedModelShader->setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(FRD->projection));
			forwardShadedModelShader->setUniform3f("lightPosition", glm::vec3(0, 0, 0));
			forwardShadedModelShader->setUniform3f("lightDiffuseIntensity", glm::vec3(0, 0, 0));
			forwardShadedModelShader->setUniform3f("lightSpecularIntensity", glm::vec3(0, 0, 0));
			forwardShadedModelShader->setUniform3f("lightAmbientIntensity", glm::vec3(0, 0, 0)); //perhaps drive this from level information
			for (size_t light = 0; light < FRD->dirLights.size(); ++light)
			{
				FRD->dirLights[light].applyToShader(*forwardShadedModelShader, light);
			}
			forwardShadedModelShader->setUniform3f("cameraPosition", camera->getPosition());
			forwardShadedModelShader->setUniform1i("material.shininess", 32);
			for (const sp<RenderModelEntity>& entity : renderEntities) 
			{
				forwardShadedModelShader->setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(entity->getTransform().getModelMatrix()));
				entity->draw(*forwardShadedModelShader);
			}
		}
		else
		{
			log(__FUNCTION__, LogLevel::LOG_ERROR, "failed to get required resources for rendering");
		}
	}

	TeamCommander* SpaceLevelBase::getTeamCommander(size_t teamIdx)
	{
		return teamIdx < commanders.size() ? commanders[teamIdx].get() : nullptr;
	}

	void SpaceLevelBase::startLevel_v()
	{
		LevelBase::startLevel_v();

		forwardShadedModelShader = new_sp<SA::Shader>(spaceModelShader_forward_vs, spaceModelShader_forward_fs, false);

		for (size_t teamId = 0; teamId < numTeams; ++teamId)
		{
			commanders.push_back(new_sp<TeamCommander>(teamId));
		}
	}

	void SpaceLevelBase::endLevel_v()
	{
		//this means that we're going to generate a new shader between each level transition.
		//this can be avoided with static members or by some other mechanism, but I do not see 
		//transitioning levels being a slow process currently, so each level gets its own shaders.
		forwardShadedModelShader = nullptr;

		LevelBase::endLevel_v();
	}

	void SpaceLevelBase::postConstruct()
	{
		createTypedGrid<AvoidanceSphere>(glm::vec3(128));
		
		starField = onCreateStarField();
		{ 
			bGeneratingLocalStars = true;
			onCreateLocalStars();
			bGeneratingLocalStars = false;

			onCreateLocalPlanets();
		}

		refreshStarLightMapping();
	}

	sp<SA::StarField> SpaceLevelBase::onCreateStarField()
	{
		//by default generate a star field, if no star field should be in the level return null in an override;
		sp<StarField> defaultStarfield = new_sp<StarField>();
		return defaultStarfield;
	}

	void SpaceLevelBase::refreshStarLightMapping()
	{
		uint32_t MAX_DIR_LIGHTS = GameBase::getConstants().MAX_DIR_LIGHTS;
		if (localStars.size() > MAX_DIR_LIGHTS)
		{
			log(__FUNCTION__,LogLevel::LOG_WARNING, "More local stars than there exist slots for directional lights");
		}

		for (size_t starIdx = 0; starIdx < localStars.size() && starIdx < MAX_DIR_LIGHTS; starIdx++)
		{
			if (sp<Star> star = localStars[starIdx])
			{
				if (dirLights.size() <= starIdx)
				{
					dirLights.push_back({});
				}
				dirLights[starIdx].direction_n = normalize(star->getLightDirection());
				dirLights[starIdx].lightIntensity = star->getLightLDR(); //#TODO #HDR
			}
		}
	}

	void SpaceLevelBase::onCreateLocalStars()
	{
		Transform defaultStarXform;
		defaultStarXform.position = glm::vec3(50, 50, 50);

		sp<Star> defaultStar = new_sp<Star>();
		defaultStar->setXform(defaultStarXform);
		localStars.push_back(defaultStar);
	}

}

