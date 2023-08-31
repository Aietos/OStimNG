#include "Condition.h"

#include "TraitTable.h"

#include "Core/ThreadActor.h"
#include "Core/ThreadManager.h"
#include "Graph/Requirement.h"
#include "MCM/MCMTable.h"
#include "Util/CompatibilityTable.h"

namespace Trait {
    ActorCondition ActorCondition::create(GameAPI::GameActor actor) {
        return create(actor, OStim::ThreadManager::GetSingleton()->findActor(actor));
    }

    ActorCondition ActorCondition::create(OStim::ThreadActor* actor) {
        return create(actor->getActor(), actor);
    }

    ActorCondition ActorCondition::create(GameAPI::GameActor actor, OStim::ThreadActor* threadActor) {
        ActorCondition condition;

        condition.type = TraitTable::getActorType(actor);

        // nullptr needs to meet all conditions, it's important for some add-ons
        if (!actor) {
            condition.requirements = ~0;
            return condition;
        }

        if (MCM::MCMTable::futaUseMaleRole()) {
            if (Compatibility::CompatibilityTable::hasSchlong(actor)) {
                condition.sex = GameAPI::GameSex::MALE;
            } else {
                condition.sex = GameAPI::GameSex::FEMALE;
            }
        } else {
            condition.sex = actor.getSex();
        }

        condition.requirements |= Graph::Requirement::ANUS;

        if (actor.isSex(GameAPI::GameSex::FEMALE)) {
            condition.requirements |= Graph::Requirement::BREAST;
        }

        condition.requirements |= Graph::Requirement::FOOT;
        condition.requirements |= Graph::Requirement::HAND;
        condition.requirements |= Graph::Requirement::MOUTH;
        condition.requirements |= Graph::Requirement::NIPPLE;

        bool hasSchlong = Compatibility::CompatibilityTable::hasSchlong(actor);
        if (hasSchlong) {
            condition.requirements |= Graph::Requirement::PENIS;
            condition.requirements |= Graph::Requirement::TESTICLES;
        } else {
            if (MCM::MCMTable::unequipStrapOnIfNotNeeded() || MCM::MCMTable::unequipStrapOnIfInWay()) {
                condition.requirements |= Graph::Requirement::VAGINA;
            } else {
                if (threadActor) {
                    if (!threadActor->isObjectEquipped("strapon")) {
                        condition.requirements |= Graph::Requirement::VAGINA;
                    }
                } else {
                    condition.requirements |= Graph::Requirement::VAGINA;
                }
            }

            if (MCM::MCMTable::equipStrapOnIfNeeded()) {
                condition.requirements |= Graph::Requirement::PENIS;
            } else {
                if (threadActor && threadActor->isObjectEquipped("strapon")) {
                    condition.requirements |= Graph::Requirement::PENIS;
                }
            }
        }

        if (actor.isVampire()) {
            condition.requirements |= Graph::Requirement::VAMPIRE;
        }

        return condition;
    }

    std::vector<ActorCondition> ActorCondition::create(std::vector<GameAPI::GameActor> actors) {
        std::vector<ActorCondition> ret;
        for (auto& actor : actors) {
            ret.push_back(create(actor));
        }
        return ret;
    }

    bool ActorCondition::fulfills(ActorCondition other) {
        if (type != other.type) {
            return false;
        }

        if (MCM::MCMTable::intendedSexOnly() && sex != GameAPI::GameSex::AGENDER && other.sex != GameAPI::GameSex::AGENDER && sex != other.sex) {
            return false;
        }

        if ((requirements & other.requirements) != other.requirements) {
            return false;
        }
        
        return true;
    }
}