#pragma once

#include <shared_mutex>

#include "AutoModeStage.h"
#include "ThreadActor.h"

#include "Alignment/ActorAlignment.h"
#include "Alignment/ThreadKey.h"
#include "GameAPI/GameActor.h"
#include "Graph/Node.h"
#include "Serial/OldThread.h"

namespace OStim {

    using ThreadId = int64_t;
    class Thread : public RE::BSTEventSink<RE::BSAnimationGraphEvent>{
    public:
        ThreadId m_threadId;

        Thread(ThreadId id, RE::TESObjectREFR* furniture, std::vector<RE::Actor*> actors);

        ~Thread();

        void initContinue();

        inline Graph::Node* getCurrentNode() { return m_currentNode; }

        std::string getAlignmentKey();
        Alignment::ActorAlignment getActorAlignment(int index);
        void updateActorAlignment(int index, Alignment::ActorAlignment alignment);
        void alignActors();

        inline void setStopTimer(int timer) { stopTimer = timer; }
        
        void ChangeNode(Graph::Node* a_node);

        void navigateTo(Graph::Node* node, bool useFades);

        void AddActor(RE::Actor* a_actor);
        void RemoveActor();

        std::vector<Trait::ActorConditions> getActorConditions();

        void loop();

        inline std::map<int32_t, ThreadActor> getActors() { return m_actors; }

        ThreadActor* GetActor(GameAPI::GameActor a_actor);
        ThreadActor* GetActor(int a_position);
        int getActorPosition(GameAPI::GameActor actor);

        void SetSpeed(int speed);

        void callEvent(std::string eventName, int actorIndex, int targetIndex, int performerIndex);

        void stop();
        void close();

        inline RE::TESObjectREFR* GetStageObject() { return vehicle; }

        inline bool isPlayerThread() {
            return playerThread;
        }

    public:
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) override;

        Serialization::OldThread serialize();

    private:
        Graph::Node* m_currentNode = nullptr;
        bool playerThread = false;
        std::map<int32_t, ThreadActor> m_actors;

        RE::TESObjectREFR* furniture;
        Furniture::FurnitureType furnitureType = Furniture::FurnitureType::NONE;
        RE::TESForm* furnitureOwner = nullptr;
        RE::TESObjectREFR* vehicle;
        std::shared_mutex nodeLock;

        int m_currentNodeSpeed = 0;
        std::string alignmentKey;

        float freeCamSpeedBefore = 0;
        float worldFOVbefore = 0;
        float timeScaleBefore = 0;

        int stopTimer = 0;

        std::vector<Sound::SoundPlayer*> soundPlayers;

        void addActorInner(int index, RE::Actor* actor);
        void addActorSink(RE::Actor* a_actor);
        void removeActorSink(RE::Actor* a_actor);

        void rebuildAlignmentKey();
        void alignActor(ThreadActor* threadActor, Alignment::ActorAlignment alignment);

#pragma region automode
    public:
        inline bool isInAutoMode() { return autoMode; }
        void startAutoMode();
        inline void stopAutoMode() { autoMode = false; }

        void setAutoModeToMainStage();

    private:
        bool autoMode = false;
        AutoModeStage autoModeStage = AutoModeStage::NONE;
        int foreplayThreshold = 0;
        int pulloutThreshold = 0;
        int autoModeCooldown = 0;

        void evaluateAutoMode();
        void startAutoModeCooldown();
        void progressAutoMode();
        void loopAutoMode();
#pragma endregion
    };

}  // namespace OStim