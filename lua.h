#pragma once
#include "main.h"
#include "sdk.h"

/*
#define	LUA_FUNCTION(_func_)	int _func_(lua_State* L)
#define	LUA	L->base
#define	_G	-10002
*/

enum
{
	LUA_CLIENT,
	LUA_SERVER,
	LUA_MENU
};

/*
enum
{
	SPECIAL_GLOB,
	SPECIAL_ENV,
	SPECIAL_REG,
};

enum
{
	TYPE_INVALID = -1,
	TYPE_NIL,
	TYPE_BOOL,
	TYPE_LIGHTUSERDATA,
	TYPE_NUMBER,
	TYPE_STRING,
	TYPE_TABLE,
	TYPE_FUNCTION,
	TYPE_USERDATA,
	TYPE_THREAD,

	TYPE_ENTITY,
	TYPE_VECTOR,
	TYPE_ANGLE,
	TYPE_PHYSOBJ,
	TYPE_SAVE,
	TYPE_RESTORE,
	TYPE_DAMAGEINFO,
	TYPE_EFFECTDATA,
	TYPE_MOVEDATA,
	TYPE_RECIPIENTFILTER,
	TYPE_USERCMD,
	TYPE_SCRIPTEDVEHICLE,

	TYPE_MATERIAL,
	TYPE_PANEL,
	TYPE_PARTICLE,
	TYPE_PARTICLEEMITTER,
	TYPE_TEXTURE,
	TYPE_USERMSG,

	TYPE_CONVAR,
	TYPE_IMESH,
	TYPE_MATRIX,
	TYPE_SOUND,
	TYPE_PIXELVISHANDLE,
	TYPE_DLIGHT,
	TYPE_VIDEO,
	TYPE_FILE,
};

static char* lua_typename[] =
{
	"nil",
	"bool",
	"lightuserdata",
	"number",
	"string",
	"table",
	"function",
	"userdata",
	"thread",
	"entity",
	"vector",
	"angle",
	"physobj",
	"save",
	"restore",
	"damageinfo",
	"effectdata",
	"movedata",
	"recipientfilter",
	"usercmd",
	"vehicle",
	"material",
	"panel",
	"particle",
	"particleemitter",
	"texture",
	"usermsg",
	"convar",
	"mesh",
	"matrix",
	"sound",
	"pixelvishandle",
	"dlight",
	"video",
	"file"
};

inline const char* GetLuaTypeName(int i)
{
	return lua_typename[i];
}

*/

inline const char* GetLuaStateName(char state)
{
	switch (state)
	{
	case LUA_CLIENT:	return "Client";
	case LUA_SERVER:	return "Server";
	case LUA_MENU:		return "Menu";
	};

	return 0;
}

class ILuaInterface;

/*

class ILuaObject;
class ILuaBase;

class lua_State
{
public:
	char		header[69];
	ILuaBase*	base;
};

typedef int (*CFunc) (lua_State* L);

class ILuaInterface
{
public:
	virtual void*	Top(void) = 0;	// 0
	virtual void*	Push(int) = 0;	// 1
	virtual void*	Pop(int) = 0;	// 2
	virtual void*	GetTable(int) = 0;	// 3
	virtual void*	GetField(int,char  const*) = 0;	// 4
	virtual void*	SetField(int,char  const*) = 0;	// 5
	virtual void*	CreateTable(void) = 0;	// 6
	virtual void*	SetTable(int) = 0;	// 7
	virtual void*	SetMetaTable(int) = 0;	// 8
	virtual void*	GetMetaTable(int) = 0;	// 9
	virtual void*	Call(int,int) = 0;	// 10
	virtual void*	PCall(int,int,int) = 0;	// 11
	virtual void*	Equal(int,int) = 0;	// 12
	virtual void*	RawEqual(int,int) = 0;	// 13
	virtual void*	Insert(int) = 0;	// 14
	virtual void*	Remove(int) = 0;	// 15
	virtual void*	Next(int) = 0;	// 16
	virtual void*	NewUserdata(unsigned) = 0;	// 17
	virtual void*	ThrowError(char  const*) = 0;	// 18
	virtual void*	CheckType(int,int) = 0;	// 19
	virtual void*	ArgError(int,char  const*) = 0;	// 20
	virtual void*	RawGet(int) = 0;	// 21
	virtual void*	RawSet(int) = 0;	// 22
	virtual const char*	GetString(int,unsigned *) = 0;	// 23
	virtual double	GetNumber(int) = 0;	// 24
	virtual bool	GetBool(int) = 0;	// 25
	virtual CFunc	GetCFunction(int) = 0;	// 26
	virtual void*	GetUserdata(int) = 0;	// 27
	virtual void	PushNil(void) = 0;	// 28
	virtual void	PushString(char  const*,unsigned) = 0;	// 29
	virtual void	PushNumber(double) = 0;	// 30
	virtual void	PushBool(bool) = 0;	// 31
	virtual void	PushCFunction(CFunc) = 0;	// 32
	virtual void	PushCClosure(CFunc,int) = 0;	// 33
	virtual void	PushUserdata(void*) = 0;	// 34
	virtual void*	ReferenceCreate(void) = 0;	// 35
	virtual void*	ReferenceFree(int) = 0;	// 36
	virtual void*	ReferencePush(int) = 0;	// 37
	virtual void*	PushSpecial(int) = 0;	// 38
	virtual void*	IsType(int,int) = 0;	// 39
	virtual void*	GetType(int) = 0;	// 40
	virtual void*	GetTypeName(int) = 0;	// 41
	virtual void*	CreateMetaTableType(char  const*,int) = 0;	// 42
	virtual void*	CheckString(int) = 0;	// 43
	virtual void*	CheckNumber(int) = 0;	// 44
	virtual void*	ObjLen(int) = 0;	// 45
	virtual void*	AddThreadedCall(void*) = 0;	// 46
	virtual void*	Init(void *,bool) = 0;	// 47
	virtual void*	Shutdown(void) = 0;	// 48
	virtual void*	Cycle(void) = 0;	// 49
	virtual lua_State*	GetLuaState(void) = 0;	// 50
	virtual void*	Global(void) = 0;	// 51
	virtual void*	GetObject(int) = 0;	// 52
	virtual void*	DELETE_ME_5437(int) = 0;	// 53
	virtual void*	_DELETE_ME2466(int) = 0;	// 54
	virtual void*	PushLuaObject(ILuaObject *) = 0;	// 55
	virtual void*	PushLuaFunction(CFunc) = 0;	// 56
	virtual void*	LuaError(char  const*,int) = 0;	// 57
	virtual void*	TypeError(char  const*,int) = 0;	// 58
	virtual void*	CallInternal(int,int) = 0;	// 59
	virtual void*	CallInternalNoReturns(int) = 0;	// 60
	virtual void*	CallInternalGetBool(int) = 0;	// 61
	virtual void*	CallInternalGetString(int) = 0;	// 62
	virtual void*	CallInternalGet(int,ILuaObject *) = 0;	// 63
	virtual void*	_DELETE_ME(ILuaObject *,void *) = 0;	// 64
	virtual void*	NewGlobalTable(char  const*) = 0;	// 65
	virtual void*	NewTemporaryObject(void) = 0;	// 66
	virtual void*	isUserData(int) = 0;	// 67
	virtual void*	GetMetaTableObject(char  const*,int) = 0;	// 68
	virtual void*	GetMetaTableObject(int) = 0;	// 69
	virtual void*	GetReturn(int) = 0;	// 70
	virtual void*	IsServer(void) = 0;	// 71
	virtual void*	IsClient(void) = 0;	// 72
	virtual void*	IsDedicatedServer(void) = 0;	// 73
	virtual void*	DestroyObject(ILuaObject *) = 0;	// 74
	virtual void*	CreateObject(void) = 0;	// 75
	virtual void*	SetMember(ILuaObject *,ILuaObject *,ILuaObject *) = 0;	// 76
	virtual void*	GetNewTable(void) = 0;	// 77
	virtual void*	SetMember(ILuaObject *,float) = 0;	// 78
	virtual void*	SetMember(ILuaObject *,float,ILuaObject *) = 0;	// 79
	virtual void*	SetMember(ILuaObject *,char  const*) = 0;	// 80
	virtual void*	SetMember(ILuaObject *,char  const*,ILuaObject *) = 0;	// 81
	virtual void*	SetIsServer(bool) = 0;	// 82
	virtual void*	PushLong(long) = 0;	// 83
	virtual void*	GetFlags(int) = 0;	// 84
	virtual void*	FindOnObjectsMetaTable(int,int) = 0;	// 85
	virtual void*	FindObjectOnTable(int,int) = 0;	// 86
	virtual void*	SetMemberFast(ILuaObject *,int,int) = 0;	// 87
	virtual void*	RunString(char  const*,char  const*,char  const*,bool,bool) = 0;	// 88
	virtual void*	IsEqual(ILuaObject *,ILuaObject *) = 0;	// 89
	virtual void*	Error(char  const*) = 0;	// 90
	virtual void*	GetStringOrError(int) = 0;	// 91
	virtual void*	RunLuaModule(char  const*) = 0;	// 92
	virtual void*	FindAndRunScript(char  const*,bool,bool,char  const*) = 0;	// 93
	virtual void*	SetPathID(char  const*) = 0;	// 94
	virtual void*	GetPathID(void) = 0;	// 95
	virtual void*	ErrorNoHalt(char  const*,...) = 0;	// 96
	virtual void*	Msg(char  const*,...) = 0;	// 97
	virtual void*	PushPath(char  const*) = 0;	// 98
	virtual void*	PopPath(void) = 0;	// 99
	virtual void*	GetPath(void) = 0;	// 100
	virtual void*	GetColor(int) = 0;	// 101
	virtual void*	PushColor(int,int,int,int) = 0;	// 102
	virtual void*	GetStack(int,void *) = 0;	// 103
	virtual void*	GetInfo(char  const*,void *) = 0;	// 104
	virtual void*	GetLocal(void *,int) = 0;	// 105
	virtual void*	GetUpvalue(int,int) = 0;	// 106
	virtual void*	RunStringEx(char  const*,char  const*,char  const*,bool,bool,bool) = 0;	// 107
	virtual void*	DELETE_ME1(char  const*,int) = 0;	// 108
	virtual void*	GetDataString(int,void **) = 0;	// 109
	virtual void*	ErrorFromLua(char  const*,...) = 0;	// 110
	virtual void*	GetCurrentLocation(void) = 0;	// 111
	virtual void*	MsgColour(void*,char  const*,...) = 0;	// 112
	virtual void*	SetState(lua_State *) = 0;	// 113
	virtual void*	DELETE_ME2(void) = 0;	// 114
	virtual void*	GetCurrentFile(std::string &) = 0;	// 115
	virtual void*	CompileString(void*,std::string  const&) = 0;	// 116
	virtual void*	ThreadLock(void) = 0;	// 117
	virtual void*	ThreadUnlock(void) = 0;	// 118
	virtual void*	CallFunctionProtected(int,int,bool) = 0;	// 119
	virtual void*	Require(char  const*) = 0;	// 120
	virtual void*	GetActualTypeName(int) = 0;	// 121
	virtual void*	SetupInfiniteLoopProtection(void) = 0;	// 122
};

class ILuaBase
{
public:
	virtual int		Top( void ) = 0;
	virtual void	Push( int iStackPos ) = 0;
	virtual void	Pop( int iAmt = 1 ) = 0;
	virtual void	GetTable( int iStackPos ) = 0;
	virtual void	GetField( int iStackPos, const char* strName ) = 0;
	virtual void	SetField( int iStackPos, const char* strName ) = 0;
	virtual void	CreateTable() = 0;
	virtual void	SetTable( int i ) = 0;
	virtual void	SetMetaTable( int i ) = 0;
	virtual bool	GetMetaTable( int i ) = 0;
	virtual void	Call( int iArgs, int iResults ) = 0;
	virtual int		PCall( int iArgs, int iResults, int iErrorFunc ) = 0;
	virtual int		Equal( int iA, int iB ) = 0;
	virtual int		RawEqual( int iA, int iB ) = 0;
	virtual void	Insert( int iStackPos ) = 0;
	virtual void	Remove( int iStackPos ) = 0;
	virtual int		Next( int iStackPos ) = 0;
	virtual void*	NewUserdata( unsigned iSize ) = 0;
	virtual void	ThrowError( const char* strError ) = 0;
	virtual void	CheckType( int iStackPos, int iType ) = 0;
	virtual void	ArgError( int iArgNum, const char* strMessage ) = 0;
	virtual void	RawGet( int iStackPos ) = 0;
	virtual void	RawSet( int iStackPos ) = 0;
	virtual const char*	GetString( int iStackPos = -1, unsigned* iOutLen = NULL ) = 0;
	virtual double	GetNumber( int iStackPos = -1 ) = 0;
	virtual bool	GetBool( int iStackPos = -1 ) = 0;
	virtual CFunc	GetCFunction( int iStackPos = -1 ) = 0;
	virtual void*	GetUserdata( int iStackPos = -1 ) = 0;

	virtual void	PushNil() = 0;
	virtual void	PushString( const char* val, unsigned iLen = 0 ) = 0;
	virtual void	PushNumber( double val ) = 0;
	virtual void	PushBool( bool val ) = 0;
	virtual void	PushCFunction( CFunc val ) = 0;
	virtual void	PushCClosure( CFunc val, int iVars ) = 0;
	virtual void	PushUserdata( void* ) = 0;

	virtual int		ReferenceCreate() = 0;
	virtual void	ReferenceFree( int i ) = 0;
	virtual void	ReferencePush( int i ) = 0;

	virtual void	PushSpecial( int iType ) = 0;

	virtual bool		IsType( int iStackPos, int iType ) = 0;
	virtual int			GetType( int iStackPos ) = 0;
	virtual const char*	GetTypeName( int iType ) = 0;
	virtual void		CreateMetaTableType( const char* strName, int iType ) = 0;
	virtual const char*	CheckString( int iStackPos = -1 ) = 0;
	virtual double		CheckNumber( int iStackPos = -1 ) = 0;
};
*/