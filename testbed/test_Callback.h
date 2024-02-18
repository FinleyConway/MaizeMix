#pragma once

#include "MaizeMix/MaizeMix.h"
#include "test_Components.h"
#include "entt/entt.hpp"

class test_Callback : public Mix::AudioFinishCallback
{
 public:
	explicit test_Callback(entt::registry& registry) : m_Reg(&registry) { }

	void OnAudioFinish(uint8_t audioSourceID, const std::any& userData) override
	{
		auto entity = std::any_cast<entt::entity>(userData);

		m_Reg->remove<PlayingAudioSourceTag>(entity);
	}

 private:
	entt::registry* m_Reg = nullptr;
};