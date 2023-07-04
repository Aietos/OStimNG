#include "GameActor.h"

#include "GameTable.h"
#include "GameUtil.h"

namespace GameAPI {
    void GameActor::update3D() const {
        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
            auto args = RE::MakeFunctionArguments();
            auto handle = skyrimVM->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(form->FORMTYPE), form);
            vm->DispatchMethodCall2(handle, "Actor", "QueueNiNodeUpdate", args, callback);
        }
    }

    void GameActor::lock() const {
        if (form->IsPlayerRef()) {
            if (form->AsActorState()->IsWeaponDrawn()) {
                sheatheWeapon();
            }

            RE::PlayerCharacter::GetSingleton()->SetAIDriven(true);
            RE::PlayerControls::GetSingleton()->activateHandler->disabled = true;
        } else {
            bool stop = false;
            stop |= setRestrained(form, true);
            stop |= setDontMove(form, true);
            if (stop) {
                stopMovement(form);
            }
        }

        RE::Actor* actor = form;
        SKSE::GetTaskInterface()->AddTask([actor]() {
            actor->SetGraphVariableBool("bHumanoidFootIKDisable", true);
        });
    }

    void GameActor::unlock() const{
        if (form->IsPlayerRef()) {
            RE::PlayerCharacter::GetSingleton()->SetAIDriven(false);
            RE::PlayerControls::GetSingleton()->activateHandler->disabled = false;
        } else {
            bool stop = false;
            stop |= setRestrained(form, false);
            stop |= setDontMove(form, false);
            if (stop) {
                stopMovement(form);
            }
        }

        StopTranslation(nullptr, 0, form);

        RE::Actor* actor = form;
        SKSE::GetTaskInterface()->AddTask([actor]() {
            actor->SetGraphVariableBool("bHumanoidFootIKDisable", false);
            actor->NotifyAnimationGraph("IdleForceDefaultState");
        });
    }

    void GameActor::playAnimation(std::string animation) const {
        RE::Actor* actor = form;
        SKSE::GetTaskInterface()->AddTask([actor, animation] {
            actor->NotifyAnimationGraph(animation);
        });
    }

    bool GameActor::isSex(GameSex sex) const {
        RE::SEX actorSex = form->GetActorBase()->GetSex();
        switch (sex) {
            case MALE:
                return actorSex == RE::SEX::kMale;
            case FEMALE:
                return actorSex == RE::SEX::kFemale;
            case NONE:
                return actorSex == RE::SEX::kNone;
        }
        return false;
    }

    bool GameActor::isHuman() const {
        return !form->IsChild() && form->HasKeyword(GameTable::getNPCKeyword()) && form->GetActorBase()->GetRace() != GameTable::getManakinRace();
    }

    void GameActor::setScale(float scale) const {
        RE::Actor* actor = form;
        SKSE::GetTaskInterface()->AddTask([actor, scale] {
            SetScale(actor, scale);
        });
    }

    void GameActor::setFactionRank(GameFaction faction, int rank) const {
        for (auto& factionInfo : form->GetActorBase()->factions) {
            if (factionInfo.faction == faction.form) {
                factionInfo.rank = rank;
            }
        }
    }

    int GameActor::getFactionRank(GameFaction faction) const {
        for (RE::FACTION_RANK rank : form->GetActorBase()->factions) {
            if (rank.faction == faction.form) {
                return rank.rank;
            }
        }
        
        return -2;
    }

    int GameActor::getRelationshipRank(GameActor other) const {
        return 4 - RE::BGSRelationship::GetRelationship(form->GetActorBase(), other.form->GetActorBase())->level.underlying();
    }

    void GameActor::sheatheWeapon() const {
        const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
        const auto script = factory ? factory->Create() : nullptr;
        if (script) {
            script->SetCommand("rae WeaponSheathe"sv);
            script->CompileAndRun(form);
            delete script;
        }
    }

    std::vector<GameActor> GameActor::getNearbyActors(float radius, std::function<bool(GameActor)> condition) {
        std::vector<GameActor> actors;

        GameUtil::forEachReferenceInRange(form->GetPosition(), radius, [&actors, &condition](RE::TESObjectREFR& ref) {
            if (!ref.Is(RE::Actor::FORMTYPE)) {
                return RE::BSContainer::ForEachResult::kContinue;
            }

            GameActor actor = ref.As<RE::Actor>();

            if (!condition(actor)) {
                return RE::BSContainer::ForEachResult::kContinue;
            }

            actors.push_back(actor);

            return RE::BSContainer::ForEachResult::kContinue;
        });

        RE::NiPoint3 center = form->GetPosition();
        std::sort(actors.begin(), actors.end(), [&](GameActor actorA, GameActor actorB) {
            return actorA.form->GetPosition().GetSquaredDistance(center) < actorB.form->GetPosition().GetSquaredDistance(center);
        });

        return actors;
    }
}