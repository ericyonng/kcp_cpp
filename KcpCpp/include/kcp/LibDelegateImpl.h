#ifdef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_DELEGATE_H__

#pragma once

KCP_CPP_BEGIN

// 委托抽象类
template <typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...>::IDelegate()
{
}

template <typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...>::~IDelegate()
{
}

template <typename Rtn, typename... Args>
inline void IDelegate<Rtn, Args...>::Release()
{
    delete this;
}

// 类委托
template <typename ObjType, class Rtn, typename... Args>
inline DelegateClass<ObjType, Rtn, Args...>::DelegateClass(ObjType *t, Rtn(ObjType::*f)(Args...))
    :_obj(t)
    ,_f(f)
{
}

template <typename ObjType, class Rtn, typename... Args>
inline DelegateClass<ObjType, Rtn, Args...>::DelegateClass(ObjType *t, Rtn(ObjType::*f)(Args...) const)
    :_obj(t)
    , _f(decltype(_f)(f))
{
}

template <typename ObjType, typename Rtn, typename... Args>
inline DelegateClass<ObjType, Rtn, Args...>::~DelegateClass()
{
}

template <typename ObjType, typename Rtn, typename... Args>
inline Rtn DelegateClass<ObjType, Rtn, Args...>::Invoke(Args... args)
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}

template <typename ObjType, typename Rtn, typename... Args>
inline Rtn DelegateClass<ObjType, Rtn, Args...>::Invoke(Args... args) const
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}

template <typename ObjType, typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegateClass<ObjType, Rtn, Args...>::CreateNewCopy() const
{
    return DelegateFactory::Create(_obj, _f);
}

// 类委托带删除对象
template <typename ObjType, class Rtn, typename... Args>
inline DelegateClassDelObj<ObjType, Rtn, Args...>::DelegateClassDelObj(ObjType *t, Rtn(ObjType::*f)(Args...))
    :_obj(t)
    ,_f(f)
{
}

template <typename ObjType, class Rtn, typename... Args>
inline DelegateClassDelObj<ObjType, Rtn, Args...>::DelegateClassDelObj(ObjType *t, Rtn(ObjType::*f)(Args...) const)
    :_obj(t)
    , _f(decltype(_f)(f))
{
}

template <typename ObjType, typename Rtn, typename... Args>
inline DelegateClassDelObj<ObjType, Rtn, Args...>::~DelegateClassDelObj()
{
    delete _obj;
}

template <typename ObjType, typename Rtn, typename... Args>
inline Rtn DelegateClassDelObj<ObjType, Rtn, Args...>::Invoke(Args... args)
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}

template <typename ObjType, typename Rtn, typename... Args>
inline Rtn DelegateClassDelObj<ObjType, Rtn, Args...>::Invoke(Args... args) const
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}


// 函数委托
template <typename Rtn, typename... Args>
inline DelegateFunction<Rtn, Args...>::DelegateFunction(Rtn(*f)(Args...))
    :_f(f)
{
}

template <typename Rtn, typename... Args>
inline DelegateFunction<Rtn, Args...>::~DelegateFunction()
{
}

template <typename Rtn, typename... Args>
inline Rtn DelegateFunction<Rtn, Args...>::Invoke(Args... args)
{
    return (*_f)(std::forward<Args>(args)...);
}

template <typename Rtn, typename... Args>
inline Rtn DelegateFunction<Rtn, Args...>::Invoke(Args... args) const
{
    return (*_f)(std::forward<Args>(args)...);
}

template <typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegateFunction<Rtn, Args...>::CreateNewCopy() const
{
    return DelegateFactory::Create(_f);
}


// 闭包委托

template <typename ClosureFuncType, typename Rtn, typename... Args>
inline DelegateClosureFunc<ClosureFuncType, Rtn, Args...>::DelegateClosureFunc(ClosureFuncType &&closureFunc)
    :_closureFun(std::forward<ClosureFuncType>(closureFunc))
{

}

template <typename ClosureFuncType, typename Rtn, typename... Args>
inline DelegateClosureFunc<ClosureFuncType, Rtn, Args...>::DelegateClosureFunc(ClosureFuncType const&closureFunc)
    :_closureFun(closureFunc)
{

}

template <typename ClosureFuncType, typename Rtn, typename... Args>
inline DelegateClosureFunc<ClosureFuncType, Rtn, Args...>::~DelegateClosureFunc()
{
}

template <typename ClosureFuncType, typename Rtn, typename... Args>
inline Rtn DelegateClosureFunc<ClosureFuncType, Rtn, Args...>::Invoke(Args... args)
{
    return _closureFun(std::forward<Args>(args)...);
}

template <typename ClosureFuncType, typename Rtn, typename... Args>
inline Rtn DelegateClosureFunc<ClosureFuncType, Rtn, Args...>::Invoke(Args... args) const
{
    return _closureFun(std::forward<Args>(args)...);
}

template <typename ClosureFuncType, typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegateClosureFunc<ClosureFuncType, Rtn, Args...>::CreateNewCopy() const
{
    return DelegateFactory::Create<decltype(_closureFun), Rtn, Args...>(_closureFun);
}


// 委托工厂方法
template <typename ObjType, typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegateFactory::Create(ObjType *obj, Rtn(ObjType::*f)(Args...))
{
    return new DelegateClass<ObjType, Rtn, Args...>(obj, f);
}

template <typename ObjType, typename Rtn, typename... Args>
inline const IDelegate<Rtn, Args...> *DelegateFactory::Create(ObjType *obj, Rtn(ObjType::*f)(Args...) const)
{
    return new DelegateClass<ObjType, Rtn, Args...>(obj, f);
}

template <typename ObjType, typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegateFactory::CreateAndHelpDelObj(ObjType *obj, Rtn(ObjType::*f)(Args...))
{
    return new DelegateClassDelObj<ObjType, Rtn, Args...>(obj, f);
}

template <typename ObjType, typename Rtn, typename... Args>
inline const IDelegate<Rtn, Args...> *DelegateFactory::CreateAndHelpDelObj(ObjType *obj, Rtn(ObjType::*f)(Args...) const)
{
    return new DelegateClassDelObj<ObjType, Rtn, Args...>(obj, f);
}


template <typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegateFactory::Create(Rtn(*f)(Args...))
{
    return new DelegateFunction<Rtn, Args...>(f);
}

template <typename ClosureFuncType, typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegateFactory::Create(ClosureFuncType &&func)
{
    return new DelegateClosureFunc<ClosureFuncType, Rtn, Args...>(std::forward<ClosureFuncType>(func));
}

template <typename ClosureFuncType, typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegateFactory::Create(ClosureFuncType const&func)
{
    return new DelegateClosureFunc<ClosureFuncType, Rtn, Args...>(std::forward<ClosureFuncType const&>(func));
}

KCP_CPP_END

#endif
