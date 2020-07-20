#include "SAHUD.h"
#include "../../../GameFramework/SAGameBase.h"
#include "../../../GameFramework/SAWindowSystem.h"
#include "../../../Rendering/BuiltInShaders.h"
#include "../../../Rendering/SAGPUResource.h"
#include "../../../Rendering/SAShader.h"
#include "../../../Tools/Geometry/SimpleShapes.h"
#include "../../../Rendering/Camera/Texture_2D.h"
#include "Widgets3D/Widget3D_Respawn.h"
#include "Widgets3D/Widget3D_DigitalClockFontTest.h"
#include "../../../Tools/DataStructures/SATransform.h"
#include "../../GameSystems/SAUISystem_Game.h"
#include "../../SpaceArcade.h"
#include "Widgets3D/HUD/Widget3D_PlayerStatusBarBase.h"
#include "../../../GameFramework/SADebugRenderSystem.h"

using namespace glm;

namespace SA
{
	void HUD::postConstruct()
	{
		SpaceArcade& game = SpaceArcade::get();
		game.getGameUISystem()->onUIGameRender.addWeakObj(sp_this(), &HUD::handleGameUIRender);

		reticleTexture = new_sp<Texture_2D>("GameData/engine_assets/SimpleCrosshair.png");
		quadShape = new_sp<TexturedQuad>();
		spriteShader = new_sp<Shader>(spriteVS_src, spriteFS_src, false);

		respawnWidget = new_sp<Widget3D_Respawn>();

		healthBar = new_sp<Widget3D_HealthBar>(playerIdx);
		energyBar = new_sp<Widget3D_EnergyBar>(playerIdx);

#if HUD_FONT_TEST 
		fontTest = new_sp<class Widget3D_DigitalClockFontTest>();
#endif
	}

	void HUD::handleGameUIRender(GameUIRenderData& rd_ui)
	{
		if (bRenderHUD)
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// this is a bit of a hack. originally I anticipated a distance of 10.f in front of hud was a good amount
			// but it isn't great because HUD widgets will clip into 3d structures. There's obvious two ways to fix 
			// this, one lower the distance, or 2 always draw ui without consideration of depth. both are costly in that the
			// laser system is largly independent and desigend to be drawn with 3D depth and that the main menu has scaled 
			// elements based on a distance of 10.f. A simple fix is to just recalculate HUD data with a shorter distance only
			// for the in game hud. So we copy the hud data and recalcualte it at a new distnace
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			HUDData3D hudData = rd_ui.getHUDData3D(); //make a copy
			if (CameraBase* gameCam = rd_ui.camera())
			{
				hudData.frontOffsetDist = 1.f; //be careful, this must be beyond near clip plane!
				calculateHUDData3D(hudData, *gameCam, rd_ui);
			}
			float distScaleCorrection = hudData.frontOffsetDist / 10.f; //10.f is the default distance for hudData

			////////////////////////////////////////////////////////
			// calculations
			////////////////////////////////////////////////////////

			vec3 hudCenterPoint = hudData.camPos + (hudData.frontOffsetDist * hudData.camFront);
			vec2 statusBarOffsetPerc{ 0.75f, -0.8f};
			//vec3 healthBarLoc = hudCenterPoint;
			//vec3 energyBarLoc = hudCenterPoint;
			//vec3 statusBarTextOffset = vec3(0.f);
			vec3 healthBarLoc = hudCenterPoint + hudData.camRight * -(statusBarOffsetPerc.x*hudData.savezoneMax_x) + hudData.camUp * (statusBarOffsetPerc.y *hudData.savezoneMax_y);
			vec3 energyBarLoc = hudCenterPoint + hudData.camRight * (statusBarOffsetPerc.x*hudData.savezoneMax_x) + hudData.camUp * (statusBarOffsetPerc.y *hudData.savezoneMax_y);
			vec3 statusBarTextOffset = (hudData.camUp * hudData.savezoneMax_y * /*perc*/0.1f);
			
			Transform healthBarXform;
			healthBarXform.rotQuat = rd_ui.camQuat();
			healthBarXform.position = healthBarLoc;
			healthBarXform.scale = vec3(2.f,1.f,1.f) * distScaleCorrection;
			healthBar->setBarTransform(healthBarXform);
			Transform healthBarTextXform = healthBarXform;
			healthBarTextXform.scale = vec3(hudData.textScale);
			healthBarTextXform.position += statusBarTextOffset;
			healthBar->setTextTransform(healthBarTextXform);
			healthBar->renderGameUI(rd_ui); //must come after transform update

			Transform energyBarXform = healthBarXform; //copy quat and scale
			energyBarXform.position = energyBarLoc;
			energyBar->setBarTransform(energyBarXform);
			Transform energyBarTextXform = energyBarXform;
			energyBarTextXform.scale = vec3(hudData.textScale);
			energyBarTextXform.position += statusBarTextOffset;
			energyBar->setTextTransform(energyBarTextXform);
			energyBar->renderGameUI(rd_ui); //must come after transform update

			if (spriteShader)
			{
				//draw reticle
				if (quadShape && reticleTexture && reticleTexture->hasAcquiredResources() && spriteShader)
				{
					float reticalSize = 0.015f * float(rd_ui.frameBuffer_MinDimension()); //X% of smallest screen dimension 

					mat4 model(1.f);
					model = glm::scale(model, vec3(reticalSize));

					spriteShader->use();
					spriteShader->setUniform1i("textureData", 0); 
					spriteShader->setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
					spriteShader->setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(rd_ui.orthographicProjection_m()));
					reticleTexture->bindTexture(GL_TEXTURE0); 
					quadShape->render();
				}
			}

			respawnWidget->renderGameUI(rd_ui);
#if HUD_FONT_TEST 
			fontTest->renderGameUI(rd_ui);
#endif
			if constexpr (constexpr bool bDebugHudPlacement = false)
			{
				DebugRenderSystem& db = GameBase::get().getDebugRenderSystem();
				vec3 camNudge = vec3(0, 0.001f, 0);
				if (bool bNudgeCamForward = false) { camNudge += hudData.camFront * 10.f; }

				////////////////////////////////////////////////////////
				// render camera data
				////////////////////////////////////////////////////////
				float normScaleUp = 3.f;
				if (bool bDebugCamera = false) 
				{ 
					db.renderSphere(hudData.camPos, vec3(0.05f), vec3(0, 0, 1)); 
					db.renderLine(hudData.camPos + camNudge, hudData.camPos + camNudge + hudData.camRight*normScaleUp, vec3(1, 0, 0));
					db.renderLine(hudData.camPos + camNudge, hudData.camPos + camNudge + hudData.camUp*normScaleUp, vec3(0, 1, 0));
					db.renderLine(hudData.camPos + camNudge, hudData.camPos + camNudge + hudData.camFront*normScaleUp, vec3(0, 0, 1));
				}

				////////////////////////////////////////////////////////
				// render placement data
				////////////////////////////////////////////////////////
				db.renderSphere(healthBarXform.position, vec3(0.005f), vec3(1, 0, 0));
				db.renderLine(hudData.camPos + camNudge, healthBarXform.position, vec3(1, 0, 0));
				db.renderLine(hudData.camPos + camNudge, energyBarXform.position, vec3(0, 1, 0));
			}
		}
	}


}

