#pragma once

class FRefCountBase
{
public:
	FRefCountBase()
		: NumRefs(1)
	{}

	virtual ~FRefCountBase()
	{}

	int32 GetRefCount() const { return NumRefs; }
	int32 AddRef() { return ++NumRefs; }
	int32 Release()
	{
		if (--NumRefs == 0)
		{
			delete this;
		}

		return NumRefs;
	}

protected:
	TAtomic<int32> NumRefs;
};

template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* InPtr) { Set(InPtr); }
	TSharedPtr(const TSharedPtr& Rhs) { Set(Rhs.Ptr); }
	TSharedPtr(TSharedPtr&& Rhs) noexcept
	{
		Ptr = Rhs.Ptr;
		Rhs.Ptr = nullptr;
	}

	template<typename U>
	TSharedPtr(const TSharedPtr<U>& Rhs)
	{
		Set(static_cast<T*>(Rhs.Ptr));
	}

	~TSharedPtr() { Reset(); }

public:
	TSharedPtr& operator=(const TSharedPtr& Rhs)
	{
		if (Ptr != Rhs.Ptr)
		{
			Reset();
			Set(Rhs.Ptr);
		}

		return *this;
	}

	TSharedPtr& operator=(TSharedPtr&& Rhs) noexcept
	{
		Reset();
		Ptr = Rhs.Ptr;
		Rhs.Ptr = nullptr;
		return *this;
	}

	bool		operator==(const TSharedPtr& Rhs) const { return Ptr == Rhs.Ptr; }
	bool		operator==(T* InPtr) const { return Ptr == InPtr; }
	bool		operator!=(const TSharedPtr& Rhs) const { return Ptr != Rhs.Ptr; }
	bool		operator!=(T* InPtr) const { return Ptr != InPtr; }
	bool		operator<(const TSharedPtr& Rhs) const { return Ptr < Rhs.Ptr; }
	T*			operator*() { return Get(); }
	const T*	operator*() const { return Get(); }
	explicit	operator T* () const { return Get(); }
	T*			operator->() { return Get(); }
	const T*	operator->() const { return Get(); }

public:

	T* Get() { return Ptr; }
	bool IsValid() { return Ptr != nullptr; }

	inline void Set(T* InPtr)
	{
		Ptr = InPtr;
		if (Ptr)
		{
			Ptr->AddRef();
		}
	}

	inline void Reset()
	{
		if (Ptr)
		{
			Ptr->Release();
			Ptr = nullptr;
		}
	}

private:
	T* Ptr = nullptr;
};