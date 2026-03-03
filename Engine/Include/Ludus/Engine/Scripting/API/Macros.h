#pragma once

namespace Ludus::Engine::Scripts::API
{

#define LUDUS_FORWARD_DECLARE_SCRIPT(SymbolName)                                \
    void Add_##SymbolName(::Ludus::Scripts::Registry* registry)

#define LUDUS_DEFINE_SCRIPT(SymbolName, CreateFn, DestroyFn, UpdateFn)			\
    static const ::Ludus::Engine::Scripting::API::ScriptDefinition				\
        Definition_##SymbolName { #SymbolName, CreateFn, DestroyFn, UpdateFn };	\
    void Add_##SymbolName(::Ludus::Engine::Scripting::API::Registry* registry)	\
    {																			\
        registry->Add(&Definition_##SymbolName);								\
    }
}
