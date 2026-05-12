// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Async/Async.h"

struct FZDelegateConnection
{
	FZDelegateConnection() = default;

	// Multicast - with handle
	FZDelegateConnection(FDelegateHandle InHandle, TFunction<void(FDelegateHandle)> InUnbindFunc)
		: Handle(InHandle)
		, bAlive(MakeShared<bool>(true))
		, UnbindFunc([InHandle, InUnbindFunc = MoveTemp(InUnbindFunc)]()
		{
			AsyncTask(ENamedThreads::GameThread, [InHandle, InUnbindFunc]()
			{
				InUnbindFunc(InHandle);
			});
		})
	{}

	// Single delegate - no handle
	explicit FZDelegateConnection(TFunction<void()> InUnbindFunc)
		: bAlive(MakeShared<bool>(true))
		, UnbindFunc([InUnbindFunc = MoveTemp(InUnbindFunc)]()
		{
			AsyncTask(ENamedThreads::GameThread, [InUnbindFunc]()
			{
				InUnbindFunc();
			});
		})
	{}

	~FZDelegateConnection() { Disconnect(); }

	void Disconnect()
	{
		if (UnbindFunc)
		{
			if (bAlive)
			{
				*bAlive = false;
			}
			// Queue the actual removal
			TFunction<void()> LocalUnbind = MoveTemp(UnbindFunc);
			UnbindFunc = nullptr;
			Handle.Reset();
			LocalUnbind();
		}
	}
	// Copy constructor intentionally does nothing - required for use in USTRUCTs
	// Always use MoveTemp when storing in containers.
	FZDelegateConnection(const FZDelegateConnection&) : Handle(), bAlive(nullptr), UnbindFunc(nullptr) {}
	FZDelegateConnection& operator=(const FZDelegateConnection&) 
	{ 
		return *this;
	}
	FZDelegateConnection(FZDelegateConnection&& Other)
	{
		Handle = MoveTemp(Other.Handle);
		bAlive = MoveTemp(Other.bAlive);
		UnbindFunc = MoveTemp(Other.UnbindFunc);

		Other.Handle.Reset();
		Other.bAlive = nullptr;
		Other.UnbindFunc = nullptr;
	}
	FZDelegateConnection& operator=(FZDelegateConnection&& Other)
	{
		if (this != &Other)
		{
			Disconnect(); // Unbind old binding first
			Handle = MoveTemp(Other.Handle);
			bAlive = MoveTemp(Other.bAlive);
			UnbindFunc = MoveTemp(Other.UnbindFunc);

			Other.Handle.Reset();
			Other.bAlive = nullptr;
			Other.UnbindFunc = nullptr;
		}
		return *this;
	}

	bool GetAlive() const { return bAlive.IsValid() && bAlive.Get(); }

private:
	FDelegateHandle Handle;
	TSharedPtr<bool> bAlive;
	TFunction<void()> UnbindFunc;
};

// Multicast lambda binding
#define BIND_MULTICAST(Connection, DelegateOwner, Delegate, ...) \
    ensure(Cast<UObject>(DelegateOwner)); \
    Connection = FZDelegateConnection( \
        Delegate.AddLambda(__VA_ARGS__), \
        [DelegatePtr = &Delegate, WeakDelegateOwner = MakeWeakObjectPtr(Cast<UObject>(DelegateOwner))](FDelegateHandle Handle) \
        { if (WeakDelegateOwner.IsValid()) { DelegatePtr->Remove(Handle); } } \
)
// Multicast UObject binding
#define BIND_MULTICAST_UOBJECT(Connection, DelegateOwner, Delegate, Object, Function, ...) \
    ensure(Cast<UObject>(DelegateOwner)); \
    Connection = FZDelegateConnection( \
        Delegate.AddUObject(Object, Function, ##__VA_ARGS__), \
        [DelegatePtr = &Delegate, WeakDelegateOwner = MakeWeakObjectPtr(Cast<UObject>(DelegateOwner))](FDelegateHandle Handle) \
        { if (WeakDelegateOwner.IsValid()) { DelegatePtr->Remove(Handle); } } \
)
// Single delegate lambda binding
#define BIND_DELEGATE(Connection, DelegateOwner, Delegate, ...) \
    ensure(Cast<UObject>(DelegateOwner)); \
    Connection = FZDelegateConnection( \
        [DelegatePtr = &Delegate, WeakDelegateOwner = MakeWeakObjectPtr(Cast<UObject>(DelegateOwner))]() \
        { if (WeakDelegateOwner.IsValid()) { DelegatePtr->Unbind(); } } \
    ); \
    Delegate.BindLambda(__VA_ARGS__)

// Single delegate UObject binding
#define BIND_DELEGATE_UOBJECT(Connection, DelegateOwner, Delegate, Object, Function, ...) \
    ensure(Cast<UObject>(DelegateOwner)); \
    Connection = FZDelegateConnection( \
        [DelegatePtr = &Delegate, WeakDelegateOwner = MakeWeakObjectPtr(Cast<UObject>(DelegateOwner))]() \
        { if (WeakDelegateOwner.IsValid()) { DelegatePtr->Unbind(); } } \
    ); \
    Delegate.BindUObject(Object, Function, ##__VA_ARGS__)
// Raw member function binding (use with caution)
//#define BIND_MULTICAST_RAW(Connection, Delegate, Object, Function) \
//    Connection = FDelegateConnection( \
//        Delegate.AddRaw(Object, Function), \
//        [&Delegate](FDelegateHandle Handle) { Delegate.Remove(Handle); } \
//)
