#include "Lua/Interface.h"
#include "tier0/dbg.h"
#include "Color.h"

using namespace GarrysMod::Lua;

ILuaBase* lua = NULL;
SpewOutputFunc_t OldSpew = NULL;
int LuaColor = NULL;

SpewRetval_t NewSpew(SpewType_t spewType, const char *pMsg)
{
	const Color* color = GetSpewOutputColor();
	int spewColor = NULL;

	lua->PushSpecial(SPECIAL_GLOB);
		lua->ReferencePush(LuaColor);
			lua->PushNumber(color->r());
			lua->PushNumber(color->g());
			lua->PushNumber(color->b());
			lua->PushNumber(color->a());
		lua->Call(4, 1);

		spewColor = lua->ReferenceCreate();

		lua->GetField(-1, "hook");
			lua->GetField(-1, "Call");
			lua->PushString("ShouldSpew");
			lua->PushNil();
			lua->PushString(pMsg);
			lua->PushNumber(spewType);
			lua->ReferencePush(spewColor);
			lua->PushNumber(GetSpewOutputLevel());
			lua->PushString(GetSpewOutputGroup());
		lua->Call(7, 1);

		lua->ReferenceFree(spewColor);

		if (lua->IsType(-1, Type::BOOL) && lua->GetBool(-1) == false)
		{
			lua->Pop(3);

			return SPEW_CONTINUE;
		}

	lua->Pop(3);

	if (OldSpew)
	{
		return OldSpew(spewType, pMsg);
	} else {
		return DefaultSpewFunc(spewType, pMsg);
	}
}



int HookSpew(lua_State* state)
{
	lua->CheckType(-1, Type::BOOL);

	if (lua->GetBool(-1))
	{
		SpewOutputFunc(NewSpew);
	} else {
		if (OldSpew)
		{
			SpewOutputFunc(OldSpew);
		} else {
			SpewOutputFunc(DefaultSpewFunc);
		}
	}

	return 0;
}

GMOD_MODULE_OPEN()
{
	lua = LUA;
	OldSpew = GetSpewOutputFunc();
	SpewOutputFunc(NewSpew);

	lua->PushSpecial(SPECIAL_GLOB);
		lua->GetField(-1, "Color");
		LuaColor = lua->ReferenceCreate();

		lua->GetField(-1, "engine");
		lua->PushCFunction(HookSpew);
		lua->SetField(-2, "HookSpew");

		lua->PushNumber(SPEW_MESSAGE);
		lua->SetField(-2, "SPEW_MESSAGE");

		lua->PushNumber(SPEW_WARNING);
		lua->SetField(-2, "SPEW_WARNING");

		lua->PushNumber(SPEW_ASSERT);
		lua->SetField(-2, "SPEW_ASSERT");

		lua->PushNumber(SPEW_ERROR);
		lua->SetField(-2, "SPEW_ERROR");

		lua->PushNumber(SPEW_LOG);
		lua->SetField(-2, "SPEW_LOG");
	lua->Pop(2);

	ColorSpewMessage(SPEW_MESSAGE, &(Color(0, 255, 0, 255)), "spew module loaded.\n");

	return 0;
}

GMOD_MODULE_CLOSE()
{
	lua->ReferenceFree(LuaColor);
	lua = NULL;
	OldSpew = NULL;

	return 0;
}