#pragma once

namespace GameAPI {
    namespace GameUtil {
        void forEachReferenceInRange(const RE::NiPoint3& center, float radius, std::function<RE::BSContainer::ForEachResult(RE::TESObjectREFR&)> callback);

        void CompileAndRun(RE::Script* script, RE::TESObjectREFR* targetRef, RE::COMPILER_NAME name = RE::COMPILER_NAME::kSystemWindowCompiler);

        inline void CompileAndRunImpl(RE::Script* script, RE::ScriptCompiler* compiler, RE::COMPILER_NAME name, RE::TESObjectREFR* targetRef) {
			using func_t = decltype(CompileAndRunImpl);
			REL::Relocation<func_t> func{ RELOCATION_ID(21416, REL::Module::get().version().patch() < 1130 ? 21890 : 441582) };
			return func(script, compiler, name, targetRef);
        }
    }
}